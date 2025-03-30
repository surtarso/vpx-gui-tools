#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "utils/structures.h"
#include "config/iconfig_provider.h"
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

    std::vector<TableEntry>& getTables() { return filteredTables; }
    bool isLoading() const { return loading; }

private:
    void loadFromCache(const std::string& jsonPath);
    void saveToCache(const std::string& jsonPath);
    void generateIndex();
    IConfigProvider& config;
    std::vector<TableEntry> tables;
    std::vector<TableEntry> filteredTables;
    int sortColumn = 0;
    bool sortAscending = true;
    bool loading = false;
    std::mutex tablesMutex;
};

#endif // TABLE_MANAGER_H