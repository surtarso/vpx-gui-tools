# resources.cmake
# Debug: Print the directories
message(STATUS "SRC_DIR: ${SRC_DIR}")
message(STATUS "BIN_DIR: ${BIN_DIR}")

# Create the resources directory in the build folder
file(MAKE_DIRECTORY "${BIN_DIR}/resources")

# List of files to copy
set(FILES_TO_COPY
    "${SRC_DIR}/resources/settings.ini"
    "${SRC_DIR}/resources/Symbola.ttf"
    "${SRC_DIR}/external/vpxtool"
)

# Copy each file only if it doesn't exist in the destination
foreach(FILE ${FILES_TO_COPY})
    get_filename_component(FILENAME "${FILE}" NAME)
    set(DEST_FILE "${BIN_DIR}/resources/${FILENAME}")
    message(STATUS "Checking source file: ${FILE}")
    message(STATUS "Destination file: ${DEST_FILE}")
    if(NOT EXISTS "${DEST_FILE}")
        if(EXISTS "${FILE}")
            file(COPY "${FILE}" DESTINATION "${BIN_DIR}/resources")
            message(STATUS "Copied ${FILENAME} to build directory")
        else()
            message(WARNING "Source file ${FILE} not found, skipping")
        endif()
    else()
        message(STATUS "${FILENAME} already exists in build directory, skipping copy")
    endif()
endforeach()

# Create logs directory
file(MAKE_DIRECTORY "${BIN_DIR}/resources/logs")