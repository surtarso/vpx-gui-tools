#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "utils/structures.h"
#include "tables/table_manager.h"
#include "config/iconfig_provider.h"
#include "launcher/table_view.h"
#include "launcher/table_actions.h"
#include <imgui.h>
#include <string>
#include <vector>

class Launcher {
public:
    Launcher(IConfigProvider& config, TableManager* tm = nullptr);
    void draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt, bool& showNoTablePopup);
    int getSelectedTable() const { return tableView.getSelectedTable(); }
    std::string getSearchQuery() const { return searchQuery; }
    std::string getSelectedIniPath() const { return selectedIniPath; }
    bool shouldCreateIni() const { return createIniConfirmed; }
    void resetCreateIni() { createIniConfirmed = false; }

private:
    IConfigProvider& config;
    TableManager* tableManager;
    TableView tableView;
    TableActions tableActions;
    std::string searchQuery;
    bool createIniConfirmed;
    std::string selectedIniPath;
};

#endif // LAUNCHER_H