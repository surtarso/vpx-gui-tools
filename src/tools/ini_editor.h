#ifndef INI_EDITOR_H
#define INI_EDITOR_H

#include "utils/structures.h"
#include <string>
#include <vector>
#include <map>

class IniEditor {
public:
    IniEditor(const std::string& initialFile, bool isConfigEditor = false);
    void loadIniFile(const std::string& filename);
    void saveIniFile();
    void draw(bool& isOpen);  // Added isOpen parameter to control visibility

    std::string getCurrentFile() const { return currentIniFile; }
    bool isEditing() const { return true; } // Always editable for now
    void setEditing(bool editing) { } // Placeholder for toggling

private:
    std::map<std::string, ConfigSection> iniData;
    std::vector<std::string> sections;
    std::map<std::string, std::string> explanations;
    std::string currentSection;
    std::string currentIniFile;
    std::vector<std::string> originalLines;
    std::map<size_t, std::pair<std::string, std::string>> lineToKey;
    bool showSavedMessage = false;
    double savedMessageTimer = 0.0;
    bool isConfigEditor;

    void initExplanations();
};

#endif // INI_EDITOR_H