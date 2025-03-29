#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "utils/structures.h"
#include <string>
#include <vector>

class Launcher {
public:
    Launcher(const std::string& tablesDir, const std::string& startArgs, const std::string& commandToRun,
             const std::string& endArgs, const std::string& vpinballXIni);
    void draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested); // Added quitRequested
    int getSelectedTable() const { return selectedTable; }

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
};

#endif // LAUNCHER_H