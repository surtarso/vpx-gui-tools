<h1 align="center">Simple VPX Front-end GUI for Linux</h1>
<p align="center">A minimal frontend for launching VPX tables, editing INI settings and extracting VBS scripts with "just a mouse click", to be used with <a href="https://github.com/vpinball/vpinball">VPinballX_GL</a>*.</p>
<p align="center">Just <a href="https://github.com/surtarso/vpx-frontend/releases/">download a release file</a> and run anywhere you want.</p>
<p align="center"><i>*If you need help installing VPX check the <a href="https://github.com/surtarso/vpx-frontend/wiki/Visual-Pinball-X-on-Debian-Linux">wiki</a>.</i></p>

| **[VPX Launcher](vpx_launcher.sh)** | **[VPX INI Editor](vpx_ini_editor.py)** |
|-------------------------------------|---------------------------------------|
| Simple list-type VPX GUI with basic icon (wheel) support. | Simple GUI editor for `VPinballX.ini` or single table ini's. |
| ![Launcher](https://github.com/user-attachments/assets/1e5699e7-5c89-4cae-b386-a75a67408c0a) | ![INI Editor](https://github.com/user-attachments/assets/010727f1-3e8a-4d2e-ac69-938a0b6bac7e) |
| **Launcher Features:**  | **Editor Features:**  |
| - Simple & Fast (less than 100kb!)  | - Category-split Sections |
| - Edit INI files & extract VBS scripts  | - Explanations for each variable **(in progress, need help!)** |
| - Keyboard/Mouse Navigation | - Can edit single table ini's |

## **Installation & Usage**  

You can download a release file here: https://github.com/surtarso/vpx-frontend/releases or follow the instructions below.

1. **Download the repository** to any folder of your choice.
   ```bash
   git clone https://github.com/surtarso/vpx-frontend.git
   ```
   
3. **Install dependencies:**  
   ```bash
   sudo apt install yad python3 python3-tk
   ```  
   > *If you don’t plan to use the launcher, you can skip installing `yad`. Move to [Standalone INI Editor](#standalone-ini-editor)*

4. **Run the launcher:**  
   ```bash
   ./vpx_launcher.sh
   ```
   
5. **Configure paths and args**
   
   ![image](https://github.com/user-attachments/assets/a17f6550-7e38-4e07-852a-b1b041ecc91c)

   - Set table folder, it will look recursevely for .vpx files.
   - Set any initial args you may need. (or leave empty)
   - Set absolute path of your VPX executable.
   - Set any final args you may need. (or leave empty)
   - Set absolute path of VPinballX.ini
   - Set fallback editor for VBS files (any text editor in case there's no default one)
  
   The code will assemble a command line that looks like this:
   ```bash
   [initial arg] [VPX executable] `-play` [table dir] [selected.vpx] [final args]
   ```
   For example:
   ```bash
   DRI_PRIME=1 gamemoderun /home/tarso/Games/vpinball/build/VPinballX_GL -play /home/tarso/Games/vpinball/build/tables/tomandjerry/tomandjerry.vpx --fullscreen`
   ```

6. **Select a table and press play!**

#### Obs: 
- The **INI Editor** will open `~/vpinball/VPinballX.ini` by default if no table is selected.
- Selecting a table and pressing the **INI Editor** button will attempt to open the table ini file if it exists or ask to create one.
- You can always manually search for the .ini you want to open.

## **Standalone INI Editor**  
You can run the INI Editor separately without the launcher with:  
```bash
python3 vpx_ini_editor.py

python3 vpx_ini_editor.py /path/to/anyfile.ini
```  
It only requires **Tkinter** and does not depend on `yad`.

## Contribute

Contributions to this project are welcome! The code is highly commented, so feel free to snoop around, open issues or submit pull requests.
