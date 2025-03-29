#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "utils/structures.h"
#include "tables/table_manager.h"  // Need TableManager for pointer
#include <string>
#include <vector>

class Launcher {
public:
    Launcher(const std::string& tablesDir, const std::string& startArgs, const std::string& commandToRun,
             const std::string& endArgs, const std::string& vpinballXIni, TableManager* tm = nullptr);
    void draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested);
    int getSelectedTable() const { return selectedTable; }
    std::string getSearchQuery() const { return searchQuery; }

private:
    void launchTable(const std::string& filepath);
    void extractVBS(const std::string& filepath);
    void openFolder(const std::string& filepath);

    std::string tablesDir;
    std::string startArgs;
    std::string commandToRun;
    std::string endArgs;
    std::string vpinballXIni;
    std::string searchQuery;
    int selectedTable = -1;
    TableManager* tableManager;  // Pointer to TableManager instance
};

#endif // LAUNCHER_H