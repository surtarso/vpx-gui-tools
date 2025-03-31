#include "tables/table_manager.h"

TableManager::TableManager(IConfigProvider& config)
    : config(config), 
      loading(false), 
      tablesLoaded(false), // Initialize to false
      loader(config), 
      updater(config, tablesMutex), 
      filter() {
    // Do not call loadTables() or updateTablesAsync() here
}

void TableManager::loadTables() {
    if (!tablesLoaded) {
        loader.load(tables, filteredTables);
        tablesLoaded = true; // Mark tables as loaded
        updateTablesAsync(); // Start async updates after loading
    }
    filter.filterTables(tables, filteredTables, ""); // Apply initial filter
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