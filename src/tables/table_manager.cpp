#include "tables/table_manager.h"
#include <fstream>
#include <filesystem>

TableManager::TableManager(IConfigProvider& config)
    : config(config), 
      loading(false), 
      tablesLoaded(false),
      loader(config), 
      updater(config, tablesMutex), 
      filter() {}

bool TableManager::hasTablesDirChanged() const {
    std::string tablesDir = config.getTablesDir();
    std::string jsonPath = config.getBasePath() + "resources/tables_index.json";

    if (!std::filesystem::exists(tablesDir) || !std::filesystem::exists(jsonPath)) {
        return true; // If either doesn't exist, we need to refresh
    }

    // Find the most recent modification time of any subdirectory in tables/
    long long mostRecentSubdirWrite = 0;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(tablesDir)) {
            if (entry.is_directory()) {
                auto subdirLastWrite = std::filesystem::last_write_time(entry.path()).time_since_epoch().count();
                if (subdirLastWrite > mostRecentSubdirWrite) {
                    mostRecentSubdirWrite = subdirLastWrite;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        LOG_DEBUG("Error iterating tables directory " << tablesDir << ": " << e.what());
        return true; // If we can't iterate, assume a change to be safe
    }

    std::ifstream file(jsonPath);
    json j;
    file >> j;
    auto lastUpdated = j["last_updated"].get<long long>();

    return mostRecentSubdirWrite > lastUpdated;
}

void TableManager::loadTables() {
    if (!tablesLoaded) {
        loader.load(tables, filteredTables);
        tablesLoaded = true;
        updateTablesAsync();
    }
    filter.filterTables(tables, filteredTables, "");
}

void TableManager::filterTables(const std::string& query) {
    filter.filterTables(tables, filteredTables, query);
}

void TableManager::setSortSpecs(int columnIdx, bool ascending) {
    filter.setSortSpecs(columnIdx, ascending);
}

void TableManager::updateTablesAsync() {
    updater.updateTablesAsync(tables, filteredTables, loading);
}

void TableManager::updateTableLastRun(const std::string& filepath, const std::string& status) {
    std::lock_guard<std::mutex> lock(tablesMutex);
    bool found = false;
    
    for (size_t i = 0; i < tables.size(); ++i) {
        if (tables[i].filepath == filepath) {
            tables[i].lastRun = status;
            if (status == "success") {
                tables[i].playCount++; // Increment on success
            }
            LOG_DEBUG("Updated table " << tables[i].name << " (" << filepath << "): lastRun=" << status << ", playCount=" << tables[i].playCount);
            found = true;
            break;
        }
    }
    
    for (size_t i = 0; i < filteredTables.size(); ++i) {
        if (filteredTables[i].filepath == filepath) {
            filteredTables[i].lastRun = status;
            if (status == "success") {
                filteredTables[i].playCount++;
            }
            break;
        }
    }
    
    if (!found) {
        LOG_DEBUG("Table not found for filepath: " << filepath);
        return;
    }
    
    saveToCache();
}

void TableManager::refreshTables(bool forceFullRefresh) {
    std::lock_guard<std::mutex> lock(tablesMutex);
    bool forceVpxToolIndex = forceFullRefresh || hasTablesDirChanged();
    tablesLoaded = false; // Force reload
    tables.clear();
    filteredTables.clear();
    loader.load(tables, filteredTables, forceVpxToolIndex); // Pass forceVpxToolIndex to loader
    tablesLoaded = true;
    updateTablesAsync();
}

void TableManager::saveToCache() {
    std::string jsonPath = config.getBasePath() + "resources/tables_index.json";
    json j;
    j["last_updated"] = std::chrono::system_clock::now().time_since_epoch().count();
    for (const auto& t : tables) {
        json tj;
        tj["filepath"] = t.filepath;
        tj["filename"] = t.filename;
        tj["year"] = t.year;
        tj["brand"] = t.brand;
        tj["name"] = t.name;
        tj["version"] = t.version;
        tj["extraFiles"] = t.extraFiles;
        tj["rom"] = t.rom;
        tj["udmd"] = t.udmd;
        tj["alts"] = t.alts;
        tj["altc"] = t.altc;
        tj["pup"] = t.pup;
        tj["music"] = t.music;
        tj["images"] = t.images;
        tj["videos"] = t.videos;
        tj["vbsModified"] = t.vbsModified;
        tj["iniModified"] = t.iniModified;
        tj["requiresPinmame"] = t.requiresPinmame;
        tj["gameName"] = t.gameName;
        tj["lastRun"] = t.lastRun;
        tj["playCount"] = t.playCount;
        j["tables"].push_back(tj);
    }
    std::ofstream file(jsonPath);
    file << j.dump(2);
    LOG_DEBUG("Saved " << tables.size() << " tables to cache: " << jsonPath);
}