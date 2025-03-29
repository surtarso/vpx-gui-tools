// vpx_gui_tools.cpp
// Combined VPX Table Launcher and Configuration Editor
// Tarso Galvão - Mar/2025 (rewritten for unified app)
// Dependencies: SDL2, OpenGL, ImGui

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
#include <cstdlib>
#include <filesystem>
#include <algorithm>

// Structure for a configuration section
struct ConfigSection {
    std::vector<std::pair<std::string, std::string>> keyValues;
    std::map<std::string, size_t> keyToLineIndex;
};

// Structure for a table entry
struct TableEntry {
    std::string year;
    std::string brand;
    std::string name;
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
};

// Main application class
class VPXGuiTools {
public:
    VPXGuiTools();
    ~VPXGuiTools();
    void run();

private:
    void loadSettings();
    void loadIniFile(const std::string& filename, std::map<std::string, ConfigSection>& iniData,
                     std::vector<std::string>& sections, std::vector<std::string>& originalLines,
                     std::map<size_t, std::pair<std::string, std::string>>& lineToKey);
    void saveIniFile(const std::string& filename, const std::map<std::string, ConfigSection>& iniData,
                     const std::vector<std::string>& originalLines,
                     const std::map<size_t, std::pair<std::string, std::string>>& lineToKey);
    void initExplanations();
    void loadTables();
    void drawLauncher();
    void drawIniEditor();
    void launchTable(const std::string& filepath);
    void extractVBS(const std::string& filepath);
    void openFolder(const std::string& filepath);

    // Configuration settings
    std::string tablesDir;
    std::string startArgs;
    std::string commandToRun;
    std::string endArgs;
    std::string vpinballXIni;
    std::string fallbackEditor;
    int windowWidth = 1024;
    int windowHeight = 768;
    std::string wheelImage;
    std::string tableImage;
    std::string backglassImage;
    std::string marqueeImage;
    std::string tableVideo;
    std::string backglassVideo;
    std::string dmdVideo;
    std::string romPath;
    std::string altSoundPath;
    std::string altColorPath;
    std::string musicPath;
    std::string pupPackPath;

    // INI Editor data
    std::map<std::string, ConfigSection> iniData;
    std::vector<std::string> sections;
    std::map<std::string, std::string> explanations;
    std::string currentSection;
    std::string currentIniFile;
    std::vector<std::string> originalLines;
    std::map<size_t, std::pair<std::string, std::string>> lineToKey;
    bool showSavedMessage = false;
    double savedMessageTimer = 0.0;

    // Launcher data
    std::vector<TableEntry> tables;
    std::vector<TableEntry> filteredTables;
    std::string searchQuery;
    int selectedTable = -1;
    bool editingIni = false;
    bool exitRequested = false;
};

// Implementation
VPXGuiTools::VPXGuiTools() {
    loadSettings();
    loadTables();
    initExplanations();
    currentIniFile = vpinballXIni;
    loadIniFile(currentIniFile, iniData, sections, originalLines, lineToKey);
    if (!sections.empty()) currentSection = sections[0];
}

VPXGuiTools::~VPXGuiTools() {}

void VPXGuiTools::loadSettings() {
    const char* homeDir = std::getenv("HOME");
    std::string defaultIniPath = std::string(homeDir) + "/.vpinball/VPinballX.ini";
    std::string configFile = "resources/settings.ini"; // Updated path

    if (!std::filesystem::exists(configFile)) {
        std::filesystem::create_directory("resources"); // Create resources dir if it doesn't exist
        std::ofstream out(configFile);
        out << "[VPinballX]\n"
            << "TablesDir=" << homeDir << "/Games/VPX_Tables/\n"
            << "StartArgs=\n"
            << "CommandToRun=" << homeDir << "/Games/vpinball/build/VPinballX_GL\n"
            << "EndArgs=\n"
            << "VPinballXIni=" << defaultIniPath << "\n"
            << "\n[Tools]\n"
            << "FallbackEditor=code\n"
            << "\n[LauncherWindow]\n"
            << "WindowWidth=1024\n"
            << "WindowHeight=768\n"
            << "\n[Images]\n"
            << "WheelImage=/images/wheel.png\n"
            << "TableImage=/images/table.png\n"
            << "BackglassImage=/images/backglass.png\n"
            << "MarqueeImage=/images/marquee.png\n"
            << "\n[Videos]\n"
            << "TableVideo=/video/table.mp4\n"
            << "BackglassVideo=/video/backglass.mp4\n"
            << "DMDVideo=/video/dmd.mp4\n"
            << "\n[ExtraFolders]\n"
            << "ROMPath=/pinmame/roms\n"
            << "AltSoundPath=/pinmame/altsound\n"
            << "AltColorPath=/pinmame/AltColor\n"
            << "MusicPath=/music\n"
            << "PUPPackPath=/pupvideos\n";
        out.close();
    }

    std::ifstream file(configFile);
    std::string line, currentSection;
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Check for section headers
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }

        // Parse key-value pairs within a section
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));

        // Assign values based on section and key
        if (currentSection == "VPinballX") {
            if (key == "TablesDir") tablesDir = value;
            else if (key == "StartArgs") startArgs = value;
            else if (key == "CommandToRun") commandToRun = value;
            else if (key == "EndArgs") endArgs = value;
            else if (key == "VPinballXIni") vpinballXIni = value;
        } else if (currentSection == "Tools") {
            if (key == "FallbackEditor") fallbackEditor = value;
        } else if (currentSection == "LauncherWindow") {
            if (key == "WindowWidth") windowWidth = std::stoi(value);
            else if (key == "WindowHeight") windowHeight = std::stoi(value);
        } else if (currentSection == "Images") {
            if (key == "WheelImage") wheelImage = value;
            else if (key == "TableImage") tableImage = value;
            else if (key == "BackglassImage") backglassImage = value;
            else if (key == "MarqueeImage") marqueeImage = value;
        } else if (currentSection == "Videos") {
            if (key == "TableVideo") tableVideo = value;
            else if (key == "BackglassVideo") backglassVideo = value;
            else if (key == "DMDVideo") dmdVideo = value;
        } else if (currentSection == "ExtraFolders") {
            if (key == "ROMPath") romPath = value;
            else if (key == "AltSoundPath") altSoundPath = value;
            else if (key == "AltColorPath") altColorPath = value;
            else if (key == "MusicPath") musicPath = value;
            else if (key == "PUPPackPath") pupPackPath = value;
        }
    }
}

void VPXGuiTools::loadIniFile(const std::string& filename, std::map<std::string, ConfigSection>& iniData,
                              std::vector<std::string>& sections, std::vector<std::string>& originalLines,
                              std::map<size_t, std::pair<std::string, std::string>>& lineToKey) {
    iniData.clear();
    sections.clear();
    originalLines.clear();
    lineToKey.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << std::endl;
        return;
    }

    std::string line, currentSectionName;
    size_t lineIndex = 0;
    while (std::getline(file, line)) {
        originalLines.push_back(line);
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) { lineIndex++; continue; }
        std::string trimmedLine = line.substr(start);

        if (trimmedLine.empty() || trimmedLine[0] == ';') { lineIndex++; continue; }
        if (trimmedLine.front() == '[' && trimmedLine.back() == ']') {
            currentSectionName = trimmedLine.substr(1, trimmedLine.size() - 2);
            sections.push_back(currentSectionName);
            iniData[currentSectionName] = ConfigSection();
        } else if (!currentSectionName.empty()) {
            size_t pos = trimmedLine.find('=');
            if (pos != std::string::npos) {
                std::string key = trimmedLine.substr(0, pos);
                std::string value = trimmedLine.substr(pos + 1);
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                iniData[currentSectionName].keyValues.emplace_back(key, value);
                iniData[currentSectionName].keyToLineIndex[key] = lineIndex;
                lineToKey[lineIndex] = {currentSectionName, key};
            }
        }
        lineIndex++;
    }
}

void VPXGuiTools::saveIniFile(const std::string& filename, const std::map<std::string, ConfigSection>& iniData,
                              const std::vector<std::string>& originalLines,
                              const std::map<size_t, std::pair<std::string, std::string>>& lineToKey) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not write " << filename << std::endl;
        return;
    }

    for (size_t i = 0; i < originalLines.size(); ++i) {
        if (lineToKey.count(i)) {
            auto [section, key] = lineToKey.at(i);
            for (const auto& kv : iniData.at(section).keyValues) {
                if (kv.first == key && iniData.at(section).keyToLineIndex.at(key) == i) {
                    file << key << " = " << kv.second << "\n";
                    break;
                }
            }
        } else {
            file << originalLines[i] << "\n";
        }
    }
}

void VPXGuiTools::initExplanations() {
    explanations["VPRegPath"] = "This can be used to override the VPinball table settings directory location.";
    explanations["PinMAMEPath"] = "Specifies the directory where the PinMAME emulator is located.";
    // Add more as needed...
}

void VPXGuiTools::loadTables() {
    tables.clear();
    std::cout << "Checking tablesDir: " << tablesDir << std::endl; // Debug output
    if (!std::filesystem::exists(tablesDir)) {
        std::cerr << "Tables directory does not exist: " << tablesDir << std::endl;
        return; // Exit early with empty tables list
    }

    std::cout << "Tables directory exists. Scanning recursively..." << std::endl; // Debug output
    try {
        int vpxCount = 0; // Track number of .vpx files found
        for (const auto& entry : std::filesystem::recursive_directory_iterator(tablesDir, std::filesystem::directory_options::skip_permission_denied)) {
            if (entry.path().extension() == ".vpx") {
                std::string filepath = entry.path().string();
                std::string basename = entry.path().stem().string();
                std::string folder = entry.path().parent_path().string();

                std::cout << "Found VPX file: " << filepath << std::endl; // Debug output
                vpxCount++;

                // Robust parsing of name, brand, and year
                std::string name, brand, year;
                size_t openParen = basename.find('(');
                size_t closeParen = basename.find(')');
                size_t lastSpace = basename.find_last_of(' ');
                if (openParen != std::string::npos && closeParen != std::string::npos && lastSpace != std::string::npos &&
                    openParen < closeParen && lastSpace > closeParen) {
                    name = basename.substr(0, openParen);
                    name.erase(name.find_last_not_of(" \t") + 1); // Trim trailing spaces
                    brand = basename.substr(openParen + 1, closeParen - openParen - 1);
                    year = basename.substr(lastSpace + 1, 4);
                    if (year.size() != 4 || !std::all_of(year.begin(), year.end(), ::isdigit)) {
                        year = "Unknown";
                    }
                } else {
                    name = basename;
                    brand = "Unknown";
                    year = "Unknown";
                }

                TableEntry table;
                table.year = year;
                table.brand = brand;
                table.name = name;
                table.filename = basename;
                table.filepath = filepath;

                // Check extra files (INI, VBS, B2S)
                std::string iniFile = folder + "/" + basename + ".ini";
                std::string vbsFile = folder + "/" + basename + ".vbs";
                std::string b2sFile = folder + "/" + basename + ".directb2s";
                table.extraFiles = std::string(std::filesystem::exists(iniFile) ? "INI " : "") +
                                   std::string(std::filesystem::exists(vbsFile) ? "VBS " : "") +
                                   std::string(std::filesystem::exists(b2sFile) ? "B2S" : "");

                // Check ROM
                std::string romDir = folder + romPath;
                table.rom = std::filesystem::exists(romDir) ? "ROM" : "";

                // Check UltraDMD, AltSound, etc.
                table.udmd = std::filesystem::exists(folder + "/UltraDMD") ? "uDMD" : ""; // Simplified, adjust if needed
                table.alts = std::filesystem::exists(folder + altSoundPath) ? "AltS" : "";
                table.altc = std::filesystem::exists(folder + altColorPath) ? "AltC" : "";
                table.pup = std::filesystem::exists(folder + pupPackPath) ? "PUP" : "";
                table.music = std::filesystem::exists(folder + musicPath) ? "Mus" : "";

                // Check images and videos
                table.images = std::string(std::filesystem::exists(folder + wheelImage) ? "Wheel " : "") +
                               std::string(std::filesystem::exists(folder + tableImage) ? "Table " : "") +
                               std::string(std::filesystem::exists(folder + backglassImage) ? "B2S " : "") +
                               std::string(std::filesystem::exists(folder + marqueeImage) ? "Marquee" : "");
                table.videos = std::string(std::filesystem::exists(folder + tableVideo) ? "Table " : "") +
                               std::string(std::filesystem::exists(folder + backglassVideo) ? "B2S " : "") +
                               std::string(std::filesystem::exists(folder + dmdVideo) ? "DMD" : "");

                tables.push_back(table);
            }
        }
        std::cout << "Total VPX files found: " << vpxCount << std::endl; // Debug output
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error in loadTables: " << e.what() << std::endl;
    }
    filteredTables = tables;
}

void VPXGuiTools::drawLauncher() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPX GUI Tools", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("Table(s) found: %zu", filteredTables.size());
    char searchBuf[256] = "";
    if (ImGui::InputText("Search", searchBuf, sizeof(searchBuf))) {
        searchQuery = searchBuf;
        filteredTables.clear();
        for (const auto& table : tables) {
            if (table.name.find(searchQuery) != std::string::npos || searchQuery.empty()) {
                filteredTables.push_back(table);
            }
        }
    }

    if (ImGui::BeginTable("Tables", 12, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Year");
        ImGui::TableSetupColumn("Brand");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Extra Files");
        ImGui::TableSetupColumn("ROM");
        ImGui::TableSetupColumn("uDMD");
        ImGui::TableSetupColumn("AltS");
        ImGui::TableSetupColumn("AltC");
        ImGui::TableSetupColumn("PUP");
        ImGui::TableSetupColumn("Music");
        ImGui::TableSetupColumn("Images");
        ImGui::TableSetupColumn("Videos");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < filteredTables.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", filteredTables[i].year.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", filteredTables[i].brand.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%s", filteredTables[i].name.c_str());
            ImGui::TableSetColumnIndex(3); ImGui::Text("%s", filteredTables[i].extraFiles.c_str());
            ImGui::TableSetColumnIndex(4); ImGui::Text("%s", filteredTables[i].rom.c_str());
            ImGui::TableSetColumnIndex(5); ImGui::Text("%s", filteredTables[i].udmd.c_str());
            ImGui::TableSetColumnIndex(6); ImGui::Text("%s", filteredTables[i].alts.c_str());
            ImGui::TableSetColumnIndex(7); ImGui::Text("%s", filteredTables[i].altc.c_str());
            ImGui::TableSetColumnIndex(8); ImGui::Text("%s", filteredTables[i].pup.c_str());
            ImGui::TableSetColumnIndex(9); ImGui::Text("%s", filteredTables[i].music.c_str());
            ImGui::TableSetColumnIndex(10); ImGui::Text("%s", filteredTables[i].images.c_str());
            ImGui::TableSetColumnIndex(11); ImGui::Text("%s", filteredTables[i].videos.c_str());
            if (ImGui::IsItemClicked()) selectedTable = static_cast<int>(i);
        }
        ImGui::EndTable();
    }

    if (ImGui::Button("Settings")) {
        currentIniFile = "resources/settings.ini"; // Updated path
        loadIniFile(currentIniFile, iniData, sections, originalLines, lineToKey);
        currentSection = sections[0];
        editingIni = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("INI Editor")) {
        if (selectedTable >= 0) {
            currentIniFile = filteredTables[selectedTable].filepath.substr(0, filteredTables[selectedTable].filepath.find_last_of('.')) + ".ini";
            if (!std::filesystem::exists(currentIniFile)) std::filesystem::copy(vpinballXIni, currentIniFile);
        } else {
            currentIniFile = vpinballXIni;
        }
        loadIniFile(currentIniFile, iniData, sections, originalLines, lineToKey);
        currentSection = sections[0];
        editingIni = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Extract VBS") && selectedTable >= 0) {
        extractVBS(filteredTables[selectedTable].filepath);
    }
    ImGui::SameLine();
    if (ImGui::Button("Open Folder")) {
        openFolder(selectedTable >= 0 ? filteredTables[selectedTable].filepath : tablesDir);
    }
    ImGui::SameLine();
    if (ImGui::Button("Launch") && selectedTable >= 0) {
        launchTable(filteredTables[selectedTable].filepath);
    }
    ImGui::SameLine();
    if (ImGui::Button("Exit")) {
        exitRequested = true;
    }

    ImGui::End();
}

void VPXGuiTools::drawIniEditor() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPinballX Configuration", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginCombo("Section", currentSection.c_str())) {
        for (const auto& section : sections) {
            if (ImGui::Selectable(section.c_str(), currentSection == section)) currentSection = section;
        }
        ImGui::EndCombo();
    }

    ImGui::BeginChild("KeyValues", ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.y), true);
    if (iniData.count(currentSection)) {
        for (auto& kv : iniData[currentSection].keyValues) {
            ImGui::PushID(kv.first.c_str());
            ImGui::Text("%s", kv.first.c_str());
            ImGui::SameLine(225);
            char buf[256];
            strncpy(buf, kv.second.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            if (ImGui::InputText("", buf, sizeof(buf))) kv.second = buf;
            if (explanations.count(kv.first)) {
                ImGui::SameLine(200);
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "?");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextWrapped("%s", explanations[kv.first].c_str());
                    ImGui::EndTooltip();
                }
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("Save")) {
        saveIniFile(currentIniFile, iniData, originalLines, lineToKey);
        showSavedMessage = true;
        savedMessageTimer = ImGui::GetTime();
    }
    ImGui::SameLine();
    if (ImGui::Button("Back")) editingIni = false;
    ImGui::SameLine();
    if (showSavedMessage) {
        ImGui::Text("Saved!");
        if (ImGui::GetTime() - savedMessageTimer > 2.0) showSavedMessage = false;
    }

    ImGui::End();
}

void VPXGuiTools::launchTable(const std::string& filepath) {
    std::string cmd = startArgs + " \"" + commandToRun + "\" -play \"" + filepath + "\" " + endArgs;
    system(cmd.c_str());
}

void VPXGuiTools::extractVBS(const std::string& filepath) {
    std::string cmd = "\"" + commandToRun + "\" -ExtractVBS \"" + filepath + "\"";
    system(cmd.c_str());
}

void VPXGuiTools::openFolder(const std::string& filepath) {
    std::string folder = filepath.empty() ? tablesDir : filepath.substr(0, filepath.find_last_of('/'));
    std::string cmd = "xdg-open \"" + folder + "\"";
    system(cmd.c_str());
}

void VPXGuiTools::run() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_Window* window = SDL_CreateWindow("VPX GUI Tools", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!exitRequested) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) exitRequested = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (editingIni) drawIniEditor();
        else drawLauncher();

        ImGui::Render();
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

int main() {
    VPXGuiTools app;
    app.run();
    return 0;
}