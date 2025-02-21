# Simple VPX Frontend GUI for Linux
A minimal frontend for launching VPX tables and editing INI settings.
Uses YAD (for launcher) and Python 3 (for ini editor)

## [VPX Launcher](vpx_launcher.sh)
Simple list-type VPX table launcher with basic icon (wheel) support.

![image](https://github.com/user-attachments/assets/09070eae-b26a-4485-95a5-6ff76f4740b4)

### Features:
- Fast
- Configurable

## [VPX INI Editor](vpx_ini_editor.py)
Simple GUI editor for VPinballX.ini

![image](https://github.com/user-attachments/assets/e7c5fa40-42ae-4413-bf3a-1bf3e0cef7e2)

### Features:
- Category-split settings
- Explanations for each variable (in progress, need help!)

# Usage:
- Download this repository to any folder you want.
- Install dependencies. `sudo apt install yad python3 python3-tk`
- Run `vpx_launcher.sh`
- Follow on-screen instructions.

You can also run the INI Editor standalone by running:
`python3 vpx_ini_editor.py`
It uses tkinter only. If you dont want to use the launcher you can skip installing 'Yad'.
