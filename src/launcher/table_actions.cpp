#include "launcher/table_actions.h"
#include <cstdlib>

TableActions::TableActions(IConfigProvider& config) : config(config) {}

void TableActions::launchTable(const std::string& filepath) {
    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + filepath + "\" " + config.getEndArgs();
    system(cmd.c_str());
}

void TableActions::extractVBS(const std::string& filepath) {
    std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getVbsSubCmd() + " \"" + filepath + "\"";
    system(cmd.c_str());
}

bool TableActions::openInExternalEditor(const std::string& filepath) {
    std::string cmd = "xdg-open \"" + filepath + "\"";
    if (system(cmd.c_str()) != 0) {
        if (!config.getFallbackEditor().empty()) {
            cmd = "\"" + config.getFallbackEditor() + "\" \"" + filepath + "\"";
            return system(cmd.c_str()) == 0;
        }
        return false;
    }
    return true;
}

void TableActions::openFolder(const std::string& filepath) {
    std::string folder = filepath.empty() ? config.getTablesDir() : filepath.substr(0, filepath.find_last_of('/'));
    std::string cmd = "xdg-open \"" + folder + "\"";
    system(cmd.c_str());
}