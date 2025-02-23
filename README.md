<h1 align="center">Simple VPX Front-end GUI for Linux</h1>
<p align="center">A minimal frontend for launching VPX tables and editing INI settings.</p>
<p align="center"><a href="https://github.com/surtarso/vpx-frontend/releases/"> Download a release file</a> and run anywhere you want.</p>

| **[VPX Launcher](vpx_launcher.sh)** | **[VPX INI Editor](vpx_ini_editor.py)** |
|-------------------------------------|---------------------------------------|
| Simple list-type VPX GUI with basic icon (wheel) support. | Simple GUI editor for `VPinballX.ini` or single table ini's. |
| ![Launcher](https://github.com/user-attachments/assets/18e11743-1ec5-4680-8ad7-c25687db0811) | ![INI Editor](https://github.com/user-attachments/assets/010727f1-3e8a-4d2e-ac69-938a0b6bac7e) |
| **Launcher Features:**  | **Editor Features:**  |
| - Simple & Fast (less than 100kb!)  | - Category-split Sections |
| - Edit INI files & extract VBS scripts  | - Explanations for each variable **(in progress, need help!)** |
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
   
4. **Configure paths and args**
   
   ![image](https://github.com/user-attachments/assets/4eea1828-c3f2-431e-b09b-951922bc9751)

   - Set table folder, it will look recursevely for .vpx files.
   - Set any initial args you may need. (or leave empty)
   - Set absolute path of your VPX executable.
   - Set any final args you may need. (or leave empty)
   - Set absolute path of VPinballX.ini
  
   The code will assemble a command line that looks like this:
   ```bash
   [initial arg] [VPX executable] `-play` [table dir] [selected.vpx] [final args]
   ```
   For example:
   ```bash
   DRI_PRIME=1 gamemoderun /home/tarso/Games/vpinball/build/VPinballX_GL -play /home/tarso/Games/vpinball/build/tables/tomandjerry/tomandjerry.vpx --fullscreen`
   ```

5. **Select a table and press play!**

#### Obs: 
- The **INI Editor** will open `~/vpinball/VPinballX.ini` by default if no table is selected.
- Selecting a table and pressing the **INI Editor** button will attempt to open the table ini file if it exists.
- You can always manually search for the .ini you want to open.

## **Standalone INI Editor**  
You can run the INI Editor separately without the launcher with:  
```bash
python3 vpx_ini_editor.py

python3 vpx_ini_editor.py /path/to/anyfile.ini
```  
It only requires **Tkinter** and does not depend on `yad`.
