#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "utils/structures.h"
#include "config/iconfig_provider.h"
#include "tables/table_loader.h"
#include "tables/table_updater.h"
#include "tables/table_filter.h"
#include <vector>
#include <string>
#include <mutex>

class TableManager {
public:
    TableManager(IConfigProvider& config);
    void loadTables(); // Explicit method to load tables
    void filterTables(const std::string& query);
    void setSortSpecs(int columnIdx, bool ascending);
    void updateTablesAsync();

    std::vector<TableEntry>& getTables() { return filteredTables; }
    bool isLoading() const { return loading; }

private:
    IConfigProvider& config;
    std::vector<TableEntry> tables;
    std::vector<TableEntry> filteredTables;
    bool loading;
    bool tablesLoaded; // Track whether tables have been loaded
    std::mutex tablesMutex;
    TableLoader loader;
    TableUpdater updater;
    TableFilter filter;
};

#endif // TABLE_MANAGER_H