#!/bin/bash
# VPX Table Launcher
# A simple GUI launcher for Visual Pinball X tables
# Tarso Galvão - 2025
# Dependencies: yad 0.40.0 (GTK+ 3.24.38)

# TODO: fetch table names from the .vpx files instead of the file names (- tablename[filename])

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
CONFIG_FILE="$HOME/.vpx_launcher_config"
DEFAULT_ICON="./default_icon.ico"  # Default icon for list view

# Load or create the config file with default values
if [ ! -f "$CONFIG_FILE" ]; then
    {
        echo "TABLES_DIR=\"$HOME/Games/vpinball/build/tables/\""
        echo "START_ARGS=\"$START_ARGS\""
        echo "COMMAND_TO_RUN=\"$HOME/Games/vpinball/build/VPinballX_GL\""
        echo "END_ARGS=\"$END_ARGS\""
        echo "VPINBALLX_INI=\"$HOME/.vpinball/VPinballX.ini\""
        echo "FALLBACK_EDITOR=\"code\""
        echo "WINDOW_WIDTH=\"800\""
        echo "WINDOW_HEIGHT=\"600\""
    } > "$CONFIG_FILE"
fi

# (Re)Load the config file
# shellcheck disable=SC1090
source "$CONFIG_FILE"

## ----------------------- FUNCTIONS -----------------------
# Function to show an error dialog and return the chosen action
handle_error() {
    yad --title="Error" --text="$1" 2>/dev/null \
        --form --width=400 --height=150 \
        --buttons-layout=center \
        --button="Settings:1" --button="Exit:252"
    
    local exit_code=$?
    if [[ $exit_code -eq 1 ]]; then
        open_launcher_settings
    fi
}

## -------------------- LAUNCHER SETTINGS ---------------------
open_launcher_settings() {
    # Show settings dialog
    NEW_VALUES=$(yad --form --title="Settings" \
        --field="Tables Directory:":DIR "$TABLES_DIR" \
        --field="Initial Arguments:":FILE "$START_ARGS" \
        --field="VPX Executable:":FILE "$COMMAND_TO_RUN" \
        --field="Final Arguments:":FILE "$END_ARGS" \
        --field="VPinballX.ini Path:":FILE "$VPINBALLX_INI" \
        --field="Fallback Editor:":FILE "$FALLBACK_EDITOR" \
        --field="Launcher Width:":FILE "$WINDOW_WIDTH" \
        --field="Launcher Height:":FILE "$WINDOW_HEIGHT" \
        --width=500 --height=150 \
        --separator="|")

    if [ -z "$NEW_VALUES" ]; then return; fi

    # Extract new values from user input using awk
    IFS="|" read -r NEW_TABLES_DIR \
                    NEW_START_ARGS \
                    NEW_COMMAND \
                    NEW_END_ARGS \
                    NEW_VPINBALLX_INI \
                    NEW_FALLBACK_EDITOR \
                    NEW_WINDOW_WIDTH \
                    NEW_WINDOW_HEIGHT \
                    <<< "$NEW_VALUES"

    # Validate new directory and executables
    if [ ! -d "$NEW_TABLES_DIR" ] || ! find "$NEW_TABLES_DIR" -type f -name "*.vpx" | grep -q .; then
        handle_error "Error: No .vpx files found in the directory!"
        return
    elif [ ! -x "$NEW_COMMAND" ]; then
        handle_error "Error: VPX executable '$NEW_COMMAND' does not exist or is not executable!"
        return
    elif ! command -v "$NEW_FALLBACK_EDITOR" >/dev/null 2>&1; then
        handle_error "Error: Fallback editor '$NEW_FALLBACK_EDITOR' cannot be opened!"
        return
    fi

    # Save settings to configuration file
    {
        echo "TABLES_DIR=\"$NEW_TABLES_DIR\""
        echo "START_ARGS=\"$NEW_START_ARGS\""
        echo "COMMAND_TO_RUN=\"$NEW_COMMAND\""
        echo "END_ARGS=\"$NEW_END_ARGS\""
        echo "VPINBALLX_INI=\"$NEW_VPINBALLX_INI\""
        echo "FALLBACK_EDITOR=\"$NEW_FALLBACK_EDITOR\""
        echo "WINDOW_WIDTH=\"$NEW_WINDOW_WIDTH\""
        echo "WINDOW_HEIGHT=\"$NEW_WINDOW_HEIGHT\""
    } > "$CONFIG_FILE"

    # Give user feedback that the paths were updated successfully
    yad --info --title="Settings" \
        --text="Paths updated successfully!" \
        --buttons-layout=center --button="OK:0" \
        --width=300 --height=100 2>/dev/null

    # Reload the config file to apply changes
    # shellcheck disable=SC1090
    source "$CONFIG_FILE"
}

## --------------- INI SETTINGS (Standalone) -----------------
# Function to check and install missing dependencies
install_python_deps() {
    if ! command -v python3 &>/dev/null; then
        # Show the missing dependency dialog
        handle_error "Missing dependency: Python3\n \
            INI Editor cannot be run.\n\nDo you want to install it now? (Requires sudo)\n \
            You can also install it manually (Debian):\nsudo apt install python3 python3-tk"

        # If user chooses not to install, exit
        if ! yad --title="Install Python3?" --form --buttons-layout=center \
                --button="Yes:0" --button="No:1"; then
            return 1
        fi

        # Attempt to install Python3
        if ! sudo apt install -y python3 python3-tk; then
            # If installation fails, show error and exit
            handle_error "Error installing Python3\n \
                Please install it manually (Debian):\n \
                sudo apt install python3 python3-tk"
            return 1
        fi
    fi
    return 0
}

# Function to open the INI editor (either default or per table)
open_ini_settings() {
    # Check for Python 3 and Python3-tk dependencies
    if ! install_python_deps; then
        return  # If dependencies fail to install, return
    fi

    # Check if a parameter (INI file) was passed
    if [ -n "$1" ]; then
        # If so, pass it to the Python script
        python3 vpx_ini_editor.py "$1"
    else
        # Otherwise, open the default INI editor
        python3 vpx_ini_editor.py
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

        # User selected "Yes" → Copy the default VPINBALLX_INI to the new location
        cp "$VPINBALLX_INI" "$ini_file"

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
        if [[ ! -f "$VPINBALLX_INI" ]]; then
            # VPinballX.ini not found, show error and ask user to set the correct path
            handle_error "VPinballX.ini not found!\nPlease set the correct path in the settings."
        else
            # Open the default INI settings
            open_ini_settings "$VPINBALLX_INI"
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
                $FALLBACK_EDITOR "$vbs_script" &
            fi
        else
            # If the VBS file doesn't exist, extract it
            eval "\"$COMMAND_TO_RUN\" -ExtractVBS \"$SELECTED_FILE\"" &
            wait $!

            # After extraction, try opening the new VBS file with xdg-open
            if ! xdg-open "$vbs_script" 2>/dev/null; then
                # If xdg-open fails (no default handler), fallback to a text editor
                handle_error "No default handler found, opening with fallback editor"
                $FALLBACK_EDITOR "$vbs_script" &
            fi
        fi
    fi
}

## -------------------- SEARCH LOGIC --------------------
handle_search_query() {
    # Open dialog box
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
        for (( i=0; i<${#FILE_LIST[@]}; i+=2 )); do
            ICON="${FILE_LIST[i]}"
            NAME="${FILE_LIST[i+1]}"
            if echo "$NAME" | grep -iq "$SEARCH_QUERY"; then
                FILTERED_FILE_LIST+=("$ICON" "$NAME")
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
if ! find "$TABLES_DIR" -type f -name "*.vpx" | grep -q .; then
    handle_error "No .vpx files found in $TABLES_DIR."
    [[ $? -eq 252 ]] && exit 0
fi

if [[ ! -x "$COMMAND_TO_RUN" ]]; then
    handle_error "The executable '$COMMAND_TO_RUN' does not exist or is not executable!"
    [[ $? -eq 252 ]] && exit 0
fi

## --------------------- MAIN LOOP ---------------------
while true; do
    FILE_LIST=() # Prepare the table list for the launcher
    declare -A FILE_MAP # Create an associative array to map table names to file paths

    # Read .vpx files and prepare the menu
    while IFS= read -r FILE; do
        BASENAME=$(basename "$FILE" .vpx) # Strip the extension

        # List view (using .ico files)
        IMAGE_PATH="${FILE%.vpx}.ico" # Use .ico file if available
        [[ ! -f "$IMAGE_PATH" ]] && IMAGE_PATH="$DEFAULT_ICON"

        FILE_LIST+=("$IMAGE_PATH" "$BASENAME") # Add to the list
        FILE_MAP["$BASENAME"]="$FILE" # Map table name to file path
    done < <(find "$TABLES_DIR" -type f -name "*.vpx" | sort)

    TABLE_NUM=0 # Initialize table(s) found count

    # Convert the array to a string for yad
    if [ ${#FILTERED_FILE_LIST[@]} -gt 0 ]; then
        # show user search-filtered list if available
        FILE_LIST_STR=$(printf "%s\n" "${FILTERED_FILE_LIST[@]}")
        TABLE_NUM=$(( ${#FILTERED_FILE_LIST[@]} / 2 ))
    else
        # show all tables
        FILE_LIST_STR=$(printf "%s\n" "${FILE_LIST[@]}")
        TABLE_NUM=$(( ${#FILE_LIST[@]} / 2 ))
    fi

    # Show launcher menu (list view)
    SELECTED_TABLE=$(yad --list --title="VPX Launcher" \
        --text="Table(s) found: $TABLE_NUM" \
        --width="$WINDOW_WIDTH" --height="$WINDOW_HEIGHT" --search=true \
        --button="INI Editor:2" --button="Extract VBS:10" \
        --button="📂 :20" --button="⚙:1" \
        --button="🔍 :30" --button="🕹️ :0" --button="🚪 :252" \
        --no-headers \
        --column="Icon:IMG" --column="Table Name" <<< "$FILE_LIST_STR" 2>/dev/null)

    EXIT_CODE=$?

    # Strip pipes from selection (if any)
    SELECTED_TABLE="${SELECTED_TABLE#|}"
    SELECTED_TABLE="${SELECTED_TABLE%|}"

    # Get full file path if a table is selected
    [[ -v FILE_MAP["$SELECTED_TABLE"] ]] && SELECTED_FILE="${FILE_MAP[$SELECTED_TABLE]}"

    case $EXIT_CODE in
        252) exit 0 ;;  # User canceled
        1) open_launcher_settings ;;  # ⚙ Settings
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
            [[ -z "$SELECTED_TABLE" ]] && xdg-open "$TABLES_DIR" >/dev/null 2>&1 \
                || xdg-open "$(dirname "$SELECTED_FILE")" >/dev/null 2>&1
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
            # Run the selected table
            if [[ -z "$SELECTED_TABLE" ]]; then
                # Handle missing selection for table launch
                handle_error "No table selected!\nPlease select a table before launching."
            else
                # Run the selected table
                eval "$START_ARGS \"$COMMAND_TO_RUN\" -play \"$SELECTED_FILE\" $END_ARGS" &
                wait $!
            fi
            ;;
    esac
done

