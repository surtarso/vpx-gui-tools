#include "launcher/table_actions.h"
#include <cstdlib>

TableActions::TableActions(IConfigProvider& config) : config(config) {}

bool TableActions::launchTable(const std::string& filepath) {
    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + filepath + "\" " + config.getEndArgs();
    LOG_DEBUG("Launching table with command: " << cmd);
    int result = system(cmd.c_str());
    if (result != 0) {
        LOG_DEBUG("Failed to launch table: " << filepath << " (command: " << cmd << ", exit code: " << result << ")");
        return false;
    }
    LOG_DEBUG("Successfully launched table: " << filepath);
    return true;
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