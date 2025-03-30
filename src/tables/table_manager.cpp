#include "tables/table_manager.h"
#include <json.hpp> // nlohmann/json
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <array>
#include <fstream>
#include <thread>

using json = nlohmann::json;

TableManager::TableManager(IConfigProvider& config) : config(config) {
    std::string cachePath = config.getBasePath() + "resources/tables_index.json";
    std::string indexPath = config.getTablesDir() + "/" + config.getVpxtoolIndexFile();
    if (std::filesystem::exists(cachePath) && std::filesystem::exists(indexPath)) {
        loadFromCache(cachePath);
    } else {
        generateIndex();
        loadTables();
        saveToCache(cachePath);
    }
    updateTablesAsync();
}

void TableManager::loadFromCache(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "Could not open cache: " << jsonPath << std::endl;
        loadTables(); // Fallback to full load
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
        tables.push_back(entry);
    }
    filteredTables = tables;
    filterTables("");
}

void TableManager::saveToCache(const std::string& jsonPath) {
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
        j["tables"].push_back(tj);
    }
    std::ofstream file(jsonPath);
    file << j.dump(2);
}

void TableManager::generateIndex() {
    std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getIndexerSubCmd() + " \"" + config.getTablesDir() + "\"";
    std::cerr << "Generating index with command: " << cmd << std::endl;
    system(cmd.c_str());
}

void TableManager::loadTables() {
    loading = true;
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

        // Table Info
        auto tableInfo = t["table_info"];
        entry.name = tableInfo["table_name"].is_string() && !tableInfo["table_name"].get<std::string>().empty() 
                     ? tableInfo["table_name"].get<std::string>() : entry.filename;
        entry.brand = tableInfo["author_name"].is_string() ? tableInfo["author_name"].get<std::string>() : "Unknown";
        std::string release = tableInfo["release_date"].is_string() ? tableInfo["release_date"].get<std::string>() : "";
        if (release.size() >= 10 && std::all_of(release.begin() + 6, release.begin() + 10, ::isdigit)) {
            entry.year = release.substr(6, 4); // "14.12.2024" → "2024"
        } else {
            entry.year = "Unknown";
            if (!release.empty()) std::cerr << "Invalid release_date for " << entry.name << ": " << release << std::endl;
        }
        entry.version = tableInfo["table_version"].is_string() ? tableInfo["table_version"].get<std::string>() : "Unknown";

        // Debug null fields
        if (!t["path"].is_string()) std::cerr << "Null path for table at " << entry.filepath << std::endl;
        if (!tableInfo["table_name"].is_string()) std::cerr << "Null table_name for " << entry.filepath << std::endl;
        if (!tableInfo["author_name"].is_string()) std::cerr << "Null author_name for " << entry.name << std::endl;
        if (!tableInfo["release_date"].is_string()) std::cerr << "Null release_date for " << entry.name << std::endl;
        if (!tableInfo["table_version"].is_string()) std::cerr << "Null table_version for " << entry.name << std::endl;

        tables.push_back(entry);
    }
    filteredTables = tables;
    filterTables("");
    loading = false;
}

void TableManager::updateTablesAsync() {
    std::thread([this]() {
        loading = true;
        std::lock_guard<std::mutex> lock(tablesMutex);
        for (auto& table : tables) {
            std::string folder = std::filesystem::path(table.filepath).parent_path().string();
            std::string basename = table.filename;

            std::string iniFile = folder + "/" + basename + ".ini";
            std::string vbsFile = folder + "/" + basename + ".vbs";
            std::string b2sFile = folder + "/" + basename + ".directb2s";

            bool vbsExists = std::filesystem::exists(vbsFile);
            table.vbsModified = false;
            if (vbsExists) {
                std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getDiffSubCmd() + " \"" + table.filepath + "\"";
                std::array<char, 128> buffer;
                std::string result;
                FILE* pipe = popen(cmd.c_str(), "r");
                if (pipe) {
                    while (fgets(buffer.data(), buffer.size(), pipe)) result += buffer.data();
                    pclose(pipe);
                    bool isWhitespace = std::all_of(result.begin(), result.end(), isspace);
                    table.vbsModified = (!result.empty() && !isWhitespace && (result.find("---") != std::string::npos || result.find("+++") != std::string::npos));
                }
            }

            bool iniExists = std::filesystem::exists(iniFile);
            table.iniModified = false;
            if (iniExists) {
                std::string cmd = "diff \"" + config.getVPinballXIni() + "\" \"" + iniFile + "\"";
                FILE* pipe = popen(cmd.c_str(), "r");
                if (pipe) {
                    std::array<char, 128> buffer;
                    std::string result;
                    while (fgets(buffer.data(), buffer.size(), pipe)) result += buffer.data();
                    table.iniModified = (pclose(pipe) != 0);
                }
            }

            // Check ROM - Moved here to ensure it runs for all tables
            table.rom = "";
            std::string indexPath = config.getTablesDir() + "/" + config.getVpxtoolIndexFile();
            std::ifstream file(indexPath);
            if (file.is_open()) {
                json j;
                file >> j;
                for (const auto& t : j["tables"]) {
                    std::string filepath = t["path"].is_string() ? t["path"].get<std::string>() : "Unknown";
                    if (filepath == table.filepath) { // Match the table by filepath
                        bool requiresPinmame = t["requires_pinmame"].is_boolean() ? t["requires_pinmame"].get<bool>() : false;
                        if (requiresPinmame && t["game_name"].is_string()) {
                            std::string gameName = t["game_name"].get<std::string>();
                            std::string romPath = folder + "/" + config.getRomPath() + "/" + gameName + ".zip";
                            std::cerr << "Checking ROM for " << table.name << ": requires_pinmame=" << requiresPinmame 
                                      << ", game_name=" << gameName << ", path=" << romPath << std::endl;
                            if (std::filesystem::exists(romPath)) {
                                table.rom = gameName; // Display the actual ROM name (e.g., "t2")
                                std::cerr << "ROM found for " << table.name << ": " << gameName << std::endl;
                            } else {
                                std::cerr << "ROM not found for " << table.name << " at " << romPath << std::endl;
                            }
                        } else if (requiresPinmame) {
                            std::cerr << "Missing or null game_name for " << table.name << std::endl;
                        } else {
                            std::cerr << "No ROM required for " << table.name << std::endl;
                        }
                        break;
                    }
                }
                file.close();
            } else {
                std::cerr << "Could not reopen index file for ROM check: " << indexPath << std::endl;
            }

            // Fix B2S typo
            table.extraFiles = std::string(iniExists ? "INI " : "") +
                               std::string(vbsExists ? "VBS " : "") +
                               std::string(std::filesystem::exists(b2sFile) ? "B2S" : "");
            table.udmd = std::filesystem::exists(folder + "/UltraDMD") ? u8"✪" : "";
            table.alts = std::filesystem::exists(folder + config.getAltSoundPath()) ? u8"♫" : "";
            table.altc = std::filesystem::exists(folder + config.getAltColorPath()) ? u8"☀" : "";
            table.pup = std::filesystem::exists(folder + config.getPupPackPath()) ? u8"▣" : "";
            table.music = std::filesystem::exists(folder + config.getMusicPath()) ? u8"♪" : "";
            table.images = std::string(std::filesystem::exists(folder + config.getWheelImage()) ? "Wheel " : "") +
                           std::string(std::filesystem::exists(folder + config.getTableImage()) ? "Table " : "") +
                           std::string(std::filesystem::exists(folder + config.getBackglassImage()) ? "B2S " : "") +
                           std::string(std::filesystem::exists(folder + config.getMarqueeImage()) ? "Marquee" : "");
            table.videos = std::string(std::filesystem::exists(folder + config.getTableVideo()) ? "Table " : "") +
                           std::string(std::filesystem::exists(folder + config.getBackglassVideo()) ? "B2S " : "") +
                           std::string(std::filesystem::exists(folder + config.getDmdVideo()) ? "DMD" : "");
        }
        saveToCache(config.getBasePath() + "resources/tables_index.json");
        filteredTables = tables;
        filterTables("");
        loading = false;
    }).detach();
}

void TableManager::filterTables(const std::string& query) {
    filteredTables.clear();
    if (query.empty()) {
        filteredTables = tables;
    } else {
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        for (const auto& table : tables) {
            std::string lowerName = table.name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            if (lowerName.find(lowerQuery) != std::string::npos) {
                filteredTables.push_back(table);
            }
        }
    }

    std::sort(filteredTables.begin(), filteredTables.end(), [this](const TableEntry& a, const TableEntry& b) {
        switch (sortColumn) {
            case 0: return sortAscending ? a.year < b.year : a.year > b.year;
            case 1: return sortAscending ? a.brand < b.brand : a.brand > b.brand;
            case 2: return sortAscending ? a.name < b.name : a.name > b.name;
            case 3: return sortAscending ? a.version < b.version : a.version > b.version;
            case 4: return sortAscending ? a.extraFiles < b.extraFiles : a.extraFiles > b.extraFiles;
            case 5: return sortAscending ? a.rom < b.rom : a.rom > b.rom;
            case 6: return sortAscending ? a.udmd < b.udmd : a.udmd > b.udmd;
            case 7: return sortAscending ? a.alts < b.alts : a.alts > b.alts;
            case 8: return sortAscending ? a.altc < b.altc : a.altc > b.altc;
            case 9: return sortAscending ? a.pup < b.pup : a.pup > b.pup;
            case 10: return sortAscending ? a.music < b.music : a.music > b.music;
            case 11: return sortAscending ? a.images < b.images : a.images > b.images;
            case 12: return sortAscending ? a.videos < b.videos : a.videos > b.videos;
            default: return false;
        }
    });
}

void TableManager::setSortSpecs(int columnIdx, bool ascending) {
    sortColumn = columnIdx;
    sortAscending = ascending;
}