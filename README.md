# Simple VPX Frontend GUI for Linux
A minimal frontend for launching VPX tables and editing INI settings.
Uses YAD (for launcher) and Python 3 (for ini editor)

## [VPX Launcher](vpx_launcher.sh)
Simple list-type VPX table launcher with basic icon (wheel) support.

![image](https://github.com/user-attachments/assets/eb60fa04-8e4d-4283-8e25-b0f3875d2ada)

### Features:
- Fast
- Configurable

## [VPX INI Editor](vpx_ini_editor.py)
Simple GUI editor for VPinballX.ini

![image](https://github.com/user-attachments/assets/e7c5fa40-42ae-4413-bf3a-1bf3e0cef7e2)

### Features:
- Category-split settings
- Explanations for each variable **(in progress, need help!)**

## **Installation & Usage**  

1. **Download the repository** to any folder of your choice.  
2. **Install dependencies:**  
   ```bash
   sudo apt install yad python3 python3-tk
   ```  
   > *If you don’t plan to use the launcher, you can skip installing `yad`.*  
3. **Run the launcher:**  
   ```bash
   ./vpx_launcher.sh
   ```  
   Follow the on-screen instructions.  

## **Standalone INI Editor**  
You can run the INI Editor separately without the launcher:  
```bash
python3 vpx_ini_editor.py
```  
It only requires **Tkinter** and does not depend on `yad`.
