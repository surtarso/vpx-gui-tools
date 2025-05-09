#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include "utils/logging.h"
#include "utils/structures.h"
#include "tables/table_manager.h"
#include "config/iconfig_provider.h"
#include <imgui.h>

class TableView {
public:
    TableView(TableManager* tm, IConfigProvider& config);
    void drawTable(std::vector<TableEntry>& tables);
    int getSelectedTable() const { return selectedTable; }
private:
    TableManager* tableManager;
    IConfigProvider& config;
    int selectedTable;
    bool checkFilePresence(const std::string& tablePath, const std::string& relativePath);
};

#endif // TABLE_VIEW_H