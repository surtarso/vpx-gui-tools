#include "tables/table_updater.h"
#include <filesystem>
#include <iostream>
#include <array>
#include <fstream>
#include <thread>

TableUpdater::TableUpdater(IConfigProvider& config, std::mutex& mutex) : config(config), tablesMutex(mutex) {}

void TableUpdater::updateTablesAsync(std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, bool& loading) {
    std::thread([this, &tables, &filteredTables, &loading]() {
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

            table.rom = "";
            std::string indexPath = config.getTablesDir() + "/" + config.getVpxtoolIndexFile();
            std::ifstream file(indexPath);
            if (file.is_open()) {
                json j;
                file >> j;
                for (const auto& t : j["tables"]) {
                    std::string filepath = t["path"].is_string() ? t["path"].get<std::string>() : "Unknown";
                    if (filepath == table.filepath) {
                        bool requiresPinmame = t["requires_pinmame"].is_boolean() ? t["requires_pinmame"].get<bool>() : false;
                        if (requiresPinmame && t["game_name"].is_string()) {
                            std::string gameName = t["game_name"].get<std::string>();
                            std::string romPath = folder + "/" + config.getRomPath() + "/" + gameName + ".zip";
                            std::cerr << "Checking ROM for " << table.name << ": requires_pinmame=" << requiresPinmame 
                                      << ", game_name=" << gameName << ", path=" << romPath << std::endl;
                            if (std::filesystem::exists(romPath)) {
                                table.rom = gameName;
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

        // Save to cache
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
        std::ofstream file(config.getBasePath() + "resources/tables_index.json");
        file << j.dump(2);

        filteredTables = tables;
        loading = false;
    }).detach();
}