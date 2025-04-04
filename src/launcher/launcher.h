#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "config/iconfig_provider.h"
#include "tables/table_manager.h"
#include "launcher/table_view.h"
#include "launcher/table_actions.h"
#include "imgui.h"
#include <string>

class Launcher {
public:
    Launcher(IConfigProvider& config, TableManager* tm);
    void draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt, bool& showNoTablePopup);
    std::string getSelectedIniPath() const { return selectedIniPath; }
    bool getCreateIniConfirmed() const { return createIniConfirmed; }
    void setCreateIniConfirmed(bool confirmed) { createIniConfirmed = confirmed; }
    std::string getSearchQuery() const { return searchQuery; }
private:
    bool isShiftKeyDown() const; // New method to check for Shift key
    IConfigProvider& config;
    TableManager* tableManager;
    TableView tableView;
    TableActions tableActions;
    std::string searchQuery;
    bool createIniConfirmed;
    std::string selectedIniPath;
    std::string feedbackMessage;
    float feedbackMessageTimer = 0.0f;
    const float FEEDBACK_MESSAGE_DURATION = 2.0f;
};

#endif // LAUNCHER_H