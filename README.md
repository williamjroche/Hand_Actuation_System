# Hand Actuation System


## Overview:
  The Hand Actuation System is a cost effective robotic hand designed for dexterous movement and strength manipulation. This system integrates mechanical design, embedded electronics, a custom control program, and a computer vision based hand tracking software.

<p align="center">
  <img width="260" height="540" alt="image" src="https://github.com/user-attachments/assets/b541f0ce-d563-47e8-be7f-3430da4784af" />
  <img width="394" height="412" alt="image" src="https://github.com/user-attachments/assets/42480471-0b95-4630-ac63-90ac728ea99d" />
</p>

*Note: this test uses an open source CAD model found here: https://www.thingiverse.com/thing:2269115*

## Features:
- 3D printed structure
- Servo actuated joints
- Wrist manipualtion via NEMA 17 stepper motor (*Not implemented yet*)
- Arduino Uno microcontroller (AVR based)
- Custom control program (AVR C program)
  - Button to open hand, button to close hand

## Hand Design Roadmap:
- Design and test finger joints
- Design and test palm designs
- Implement servo control (MG90S & MG996R servos)
- Test finger actuation
- Design and test wrist joint actuation
- Design and test forearm designs
- Redesign and retest

## Computer Vision Based Tracking system
- Computer vision based hand tracking for hand control
- OpenCV for live camera feed and visuals
- MediaPipe Hand Tracking model (hand_tracking.task file included)
- Hard coded gesture recognition (safer for servo motors)

## Demo Video:
- Robot hand:
  - https://youtube.com/shorts/E759LSxpx3w?feature=share
- Hand Tracking:
  - https://youtu.be/f70n2JQ5Q2o
