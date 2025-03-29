// vpx_config.cpp
// VPinballX - Configuration GUI
// Reads and saves VPinballX.ini file while listing in sections with explanations.
// Tarso Galvão Mar/2025

#include <SDL.h>
#include <SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib> // For std::getenv
#include <filesystem>

// Structure representing a configuration section (a set of key-value pairs)
struct ConfigSection {
    std::vector<std::pair<std::string, std::string>> keyValues; // Changed to vector to preserve order
    std::map<std::string, size_t> keyToLineIndex;               // Still using map for quick lookup
};

// --------------- IniEditor Class ------------------
class IniEditor {
public:
    IniEditor(const std::string& filename);           // Loads the INI file.
    ~IniEditor();                                     // Destructor.
    void run();                                       // Runs the main GUI loop.

private:
    void loadIniFile(const std::string& filename);    // Loads the INI file into memory.
    void saveIniFile(const std::string& filename);    // Saves the current configuration back to the INI file.
    void initExplanations();                          // Initializes tooltips.
    void drawGUI();                                   // Draws the ImGui GUI.

    // Data members
    std::map<std::string, ConfigSection> iniData;     // Map of section name to key-values.
    std::vector<std::string> sections;                // List of section names.
    std::map<std::string, std::string> explanations;  // Explanations for config keys.
    std::string currentSection;                       // Currently selected section.
    std::string iniFilename;                          // Name/path of the INI file.
    bool exitRequested = false;                       // Flag to exit the main loop.
    std::vector<std::string> originalLines;           // Stores the original file content
    std::map<size_t, std::pair<std::string, std::string>> lineToKey; // Maps line index to {section, key}
    bool showSavedMessage = false;                    // Controls display of "Saved!" message
    double savedMessageTimer = 0.0;                   // Tracks when the save message was triggered
};

// ---------------- Implementation ----------------

IniEditor::IniEditor(const std::string& filename) : iniFilename(filename) {
    loadIniFile(filename);
    initExplanations();
    if (!sections.empty()) {
        currentSection = sections[0];
    }
}

IniEditor::~IniEditor() {
    // No dynamic allocations to clean up
}

void IniEditor::loadIniFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << std::endl;
        return;
    }

    // Read all lines into originalLines
    std::string line;
    while (std::getline(file, line)) {
        originalLines.push_back(line);
    }
    file.close();

    // Parse the lines to populate iniData, sections, and line mappings
    std::string currentSectionName;
    size_t lineIndex = 0;
    for (const auto& line : originalLines) {
        // Trim leading whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) {
            lineIndex++;
            continue;
        }
        std::string trimmedLine = line.substr(start);

        // Skip empty lines or comments (but they’re preserved in originalLines)
        if (trimmedLine.empty() || trimmedLine[0] == ';') {
            lineIndex++;
            continue;
        }

        // Check for section header
        if (trimmedLine.front() == '[' && trimmedLine.back() == ']') {
            currentSectionName = trimmedLine.substr(1, trimmedLine.size() - 2);
            sections.push_back(currentSectionName);
            iniData[currentSectionName] = ConfigSection();
        } else if (!currentSectionName.empty()) {
            // Parse key=value pairs
            size_t pos = trimmedLine.find('=');
            if (pos != std::string::npos) {
                std::string key = trimmedLine.substr(0, pos);
                std::string value = trimmedLine.substr(pos + 1);
                // Trim trailing whitespace from key
                size_t endKey = key.find_last_not_of(" \t");
                if (endKey != std::string::npos)
                    key = key.substr(0, endKey + 1);
                // Trim leading whitespace from value
                size_t startValue = value.find_first_not_of(" \t");
                if (startValue != std::string::npos)
                    value = value.substr(startValue);
                // Append to vector to preserve order
                iniData[currentSectionName].keyValues.emplace_back(key, value);
                // Record the line index for this key
                iniData[currentSectionName].keyToLineIndex[key] = lineIndex;
                lineToKey[lineIndex] = {currentSectionName, key};
            }
        }
        lineIndex++;
    }
}

void IniEditor::saveIniFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not write " << filename << std::endl;
        return;
    }

    // Write back the original lines, updating only key-value pair lines
    for (size_t i = 0; i < originalLines.size(); ++i) {
        if (lineToKey.find(i) != lineToKey.end()) {
            // This line is a key-value pair; write the updated value
            auto [section, key] = lineToKey[i];
            // Find the key in the vector (since keys might not be unique, use line index)
            for (const auto& kv : iniData[section].keyValues) {
                if (kv.first == key && iniData[section].keyToLineIndex[key] == i) {
                    file << key << " = " << kv.second << "\n";
                    break;
                }
            }
        } else {
            // This line is a comment, blank line, or section header; preserve it
            file << originalLines[i] << "\n";
        }
    }
    file.close();
}

void IniEditor::initExplanations() {
    //-------------------------------------------------------------------------------------------
    //--------------------------- START OF VPinballX.ini EXPLANATIONS ---------------------------
    // STANDALONE
    explanations["VPRegPath"] = "This can be used to override the VPinball table settings directory location. If set to ./, the current table path will be used.";
    explanations["AltSound"] = "Controls sound format between 'Legacy', 'Altsound' and 'G-Sound'.\n1 - If the folder contains an 'altsound.csv' or 'g-sound.csv' file.\n1 - If the folder contains subfolders with names like 'jingle', 'single', 'voice', etc. and you are NOT running on PinSound sound hardware.\nNote: this only works for the unencrypted versions of the PinSound libraries, which are now quite old.\n2 - If the folder contains subfolders with names like 'jingle', 'single', 'voice', etc. and you are running on PinSound sound hardware.\n.\n3 - ??";
    explanations["PinMAMEPath"] = "Specifies the directory where the PinMAME emulator is located. PinMAME is an emulator for classic pinball machines.\nDefault is ~/.pinmame: If not specified, the default location for the PinMAME folder is ~/.pinmame.";
    explanations["PinMAMEWindow"] = "Controls the visibility of the PinMAME DMD (Dot Matrix Display) window.\n0 - Turns the DMD window off: Disables the PinMAME DMD window, which is often used for displaying the score and game information.\n1 - Turns the DMD window on: Enables the PinMAME DMD window to display relevant game information. (Default is 1)";
    explanations["PinMAMEWindowX"] = "Sets the X pixel coordinate of the top-left corner of the PinMAME DMD window on the screen.\nThis controls the horizontal position where the DMD window appears on the screen.";
    explanations["PinMAMEWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the PinMAME DMD window on the screen.\nThis controls the vertical position where the DMD window appears on the screen.";
    explanations["PinMAMEWindowWidth"] = "Controls the width of the PinMAME DMD window.\nYou can resize the DMD window by specifying the width in pixels.";
    explanations["PinMAMEWindowHeight"] = "Controls the height of the PinMAME DMD window.\nYou can resize the DMD window by specifying the height in pixels.";
    explanations["PinMAMEWindowRotation"] = "Controls the rotation of the PinMAME DMD window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation."; 
    explanations["FlexDMDWindow"] = "Controls the visibility of the FlexDMD (Dot Matrix Display) window.\n0 - Turns the DMD window off: Disables the FlexDMD window, which is often used for displaying the score and game information.\n1 - Turns the DMD window on: Enables the FlexDMD window to display relevant game information. (Default is 1)";
    explanations["FlexDMDWindowX"] = "Sets the X pixel coordinate of the top-left corner of the FlexDMD window on the screen.\nThis controls the horizontal position where the DMD window appears on the screen.";
    explanations["FlexDMDWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the FlexDMD window on the screen.\nThis controls the vertical position where the DMD window appears on the screen.";
    explanations["FlexDMDWindowWidth"] = "Controls the width of the FlexDMD window.\nYou can resize the DMD window by specifying the width in pixels.";
    explanations["FlexDMDWindowHeight"] = "Controls the height of the FlexDMD window.\nYou can resize the DMD window by specifying the height in pixels.";
    explanations["FlexDMDWindowRotation"] = "Controls the rotation of the FlexDMD window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["B2SHideGrill"] = "Controls the visibility of the grill in the backbox of the cabinet setup.\n0 - Show grill (if it exists): Displays the grill, a common component in pinball machines.\n1 - Hide grill: Hides the grill.";
    explanations["B2SHideB2SDMD"] = "Controls the visibility of the extra DMD frame in the backbox.\n0 - Show extra DMD frame (if it exists): Displays the DMD frame around the Dot Matrix Display.\n1 - Hide extra DMD frame: Hides the DMD frame, leaving just the DMD window.";
    explanations["B2SHideB2SBackglass"] = "Controls the visibility of the backglass in a cabinet setup.\n0 - Show backglass: Displays the backglass, which often shows game artwork.\n1 - Hide backglass: Hides the backglass, potentially saving space in a multi-display setup.";
    explanations["B2SWindows"] = "Controls the visibility of various B2S windows such as the grill, DMD frame, and backglass.\n0 - Hide all B2S windows: Hides the grill, DMD frame, and backglass.\n1 - Show B2S windows according to other settings: Displays the grill, DMD frame, and backglass according to the other settings.";
    explanations["B2SBackglassX"] = "Sets the X pixel coordinate of the top-left corner of the backglass window.\nThis allows you to position the backglass at a specific horizontal position on the screen.";
    explanations["B2SBackglassY"] = "Sets the Y pixel coordinate of the top-left corner of the backglass window.\nThis allows you to position the backglass at a specific vertical position on the screen.";
    explanations["B2SBackglassWidth"] = "Controls the pixel width of the backglass window.\nThis defines how wide the backglass window will appear on the screen.";
    explanations["B2SBackglassHeight"] = "Controls the pixel height of the backglass window.\nThis defines how tall the backglass window will appear on the screen.";
    explanations["B2SBackglassRotation"] = "Controls the rotation of the backglass window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["B2SDMDX"] = "Sets the X pixel coordinate of the top-left corner of the B2SDMD frame.\nThis controls the horizontal positioning of the B2SDMD frame relative to the screen.";
    explanations["B2SDMDY"] = "Sets the Y pixel coordinate of the top-left corner of the B2SDMD frame.\nThis controls the vertical positioning of the B2SDMD frame relative to the screen.";
    explanations["B2SDMDWidth"] = "Controls the pixel width of the B2SDMD frame.\nThis defines how wide the B2SDMD frame will appear on the screen.";
    explanations["B2SDMDHeight"] = "Controls the pixel height of the B2SDMD frame.\nThis defines how tall the B2SDMD frame will appear on the screen.";
    explanations["B2SDMDRotation"] = "Controls the rotation of the B2SDMD frame.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["B2SDMDFlipY"] = "Controls whether the B2SDMD frame is flipped vertically.\n0 - No flip: The B2SDMD frame appears normally.\n1 - Flip vertically: Flips the B2SDMD frame vertically, which may be useful for specific screen orientations.";
    explanations["PUPTopperWindow"] = "Controls the visibility of the PUPTopper window.\n0 - Hide PUPTopper window.\n1 - Show PUPTopper window.";
    explanations["PUPTopperWindowX"] = "Sets the X pixel coordinate of the top-left corner of the PUPTopper window.\nThis allows you to position the PUPTopper at a specific horizontal position on the screen.";
    explanations["PUPTopperWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the PUPTopper window.\nThis allows you to position the PUPTopper at a specific vertical position on the screen.";
    explanations["PUPTopperWindowWidth"] = "Controls the pixel width of the PUPTopper window.\nThis defines how wide the PUPTopper window will appear on the screen.";
    explanations["PUPTopperWindowHeight"] = "Controls the pixel height of the PUPTopper window.\nThis defines how tall the PUPTopper window will appear on the screen.";
    explanations["PUPTopperWindowRotation"] = "Controls the rotation of the PUPTopper window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["PUPBackglassWindow"] = "Controls the visibility of the PUPBackglass window.\n0 - Hide PUPBackglass window.\n1 - Show PUPBackglass window.";
    explanations["PUPBackglassWindowX"] = "Sets the X pixel coordinate of the top-left corner of the PUPBackglass window.\nThis allows you to position the PUPBackglass at a specific horizontal position on the screen.";
    explanations["PUPBackglassWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the PUPBackglass window.\nThis allows you to position the PUPBackglass at a specific vertical position on the screen.";
    explanations["PUPBackglassWindowWidth"] = "Controls the pixel width of the PUPBackglass window.\nThis defines how wide the PUPBackglass window will appear on the screen.";
    explanations["PUPBackglassWindowHeight"] = "Controls the pixel height of the PUPBackglass window.\nThis defines how tall the PUPBackglass window will appear on the screen.";
    explanations["PUPBackglassWindowRotation"] = "Controls the rotation of the PUPBackglass window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["PUPDMDWindow"] = "Controls the visibility of the PUPDMD window.\n0 - Hide PUPDMD window.\n1 - Show PUPDMD window.";
    explanations["PUPDMDWindowX"] = "Sets the X pixel coordinate of the top-left corner of the PUPDMD window.\nThis allows you to position the PUPDMD at a specific horizontal position on the screen.";
    explanations["PUPDMDWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the PUPDMD window.\nThis allows you to position the PUPDMD at a specific vertical position on the screen.";
    explanations["PUPDMDWindowWidth"] = "Controls the pixel width of the PUPDMD window.\nThis defines how wide the PUPDMD window will appear on the screen.";
    explanations["PUPDMDWindowHeight"] = "Controls the pixel height of the PUPDMD window.\nThis defines how tall the PUPDMD window will appear on the screen.";
    explanations["PUPDMDWindowRotation"] = "Controls the rotation of the PUPDMD window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["PUPPlayfieldWindow"] = "Controls the visibility of the PUPPlayfield window.\n0 - Hide PUPPlayfield window.\n1 - Show PUPPlayfield window.";
    explanations["PUPPlayfieldWindowX"] = "Sets the X pixel coordinate of the top-left corner of the PUPPlayfield window.\nThis allows you to position the PUPPlayfield at a specific horizontal position on the screen.";
    explanations["PUPPlayfieldWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the PUPPlayfield window.\nThis allows you to position the PUPPlayfield at a specific vertical position on the screen.";
    explanations["PUPPlayfieldWindowWidth"] = "Controls the pixel width of the PUPPlayfield window.\nThis defines how wide the PUPPlayfield window will appear on the screen.";
    explanations["PUPPlayfieldWindowHeight"] = "Controls the pixel height of the PUPPlayfield window.\nThis defines how tall the PUPPlayfield window will appear on the screen.";
    explanations["PUPPlayfieldWindowRotation"] = "Controls the rotation of the PUPPlayfield window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["PUPFullDMDWindow"] = "Controls the visibility of the PUPFullDMD window.\n0 - Hide PUPFullDMD window.\n1 - Show PUPFullDMD window.";
    explanations["PUPFullDMDWindowX"] = "Sets the X pixel coordinate of the top-left corner of the PUPFullDMD window.\nThis allows you to position the PUPFullDMD at a specific horizontal position on the screen.";
    explanations["PUPFullDMDWindowY"] = "Sets the Y pixel coordinate of the top-left corner of the PUPFullDMD window.\nThis allows you to position the PUPFullDMD at a specific vertical position on the screen.";
    explanations["PUPFullDMDWindowWidth"] = "Controls the pixel width of the PUPFullDMD window.\nThis defines how wide the PUPFullDMD window will appear on the screen.";
    explanations["PUPFullDMDWindowHeight"] = "Controls the pixel height of the PUPFullDMD window.\nThis defines how tall the PUPFullDMD window will appear on the screen.";
    explanations["PUPFullDMDWindowRotation"] = "Controls the rotation of the PUPFullDMD window.\nThe values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\nThe default value is 0, which represents no rotation.";
    explanations["ZeDMDDebug"] = "0 to disable debug messages for an attached ZeDMD (the default), 1 to enable.";
    explanations["ZeDMDWiFiAddr"] = "Default value is zedmd-wifi.local.";
    explanations["DMDServerAddr"] = "Default value is localhost.";
    explanations["DMDServerPort"] = "Default value is 6789.";

    // PLAYER
    explanations["Sound3D"] = "This is an integer value that selects how VPinball generates its audio. Six options are available.\n0 - Standard 2 channel (default)\n1 - Surround (All effects to rear channels)\n2 - Surround (Front is rear of cab)\n3 - Surround (Front is front of cab)\n4 - 7.1 Surround (Front is rear, back is side, backbox is front)\n5 - 7.1 Surround Sound Feedback (SSF) This exaggerates the positional feel of the playfield sound effects when played in a cabinet with exciter pairs positioned at each end of the cabinet";
    explanations["SoundDevice"] = "This is the text name of the main sound device that will be used by VPinball for Table mechanical sounds like flippers and bumpers\nDetermine this name by running VPinball with the -listsnd parameter.";
    explanations["SoundDeviceBG"] = "This is the text name of the main sound device that will be used by VPinball for Music and Backglass sounds, including those from PUP packs and PinMAME.\nIf you set SoundDevice to a non-default value and leave this setting blank (to use the default), that effectively tells VPinball to use two different devices and that isn't supported. So either both SoundDevice and SoundDeviceBG should both be left blank to use your system's default, or they should both be set to exactly the same value.";
    explanations["PlayMusic"] = " Setting this to 0 disables Music from being played, 1 enables Music (the default).\nMusic sounds are those routed through SoundDeviceBG and includes not only music but backglass sounds plus PinMAME and PUP audio.";
    explanations["PlaySound"] = "Setting this to 0 disables Table sounds from being played, 1 enables Table sounds (the default).\nTable sounds are those sounds routed through SoundDevice like those from flipper and bumpers.";
    explanations["MusicVolume"] = "An integer number in the range of 0 - 100 for those sounds defined as Music above.\nThe default value is 100 (maximum volume).";
    explanations["SoundVolume"] = "An integer number in the range of 0 - 100 for the Table mechanical sounds as defined above.\nThe default value is 100 (full volume)";
    explanations["BGSet"] = "Controls the display mode for the backglass and the playfield.\n0 - Desktop (default): The default desktop mode for standard use.\n1 - Fullscreen: For cabinet use or multi-window on desktop setups.\n2 - Full Single Screen (FSS): Uses the entire screen for both playfield and backglass. Falls back to desktop view if unavailable.";
    explanations["PBWEnabled"] = "This enables analog nudging from hardware acceleration sensors, both from purpose-built controllers like the KL25Z-based Pinscape or from video game console controllers like the Playstation 4 Dualshock.\nSetting this to 0 disables the acceleration sensor, 1 enables it (the default).";
    explanations["PBWAccelGainX"] = "This adjusts the accelerometer sensitivity on the X-axis representing left & right nudging expressed as a percentage in the range of 0 - 150%. The default value is the maximum of 150% (or multiply the value by 1.5x).\nSet this value to zero to disable the accelerometer along the X-axis entirely.";
    explanations["PBWAccelGainY"] = "The accelerometer sensitivy on the Y-axis representing forward nudging. It is otherwise analogous to PBWAccelGainX. Setting this value to zero will disable forward nudging.";
    explanations["VisualLatencyCorrection"] = "Correction expressed in milliseconds applied to ball rendering to account for visual latency.\nSetting this value to 0 disables this correction, while 1 (the default) derives an appropriate value from the target FPS rate.";
    explanations["MaxPrerenderedFrames"] = "Pre-rendering frames is a technique used to help reduce lag in games.\nLeave this value at the default of 0 if you have enabled 'Low Latency' or 'Anti Lag' settings in your graphics driver. Otherwise experiment with a value of 1 or 2 for a chance of lag reduction at the cost of a small framerate reduction.";
    explanations["SyncMode"] = "Controls the synchronization method for the video display.\n0 - None: No synchronization, potentially resulting in tearing or stuttering.\n1 - Vertical Sync: Synchronizes the video output with the display refresh rate, preventing screen tearing at the cost of input latency.\n2 - Adaptive Sync: Syncs video output to the display refresh rate, except for late frames, which may result in occasional tearing.\n3 - Frame Pacing (default): Synchronizes the simulation with the video frame rate while keeping input latency low and dynamically adjusting frame rates.";
    explanations["OverrideTableEmissionScale"] = "Replace table's scene lighting emission scale setup, eventually based on automatic Day/Night computed from geographic position.";
    explanations["MaxFramerate"] = "Sets the maximum frame rate for the game.\nDefaults to the playfield display refresh rate, ensuring a smooth experience.\n0 - Unbound frame rate: Removes any frame rate cap, allowing the game to run as fast as possible based on hardware.\nThe frame rate will never go below 24fps, ensuring at least minimal smoothness.";
    explanations["FXAA"] = "Controls the method used for anti-aliasing to smooth out jagged edges in the game visuals.\n0 - Disabled: No anti-aliasing applied, which may result in jagged edges.\n1 - Fast FXAA: Fast FXAA (Fast Approximate Anti-Aliasing) method for anti-aliasing, offering a good balance between performance and visual quality.\n2 - Standard FXAA: Standard FXAA method, offering better anti-aliasing than fast FXAA but with slightly more performance cost.\n3 - Quality FXAA: High-quality FXAA, providing better image smoothing at the cost of performance.\n4 - Fast NFAA: Fast NVIDIA Fast Approximate Anti-Aliasing (NFAA), a technique for smoother visuals on NVIDIA hardware.\n5 - Standard DLLA: Standard Dynamic Line Anti-Aliasing, a method designed for dynamic rendering of lines and edges.\n6 - Quality SMAA: High-quality Subpixel Morphological Anti-Aliasing (SMAA), a more advanced anti-aliasing method with high visual quality.";
    explanations["Sharpen"] = "Controls the sharpening effect applied to the game's visuals.\n0 - Disabled: No sharpening effect applied to the visuals.\n1 - CAS: Contrast Adaptive Sharpening, a method to enhance image details by adjusting contrast and edges.\n2 - Bilateral CAS: Bilateral Contrast Adaptive Sharpening, a variant of CAS that reduces noise and artifacts in the image while sharpening.";
    explanations["DisableAO"] = "Determines whether ambient occlusion (AO) is enabled or disabled. AO simulates realistic shading and lighting by darkening areas that are less exposed to light.\n0 = Enabled: Ambient occlusion is enabled, providing more realistic shadowing and depth.\n1 = Disabled: Ambient occlusion is disabled, potentially improving performance but sacrificing visual realism.";
    explanations["DynamicAO"] = "Controls the type of ambient occlusion used. This setting takes effect only when DisableAO is set to 0.\n0 = Static: Static ambient occlusion, where shadows are precomputed and don't change in real-time.\n1 = Dynamic: Dynamic ambient occlusion, where shadows and lighting change in real-time based on the scene's lighting conditions.";
    explanations["SSRefl"] = "Controls whether screen space reflections (SSR) are enabled or disabled. SSR is a technique for simulating reflections on shiny surfaces.\n0 - Disable screen space reflections: Disables SSR, resulting in no reflections on reflective surfaces.\n1 - Enable screen space reflections: Enables SSR, allowing for more realistic reflections on surfaces like water and mirrors.";
    explanations["PFReflection"] = "Determines the type of reflections on the playfield (game surface). The reflection settings apply to the ball and other game elements that interact with the playfield.\n0 - Disable reflections (default?): Disables reflections entirely on the playfield.\n1 - Balls only: Only the ball will reflect on the playfield surface.\n2 - Static only: Only static objects (such as the table or scenery) will have reflections.\n3 - Static and balls: Both static objects and the ball will reflect.\n4 - Static and unsynced dynamics: Static objects and unsynchronized dynamics (like the ball's interaction with moving elements) will have reflections.\n5 - Dynamic: Dynamic reflections will be applied to all game objects in the scene.";
    explanations["MaxTexDimension"] = "Sets the maximum dimension for textures. Higher values may improve visual quality but can decrease performance.\n0 - Unlimited (default): Textures can be any size, limited only by hardware capabilities.";
    explanations["AAFactor"] = "Controls the level of supersampling applied for anti-aliasing. Supersampling can improve visual quality by rendering the image at a higher resolution and then downsampling it.\n0.00 to 2.00: Specifies the supersampling factor, with higher values improving quality but requiring more performance.";
    explanations["MSAASamples"] = "Controls the number of samples for Multi-Sample Anti-Aliasing (MSAA), which helps reduce jagged edges and improves visual quality.\n0 - None: No MSAA applied, leading to possible jagged edges.\n4 - 4 samples: Applies 4 samples for MSAA, offering a balance of performance and quality.\n6 - 6 samples: Applies 6 samples for MSAA, improving image quality further but with a performance hit.\n8 - 8 samples: Applies 8 samples for the highest quality MSAA, with a significant performance cost.";
    explanations["ForceBloomOff"] = "Controls whether bloom effects (a lighting effect that creates a glowing halo around bright areas) are enabled or disabled.\n0 - Enabled: Bloom effects are enabled, enhancing bright areas of the screen for a more dramatic visual effect.\n1 - Disabled: Bloom effects are disabled, removing the glowing halo effect from bright areas.";
    explanations["ForceMotionBlurOff"] = "Controls whether motion blur is applied to the ball during gameplay. Motion blur can make fast-moving objects appear smoother but may reduce visual clarity.\n0 - Enabled: Ball motion blur is enabled, making the ball's movement appear smoother.\n1 - Disabled: Ball motion blur is disabled, resulting in sharper visuals but potentially making fast-moving objects look more jagged.";
    explanations["ForceAnisotropicFiltering"] = "Determines whether anisotropic filtering (AF) is enabled or disabled. AF improves the quality of textures viewed at steep angles, making them appear sharper and clearer.\n0 - Disabled: Anisotropic filtering is disabled, leading to potentially blurry textures at certain angles.\n1 - Enabled: Anisotropic filtering is enabled, improving the quality of textures, especially at angles.";
    explanations["BAMHeadTracking"] = "Makes the game appear as a 3D image without the need for 3D glasse.\nNote: BAM Head Tracking uses a PS3 Eye Camera and a 3 point Infrared LED hat.\n0 - Disabled.\n1 - Enabled.";
    explanations["GfxBackend"] = "Valid values are: Noop, Agc, Direct3D11, Direct3D12, Gnm, Metal, Nvn, OpenGLES, OpenGL, Vulkan, Default";
    explanations["BallTrail"] = "Determines whether the ball is drawn with a trail behind it to more easily follow its motion.\nA setting of 0 disables ball trails, 1 turns them on (the default).";
    explanations["BallTrailStrength"] = "Determines the length and intensity of the trail drawn behind the ball's path.\nIt is a floating point value between 0.0 and 1.0, with a default of 0.5.\nThe higher the value, the more intensely the ball's trail is drawn and the easier it is to follow. It only has an effect if BallTrail is enabled.";
    explanations["DynamicDayNight"] = "Activate this to switch the table brightness automatically based on your PC's clock and your geographic location.\n0 disables this feature (the default), 1 enables it. You will need to fill in the latitude and longitude for your location for this to work correctly.\nYou may use openstreetmap.org or Google Maps to get these values in the correct format. More below...";
    explanations["Latitude"] = "The latitude of your location. Specify only if you have DynamicDayNight enabled. Use a negative value if you are in the southern hemisphere.";
    explanations["Longitude"] = "The longitude of your location. Specify only if you have DynamicDayNight enabled. Use a negative value if you are in the western hemisphere.";
    explanations["NudgeStrength"] = "Changes the visual effect / screen shaking when nudging the table.\nThis does not change the actual strength of the nudge. The default setting is 0.02.";
    
    //----------------------------- END OF VPinballX.INI EXPLANATIONS ---------------------------
    //-------------------------------------------------------------------------------------------
    
    // VPX GUI Tools Launcher Variables Explanations (Don't change these!)
    explanations["TablesDir"] = "Path to the directory where all VPX table files (.vpx) are stored.\nSearch is recursive.";
    explanations["StartArgs"] = "Additional command-line arguments to pass when launching VPX (optional).";
    explanations["CommandToRun"] = "Full path to the VPinballX executable used to launch tables.";
    explanations["EndArgs"] = "Additional command-line arguments to append when launching VPX (optional).";
    explanations["VPinballXIni"] = "Path to the main VPinballX configuration file.";
    explanations["FallbackEditor"] = "Default text editor used if the user wants to edit configuration files.";
    explanations["WindowWidth"] = "Width (in pixels) of the GUI window for the launcher.";
    explanations["WindowHeight"] = "Height (in pixels) of the GUI window for the launcher.";
    explanations["WheelImage"] = "Path to the image used for the table selection wheel,\nrelative to TablesDir/<table_folder>/.";
    explanations["TableImage"] = "Path to the image representing the selected table,\nrelative to TablesDir/<table_folder>/.";
    explanations["BackglassImage"] = "Path to the image representing the table's backglass,\nrelative to TablesDir/<table_folder>/.";
    explanations["MarqueeImage"] = "Path to the image used as the marquee display,\nrelative to TablesDir/<table_folder>/.";
    explanations["TableVideo"] = "Path to the video preview for the selected table,\nrelative to TablesDir/<table_folder>/.";
    explanations["BackglassVideo"] = "Path to the video preview of the table's backglass,\nrelative to TablesDir/<table_folder>/.";
    explanations["DMDVideo"] = "Path to the video preview of the table's DMD display,\nrelative to TablesDir/<table_folder>/.";
    explanations["ROMPath"] = "Directory containing the ROM files for PinMAME emulation,\nrelative to TablesDir/<table_folder>/.";
    explanations["AltSoundPath"] = "Directory for alternative sound packs used by PinMAME,\nrelative to TablesDir/<table_folder>/.";
    explanations["AltColorPath"] = "Directory for alternative color DMD packs used by PinMAME,\nrelative to TablesDir/<table_folder>/.";
    explanations["MusicPath"] = "Directory where custom music files are stored,\nrelative to TablesDir/<table_folder>/.";
    explanations["PUPPackPath"] = "Directory where PinUP Player (PUP) packs are stored,\nrelative to TablesDir/<table_folder>/.";
}

void IniEditor::drawGUI() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    
    ImGui::Begin("VPinballX Configuration", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginCombo("Section", currentSection.c_str())) {
        for (const auto& section : sections) {
            bool is_selected = (currentSection == section);
            if (ImGui::Selectable(section.c_str(), is_selected))
                currentSection = section;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    float buttonHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;
    float availableHeight = ImGui::GetContentRegionAvail().y;
    float childHeight = availableHeight - buttonHeight;
    if (childHeight < 0) childHeight = 0;

    ImGui::BeginChild("KeyValues", ImVec2(0, childHeight), true);
    if (iniData.find(currentSection) != iniData.end()) {
        for (auto& kv : iniData[currentSection].keyValues) { // Iterate over vector instead of map
            ImGui::PushID(kv.first.c_str());
            
            // Display the key
            ImGui::Text("%s", kv.first.c_str());
            
            // Move to the next position for the "?"
            // ImGui::SameLine(210); // Fixed offset
            ImGui::SameLine(); // No fixed offset yet, let ImGui calculate natural spacing
            // float key_end_pos = ImGui::GetCursorPosX(); // Store where the key ends
            
            // Add the "?" if there's an explanation
            if (explanations.find(kv.first) != explanations.end()) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "?");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
                    ImGui::TextWrapped("%s", explanations[kv.first].c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            
            // Set a minimum offset for the value field, ensuring it doesn't overlap
            // float value_start_pos = key_end_pos + 20.0f; // Adjust this padding as needed (e.g., 20px after "?")
            ImGui::SameLine(225); // Position the input field
            
            // Input field for the value
            char buf[256];
            std::strncpy(buf, kv.second.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            if (ImGui::InputText("", buf, sizeof(buf))) {
                kv.second = std::string(buf); // Update the value directly in the vector
            }
            
            ImGui::PopID();
            ImGui::NewLine();
        }
    } else {
        ImGui::Text("No section data available.");
    }
    ImGui::EndChild();

    if (ImGui::Button("Save")) {
        saveIniFile(iniFilename);
        showSavedMessage = true;
        savedMessageTimer = ImGui::GetTime();
    }
    ImGui::SameLine();
    if (ImGui::Button("Exit")) {
        exitRequested = true;
    }

    ImGui::SameLine();
    if (showSavedMessage) {
        ImGui::Text("Saved!");
        if (ImGui::GetTime() - savedMessageTimer > 2.0) {
            showSavedMessage = false;
        }
    }

    ImGui::End();
}

void IniEditor::run() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return;
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_Window* window = SDL_CreateWindow("VPinballX Configuration",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          600, 500,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    exitRequested = false;
    bool done = false;
    while (!done && !exitRequested) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        drawGUI();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// ---------------- Main Entry Point ----------------

int main(int argc, char** argv) {
    // Check for the --version flag among the arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--version") {
            std::cout << "vpx_config version 1.0.0" << std::endl;
            return 0;  // Exit after printing version info
        }
    }

    std::string iniPath;

    if (argc > 1) {
        // Use provided argument as the INI file path
        iniPath = argv[1];
    } else {
        // Default to ~/.vpinball/VPinballX.ini
        const char* homeDir = std::getenv("HOME");
        if (!homeDir) {
            std::cerr << "Error: HOME environment variable not set.\n";
            return 1;
        }
        iniPath = std::string(homeDir) + "/.vpinball/VPinballX.ini";
    }

    // Check if the INI file exists
    if (!std::filesystem::exists(iniPath)) {
        std::cerr << "\033[1;31mError:\033[0m VPinballX.ini was not found at \033[1;33m" 
                  << iniPath << "\033[0m\n"
                  << "Please specify the location.\n"
                  << "Use: \033[1;32m./vpx_config /path/to/VPinballX.ini\033[0m\n"
                  << "Or \033[1;32m./vpx_config settings.ini\033[0m to edit launcher settings.\n";
        return 1;
    }

    IniEditor editor(iniPath);
    editor.run();
    return 0;
}
