<h1 align="center">VPX GUI Tools for Linux</h1>
<p align="center">A <a href="https://github.com/vpinball/vpinball">VPinballX</a> GUI tool for launching VPX tables, editing INI settings, extracting VBS scripts, and more with "just a mouse click".</p>
<p align="center"><i>Now in C++ and powered by <a href="https://github.com/francisdb/vpxtool">vpxtool</a>! "Harder, better, faster, stronger"</i></p>

![image](https://github.com/user-attachments/assets/e8d96bd5-8c40-440b-80ea-721f30e67931)

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
>_This application expects you to have files inside table folders._
>
>/allTables/[table folder]/images | video | roms etc

   - Set tables folder; it will look recursively for .vpx files.
   - Set the absolute path of your VPX executable.
   - Set the absolute path of your VPinballX.ini.

6. **Select a table and press play!**

>[!NOTE]
>   - The **INI Editor** will open `~/vpinball/VPinballX.ini` by default if no table is selected.
>   - Selecting a table and pressing the **INI Editor** button will attempt to open the table's INI file if it exists or ask to create one.
>   - The **Extract VBS** button will extract the VBS script by default if none is found and open it with your preferred editor.
>   - The **Open Folder** button will open the selected table's directory or the table's root directory if no table is selected.

## **INI Editor**  

![image](https://github.com/user-attachments/assets/35dd68cf-e8b7-43ad-bc09-bea7c6d0082e)

## **Editor Features:**  
 - Explanations for each variable [**(need help!)**](https://github.com/surtarso/vpx-gui-tools/wiki/Adding-VPinballX.ini-Tooltips)
 - Category-split Sections 
 - Create and edit single table INI files or default vpx ini
 - Won’t override INI-code comments 

## Contribute

Contributions to this project are welcome! Feel free to snoop around, open issues, submit pull requests or help with the TODO list!

Any contribution to the INI Editor explanations project is highly appreciated! Check the [wiki](https://github.com/surtarso/vpx-gui-tools/wiki/Adding-VPinballX.ini-Tooltips).

Older versions in Python + Shell (discontinued) can be downloaded here: https://github.com/surtarso/vpx-frontend/releases
```
