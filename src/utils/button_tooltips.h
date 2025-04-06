// utils/button_tooltips.h
#ifndef BUTTON_TOOLTIPS_H
#define BUTTON_TOOLTIPS_H

#include <imgui.h>
#include <string>
#include <unordered_map>

class ButtonTooltips {
public:
    ButtonTooltips() {
        tooltips["⛭"] = "Open settings.";
        tooltips["INI Editor"] = "Edit the INI file for the selected table.\nIf no table selected, open global VPinballX settings.";
        tooltips["Extract VBS"] = "Extract the VBS script for the selected table.\nIf script exists, opens the VBS script in a default editor.";
        tooltips["Open Folder"] = "Open the folder of the selected table.\nIf no table selected, opens the tables root directory.";
        tooltips["Refresh"] = "Refresh table list.\nShift + Click for a full refresh (re-indexes all tables).";
        tooltips["▶ Play"] = "Launch the selected table with VPinballX.";
        tooltips["X"] = "Clear the search query.";
        tooltips["✖ Quit"] = "Exit the application.";
    }

    void renderTooltip(const std::string& buttonLabel) {
        if (ImGui::IsItemHovered() && tooltips.count(buttonLabel)) {
            ImVec2 buttonPos = ImGui::GetItemRectMin();
            ImVec2 buttonSize = ImGui::GetItemRectSize();
            ImVec2 tooltipSize = ImGui::CalcTextSize(tooltips[buttonLabel].c_str());
            tooltipSize.x += ImGui::GetStyle().WindowPadding.x * 2;
            tooltipSize.y += ImGui::GetStyle().WindowPadding.y * 2;

            // Force position above button, centered horizontally
            ImVec2 tooltipPos = ImVec2(
                buttonPos.x + (buttonSize.x - tooltipSize.x) / 2, // Center on button width
                buttonPos.y - tooltipSize.y - 5.0f                // 5px above button
            );

            // Clamp to window bounds
            ImVec2 windowSize = ImGui::GetIO().DisplaySize;
            if (tooltipPos.y < 0) tooltipPos.y = buttonPos.y + buttonSize.y + 5.0f; // Flip below if clipping top
            if (tooltipPos.x < 0) tooltipPos.x = 0;
            if (tooltipPos.x + tooltipSize.x > windowSize.x) tooltipPos.x = windowSize.x - tooltipSize.x;

            // Use a custom popup instead of BeginTooltip to break free of ImGui's defaults
            ImGui::SetNextWindowPos(tooltipPos, ImGuiCond_Always);
            ImGui::Begin(("TT_" + buttonLabel).c_str(), nullptr, 
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | 
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("%s", tooltips[buttonLabel].c_str());
            ImGui::End();
        }
    }

private:
    std::unordered_map<std::string, std::string> tooltips;
};

#endif // BUTTON_TOOLTIPS_H