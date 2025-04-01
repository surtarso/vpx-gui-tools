#include "tools/ini_editor.h"
#include "utils/vpx_tooltips.h"
#include "utils/config_tooltips.h"
#include "imgui.h"
#include <fstream>
#include <iostream>
#include <filesystem>

IniEditor::IniEditor(const std::string& initialFile, bool isConfigEditor) 
    : currentIniFile(initialFile), isConfigEditor(isConfigEditor), wasOpen(false) {
    initExplanations();
    loadIniFile(initialFile);
}

void IniEditor::loadIniFile(const std::string& filename) {
    iniData.clear();
    sections.clear();
    originalLines.clear();
    lineToKey.clear();
    currentIniFile = filename;

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

    // Reset currentSection to the first section after loading the file
    if (!sections.empty()) {
        currentSection = sections[0];
    } else {
        currentSection.clear();
    }
}

void IniEditor::saveIniFile() {
    std::ofstream file(currentIniFile);
    if (!file.is_open()) {
        std::cerr << "Could not write " << currentIniFile << std::endl;
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

void IniEditor::draw(bool& isOpen) {
    // Get the DPI scaling factor from ImGui
    ImGuiIO& io = ImGui::GetIO();
    float dpiScale = io.FontGlobalScale; // Use the global font scale as the DPI scale
    if (dpiScale <= 0.0f) dpiScale = 1.0f; // Fallback to 1.0 if invalid

    // Detect when the editor is opened (isOpen transitions from false to true)
    if (isOpen && !wasOpen) {
        // Reset to the first section when the editor is opened
        if (!sections.empty()) {
            currentSection = sections[0];
        } else {
            currentSection.clear();
        }
    }
    wasOpen = isOpen;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    // Set the window title dynamically
    std::string windowTitle;
    if (isConfigEditor) {
        windowTitle = "Settings Configuration";
    } else {
        std::string filename = std::filesystem::path(currentIniFile).filename().string();
        windowTitle = "VPinballX Configuration - Editing: " + filename;
    }

    ImGui::Begin(windowTitle.c_str(), nullptr, 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginCombo("Section", currentSection.c_str())) {
        for (const auto& section : sections) {
            bool is_selected = (currentSection == section);
            if (ImGui::Selectable(section.c_str(), is_selected)) currentSection = section;
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    float buttonHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;
    float availableHeight = ImGui::GetContentRegionAvail().y;
    float childHeight = availableHeight - buttonHeight;
    if (childHeight < 0) childHeight = 0;

    ImGui::BeginChild("KeyValues", ImVec2(0, childHeight), true);
    if (iniData.count(currentSection)) {
        for (auto& kv : iniData[currentSection].keyValues) {
            ImGui::PushID(kv.first.c_str());
            
            ImGui::Text("%s", kv.first.c_str());
            ImGui::SameLine();
            
            if (explanations.count(kv.first)) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "?");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
                    ImGui::TextWrapped("%s", explanations[kv.first].c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            
            ImGui::SameLine(225 * dpiScale); // Scale the position of the input field
            char buf[256];
            strncpy(buf, kv.second.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            ImGui::PushItemWidth(-1);
            if (ImGui::InputText("", buf, sizeof(buf))) kv.second = buf;
            ImGui::PopItemWidth();
            
            ImGui::PopID();
            ImGui::NewLine();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("Save")) {
        saveIniFile();
        showSavedMessage = true;
        savedMessageTimer = ImGui::GetTime();
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        isOpen = false;
    }
    ImGui::SameLine();
    if (showSavedMessage) {
        ImGui::Text("Saved!");
        if (ImGui::GetTime() - savedMessageTimer > 2.0) showSavedMessage = false;
    }

    ImGui::End();
}

void IniEditor::initExplanations() {
    explanations = isConfigEditor ? CONFIG_TOOLTIPS : VPX_TOOLTIPS;
}