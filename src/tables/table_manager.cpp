#include "tables/table_manager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>

TableManager::TableManager(const std::string& tablesDir, const std::string& romPath, const std::string& altSoundPath,
                           const std::string& altColorPath, const std::string& musicPath, const std::string& pupPackPath,
                           const std::string& wheelImage, const std::string& tableImage, const std::string& backglassImage,
                           const std::string& marqueeImage, const std::string& tableVideo, const std::string& backglassVideo,
                           const std::string& dmdVideo)
    : tablesDir(tablesDir), romPath(romPath), altSoundPath(altSoundPath), altColorPath(altColorPath),
      musicPath(musicPath), pupPackPath(pupPackPath), wheelImage(wheelImage), tableImage(tableImage),
      backglassImage(backglassImage), marqueeImage(marqueeImage), tableVideo(tableVideo),
      backglassVideo(backglassVideo), dmdVideo(dmdVideo) {
    loadTables();
}

void TableManager::loadTables() {
    tables.clear();
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
                table.extraFiles = std::string(std::filesystem::exists(iniFile) ? "INI " : "") +
                                   std::string(std::filesystem::exists(vbsFile) ? "VBS " : "") +
                                   std::string(std::filesystem::exists(b2sFile) ? "B2S" : "");

                std::string romDir = folder + romPath;
                table.rom = std::filesystem::exists(romDir) ? "ROM" : "";
                table.udmd = std::filesystem::exists(folder + "/UltraDMD") ? u8"✪" : "";
                table.alts = std::filesystem::exists(folder + altSoundPath) ? u8"♫" : "";
                table.altc = std::filesystem::exists(folder + altColorPath) ? u8"☀" : "";
                table.pup = std::filesystem::exists(folder + pupPackPath) ? u8"▣" : "";
                table.music = std::filesystem::exists(folder + musicPath) ? u8"♪" : "";

                table.images = std::string(std::filesystem::exists(folder + wheelImage) ? "Wheel " : "") +
                               std::string(std::filesystem::exists(folder + tableImage) ? "Table " : "") +
                               std::string(std::filesystem::exists(folder + backglassImage) ? "B2S " : "") +
                               std::string(std::filesystem::exists(folder + marqueeImage) ? "Marquee" : "");
                table.videos = std::string(std::filesystem::exists(folder + tableVideo) ? "Table " : "") +
                               std::string(std::filesystem::exists(folder + backglassVideo) ? "B2S " : "") +
                               std::string(std::filesystem::exists(folder + dmdVideo) ? "DMD" : "");

                tables.push_back(table);
            }
        }
        std::cout << "Total VPX files found: " << vpxCount << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error in loadTables: " << e.what() << std::endl;
    }
    filteredTables = tables;
    filterTables("");  // Apply initial sort
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

    // Apply sorting to filteredTables
    std::sort(filteredTables.begin(), filteredTables.end(), [this](const TableEntry& a, const TableEntry& b) {
        switch (sortColumn) {
            case 0:  // Year
                return sortAscending ? a.year < b.year : a.year > b.year;
            case 1:  // Brand
                return sortAscending ? a.brand < b.brand : a.brand > b.brand;
            case 2:  // Name
                return sortAscending ? a.name < b.name : a.name > b.name;
            case 3:  // Extra Files
                return sortAscending ? a.extraFiles < b.extraFiles : a.extraFiles > b.extraFiles;
            case 4:  // ROM
                return sortAscending ? a.rom < b.rom : a.rom > b.rom;
            case 5:  // uDMD
                return sortAscending ? a.udmd < b.udmd : a.udmd > b.udmd;
            case 6:  // AltS
                return sortAscending ? a.alts < b.alts : a.alts > b.alts;
            case 7:  // AltC
                return sortAscending ? a.altc < b.altc : a.altc > b.altc;
            case 8:  // PUP
                return sortAscending ? a.pup < b.pup : a.pup > b.pup;
            case 9:  // Music
                return sortAscending ? a.music < b.music : a.music > b.music;
            case 10: // Images
                return sortAscending ? a.images < b.images : a.images > b.images;
            case 11: // Videos
                return sortAscending ? a.videos < b.videos : a.videos > b.videos;
            default:
                return false;
        }
    });
}

void TableManager::setSortSpecs(int columnIdx, bool ascending) {
    sortColumn = columnIdx;
    sortAscending = ascending;
}