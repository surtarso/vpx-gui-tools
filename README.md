<h1 align="center">VPX GUI Tools for Linux</h1>
<p align="center">A GUI tool for launching VPX tables, editing INI settings, extracting VBS scripts, and more with "just a mouse click", to be used with <a href="https://github.com/vpinball/vpinball">VPinballX</a>.</p>

| **[VPX GUI Tools Launcher](VPXGUITools)** | 
|-------------------------------------|
| ![image](https://github.com/user-attachments/assets/d424151f-ce64-46d9-a42a-8e12e884b896) | 

<p align="center"><i>This tool is part of the <a href="https://github.com/surtarso/ASAPCabinetFE">ASAPCabinetFE</a> ecosystem.</i></p>

## **Launcher Features:** 
 - Lists table info and extra files 
 - Lists frontend media files
 - Extract and edit VBS scripts
 - Show if INI/VBS files have diff
 - Search and folder shortcuts

>[!TIP]
>If you need help installing VPX, check the [WIKI](https://github.com/surtarso/vpx-frontend/wiki/Visual-Pinball-X-on-Debian-Linux).
>
>If you need a dual monitor frontend "as simple as possible", check [ASAPCabinetFE](https://github.com/surtarso/ASAPCabinetFE/).

## **Installation & Usage**  

1. **Download the repository** to any folder of your choice.
   ```bash
   git clone --recurse-submodules https://github.com/surtarso/vpx-frontend.git
   ```
   
2. **Install dependencies:**  
   ```bash
   sudo apt install build-essential cmake libsdl2-dev
   ```

3. **Build the application**
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make
   ```

4. **Run the launcher:**  
   ```bash
   ./VPXGUITools
   ```
   
5. **Configure paths and args**

>[!CAUTION]
>_This application expects you to have a specific naming convention._
>
>/tables/[table folder]/[table file (xxxxx YYYY)].vpx
>
>**E.g.** /tables/Mars (Original)/Mars (Stern 2024).vpx

   - Set table folder; it will look recursively for .vpx files.
   - Set any initial args you may need (or leave empty).
   - Set the absolute path of your VPX executable.
   - Set any final args you may need (or leave empty).
   - Set the absolute path of your VPinballX.ini.
   - Set the fallback editor for VBS files (any text editor in case there's no default one).
   - Set the launcher window width and height.
   - Set your own media path for images and videos (from /tables/table_name/).
  
   The application will assemble a command line that looks like this:
   ```bash
   [initial arg] [VPX executable] `-play` [table dir] [selected.vpx] [final args]
   ```
   For example:
   ```bash
   DRI_PRIME=1 gamemoderun /home/tarso/Games/vpinball/build/VPinballX_GL -play /home/tarso/Games/vpinball/build/tables/tomandjerry/tomandjerry.vpx --fullscreen
   ```

6. **Select a table and press play!**
>[!NOTE]
>   - The **INI Editor** will open `~/vpinball/VPinballX.ini` by default if no table is selected.
>   - Selecting a table and pressing the **INI Editor** button will attempt to open the table's INI file if it exists or ask to create one.
>   - The **Extract VBS** button will extract the VBS script by default if none is found and open it with your preferred editor.
>   - The **Open Folder** button will open the selected table's directory or the root directory if no table is selected.

## **INI Editor**  
| **[INI Editor (Integrated)](VPXGUITools)** |
|---------------------------------------|
| ![image](https://github.com/user-attachments/assets/b11bbccf-2844-4c14-b93f-0049c8b97717) |

## **Editor Features:**  
 - Explanations for each variable [**(need help!)**](https://github.com/surtarso/vpx-gui-tools/wiki/Adding-VPinballX.ini-Tooltips)
 - Category-split Sections 
 - Create and edit single table INI files 
 - Won’t override INI-code comments 

## Contribute

Contributions to this project are welcome! Feel free to snoop around, open issues, submit pull requests or help with the TODO list!

Any contribution to the INI Editor explanations project is highly appreciated! Check the [wiki](https://github.com/surtarso/vpx-gui-tools/wiki/Adding-VPinballX.ini-Tooltips).

Older versions in Python + Shell (discontinued) can be downloaded here: https://github.com/surtarso/vpx-frontend/releases
```