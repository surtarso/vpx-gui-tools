#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "utils/structures.h"
#include "tables/table_manager.h"
#include "config/iconfig_provider.h"
#include <string>
#include <vector>

class Launcher {
public:
    Launcher(IConfigProvider& config, TableManager* tm = nullptr);
    void draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt);
    int getSelectedTable() const { return selectedTable; }
    std::string getSearchQuery() const { return searchQuery; }
    std::string getSelectedIniPath() const { return selectedIniPath; }
    bool shouldCreateIni() const { return createIniConfirmed; }
    void resetCreateIni() { createIniConfirmed = false; }

private:
    void launchTable(const std::string& filepath);
    void extractVBS(const std::string& filepath);
    void openFolder(const std::string& filepath);
    bool openInExternalEditor(const std::string& filepath);

    IConfigProvider& config;
    std::string searchQuery;
    int selectedTable = -1;
    bool createIniConfirmed = false;
    TableManager* tableManager;
    std::string selectedIniPath;
};

#endif // LAUNCHER_H