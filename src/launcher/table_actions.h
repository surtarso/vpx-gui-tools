#ifndef TABLE_ACTIONS_H
#define TABLE_ACTIONS_H

#include "utils/logging.h"
#include "config/iconfig_provider.h"
#include <string>

class TableActions {
public:
    TableActions(IConfigProvider& config);
    bool launchTable(const std::string& filepath); // Changed to return bool
    void extractVBS(const std::string& filepath);
    bool openInExternalEditor(const std::string& filepath);
    void openFolder(const std::string& filepath);
private:
    IConfigProvider& config;
};

#endif // TABLE_ACTIONS_H