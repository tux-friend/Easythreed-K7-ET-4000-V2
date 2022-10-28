# Mod for Easythreed K7 with ET-4000-V2 board (ATmega 1284p, 8-bit)

This mod is ported from https://github.com/sandor-ino/MARLIN-LABISTS-X1-SANDOR-MOD. It adds a SSD1306 Display + Encoder and ESP3D to the K7.
Video of Sandor's mod: https://www.youtube.com/watch?v=PBdJpTGQPfE. In contrast to sandor no hotbed is used, but could also be installed.

I also used the configuration file of https://github.com/schmttc/EasyThreeD-K7-STM32 for porting Marlin to K7.

## Custom Buttons Functions
This mod adds also the following button functions:
- HOME BUTTON
- Pressed for a short time:
Z axis in Home position and deactivates the Motor Stepper X and Y (manual flat calibration)
- RETRACT BUTTON
After heating extracts filament and automatically turn off extruder
- FEED BUTTON
After heating extrudation 20 mm of filament, pressing again repeats the thing. Press Home button to turn off extruder
Note: extruder turns off automatically after 3 minutes of inactivity.
- PLAY BUTTON
(1)- Pressed for a short time:
All X Y Z axles in Home position
If all axes are in Home moves the Z axis of 10 mm
Note: The program does not allow to exceed 90 mm, if it moves down for 10 mm
(2)- Long pressed:
Start Calibration procedure with placement of bed automatic with 5 points X - Y.
Press normally by next step 1 - 80 x 80, 2 - 80 x 20, 3 - 20 x 80, 4 - 20 x 20, 5 - 50 x 50
Complete procedure with all axes in Home
During procedure if you press long, you stop sequence, and axes go to home, 
use function only to rectify manual calibration to avoid of ruining bed


![alt text](https://github.com/sandor-ino/MARLIN-2.0.8-LABISTS-X1-SANDOR-MOD/blob/SANDOR-MOD-V1.1/w_PINOUT%20sandor%20mod%20V1.jpg)
