#include "tools/ini_editor.h"
#include "imgui.h"
#include <fstream>
#include <iostream>

IniEditor::IniEditor(const std::string& initialFile) : currentIniFile(initialFile) {
    initExplanations();
    loadIniFile(initialFile);
    if (!sections.empty()) currentSection = sections[0];
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

void IniEditor::draw() {
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
        saveIniFile();
        showSavedMessage = true;
        savedMessageTimer = ImGui::GetTime();
    }
    ImGui::SameLine();
    if (showSavedMessage) {
        ImGui::Text("Saved!");
        if (ImGui::GetTime() - savedMessageTimer > 2.0) showSavedMessage = false;
    }

    ImGui::End();
}

void IniEditor::initExplanations() {
    explanations["VPRegPath"] = "This can be used to override the VPinball table settings directory location.";
    explanations["PinMAMEPath"] = "Specifies the directory where the PinMAME emulator is located.";
}