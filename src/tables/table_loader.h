#ifndef TABLE_LOADER_H
#define TABLE_LOADER_H

#include "utils/logging.h"
#include "utils/structures.h"
#include "config/iconfig_provider.h"
#include <json.hpp>
#include <vector>
#include <filesystem>

using json = nlohmann::json;

class TableLoader {
public:
    TableLoader(IConfigProvider& config);
    void load(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool forceVpxToolIndex = false); // Modified
private:
    void loadFromCache(const std::string& jsonPath, std::vector<TableEntry>& tables);
    void saveToCache(const std::string& jsonPath, const std::vector<TableEntry>& tables);
    void generateIndex();
    void loadTables(std::vector<TableEntry>& tables);
    IConfigProvider& config;
};

#endif // TABLE_LOADER_H