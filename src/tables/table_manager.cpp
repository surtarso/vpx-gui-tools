#include "tables/table_manager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <array>

TableManager::TableManager(IConfigProvider& config) : config(config) {
    loadTables();
}

void TableManager::loadTables() {
    tables.clear();
    std::string tablesDir = config.getTablesDir();
    std::cout << "Checking tablesDir: " << tablesDir << std::endl;
    if (!std::filesystem::exists(tablesDir)) {
        std::cerr << "Tables directory does not exist: " << tablesDir << std::endl;
        return;
    }

    std::cout << "Tables directory exists. Scanning recursively..." << std::endl;
    try {
        int vpxCount = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(tablesDir, std::filesystem::directory_options::skip_permission_denied)) {
            if (entry.path().extension() == ".vpx") {
                std::string filepath = entry.path().string();
                std::string basename = entry.path().stem().string();
                std::string folder = entry.path().parent_path().string();

                std::cout << "Found VPX file: " << filepath << std::endl;
                vpxCount++;

                std::string name, brand, year;
                size_t openParen = basename.find('(');
                size_t closeParen = basename.find(')');
                size_t lastSpace = basename.find_last_of(' ');
                if (openParen != std::string::npos && closeParen != std::string::npos && lastSpace != std::string::npos &&
                    openParen < closeParen && lastSpace > closeParen) {
                    name = basename.substr(0, openParen);
                    name.erase(name.find_last_not_of(" \t") + 1);
                    brand = basename.substr(openParen + 1, closeParen - openParen - 1);
                    year = basename.substr(lastSpace + 1, 4);
                    if (year.size() != 4 || !std::all_of(year.begin(), year.end(), ::isdigit)) {
                        year = "Unknown";
                    }
                } else {
                    name = basename;
                    brand = "Unknown";
                    year = "Unknown";
                }

                TableEntry table;
                table.year = year;
                table.brand = brand;
                table.name = name;
                table.filename = basename;
                table.filepath = filepath;

                std::string iniFile = folder + "/" + basename + ".ini";
                std::string vbsFile = folder + "/" + basename + ".vbs";
                std::string b2sFile = folder + "/" + basename + ".directb2s";

                // Check VBS diff only if .vbs exists
                bool vbsExists = std::filesystem::exists(vbsFile);
                table.vbsModified = false;
                if (vbsExists) {
                    std::string vbsDiffCmd = "\"" + config.getVpxTool() + "\" " + config.getDiffSubCmd() + " \"" + filepath + "\"";
                    std::array<char, 128> buffer;
                    std::string result;
                    FILE* pipe = popen(vbsDiffCmd.c_str(), "r");
                    if (pipe) {
                        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                            result += buffer.data();
                        }
                        pclose(pipe);
                        // Check if result is empty or whitespace (no diff) vs. containing diff markers
                        bool isWhitespace = std::all_of(result.begin(), result.end(), isspace);
                        table.vbsModified = (!result.empty() && !isWhitespace && (result.find("---") != std::string::npos || result.find("+++") != std::string::npos));
                    }
                }

                // Check INI diff
                bool iniExists = std::filesystem::exists(iniFile);
                table.iniModified = false;
                if (iniExists) {
                    std::string iniDiffCmd = "diff \"" + config.getVPinballXIni() + "\" \"" + iniFile + "\"";
                    FILE* pipe = popen(iniDiffCmd.c_str(), "r");
                    if (pipe) {
                        std::array<char, 128> buffer;
                        std::string result;
                        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                            result += buffer.data();
                        }
                        int status = pclose(pipe);
                        table.iniModified = (status != 0); // Non-zero exit means a diff
                    }
                }

                // Build extraFiles string
                table.extraFiles = std::string(iniExists ? "INI " : "") +
                                   std::string(vbsExists ? "VBS " : "") +
                                   std::string(std::filesystem::exists(b2sFile) ? "B2S" : "");

                std::string romDir = folder + config.getRomPath();
                table.rom = std::filesystem::exists(romDir) ? "ROM" : "";
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

                tables.push_back(table);
            }
        }
        std::cout << "Total VPX files found: " << vpxCount << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error in loadTables: " << e.what() << std::endl;
    }
    filteredTables = tables;
    filterTables("");
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
            case 3: return sortAscending ? a.extraFiles < b.extraFiles : a.extraFiles > b.extraFiles;
            case 4: return sortAscending ? a.rom < b.rom : a.rom > b.rom;
            case 5: return sortAscending ? a.udmd < b.udmd : a.udmd > b.udmd;
            case 6: return sortAscending ? a.alts < b.alts : a.alts > b.alts;
            case 7: return sortAscending ? a.altc < b.altc : a.altc > b.altc;
            case 8: return sortAscending ? a.pup < b.pup : a.pup > b.pup;
            case 9: return sortAscending ? a.music < b.music : a.music > b.music;
            case 10: return sortAscending ? a.images < b.images : a.images > b.images;
            case 11: return sortAscending ? a.videos < b.videos : a.videos > b.videos;
            default: return false;
        }
    });
}

void TableManager::setSortSpecs(int columnIdx, bool ascending) {
    sortColumn = columnIdx;
    sortAscending = ascending;
}