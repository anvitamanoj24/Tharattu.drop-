# Tharattu.Drop() 

### കുഞ്ഞിന് പ്രാണ വേദന, അമ്മക്ക് വീണ വായന

## Basic Details

### Team Name: Overthink()

### Team Members

* **Team Lead:** Anvita M.K - College of Engineering Chengannur
* **Member:** Hrudya Mohan - College of Engineering Chengannur

---

## Project Description

An ESP32-powered smart cradle that listens for a baby's cry and responds in the most unnecessarily entertaining way possible. It rocks the cradle, flashes LEDs, and turns a connected device into a DJ dashboard with pulsing visuals and music.
What if a baby’s cry didn’t mean *“here we go again”*, but *“DJ, drop the beat!”*?
Inspired by the Malayalam proverb *“അമ്മയ്ക്ക് പ്രാണ വേദന, മകനു വീണ വായന”*, Tharattu.Drop() is the flipped version of the proverb. When the baby cries, the cradle detects it, starts rocking, and drops a DJ beat to give the mother an unexpected energy boost.
We took a traditional lullaby moment and gave it an unnecessarily over-engineered Gen-Z upgrade: *baby cries → cradle rocks → DJ drops → mom gets the vibe.*
While the baby cries, mother vibes the beat!!
The baby gets rocked. The mother gets a rave.

---
## The Problem 

Babies cry. Mothers hear them, walk over, and rock them back to sleep.

This ancient system has worked for generations and requires absolutely no software, hardware, Wi-Fi, or electricity.

Naturally, we identified the real problem:

**What does the mother get out of this?**

Nothing.

Not even a light show.

We fixed that.

---

## The Solution

Tharattu.Drop() detects a baby's cry using an INMP441 microphone connected to an ESP32.

The moment a cry is detected:

**Baby cries → ESP32 detects it → Cradle starts rocking → LEDs start flashing → DJ dashboard activates → Music plays**

The servo rocks the cradle while the phone displays a ridiculous DJ-style interface with a pulsing glow, animated bass bars, and a party track.

The baby gets soothed.

The mother gets **3 AM main-stage energy.**

Everybody's needs are met.

---

# Technical Details

## Technologies/Components Used

### For Software

* **Languages:** C++ (Arduino), HTML, CSS, JavaScript
* **Framework:** Arduino Core for ESP32
* **Libraries:** `WiFi.h`, `WebServer.h`, I2S audio handling
* **Tools:** Arduino IDE, Serial Monitor
* **Communication:** Wi-Fi hosted by ESP32
* **Interface:** ESP32-hosted web dashboard

### For Hardware

* ESP32 Development Board
* INMP441 I2S Digital Microphone
* SG90 Micro Servo
* JHD162A 16×2 LCD
* 2 × LEDs
* Bluetooth Speaker
* 3.7V Rechargeable Battery
* Battery Holder
* Buck Converter
* Breadboard
* Jumper Wires
* Cradle/frame and servo mechanism

---

# Implementation

## For Software

The ESP32 acts as the brain of Tharattu.Drop().

1. The **INMP441 microphone** continuously captures surrounding audio.
2. The ESP32 processes the incoming audio to identify cry-like sound patterns.
3. When a cry is detected, the system triggers the response sequence.
4. The **servo motor** rocks the cradle.
5. The **LCD** displays the current cry/status information.
6. The LEDs provide the necessary emergency disco lighting.
7. The ESP32 updates the web dashboard over Wi-Fi.
8. The phone dashboard responds with animated visuals and music.

### Installation

```text
1. Install Arduino IDE.

2. Add ESP32 board support:
   File → Preferences → Additional Board URLs

   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

3. Install the ESP32 package through Boards Manager.

4. Connect the ESP32 to the computer through USB.

5. Install any additional libraries required by the final firmware.

6. Open the Tharattu.Drop() .ino file.
```

## Run

```text
1. Open the project in Arduino IDE.

2. Select the appropriate ESP32 board.

3. Select the correct COM/USB port.

4. Upload the firmware.

5. Open Serial Monitor at 115200 baud.

6. Note the IP address displayed by the ESP32.

7. Connect the phone to the same Wi-Fi network.

8. Open the ESP32 IP address in the phone's browser.

9. Let the baby cry.

10. Let the DJ department handle the rest.
```

---

# Project Documentation

## Screenshots


https://drive.google.com/file/d/12mTMhpl9ClI821X3TFe0-nuY_IAyXKsH/view?usp=sharing
*ESP32 web dashboard in its idle/listening state.*


https://drive.google.com/file/d/1V72U32nq3oGo4PVFwc7DPBc9Bn_KEi2R/view?usp=sharing
*DJ dashboard activated after a cry is detected, with animated visuals and music.*

https://drive.google.com/file/d/1iWb9EnLBPXgJ0CrzYBdsgp7y8QtZQbH6/view?usp=sharing
*Manual cry simulation/testing interface demonstrating the system response.*

---

# Diagrams
https://drive.google.com/file/d/1jS_Qo9sCgXol5MVMvi9IMqs1Vw4HxM-3/view?usp=sharing

---

# Build Photos
https://drive.google.com/drive/folders/1E-ZdPW4oYo9kN6GJ1nu4I6ktob4HrA8V?usp=sharing
---

# Project Demo

## Video
https://drive.google.com/file/d/1yRRUoONlQufZ2jYV_ixlgEXxsDRzcD6f/view?usp=sharing
https://drive.google.com/file/d/16kraYnzP26h01yMjYuhbxa_RfABJdHpi/view?usp=sharing
*The demo shows a simulated baby cry triggering the complete response: cry detection, cradle rocking, LED activation, dashboard animation and the all-important DJ drop.*

Core project idea : https://drive.google.com/file/d/1O9afWyGnTwVjqKNL_oX_tFpobcRPQ82N/view?usp=sharing
---


# Team Contributions

* **Anvita M.K:** ESP32 programming, web dashboard, cry detection logic and system integration.
* **Hrudya Mohan:** Team coordination, hardware integration, cradle mechanism and system development. 

---

## The Inspiration

### അമ്മയ്ക്ക് പ്രാണ വേദന മകന് വീണ വായന

The project takes inspiration from the Malayalam proverb describing a child enjoying music while her mother suffers.

We already had the proverb.

We just decided to **take it literally, reverse the situation, and add an ESP32.**

Instead of a mother casually enjoying music while the baby cries, the **baby's cry becomes the DJ's cue.**

**Baby cries.
Cradle rocks.
Music drops.
Mom vibes.**

That's Tharattu.Drop().
