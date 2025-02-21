<h1 align="center">Simple VPX Front-end GUI for Linux</h1>
<p align="center">A minimal frontend for launching VPX tables and editing INI settings.</p>

| **[VPX Launcher](vpx_launcher.sh)** | **[VPX INI Editor](vpx_ini_editor.py)** |
|-------------------------------------|---------------------------------------|
| Simple list-type VPX GUI with basic icon (wheel) support. | Simple GUI editor for `VPinballX.ini` or single table ini's. |
| ![Launcher](https://github.com/user-attachments/assets/eb60fa04-8e4d-4283-8e25-b0f3875d2ada) | ![INI Editor](https://github.com/user-attachments/assets/e7c5fa40-42ae-4413-bf3a-1bf3e0cef7e2) |
| **Launcher Features:**  | **Editor Features:**  |
| - Simple & Fast  | - Category-split Sections |
| - Configurable  | - Explanations for each variable **(in progress, need help!)** |
| - Keyboard/Mouse Navigation | - Can edit single table ini's |

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
