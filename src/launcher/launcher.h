#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "utils/structures.h"
#include "tables/table_manager.h"
#include <string>
#include <vector>

class Launcher {
public:
    Launcher(const std::string& tablesDir, const std::string& startArgs, const std::string& commandToRun,
             const std::string& endArgs, const std::string& vpinballXIni, const std::string& vpxTool,
             const std::string& fallbackEditor, const std::string& vbsSubCmd, const std::string& playSubCmd,
             TableManager* tm = nullptr);
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

    std::string tablesDir;
    std::string startArgs;
    std::string commandToRun;
    std::string endArgs;
    std::string vpinballXIni;
    std::string vpxTool;
    std::string fallbackEditor;
    std::string vbsSubCmd;
    std::string playSubCmd;
    std::string searchQuery;
    int selectedTable = -1;
    bool createIniConfirmed = false;
    TableManager* tableManager;
    std::string selectedIniPath;
};

#endif // LAUNCHER_H