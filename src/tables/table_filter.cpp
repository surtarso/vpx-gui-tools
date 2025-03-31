#include "tables/table_filter.h"
#include <cctype>

void TableFilter::filterTables(const std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, const std::string& query) {
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

void TableFilter::setSortSpecs(int columnIdx, bool ascending) {
    sortColumn = columnIdx;
    sortAscending = ascending;
}