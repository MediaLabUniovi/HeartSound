# HeartSound

A project developed in collaboration with the **Medical University of Sofia** to create an educational device that simulates the heart sounds heard with a stethoscope at different points of the body and from various types of hearts.

The system combines **3D printing, electronics, and ESP32 programming**, allowing medical students and professionals to practice auscultation in an interactive way.

---

## Functionality

* **3D Printed Heart**:

  * Contains an **ESP32** and a **DFPlayer Mini** that reproduces heart sounds.
  * Uses **5 relays** to route audio to different speakers (each representing an auscultation point).
  * Each speaker has a **rear push button**: the sound is only played while the button is pressed, simulating the pressure of a stethoscope.

* **Remote Control** (ESP32 + ESP-NOW):

  * Communicates wirelessly with the heart using **WiFi ESP-NOW** (no router required).
  * Includes a **potentiometer** that adjusts between 6 levels (e.g., different intensities or sound variations).
  * A **6-LED strip** indicates the current level in real time.

---

## Components

| Component            | Description                              | Quantity                                |
| -------------------- | ---------------------------------------- | --------------------------------------- |
| ESP32 Dev Module     | Microcontroller with Wi-Fi and Bluetooth | 2 (one in the heart, one in the remote) |
| DFPlayer Mini        | MP3 playback module with SD card reader  | 1                                       |
| Relays               | Switch audio routing between speakers    | 5                                       |
| Speakers             | 2 W, 8 Ω                                 | 5                                       |
| Push Buttons         | Momentary switches (internal pull-up)    | 5                                       |
| Potentiometer        | Analog control for level selection       | 1                                       |
| LED Strip (6 LEDs)   | Visual indicator of the current level    | 1                                       |
| 3D Printed Enclosure | Anatomical heart housing                 | 1                                       |

---

## Schematics

<img width="1527" height="1079" alt="image" src="https://github.com/user-attachments/assets/c6579502-7597-46c8-8a32-b9c7d966c85e" />

<img width="1526" height="1072" alt="image" src="https://github.com/user-attachments/assets/8ace285f-435e-4f7d-9b68-13b5cc935aef" />

---

## How It Works

1. Press a **speaker button** → the system activates the corresponding relay.
2. The **DFPlayer Mini** plays the audio associated with that auscultation point.
3. The **remote control** adjusts the current level using the potentiometer.
4. The selected level is displayed on the LEDs and transmitted to the heart.
5. The heart adapts the sound according to the selected level.

---

## Setup and Usage

1. Upload the firmware to both **ESP32 modules** (heart and remote).
2. Copy the heart sound files onto the SD card for the DFPlayer Mini.
3. Assemble the components inside the 3D printed heart and remote control.
4. Power on both devices → the remote automatically connects to the heart via ESP-NOW.
5. Ready for auscultation practice.

---

## Applications

* Medical education in universities.
* Realistic simulation of different **heart sounds**.
* Interactive training device for cardiology.

---


