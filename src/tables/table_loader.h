#ifndef TABLE_LOADER_H
#define TABLE_LOADER_H

#include "utils/logging.h"
#include "utils/structures.h"
#include "config/iconfig_provider.h"
#include <json.hpp>
#include <vector>
#include <filesystem>
#include <string>

using json = nlohmann::json;

class TableLoader {
public:
    // Constructor: Initializes with a config provider reference
    TableLoader(IConfigProvider& config);

    // Loads table data into tables and filteredTables, with optional force re-indexing
    // tables: Vector to store loaded table entries
    // filteredTables: Vector to store filtered table entries (initially same as tables)
    // forceVpxToolIndex: If true, skips cache and regenerates index
    void load(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool forceVpxToolIndex = false);

private:
    // Loads table data from cached JSON file
    // jsonPath: Path to the cache file (tables_index.json)
    // tables: Vector to populate with cached table entries
    void loadFromCache(const std::string& jsonPath, std::vector<TableEntry>& tables);

    // Saves table data to cached JSON file with timestamp and hash
    // jsonPath: Path to save the cache file
    // tables: Vector of table entries to save
    void saveToCache(const std::string& jsonPath, const std::vector<TableEntry>& tables);

    // Generates table index using external vpxtool
    void generateIndex();

    // Loads table data from vpxtool index file, parsing in parallel
    // tables: Vector to populate with parsed table entries
    void loadTables(std::vector<TableEntry>& tables);

    // Computes a hash of .vpx filenames in the tables directory for cache validation
    // dir: Directory path to scan for .vpx files
    // Returns: String hash of concatenated filenames
    std::string computeTablesHash(const std::string& dir);

    // Parses a chunk of JSON table data in a separate thread
    // jt: JSON array of tables from index file
    // chunk: Vector to store parsed table entries for this chunk
    // start: Starting index in jt for this chunk
    // end: Ending index in jt for this chunk
    void parseTableChunk(const json& jt, std::vector<TableEntry>& chunk, size_t start, size_t end, std::map<std::string, std::string>& cachedLastRun);

    IConfigProvider& config; // Reference to configuration provider
};

#endif // TABLE_LOADER_H