#ifndef TABLE_FILTER_H
#define TABLE_FILTER_H

#include "utils/structures.h"
#include <vector>
#include <algorithm>

class TableFilter {
public:
    TableFilter() : sortColumn(0), sortAscending(true) {}
    void filterTables(const std::vector<TableEntry>& tables, std::vector<TableEntry>& filteredTables, const std::string& query);
    void setSortSpecs(int columnIdx, bool ascending);
private:
    int sortColumn;
    bool sortAscending;
};

#endif // TABLE_FILTER_H