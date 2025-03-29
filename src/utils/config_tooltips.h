#ifndef CONFIG_TOOLTIPS_H
#define CONFIG_TOOLTIPS_H

#include <map>
#include <string>

static std::map<std::string, std::string> CONFIG_TOOLTIPS = {
    {"TablesDir", "Directory where VPX table files are stored"},
    {"StartArgs", "Command-line arguments to pass before the main command"},
    {"CommandToRun", "Path to the VPinballX executable"},
    {"EndArgs", "Command-line arguments to pass after the table path"},
    {"VPinballXIni", "Path to the VPinballX.ini configuration file"},
    {"WindowWidth", "Width of the launcher window in pixels"},
    {"WindowHeight", "Height of the launcher window in pixels"},
    {"WheelImage", "Path to the wheel image relative to table folder"},
    {"TableImage", "Path to the table image relative to table folder"},
    {"BackglassImage", "Path to the backglass image relative to table folder"},
    {"MarqueeImage", "Path to the marquee image relative to table folder"},
    {"TableVideo", "Path to the table video relative to table folder"},
    {"BackglassVideo", "Path to the backglass video relative to table folder"},
    {"DMDVideo", "Path to the DMD video relative to table folder"},
    {"ROMPath", "Path to ROM files relative to table folder"},
    {"AltSoundPath", "Path to alternative sound files relative to table folder"},
    {"AltColorPath", "Path to alternative color files relative to table folder"},
    {"MusicPath", "Path to music files relative to table folder"},
    {"PUPPackPath", "Path to PUP pack files relative to table folder"},
    {"FallbackEditor", "Default text editor to use"},
    {"VpxTool", "Path to the VPX tool utility"}
};

#endif // CONFIG_TOOLTIPS_H