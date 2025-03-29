#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "utils/structures.h"
#include <vector>
#include <string>

class TableManager {
public:
    TableManager(const std::string& tablesDir, const std::string& romPath, const std::string& altSoundPath,
                 const std::string& altColorPath, const std::string& musicPath, const std::string& pupPackPath,
                 const std::string& wheelImage, const std::string& tableImage, const std::string& backglassImage,
                 const std::string& marqueeImage, const std::string& tableVideo, const std::string& backglassVideo,
                 const std::string& dmdVideo);
    void loadTables();
    void filterTables(const std::string& query);

    std::vector<TableEntry>& getTables() { return filteredTables; }

private:
    std::string tablesDir;
    std::string romPath;
    std::string altSoundPath;
    std::string altColorPath;
    std::string musicPath;
    std::string pupPackPath;
    std::string wheelImage;
    std::string tableImage;
    std::string backglassImage;
    std::string marqueeImage;
    std::string tableVideo;
    std::string backglassVideo;
    std::string dmdVideo;

    std::vector<TableEntry> tables;
    std::vector<TableEntry> filteredTables;
};

#endif // TABLE_MANAGER_H