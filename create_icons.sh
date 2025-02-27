#!/bin/bash
# This script searches for 'wheel.png' files under a hardcoded tables directory.
# For each table that contains a .vpx file, it:
#  1. Determines the table directory (the parent of the images folder).
#  2. Uses the base name of the .vpx file (without extension) as the name for the output .ico file.
#  3. Checks if the .ico file already exists; if it does, it skips that table.
#  4. If not, it resizes wheel.png to 24x24 pixels and saves it as <vpx_basename>.ico in the table directory.
#
# Additionally, if the script is run with the --missing argument,
# it will skip the icon conversion and simply list all tables (directories with a .vpx file)
# that are missing a wheel.png file in their images folder.
#
# Dependencies: ImageMagick must be installed (for the 'convert' command).

# Hardcoded tables directory - modify this variable as needed.
TABLES_DIR="$HOME/Games/vpinball/build/tables"

# Define color variables for echo output.
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'  # No Color

echo -e "${BLUE}Using tables directory: $TABLES_DIR${NC}"

# If the script is called with the --missing argument, list tables missing wheel.png and exit.
if [ "$1" == "--missing" ]; then
    echo -e "${BLUE}Checking for tables missing wheel.png...${NC}"
    # Iterate over each table directory that contains a .vpx file.
    for vpx_file in "$TABLES_DIR"/*/*.vpx; do
        if [ -f "$vpx_file" ]; then
            table_dir=$(dirname "$vpx_file")
            wheel_file="$table_dir/images/wheel.png"
            if [ ! -f "$wheel_file" ]; then
                echo -e "${GREEN}->${YELLOW} '$(basename "$table_dir")'${NC}"
            fi
        fi
    done
    echo -e "${BLUE}These tables have no image source for icons.${NC}"
    exit 0
fi

# Find all wheel.png files in the hardcoded tables directory.
find "$TABLES_DIR" -type f -name "wheel.png" | while read -r file; do
    # Assume file path is: <TABLES_DIR>/<table_dir>/images/wheel.png
    images_dir=$(dirname "$file")
    table_dir=$(dirname "$images_dir")

    # Find a .vpx file in the table directory (assuming there is one)
    vpx_file=$(find "$table_dir" -maxdepth 1 -type f -name "*.vpx" | head -n 1)
    
    # If no .vpx file is found, skip this table.
    if [ -z "$vpx_file" ]; then
        echo -e "${YELLOW}Skipping $table_dir: No .vpx file found.${NC}"
        continue
    fi
    
    # Extract the base name from the .vpx file (e.g., pinball from pinball.vpx)
    base_name=$(basename "$vpx_file" .vpx)
    
    # Define the output ICO file path in the table directory.
    output_file="$table_dir/${base_name}.ico"
    
    # Check if the .ico file already exists; if so, skip this table.
    if [ -f "$output_file" ]; then
        echo -e "${YELLOW}Skipping $table_dir"
        echo -e "${GREEN}$output_file already exists.${NC}"
        continue
    fi
    
    echo -e "${BLUE}Processing table: $table_dir${NC}"
    echo -e "${BLUE}Using .vpx file: $vpx_file${NC}"
    
    # Convert wheel.png to a 24x24 ICO using ImageMagick.
    convert "$file" -resize 24x24 "$output_file"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Created $output_file successfully.${NC}"
    else
        echo -e "${RED}Failed to create $output_file.${NC}"
    fi
done
