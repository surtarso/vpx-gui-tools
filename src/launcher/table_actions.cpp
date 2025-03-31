#include "launcher/table_actions.h"
#include "utils/logging.h"
#include <cstdlib>

TableActions::TableActions(IConfigProvider& config) : config(config) {}

void TableActions::launchTable(const std::string& filepath) {
    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + filepath + "\" " + config.getEndArgs();
    int result = system(cmd.c_str());
    if (result != 0) {
        LOG_DEBUG("Failed to launch table: " << filepath << " (command: " << cmd << ")");
    }
}

void TableActions::extractVBS(const std::string& filepath) {
    std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getVbsSubCmd() + " \"" + filepath + "\"";
    int result = system(cmd.c_str());
    if (result != 0) {
        LOG_DEBUG("Failed to extract VBS from table: " << filepath << " (command: " << cmd << ")");
    }
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
    int result = system(cmd.c_str());
    if (result != 0) {
        LOG_DEBUG("Failed to open folder: " << folder << " (command: " << cmd << ")");
    }
}