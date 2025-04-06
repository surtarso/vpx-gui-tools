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
    void loadTables();
    void filterTables(const std::string& query);
    void setSortSpecs(int columnIdx, bool ascending);
    void updateTablesAsync();
    void updateTableLastRun(const std::string& filepath, const std::string& status); // Replace size_t index version
    void refreshTables(bool forceFullRefresh = false); // Modified to accept forceFullRefresh

    std::vector<TableEntry>& getTables() { return filteredTables; }
    bool isLoading() const { return loading; }

private:
    bool hasTablesDirChanged() const; // New method to check if tables/ folder has changed
    void saveToCache();
    IConfigProvider& config;
    std::vector<TableEntry> tables;
    std::vector<TableEntry> filteredTables;
    bool loading;
    bool tablesLoaded;
    std::mutex tablesMutex;
    TableLoader loader;
    TableUpdater updater;
    TableFilter filter;
};

#endif // TABLE_MANAGER_H