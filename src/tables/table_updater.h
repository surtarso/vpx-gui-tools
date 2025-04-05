#ifndef TABLE_UPDATER_H
#define TABLE_UPDATER_H

#include "utils/logging.h"
#include "utils/structures.h"
#include "config/iconfig_provider.h"
#include <json.hpp>
#include <vector>
#include <mutex>

using json = nlohmann::json;

class TableUpdater {
public:
    // Constructor: Initializes with config provider and mutex for thread safety
    TableUpdater(IConfigProvider& config, std::mutex& mutex);

    // Updates table metadata asynchronously in a detached thread
    // tables: Vector of table entries to update
    // filteredTables: Vector to store updated filtered entries
    // loading: Flag to indicate update in progress
    void updateTablesAsync(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool& loading);

private:
    // Updates a chunk of tables (file checks, ROMs, diffs) in a separate thread
    // tables: Vector of table entries to update
    // start: Starting index of the chunk
    // end: Ending index of the chunk
    void updateChunk(std::vector<TableEntry>& tables, size_t start, size_t end);

    // Checks ROM availability for a chunk of tables (kept for reference, not used in full threading)
    void checkRomForChunk(std::vector<TableEntry>& tables, size_t start, size_t end);

    IConfigProvider& config; // Reference to configuration provider
    std::mutex& tablesMutex; // Mutex for thread-safe table access
};

#endif // TABLE_UPDATER_H