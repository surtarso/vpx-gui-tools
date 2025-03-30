#ifndef TABLE_UPDATER_H
#define TABLE_UPDATER_H

#include "utils/structures.h"
#include "config/iconfig_provider.h"
#include <json.hpp>
#include <vector>
#include <mutex>

using json = nlohmann::json;

class TableUpdater {
public:
    TableUpdater(IConfigProvider& config, std::mutex& mutex);
    void updateTablesAsync(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool& loading);
private:
    IConfigProvider& config;
    std::mutex& tablesMutex;
};

#endif // TABLE_UPDATER_H