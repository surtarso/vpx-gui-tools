#include "tools/ini_editor.h"
#include "utils/vpx_tooltips.h"
#include "utils/config_tooltips.h"
#include <imgui.h>
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
    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    if (isOpen && !wasOpen) {
        if (!sections.empty()) {
            currentSection = sections[0];
        } else {
            currentSection.clear();
        }
    }
    wasOpen = isOpen;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    std::string windowTitle = isConfigEditor ? "Settings Configuration" : "VPinballX Configuration - Editing: " + std::filesystem::path(currentIniFile).filename().string();
    ImGui::Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Calculate heights
    float buttonHeight = ImGui::GetFrameHeight() * dpiScale + ImGui::GetStyle().ItemSpacing.y * dpiScale;
    float sectionsHeight = ImGui::GetContentRegionAvail().y - buttonHeight;
    if (sectionsHeight < 0) sectionsHeight = 0;

    // Left side: Sections pane and buttons below
    ImGui::BeginGroup();
    ImGui::BeginChild("SectionsPane", ImVec2(200 * dpiScale, sectionsHeight), true);
    for (const auto& section : sections) {
        bool is_selected = (currentSection == section);
        if (ImGui::Selectable(section.c_str(), is_selected)) {
            currentSection = section;
        }
        if (is_selected) {
            ImGui::SetItemDefaultFocus();
        }
    }
    ImGui::EndChild();

    // Buttons below sections pane
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
    ImGui::EndGroup();

    // Right side: Key-values pane, aligned beside sections pane
    ImGui::SameLine();
    ImGui::BeginChild("KeyValues", ImVec2(0, sectionsHeight), true);
    if (iniData.count(currentSection)) {
        for (auto& kv : iniData[currentSection].keyValues) {
            ImGui::PushID(kv.first.c_str());
            
            ImGui::Text("%s", kv.first.c_str());
            ImGui::SameLine();
            
            if (explanations.count(kv.first)) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "?");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f * dpiScale);
                    ImGui::TextWrapped("%s", explanations[kv.first].c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            
            ImGui::SameLine(225 * dpiScale);
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

    ImGui::End();
}

void IniEditor::initExplanations() {
    const auto& source = isConfigEditor ? CONFIG_TOOLTIPS : VPX_TOOLTIPS;
    explanations.clear();
    for (const auto& [key, value] : source) {
        explanations[key] = value;
    }
}