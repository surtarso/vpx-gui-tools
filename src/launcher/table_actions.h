#ifndef TABLE_ACTIONS_H
#define TABLE_ACTIONS_H

#include "config/iconfig_provider.h"
#include <string>

class TableActions {
public:
    TableActions(IConfigProvider& config);
    void launchTable(const std::string& filepath);
    void extractVBS(const std::string& filepath);
    bool openInExternalEditor(const std::string& filepath);
    void openFolder(const std::string& filepath);
private:
    IConfigProvider& config;
};

#endif // TABLE_ACTIONS_H