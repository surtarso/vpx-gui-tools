#ifndef INI_EDITOR_H
#define INI_EDITOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include "utils/structures.h"

class IniEditor {
public:
    IniEditor(const std::string& initialFile, bool isConfigEditor);
    void loadIniFile(const std::string& filename);
    void saveIniFile();
    void draw(bool& isOpen); // Removed needRescale parameter

private:
    void initExplanations();

    std::string currentIniFile;
    bool isConfigEditor;
    std::unordered_map<std::string, std::string> explanations;
    std::unordered_map<std::string, ConfigSection> iniData;
    std::vector<std::string> sections;
    std::string currentSection;
    std::vector<std::string> originalLines;
    std::unordered_map<size_t, std::pair<std::string, std::string>> lineToKey;
    bool wasOpen;
    bool showSavedMessage = false;
    double savedMessageTimer = 0.0;
};

#endif // INI_EDITOR_H