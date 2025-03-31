#include "tables/table_loader.h"
#include "utils/logging.h"
#include <fstream>
#include <iostream>

TableLoader::TableLoader(IConfigProvider& config) : config(config) {}

void TableLoader::load(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables) {
    std::string cachePath = config.getBasePath() + "resources/tables_index.json";
    std::string indexPath = config.getTablesDir() + "/" + config.getVpxtoolIndexFile();
    LOG_DEBUG("Checking cache at " << cachePath << " and index at " << indexPath);

    bool useCache = false;
    if (std::filesystem::exists(cachePath) && std::filesystem::exists(indexPath)) {
        std::ifstream file(cachePath);
        if (!file.is_open()) {
            LOG_DEBUG("Failed to open cache file: " << cachePath);
        } else {
            json j;
            file >> j;
            // Check if the cache contains the new fields
            bool hasNewFields = !j["tables"].empty() && j["tables"][0].contains("requiresPinmame") && j["tables"][0].contains("gameName");
            file.close();
            if (hasNewFields) {
                LOG_DEBUG("Cache is valid, loading from cache: " << cachePath);
                useCache = true;
            } else {
                LOG_DEBUG("Cache is outdated (missing requiresPinmame/gameName), regenerating index...");
                std::filesystem::remove(cachePath);
            }
        }
    } else {
        LOG_DEBUG("Cache or index not found (cache exists: " << std::filesystem::exists(cachePath) 
                  << ", index exists: " << std::filesystem::exists(indexPath) << "), generating index...");
    }

    if (useCache) {
        loadFromCache(cachePath, tables);
    } else {
        generateIndex();
        loadTables(tables);
        saveToCache(cachePath, tables);
    }
    filteredTables = tables;
    std::cerr << "Loaded " << tables.size() << " tables." << std::endl;
}

void TableLoader::loadFromCache(const std::string& jsonPath, std::vector<TableEntry>& tables) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        LOG_DEBUG("Could not open cache: " << jsonPath);
        loadTables(tables); // Fallback to full load
        return;
    }
    json j;
    file >> j;
    tables.clear();
    for (const auto& t : j["tables"]) {
        TableEntry entry;
        entry.filepath = t["filepath"];
        entry.filename = t["filename"];
        entry.year = t["year"];
        entry.brand = t["brand"];
        entry.name = t["name"];
        entry.version = t["version"];
        entry.extraFiles = t["extraFiles"];
        entry.rom = t["rom"];
        entry.udmd = t["udmd"];
        entry.alts = t["alts"];
        entry.altc = t["altc"];
        entry.pup = t["pup"];
        entry.music = t["music"];
        entry.images = t["images"];
        entry.videos = t["videos"];
        entry.vbsModified = t["vbsModified"];
        entry.iniModified = t["iniModified"];
        entry.requiresPinmame = t.contains("requiresPinmame") ? t["requiresPinmame"].get<bool>() : false;
        entry.gameName = t.contains("gameName") ? t["gameName"].get<std::string>() : "";
        tables.push_back(entry);
        LOG_DEBUG("Loaded from cache: " << entry.name << ", requiresPinmame=" << entry.requiresPinmame 
                  << ", gameName=" << entry.gameName << ", rom=" << entry.rom);
    }
}

void TableLoader::saveToCache(const std::string& jsonPath, const std::vector<TableEntry>& tables) {
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
        j["tables"].push_back(tj);
    }
    std::ofstream file(jsonPath);
    file << j.dump(2);
    LOG_DEBUG("Saved " << tables.size() << " tables to cache: " << jsonPath);
}

void TableLoader::generateIndex() {
    std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getIndexerSubCmd() + " \"" + config.getTablesDir() + "\"";
    LOG_DEBUG("Generating index with command: " << cmd);
    int result = system(cmd.c_str());
    if (result != 0) {
        LOG_DEBUG("Failed to index." << " (command: " << cmd << ")");
    }
    
}

void TableLoader::loadTables(std::vector<TableEntry>& tables) {
    tables.clear();
    std::string indexPath = config.getTablesDir() + "/" + config.getVpxtoolIndexFile();
    if (!std::filesystem::exists(indexPath)) {
        std::cerr << "Index file not found: " << indexPath << std::endl;
        return;
    }

    std::ifstream file(indexPath);
    json j;
    file >> j;
    for (const auto& t : j["tables"]) {
        TableEntry entry;
        entry.filepath = t["path"].is_string() ? t["path"].get<std::string>() : "Unknown";
        entry.filename = std::filesystem::path(entry.filepath).stem().string();

        auto tableInfo = t["table_info"];
        entry.name = tableInfo["table_name"].is_string() && !tableInfo["table_name"].get<std::string>().empty() 
                     ? tableInfo["table_name"].get<std::string>() : entry.filename;
        entry.brand = tableInfo["author_name"].is_string() ? tableInfo["author_name"].get<std::string>() : "Unknown";
        std::string release = tableInfo["release_date"].is_string() ? tableInfo["release_date"].get<std::string>() : "";
        if (release.size() >= 10 && std::all_of(release.begin() + 6, release.begin() + 10, ::isdigit)) {
            entry.year = release.substr(6, 4); // "14.12.2024" → "2024"
        } else {
            entry.year = "Unknown";
            if (!release.empty()) LOG_DEBUG("Invalid release_date for " << entry.name << ": " << release);
        }
        entry.version = tableInfo["table_version"].is_string() ? tableInfo["table_version"].get<std::string>() : "Unknown";

        // Extract ROM metadata
        entry.requiresPinmame = t["requires_pinmame"].is_boolean() ? t["requires_pinmame"].get<bool>() : false;
        entry.gameName = t["game_name"].is_string() ? t["game_name"].get<std::string>() : "";
        LOG_DEBUG("Loaded from index: " << entry.name << ", requiresPinmame=" << entry.requiresPinmame 
                  << ", gameName=" << entry.gameName);

        if (!t["path"].is_string()) LOG_DEBUG("Null path for table at " << entry.filepath);
        if (!tableInfo["table_name"].is_string()) LOG_DEBUG("Null table_name for " << entry.filepath);
        if (!tableInfo["author_name"].is_string()) LOG_DEBUG("Null author_name for " << entry.name);
        if (!tableInfo["release_date"].is_string()) LOG_DEBUG("Null release_date for " << entry.name);
        if (!tableInfo["table_version"].is_string()) LOG_DEBUG("Null table_version for " << entry.name);

        tables.push_back(entry);
    }
}