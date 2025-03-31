#include "tables/table_updater.h"
#include "utils/logging.h"
#include <filesystem>
#include <iostream>
#include <array>
#include <fstream>
#include <thread>
#include <vector>

TableUpdater::TableUpdater(IConfigProvider& config, std::mutex& mutex) : config(config), tablesMutex(mutex) {}

void TableUpdater::checkRomForChunk(std::vector<TableEntry>& tables, size_t start, size_t end) {
    LOG_DEBUG("Starting ROM check for chunk [" << start << ", " << end << ") in thread " << std::this_thread::get_id());
    for (size_t i = start; i < end && i < tables.size(); ++i) {
        auto& table = tables[i];
        std::string folder = std::filesystem::path(table.filepath).parent_path().string();
        table.rom = "";
        if (table.requiresPinmame && !table.gameName.empty()) {
            std::string romPath = folder + "/" + config.getRomPath() + "/" + table.gameName + ".zip";
            LOG_DEBUG("Checking ROM for " << table.name << ": requiresPinmame=" << table.requiresPinmame 
                      << ", gameName=" << table.gameName << ", path=" << romPath);
            if (std::filesystem::exists(romPath)) {
                table.rom = table.gameName;
                LOG_DEBUG("ROM found for " << table.name << ": " << table.gameName);
            } else {
                LOG_DEBUG("ROM not found for " << table.name << " at " << romPath);
            }
        } else if (table.requiresPinmame) {
            LOG_DEBUG("Missing or null gameName for " << table.name);
        } else {
            LOG_DEBUG("No ROM required for " << table.name);
        }
    }
    LOG_DEBUG("Finished ROM check for chunk [" << start << ", " << end << ") in thread " << std::this_thread::get_id());
}

void TableUpdater::updateTablesAsync(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool& loading) {
    std::thread([this, &tables, &filteredTables, &loading]() {
        loading = true;
        std::lock_guard<std::mutex> lock(tablesMutex);

        // Step 1: Update VBS and INI checks
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

            table.extraFiles = std::string(iniExists ? "INI " : "") +
                               std::string(vbsExists ? "VBS " : "") +
                               std::string(std::filesystem::exists(b2sFile) ? "B2S" : "");
            
            bool hasUltraDmdFolder = false;
            for (const auto& entry : std::filesystem::directory_iterator(folder)) {
                if (entry.is_directory()) {
                    std::string folderName = entry.path().filename().string();
                    LOG_DEBUG("Checking folder in " << folder << ": " << folderName);
                    if (folderName.length() >= 9 && folderName.substr(folderName.length() - 9) == ".UltraDMD") {
                        hasUltraDmdFolder = true;
                        LOG_DEBUG("Found UltraDMD folder for " << table.name << ": " << folderName);
                        break;
                    }
                }
            }
            table.udmd = hasUltraDmdFolder ? u8"✪" : "";
            LOG_DEBUG("Set udmd for " << table.name << ": " << (table.udmd.empty() ? "empty" : table.udmd));
            
            table.alts = std::filesystem::exists(folder + config.getAltSoundPath()) ? u8"♪" : "";
            table.altc = std::filesystem::exists(folder + config.getAltColorPath()) ? u8"☀" : "";
            table.pup = std::filesystem::exists(folder + config.getPupPackPath()) ? u8"▣" : "";
            table.music = std::filesystem::exists(folder + config.getMusicPath()) ? u8"♫" : "";
            table.images = std::string(std::filesystem::exists(folder + config.getWheelImage()) ? "Wheel " : "") +
                           std::string(std::filesystem::exists(folder + config.getTableImage()) ? "Table " : "") +
                           std::string(std::filesystem::exists(folder + config.getBackglassImage()) ? "B2S " : "") +
                           std::string(std::filesystem::exists(folder + config.getMarqueeImage()) ? "Marquee" : "");
            table.videos = std::string(std::filesystem::exists(folder + config.getTableVideo()) ? "Table " : "") +
                           std::string(std::filesystem::exists(folder + config.getBackglassVideo()) ? "B2S " : "") +
                           std::string(std::filesystem::exists(folder + config.getDmdVideo()) ? "DMD" : "");
        }

        // Step 2: Threaded ROM checks
        const size_t numThreads = std::thread::hardware_concurrency();
        const size_t chunkSize = tables.size() / numThreads + (tables.size() % numThreads != 0 ? 1 : 0);
        std::vector<std::thread> threads;

        LOG_DEBUG("Starting ROM checks with " << numThreads << " threads, chunk size=" << chunkSize);
        for (size_t i = 0; i < tables.size(); i += chunkSize) {
            size_t start = i;
            size_t end = std::min(i + chunkSize, tables.size());
            threads.emplace_back(&TableUpdater::checkRomForChunk, this, std::ref(tables), start, end);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // Step 3: Log final ROM status
        // for (const auto& table : tables) {
        //     LOG_DEBUG("Final ROM status for " << table.name << ": rom=" << table.rom);
        // }

        // Step 4: Save to cache
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
        std::ofstream file(config.getBasePath() + "resources/tables_index.json");
        file << j.dump(2);

        filteredTables = tables;
        loading = false;
        LOG_DEBUG("Finished updating tables, loading=false");
    }).detach();
}