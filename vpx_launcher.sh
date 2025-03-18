#!/bin/bash
# VPX Table Launcher
# A simple GUI launcher for Visual Pinball X tables
# Tarso Galvão - 2025
# Dependencies: yad 0.40.0 (GTK+ 3.24.38)

# TODO:
# someshow diff .vbs with .vbs inside .vpx
# refresh button when files are changed
# avoid reloading the UI all the time.

# add buttons to extract media (image or video)
# - no selection: runs the full script 
# - table selected: --force runs only table selected
# - ask if user wants table or backglass (or both)
# this will use code from the art_generators script.

# Suppress GTK warnings (?)
export NO_AT_BRIDGE=1

# Check for dependencies
if ! command -v yad &>/dev/null; then
    echo "Error: 'yad' is not installed. Please install 'yad' and try again."
    echo "For Debian-based systems, run: sudo apt install yad"
    exit 1
fi

# Check for graphical environment
if [ -z "$DISPLAY" ]; then
    echo "This script requires a graphical environment!"
    exit 1
fi

## --------------------- CONFIGURATION ---------------------
# Config file path
#LOGFILE="$HOME/.vpx-gui-tools/erros.log"
CONFIG_FILE="settings.ini"
# DEFAULT_ICON="default_icon.ico"  # Default icon for list view
# HELP_TEXT="Files: missing, present, modified - Media: present, missing"
# HELP_TEXT="   |   Color codes - Extra Files: <span foreground='gray'>missing</span>, \
# <span foreground='white'>present</span>, \
# <span foreground='yellow'>modified</span>   |   \
# Front-End Media: <span foreground='green'>present</span>, \
# <span foreground='red'>missing</span>"
HELP_TEXT=""
COLS=13 # number of columns on the list

# Load or create the config file with default values
if [ ! -f "$CONFIG_FILE" ]; then
    cat <<EOF > settings.ini
[LauncherSettings]
TablesDir = $HOME/Games/VPX_tables/
StartArgs = 
CommandToRun = $HOME/Games/vpinball/build/VPinballX_GL
EndArgs = 
VPinballXIni = $HOME/.vpinball/VPinballX.ini
FallbackEditor = code
WindowWidth = 1024
WindowHeight = 768
WheelImage = /images/wheel.png
TableImage = /images/table.png
BackglassImage = /images/backglass.png
MarqueeImage = /images/marquee.png
TableVideo = /video/table.mp4
BackglassVideo = /video/backglass.mp4
DMDVideo = /video/dmd.mp4
ROMPath = /pinmame/roms
AltSoundPath = /pinmame/altsound
AltColorPath = /pinmame/AltColor
MusicPath = /music
PUPPackPath = /pupvideos
EOF
fi
load_config() {
    if [ -f "$CONFIG_FILE" ]; then
        # Read each line and ignore section headers ([LauncherSettings])
        while IFS='=' read -r key value; do
            key=$(echo "$key" | tr -d ' ')  # Remove spaces in keys
            value=$(echo "$value" | sed 's/^ *//;s/ *$//')  # Trim spaces in values
            eval "$key=\"$value\""
            echo "$key=\"$value\"" >/dev/null 2>&1
        done < <(grep -v '^\[.*\]' "$CONFIG_FILE")
    else
        echo "Error: Config file not found!"
        exit 1
    fi
}

## ----------------------- FUNCTIONS -----------------------
# Function to show an error dialog and return the chosen action
handle_error() {
    yad --title="Error" --text="$1" 2>/dev/null \
        --form --width=400 --height=150 \
        --buttons-layout=center \
        --button="Settings:1" --button="Exit:252"
    
    local exit_code=$?
    if [[ $exit_code -eq 1 ]]; then
        eval "./vpx_config settings.ini"
        load_config
    else
        exit 0
    fi
}

## --------------- INI SETTINGS (Standalone) -----------------

# Function to open the INI editor (either default or per table)
open_ini_settings() {
    # Check if a parameter (INI file) was passed
    if [ -n "$1" ]; then
        # If so, pass it to the Python script
        eval "./vpx_config" "\"$1\""
    else
        # Otherwise, open the default INI editor
        eval "./vpx_config"
    fi
}

# Function to create a missing INI file for a table
create_table_ini() {
    local ini_file="$1"  # INI file path

    # If the file exists, no need to do anything
    if [[ -f "$ini_file" ]]; then
        return 0
    fi

    # Prompt the user to create a missing INI file
    if yad --title="Missing INI File" \
            --text="The INI file for this table does not exist.\nDo you want to create it?" \
            --width=400 --height=150 --buttons-layout=center \
            --button="Yes:0" --button="No:1" 2>/dev/null; then

        # User selected "Yes" → Copy the default VPinballXIni to the new location
        cp "$VPinballXIni" "$ini_file"

        # Inform the user that the new INI file was created
        yad --info --title="INI File Created" \
            --text="INI file created: $ini_file" \
            --buttons-layout=center --button="OK:0" \
            --width=300 --height=100 2>/dev/null
        return 0  # Success
    fi

    return 1  # User chose not to create the file
}

# Function to launch the INI editor (either default or per table)
launch_ini_editor() {
    local ini_file="$1"  # INI file path as the argument

    # Check if a table was selected
    if [[ -z "$SELECTED_TABLE" ]]; then
        # No table selected, open default INI settings
        if [[ ! -f "$VPinballXIni" ]]; then
            # VPinballX.ini not found, show error and ask user to set the correct path
            handle_error "VPinballX.ini not found!\nPlease set the correct path in the settings."
        else
            # Open the default INI settings
            open_ini_settings "$VPinballXIni"
        fi
    else
        # Table selected, ensure the INI file exists (create if missing)
        create_table_ini "$ini_file" || return 1

        # Open the table-specific INI file in the settings editor
        open_ini_settings "$ini_file"
    fi
}

## ------------------ VBS SCRIPTS LOGIC ----------------

handle_vbs_scripts() {
    local vbs_script="$1"

    if [[ -z "$SELECTED_TABLE" ]]; then
        # Show the error dialog and capture the exit code
        handle_error "No table selected!\nPlease select a table before extracting."
    else
        # Check if the VBS file exists
        if [[ -f "$vbs_script" ]]; then
            # Try to open the file with xdg-open
            if ! xdg-open "$vbs_script" 2>/dev/null; then
                # If xdg-open fails (no default handler), fallback to a text editor
                handle_error "No default handler found, opening with fallback editor"
                $FallbackEditor "$vbs_script" &
            fi
        else
            # If the VBS file doesn't exist, extract it
            eval "\"$CommandToRun\" -ExtractVBS \"$SELECTED_FILE\"" &
            wait $!
            
            # After extraction, try opening the new VBS file with xdg-open
            if ! xdg-open "$vbs_script" 2>/dev/null; then
                # If xdg-open fails (no default handler), fallback to a text editor
                handle_error "No default handler found, opening with fallback editor"
                $FallbackEditor "$vbs_script" &
            fi
        fi
    fi
}

## -------------------- SEARCH LOGIC --------------------

handle_search_query() {
    # Open dialog box
    local SEARCH_QUERY
    SEARCH_QUERY=$(yad --entry --title="Search Tables" \
                    --text="Enter table name filter:" \
                    --width=300 --height=100 2>/dev/null)
                    
    if [[ -z "$SEARCH_QUERY" ]]; then
        # Handle the case where the user presses Enter without typing anything.
        handle_error "You need to enter a search term."

    elif [[ -n "$SEARCH_QUERY" ]]; then
        # Handle the case where the user provides a search term.
        CURRENT_SEARCH="$SEARCH_QUERY"
        FILTERED_FILE_LIST=()
        for (( i=0; i<${#FILE_LIST[@]}; i+=COLS )); do
            local YEAR="${FILE_LIST[i]}"
            local BRAND="${FILE_LIST[i+1]}"
            local NAME="${FILE_LIST[i+2]}"
            local EXTRA="${FILE_LIST[i+3]}"
            local ROM="${FILE_LIST[i+4]}"
            local UDMD="${FILE_LIST[i+5]}"
            local ALTS="${FILE_LIST[i+6]}"
            local ALTC="${FILE_LIST[i+7]}"
            local MUSIC="${FILE_LIST[i+8]}"
            local PUP="${FILE_LIST[i+9]}"
            local IMAGES="${FILE_LIST[i+10]}"
            local VIDEOS="${FILE_LIST[i+11]}"
            local TFILE="${FILE_LIST[i+12]}"
            if echo "$NAME" | grep -iq "$SEARCH_QUERY"; then
                FILTERED_FILE_LIST+=("$YEAR" "$BRAND" "$NAME" "$EXTRA" "$ROM" \
                "$UDMD" "$ALTS" "$ALTC" "$MUSIC" "$PUP" "$IMAGES" "$VIDEOS" "$TFILE")
            fi
        done

        # If no results, notify and revert to the original list.
        if [ ${#FILTERED_FILE_LIST[@]} -eq 0 ]; then
            handle_error "No tables found matching '$SEARCH_QUERY'"
            CURRENT_SEARCH=""
            FILTERED_FILE_LIST=("${FILE_LIST[@]}")
        fi
    fi
}

## ----------------- PRE-LAUNCH CHECKS ------------------
# Validate .vpx files and executable before launch
if ! find "$TablesDir" -type f -name "*.vpx" | grep -q .; then
    handle_error "No .vpx files found in $TablesDir."
    [[ $? -eq 252 ]] && exit 0
fi

if [[ ! -x "$CommandToRun" ]]; then
    handle_error "The executable '$CommandToRun' does not exist or is not executable!"
    [[ $? -eq 252 ]] && exit 0
fi

## --------------------- MAIN LOOP ---------------------
while true; do
    FILE_LIST=() # Prepare the table list for the launcher
    declare -A FILE_MAP # Create an associative array to map table names to file paths

    # Loading screen
    LOADING_PID=
    (
        for i in {5..100..5}; do
            echo "$i"
            sleep 0.1
        done
    ) | yad --progress --title="Loading" --text=" Reading files..." \
        --width=300 --height=70 --percentage=0 --auto-close \
        --undecorated --no-buttons 2>/dev/null &
    LOADING_PID=$!

    # Read .vpx files and prepare the menu
    while IFS= read -r FILE; do
        
        VPX_FOLDER=$(dirname "$FILE")
        BASENAME=$(basename "$FILE" .vpx | sed 's/&/\&amp;/g; s/'"'"'/\&apos;/g') # Strip the extension
        # Extract name (everything before the first opening parenthesis)
        T_NAME=$(echo "$BASENAME" | sed -E 's/\s*\(.*\)//')
        # T_NAME=$(echo -e "$BASENAME" | sed -E 's/\s*\(.*\)//') # Yad can't handle special characters?
        # Extract brand (text inside parentheses before the year)
        T_BRAND=$(echo -e "$BASENAME" | sed -E 's/.*\((.*) [0-9]{4}\).*/\1/')
        # Extract year (last four-digit number in parentheses)
        T_YEAR=$(echo -e "$BASENAME" | sed -E 's/.*([0-9]{4}).*/\1/')

        # ------------------------------------Check for INI, VBS(?), directb2s
        set_extras_status() {
            local base_name="$1"
            local extension="$2"
            local status_var="$3"
            local display_name
            # Set display name: "B2s" for directb2s, capitalized extension otherwise
            if [[ "$extension" == "directb2s" ]]; then
                display_name="B2s"
            else
                display_name="${extension^}"
            fi
            local found_file=$(find "$VPX_FOLDER" -iname "${base_name}.${extension}" -print -quit)
            if [[ -f "$found_file" ]]; then
                # File exists: set status to white
                eval "$status_var='<span foreground=\"white\">${display_name} </span>'"
                # Special case for INI: check if it differs from VPinballXIni
                if [[ "$extension" == "ini" && -f "$VPinballXIni" ]]; then
                    if [[ "$(diff "$found_file" "$VPinballXIni")" != "" ]]; then
                        eval "$status_var='<span foreground=\"yellow\">${display_name} </span>'"
                    fi
                fi
            else
                # File doesn't exist: gray for INI/VBS, red for B2S
                if [[ "$extension" == "directb2s" ]]; then
                    eval "$status_var='<span foreground=\"red\">${display_name}</span>'"
                else
                    eval "$status_var='<span foreground=\"gray\">${display_name} </span>'"
                fi
            fi
        }

        # Set statuses for INI, VBS, and B2S files
        base_name=$(basename "$FILE" .vpx)
        set_extras_status "$base_name" "ini" "INI_STATUS"
        set_extras_status "$base_name" "vbs" "VBS_STATUS"
        set_extras_status "$base_name" "directb2s" "DIRECTB2S_STATUS"

        # --------------------------------------------Check for images
        image_types=("Wheel" "Table" "B2S" "Marquee")
        image_vars=("WheelImage" "TableImage" "BackglassImage" "MarqueeImage")
        status_parts=()

        for i in "${!image_types[@]}"; do
            type="${image_types[$i]}"
            image_var="${image_vars[$i]}"
            found_file=$(find "$VPX_FOLDER" -iname "$(basename "${!image_var}")" -print -quit)
            if [[ -f "$found_file" ]]; then
                color="green"
            else
                color="red"
            fi
            # Add comma for all except the last item
            if [[ $i -lt $((${#image_types[@]} - 1)) ]]; then
                status_parts+=("<span foreground='${color}'>${type},</span>")
            else
                status_parts+=("<span foreground='${color}'>${type}</span>")
            fi
        done

        IMAGES_STATUS="🖼️ [${status_parts[*]}] "

        # -----------------------------------------------Check for videos
        # Check for videos
        video_types=("Table" "B2S" "DMD")
        video_vars=("TableVideo" "BackglassVideo" "DMDVideo")
        video_status_parts=()

        for i in "${!video_types[@]}"; do
            type="${video_types[$i]}"
            video_var="${video_vars[$i]}"
            found_file=$(find "$VPX_FOLDER" -iname "$(basename "${!video_var}")" -print -quit)
            if [[ -f "$found_file" ]]; then
                color="green"
            else
                color="red"
            fi
            # Add comma for all except the last item
            if [[ $i -lt $((${#video_types[@]} - 1)) ]]; then
                video_status_parts+=("<span foreground='${color}'>${type},</span>")
            else
                video_status_parts+=("<span foreground='${color}'>${type}</span>")
            fi
        done

        VIDEOS_STATUS="🎬 [${video_status_parts[*]}]"

        # -------------------------------Check for Music, ROM and AltSound/Color folders
        RomStatus="<span foreground='red'>✗</span>"
        AltSoundStatus="<span foreground='gray'>–</span>"
        AltColorStatus="<span foreground='gray'>–</span>"
        MusicStatus="<span foreground='gray'>–</span>"
        UltraDMDStatus="<span foreground='gray'>–</span>"
        PUPPackStatus="<span foreground='gray'>–</span>"
        RomName="<span foreground='gray'>none</span>"

        # Function to set status for components with path variables
        set_status() {
            local comp=$1        # Component name (e.g., SND, CRZ)
            local emoji=$2       # Emoji to set if directory exists
            local path_var="${comp}Path"
            local status_var="${comp}Status"
            if [[ -n "${!path_var}" && -n "$VPX_FOLDER" ]]; then
                local local_dir="$VPX_FOLDER${!path_var}"
                if [[ -d "$local_dir" ]]; then
                    eval "$status_var=\"$emoji\""
                fi
            fi
        }

        # Set statuses for components using the function
        set_status "AltSound" "🎵"
        set_status "AltColor" "🌈"
        set_status "Music" "📀"
        set_status "PUPPack" "📺"

        # Handle ROM separately due to RomName logic
        if [[ -n "$ROMPath" && -n "$VPX_FOLDER" ]]; then
            local_rom_dir="$VPX_FOLDER$ROMPath"
            if [[ -d "$local_rom_dir" ]]; then
                RomStatus="💾"
                zip_file=$(find "$local_rom_dir" -maxdepth 1 -type f -name "*.zip" | head -n 1)
                if [[ -n "$zip_file" ]]; then
                    RomName=$(basename "$zip_file" .zip)
                fi
            fi
        fi

        # Handle UDMD separately due to its unique directory search
        if [[ -n "$VPX_FOLDER" ]]; then
            local_udmd_dir=$(find "$VPX_FOLDER" -maxdepth 1 -type d -name "*.UltraDMD" | head -n 1)
            if [[ -d "$local_udmd_dir" ]]; then
                UltraDMDStatus="🎞️"
            fi
        fi

        # --------------Create the array of columns------------
        FILE_LIST+=("$T_YEAR" "$T_BRAND" "$T_NAME" \
                    "$INI_STATUS $VBS_STATUS $DIRECTB2S_STATUS" \
                    "$RomStatus $RomName" \
                    "$UltraDMDStatus" "$AltSoundStatus" "$AltColorStatus" "$PUPPackStatus" \
                    "$MusicStatus" "$IMAGES_STATUS" "$VIDEOS_STATUS" \
                    "$BASENAME") # Add to the list

        FILE_MAP["$BASENAME"]="$FILE" # Map table name to file path
        done < <(find "$TablesDir" -type f -name "*.vpx" | sort)

    TABLE_NUM=0 # Initialize table(s) found count

    # Convert the array to a string for yad
    if [ ${#FILTERED_FILE_LIST[@]} -gt 0 ]; then
        # show user search-filtered list if available
        FILE_LIST_STR=$(printf "%s\n" "${FILTERED_FILE_LIST[@]}")
        TABLE_NUM=$(( ${#FILTERED_FILE_LIST[@]} / COLS )) # Adjust for the extra columns
    else
        # show all tables
        FILE_LIST_STR=$(printf "%s\n" "${FILE_LIST[@]}")
        TABLE_NUM=$(( ${#FILE_LIST[@]} / COLS )) # Adjust for the extra columns
    fi

    kill $LOADING_PID 2>/dev/null

    #Show launcher menu (list view)
    #Keep filename as last and ajust COLS variable with the number of total columns
        SELECTED_TABLE=$(LC_ALL=en_US.UTF-8 yad --list --title="VPX GUI Tools" \
            --text="Table(s) found: $TABLE_NUM $HELP_TEXT" \
            --width="$WindowWidth" --height="$WindowHeight" --search=true \
            --button="⚙!!Settings :1" \
            --button="INI Editor!!Create and edit INI files:2" \
            --button="Extract VBS!!Extract and edit VBS scripts:10" \
            --button="📂!!Open a table folder :20" \
            --button="🔍!!Filter tables:30" \
            --button="🕹️!!Launch selected table :0" \
            --button="🚪!!Exit :252" \
            --buttons-layout=center \
            --column="Year:NUM" --column="Author" --column="Title" \
            --column="Extra Files" --column="ROM" --column="uDMD" --column="AltS" --column="AltC"\
            --column="Pup" --column="Mus" --column="Images" --column="Videos" \
            --column="Filename" --markup-columns=4 \
            --hide-column="$COLS" --print-column="$COLS" <<< "$FILE_LIST_STR" 2>/dev/null)
    
    EXIT_CODE=$?

    # Strip pipes from selection (if any)
    SELECTED_TABLE="${SELECTED_TABLE#|}"
    SELECTED_TABLE="${SELECTED_TABLE%|}"

    # Get full file path if a table is selected
    [[ -v FILE_MAP["$SELECTED_TABLE"] ]] && SELECTED_FILE="${FILE_MAP[$SELECTED_TABLE]}"

    case $EXIT_CODE in
        252) exit 0 ;;  # User canceled
        1)  
            # ⚙ Settings
            eval "./vpx_config settings.ini"
            load_config
            ;;  
        2)  
            # Open INI Editor (default or per table)
            INI_FILE="${SELECTED_FILE%.vpx}.ini"
            launch_ini_editor "$INI_FILE"
            ;;
        10) 
            # Extract or open VBS script
            VBS_FILE="${SELECTED_FILE%.vpx}.vbs"
            handle_vbs_scripts "$VBS_FILE"
            ;;
        20)
            # Open the tables folder if no table selected
            if [[ -z "$SELECTED_TABLE" ]]; then xdg-open "$TablesDir" >/dev/null 2>&1; \
            else xdg-open "$(dirname "$SELECTED_FILE")" >/dev/null 2>&1; fi
            continue
            ;;
        30)
            # Search filter or show all tables
            if [[ -n "$CURRENT_SEARCH" ]]; then
                CURRENT_SEARCH=""
                FILTERED_FILE_LIST=("${FILE_LIST[@]}")
            else
                handle_search_query
            fi
            continue
            ;; 
        *)  
            if [[ -z "$SELECTED_TABLE" ]]; then
                handle_error "No table selected!\nPlease select a table before launching."
            else
                # Normal launch (no terminal)
                eval "$StartArgs \"$CommandToRun\" -play \"$SELECTED_FILE\" $EndArgs" &
                wait $!
            fi
    esac
done
