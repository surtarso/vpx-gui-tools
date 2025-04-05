#include "tables/table_loader.h"
#include <fstream>
#include <iostream>
#include <functional> // For std::hash
#include <thread>     // For parallel parsing

TableLoader::TableLoader(IConfigProvider& config) : config(config) {}

void TableLoader::load(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool forceVpxToolIndex) {
    std::string cachePath = config.getBasePath() + "resources/tables_index.json";
    std::string indexPath = config.getTablesDir() + "/" + config.getVpxtoolIndexFile();
    LOG_DEBUG("Checking cache at " << cachePath << " and index at " << indexPath);

    bool useCache = false;
    if (!forceVpxToolIndex && std::filesystem::exists(cachePath) && std::filesystem::exists(indexPath)) {
        std::ifstream file(cachePath);
        if (!file.is_open()) {
            LOG_DEBUG("Failed to open cache file: " << cachePath);
        } else {
            json j;
            file >> j;
            bool hasRequiredFields = !j["tables"].empty() && 
                                     j["tables"][0].contains("requiresPinmame") && 
                                     j["tables"][0].contains("gameName") && 
                                     j["tables"][0].contains("lastRun");
            file.close();
            if (hasRequiredFields) {
                auto tablesDirLastWrite = std::filesystem::last_write_time(config.getTablesDir()).time_since_epoch().count();
                auto lastUpdated = j["last_updated"].get<long long>();
                std::string cachedHash = j.contains("tables_hash") ? j["tables_hash"].get<std::string>() : "";
                std::string currentHash = computeTablesHash(config.getTablesDir());
                if (lastUpdated >= tablesDirLastWrite && cachedHash == currentHash) {
                    LOG_DEBUG("Cache is valid (hash match: " << cachedHash << "), loading from cache: " << cachePath);
                    useCache = true;
                } else {
                    LOG_DEBUG("Cache outdated (time: " << lastUpdated << " < " << tablesDirLastWrite 
                              << ", hash: " << cachedHash << " != " << currentHash << "), regenerating...");
                }
            } else {
                LOG_DEBUG("Cache lacks required fields, regenerating index...");
                std::filesystem::remove(cachePath);
            }
        }
    } else {
        LOG_DEBUG("Cache or index missing (cache: " << std::filesystem::exists(cachePath) 
                  << ", index: " << std::filesystem::exists(indexPath) << "), generating index...");
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
        entry.lastRun = t.contains("lastRun") ? t["lastRun"].get<std::string>() : "clear";
        tables.push_back(entry);
        LOG_DEBUG("Loaded from cache: " << entry.name << ", requiresPinmame=" << entry.requiresPinmame 
                  << ", gameName=" << entry.gameName << ", rom=" << entry.rom << ", lastRun=" << entry.lastRun);
    }
}

void TableLoader::saveToCache(const std::string& jsonPath, const std::vector<TableEntry>& tables) {
    json j;
    j["last_updated"] = std::chrono::system_clock::now().time_since_epoch().count();
    j["tables_hash"] = computeTablesHash(config.getTablesDir()); // Store hash for validation
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
        j["tables"].push_back(tj);
    }
    std::ofstream file(jsonPath);
    file << j.dump(2);
    LOG_DEBUG("Saved " << tables.size() << " tables to cache: " << jsonPath << " with hash: " << j["tables_hash"]);
}

void TableLoader::generateIndex() {
    std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getIndexerSubCmd() + " \"" + config.getTablesDir() + "\"";
    LOG_DEBUG("Generating index with command: " << cmd);
    int result = system(cmd.c_str());
    if (result != 0) {
        LOG_DEBUG("Failed to index (command: " << cmd << ")");
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
    auto& jt = j["tables"];
    if (jt.empty()) {
        LOG_DEBUG("No tables found in index: " << indexPath);
        return;
    }

    // Parallel parsing
    const size_t numThreads = std::thread::hardware_concurrency();
    const size_t chunkSize = jt.size() / numThreads + (jt.size() % numThreads != 0 ? 1 : 0);
    std::vector<std::thread> threads;
    std::vector<std::vector<TableEntry>> threadTables(numThreads);

    LOG_DEBUG("Parsing " << jt.size() << " tables with " << numThreads << " threads, chunk size=" << chunkSize);
    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = std::min(start + chunkSize, jt.size());
        if (start < end) {
            threads.emplace_back(&TableLoader::parseTableChunk, this, std::ref(jt), std::ref(threadTables[i]), start, end);
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    // Merge thread results
    for (const auto& chunk : threadTables) {
        tables.insert(tables.end(), chunk.begin(), chunk.end());
    }
    LOG_DEBUG("Loaded " << tables.size() << " tables from index");
}

std::string TableLoader::computeTablesHash(const std::string& dir) {
    std::string filenames;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
            if (entry.path().extension() == ".vpx") {
                filenames += entry.path().filename().string(); // Concatenate .vpx filenames
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        LOG_DEBUG("Failed to hash tables dir " << dir << ": " << e.what());
        return "";
    }
    std::string hash = std::to_string(std::hash<std::string>{}(filenames));
    LOG_DEBUG("Computed tables hash for " << dir << ": " << hash);
    return hash;
}

void TableLoader::parseTableChunk(const json& jt, std::vector<TableEntry>& chunk, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        const auto& t = jt[i];
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

        entry.requiresPinmame = t["requires_pinmame"].is_boolean() ? t["requires_pinmame"].get<bool>() : false;
        entry.gameName = t["game_name"].is_string() ? t["game_name"].get<std::string>() : "";
        entry.lastRun = "clear";
        LOG_DEBUG("Parsed in thread " << std::this_thread::get_id() << ": " << entry.name 
                  << ", requiresPinmame=" << entry.requiresPinmame << ", gameName=" << entry.gameName);

        if (!t["path"].is_string()) LOG_DEBUG("Null path for table at " << entry.filepath);
        if (!tableInfo["table_name"].is_string()) LOG_DEBUG("Null table_name for " << entry.filepath);
        if (!tableInfo["author_name"].is_string()) LOG_DEBUG("Null author_name for " << entry.name);
        if (!tableInfo["release_date"].is_string()) LOG_DEBUG("Null release_date for " << entry.name);
        if (!tableInfo["table_version"].is_string()) LOG_DEBUG("Null table_version for " << entry.name);

        chunk.push_back(entry);
    }
}