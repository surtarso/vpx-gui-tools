<h1 align="center">Simple VPX GUI Tools for Linux</h1>
<p align="center">A minimal GUI tool for launching VPX tables, editing INI settings, extracting VBS scripts and more with "just a mouse click", to be used with <a href="https://github.com/vpinball/vpinball">VPinballX</a>.</p>

| **[VPX GUI Launcher](vpx_launcher.sh)** | 
|-------------------------------------|
| ![image](https://github.com/user-attachments/assets/d424151f-ce64-46d9-a42a-8e12e884b896) | 

## **Launcher Features:** 
 - Lists tables and extra files 
 - Lists frontend media files
 - Extract and edit VBS scripts
 - Search and folder shortcuts
 - Show if files have diff*
<p></p><i>*Shows only INI diffs. Not yet implemented for .vbs scripts.</i></p>

>[!TIP]
>If you need help installing VPX check the [WIKI](https://github.com/surtarso/vpx-frontend/wiki/Visual-Pinball-X-on-Debian-Linux).
>
>If you need a dual monitor frontend 'as simple as possible', check [ASAPCabinetFE](https://github.com/surtarso/ASAPCabinetFE/).


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


   ![image](https://github.com/user-attachments/assets/f3d4ca55-99b2-4b96-be17-027d30a7e3c4)

>[!CAUTION]
>This script expects you to have a specific naming convention.
>
>/tables/[table folder]/[table file (brand YYYY)].vpx
>
>E.g. /tables/Mars (Original)/Mars (Stern 2024).vpx

   - Set table folder, it will look recursevely for .vpx files.
   - Set any initial args you may need. (or leave empty)
   - Set absolute path of your VPX executable.
   - Set any final args you may need. (or leave empty)
   - Set absolute path of VPinballX.ini
   - Set fallback editor for VBS files (any text editor in case there's no default one)
   - Set launcher window widht and height
   - Set you own media path for images and videos (from /tables/table_name/)
  
   The code will assemble a command line that looks like this:
   ```bash
   [initial arg] [VPX executable] `-play` [table dir] [selected.vpx] [final args]
   ```
   For example:
   ```bash
   DRI_PRIME=1 gamemoderun /home/tarso/Games/vpinball/build/VPinballX_GL -play /home/tarso/Games/vpinball/build/tables/tomandjerry/tomandjerry.vpx --fullscreen`
   ```

7. **Select a table and press play!**
>[!NOTE]
>   - The **INI Editor** will open `~/vpinball/VPinballX.ini` by default if no table is selected.
>   - Selecting a table and pressing the **INI Editor** button will attempt to open the table ini file if it exists or ask to create one.
>   - You can always manually search for the .ini you want to open.
>   - The **Extract VBS** will extract by default if no script is found and open the script with your preferred editor.
>   - The **Open Folder** button will open the selected table' dir or root dir if no table selected.
>   - To clear the **Search term** just press the button again.

# **Standalone INI Editor**  
| **[INI Editor](vpx_ini_editor.py)** |
|---------------------------------------|
| ![INI Editor](https://github.com/user-attachments/assets/010727f1-3e8a-4d2e-ac69-938a0b6bac7e) |

## **Editor Features:**  
 - Explanations for each variable **(need help!)** 
 - Category-split Sections 
 - Create and edit single table ini's 
 - Open any ini you want! 
 - Wont override code comments 

You can run the INI Editor separately without the launcher with:  
```bash
python3 vpx_ini_editor.py

python3 vpx_ini_editor.py /path/to/anyfile.ini
```
>[!NOTE]
It only requires **Tkinter** and does not depend on `yad`.

## Contribute

Contributions to this project are welcome! The code is highly commented, so feel free to snoop around, open issues or submit pull requests.
