#include "tables/table_manager.h"

TableManager::TableManager(IConfigProvider& config)
    : config(config), loading(false), loader(config), updater(config, tablesMutex), filter() {
    loadTables();
    updateTablesAsync();
}

void TableManager::loadTables() {
    loader.load(tables, filteredTables);
    filter.filterTables(tables, filteredTables, "");
}

void TableManager::filterTables(const std::string& query) {
    filter.filterTables(tables, filteredTables, query);
}

void TableManager::setSortSpecs(int columnIdx, bool ascending) {
    filter.setSortSpecs(columnIdx, ascending);
}

void TableManager::updateTablesAsync() {
    updater.updateTablesAsync(tables, filteredTables, loading);
}