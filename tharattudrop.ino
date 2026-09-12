/*************************************************

         THARATTU.DROP()
         
  ESP32 + INMP441 + LCD + SERVO + WEB/PHONE SYNC
  (Final Running Version with Mobile Strobe UI)

*************************************************/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <WebServer.h>


// =============================================
// WI-FI ACCESS POINT (FOR PHONE CONNECTION)
// =============================================

const char* ssid = "Tharattu_AP";
const char* password = "password123";

WebServer server(80);


// =============================================
// HTML PAGE FOR PHONE BROWSER (Strobe UI)
// =============================================

const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { background-color: #121212; color: white; text-align: center; font-family: sans-serif; margin-top: 30px; transition: background 0.1s; }
    #box { width: 280px; height: 280px; margin: 20px auto; background: #333; border-radius: 20px; display: flex; align-items: center; justify-content: center; font-size: 22px; font-weight: bold; box-shadow: 0 0 20px rgba(0,0,0,0.5); }
    button { padding: 14px 28px; font-size: 18px; background: #ff4757; color: white; border: none; border-radius: 8px; cursor: pointer; font-weight: bold; }
  </style>
</head>
<body>
  <h1>THARATTU.DROP</h1>
  <p>Status: <span id="status" style="color: #2ed573; font-weight: bold;">Listening...</span></p>
  
  <div id="box">STANDBY</div>
  <br>
  <!-- Required for mobile browsers to allow auto-audio later -->
  <button onclick="enableAudio()">1. Tap to Enable Audio</button>

  <!-- Sample Lullaby Audio (Requires phone cellular data active if AP has no internet) -->
  <audio id="lullaby" loop src="https://www.soundhelix.com/examples/mp3/SoundHelix-Song-1.mp3"></audio>

  <script>
    let audioEnabled = false;

    function enableAudio() {
      let audio = document.getElementById('lullaby');
      audio.play().then(() => {
        audio.pause();
        audio.currentTime = 0;
        audioEnabled = true;
        alert('Audio Enabled! Ready for trigger.');
      }).catch(e => alert('Error enabling audio'));
    }

    // Poll the ESP32 every 500ms for rapid strobe response
    setInterval(function() {
      fetch('/check')
        .then(response => response.text())
        .then(data => {
          let box = document.getElementById('box');
          let status = document.getElementById('status');
          let audio = document.getElementById('lullaby');

          if(data === "TRIGGERED") {
            status.innerText = "KUNJU KARAYUNNU!";
            status.style.color = "#ff4757";
            box.innerText = "STROBE ACTIVE!";
            
            // Rapid Strobe effect: Flashes background body color and box colors intensely
            const colors = ["#ff4757", "#2ed573", "#1e90ff", "#ffeb3b", "#ffffff"];
            let randomColor = colors[Math.floor(Math.random() * colors.length)];
            let strobeColor = colors[Math.floor(Math.random() * colors.length)];
            
            document.body.style.backgroundColor = randomColor;
            box.style.background = strobeColor;
            box.style.color = (strobeColor === "#ffffff") ? "#000" : "#fff";

            if(audioEnabled && audio.paused) {
              audio.play();
            }
          } else {
            status.innerText = "Listening...";
            status.style.color = "#2ed573";
            box.innerText = "STANDBY";
            box.style.background = "#333";
            document.body.style.backgroundColor = "#121212";
            box.style.color = "#fff";
            
            if(!audio.paused) {
              audio.pause();
            }
          }
        })
        .catch(err => {
          console.log("Connection hiccup...");
        });
    }, 500);
  </script>
</body>
</html>
)rawliteral";


// =============================================
// LCD (Change to 0x3F if your screen stays blank)
// =============================================

LiquidCrystal_I2C lcd(0x27, 16, 2);


// =============================================
// SERVO
// =============================================

Servo myServo;

#define SERVO_PIN 27
#define SERVO_CENTER 90
#define SERVO_LEFT   60
#define SERVO_RIGHT  120


// =============================================
// INMP441 PINS
// =============================================

#define I2S_WS   25
#define I2S_SD   33
#define I2S_SCK  26
#define I2S_PORT I2S_NUM_0


// =============================================
// SOUND DETECTION SETTINGS
// =============================================

#define SOUND_THRESHOLD 800
#define CRY_COUNT_REQUIRED 5


// =============================================
// VARIABLES
// =============================================

int cryCount = 0;
bool systemActive = false;
unsigned long lastLogTime = 0;


// =============================================
// SETUP WEB SERVER
// =============================================

void setupWebServer() {
  WiFi.softAP(ssid, password);
  
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });
  
  server.on("/check", []() {
    if (systemActive) {
      server.send(200, "text/plain", "TRIGGERED");
    } else {
      server.send(200, "text/plain", "IDLE");
    }
  });
  
  server.begin();
}


// =============================================
// SETUP INMP441
// =============================================

void setupMicrophone() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_start(I2S_PORT);
}


// =============================================
// GET SOUND LEVEL (With Timeout & Zero-Division Guard)
// =============================================

int getSoundLevel() {
  int32_t samples[128];
  size_t bytesRead = 0;

  i2s_read(I2S_PORT, &samples, sizeof(samples), &bytesRead, pdMS_TO_TICKS(100));
  
  int samplesRead = bytesRead / sizeof(int32_t);
  if (samplesRead <= 0) {
    return 0; 
  }

  long long total = 0;
  for (int i = 0; i < samplesRead; i++) {
    int32_t sample = samples[i] >> 14;
    total += labs(sample); 
  }
  return total / samplesRead;
}


// =============================================
// LCD MODES
// =============================================

void showListening() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("THARATTU.DROP");
  lcd.setCursor(0, 1); lcd.print("Listening...");
}

void showCryDetected() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("KUNJU");
  lcd.setCursor(0, 1); lcd.print("KARAYUNNU!");
}

void showTharattu() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("THARATTU.DROP");
  lcd.setCursor(0, 1); lcd.print("Rocking...");
}


// =============================================
// ROCK THE SERVO (Responsive Web Syncing)
// =============================================

void rockServo() {
  for (int pos = SERVO_LEFT; pos <= SERVO_RIGHT; pos += 2) {
    myServo.write(pos);
    delay(30);
    server.handleClient(); 
  }
  for (int pos = SERVO_RIGHT; pos >= SERVO_LEFT; pos -= 2) {
    myServo.write(pos);
    delay(30);
    server.handleClient(); 
  }
}


// =============================================
// ACTIVATE THARATTU.DROP()
// =============================================

void activateTharattu() {
  systemActive = true;

  Serial.println("KUNJU KARAYUNNU!");
  showCryDetected();
  
  unsigned long cryTimer = millis();
  while (millis() - cryTimer < 2000) {
    server.handleClient();
    delay(1);
  }

  Serial.println("THARATTU.DROP ACTIVATED!");
  showTharattu();

  unsigned long startTime = millis();
  while (millis() - startTime < 15000) {
    rockServo(); 
  }

  myServo.write(SERVO_CENTER);
  systemActive = false;
  cryCount = 0;

  unsigned long endTimer = millis();
  while (millis() - endTimer < 1000) {
    server.handleClient();
    delay(1);
  }

  showListening();
}


// =============================================
// SETUP
// =============================================

void setup() {
  Serial.begin(115200); 
  Serial.println("Starting Setup...");

  // LCD Initialization Insurance
  delay(50);
  Wire.begin(21, 22);
  lcd.init();
  delay(50);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("THARATTU.DROP");
  lcd.setCursor(0, 1); lcd.print("Starting Wi-Fi...");

  // WI-FI ACCESS POINT
  setupWebServer();

  // SERVO
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(SERVO_CENTER);

  // MICROPHONE
  setupMicrophone();

  unsigned long bootTimer = millis();
  while (millis() - bootTimer < 2000) {
    server.handleClient();
    delay(1);
  }

  showListening();

  Serial.println("==========================");
  Serial.println("THARATTU.DROP READY!");
  Serial.println("Connect to Wi-Fi: Tharattu_AP");
  Serial.println("Open: http://192.168.4.1");
  Serial.println("==========================");
}


// =============================================
// LOOP
// =============================================

void loop() {
  server.handleClient();

  int soundLevel = getSoundLevel();

  // Gated logging (1 print per second) to keep Serial clean
  if (millis() - lastLogTime > 1000) {
    Serial.print("Sound Level: ");
    Serial.println(soundLevel);
    lastLogTime = millis();
  }

  if (soundLevel > SOUND_THRESHOLD) {
    cryCount++;
  } else {
    if (cryCount > 0) cryCount--;
  }

  if (cryCount >= CRY_COUNT_REQUIRED && !systemActive) {
    activateTharattu();
  }

  delay(100);
}
