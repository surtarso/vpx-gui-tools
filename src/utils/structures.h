#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <map>

struct ConfigSection {
    std::vector<std::pair<std::string, std::string>> keyValues;
    std::map<std::string, size_t> keyToLineIndex;
};

struct TableEntry {
    std::string year;
    std::string brand;
    std::string name;
    std::string version;    // Table version from vpxtool
    std::string extraFiles; // INI, VBS, B2S status
    std::string rom;        // ROM status and name
    std::string udmd;       // UltraDMD status
    std::string alts;       // AltSound status
    std::string altc;       // AltColor status
    std::string pup;        // PUPPack status
    std::string music;      // Music status
    std::string images;     // Image status
    std::string videos;     // Video status
    std::string filename;   // Full basename
    std::string filepath;   // Full file path
    bool vbsModified = false; // VBS differs from internal
    bool iniModified = false; // INI differs from default
    bool requiresPinmame = false; // Does the table require a ROM?
    std::string gameName;         // ROM game name (e.g., "sprk_103")
};

#endif // STRUCTURES_H