#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "utils/button_tooltips.h"
#include "config/iconfig_provider.h"
#include "tables/table_manager.h"
#include "launcher/table_view.h"
#include "launcher/table_actions.h"
#include "imgui.h"
#include <string>
#include <SDL.h>

class Launcher {
public:
    Launcher(IConfigProvider& config, TableManager* tm, SDL_Renderer* renderer);
    void draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt, bool& showNoTablePopup);
    std::string getSelectedIniPath() const { return selectedIniPath; }
    bool getCreateIniConfirmed() const { return createIniConfirmed; }
    void setCreateIniConfirmed(bool confirmed) { createIniConfirmed = confirmed; }
    std::string getSearchQuery() const { return searchQuery; }
private:
    bool isShiftKeyDown() const;
    void handlePendingOperations(std::vector<TableEntry>& tables);
    IConfigProvider& config;
    TableManager* tableManager;
    SDL_Renderer* renderer;
    TableView tableView;
    TableActions tableActions;
    std::string searchQuery;
    bool createIniConfirmed;
    std::string selectedIniPath;
    ButtonTooltips buttonTooltips;
    std::string feedbackMessage;
    float feedbackMessageTimer = 0.0f;
    const float FEEDBACK_MESSAGE_DURATION = 1.5f; // Reduced to 1.5 seconds as requested
    bool pendingExtractVBS = false;
    bool pendingPlay = false;
    size_t pendingTableIndex = static_cast<size_t>(-1); // Changed to size_t
    float delayTimer = 0.0f; // New timer to delay the operation
    const float DELAY_DURATION = 1.5f; // Delay for 1.5 seconds before starting the operation
};

#endif // LAUNCHER_H