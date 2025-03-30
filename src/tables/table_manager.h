#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "utils/structures.h"
#include "config/iconfig_provider.h"
#include <vector>
#include <string>

class TableManager {
public:
    TableManager(IConfigProvider& config);
    void loadTables();
    void filterTables(const std::string& query);
    void setSortSpecs(int columnIdx, bool ascending);

    std::vector<TableEntry>& getTables() { return filteredTables; }

private:
    IConfigProvider& config;
    std::vector<TableEntry> tables;
    std::vector<TableEntry> filteredTables;
    int sortColumn = 0;
    bool sortAscending = true;
};

#endif // TABLE_MANAGER_H