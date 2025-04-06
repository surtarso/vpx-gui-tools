#ifndef TOOLS_CONFIG_H
#define TOOLS_CONFIG_H

#include "iconfig_provider.h"
#include <string>

// Manages tool-related settings and commands
class ToolsConfig : public IConfigProvider {
public:
    explicit ToolsConfig(const std::string& basePath);
    void load(const std::string& configFile);

    // Tool-related getters
    std::string getFallbackEditor() const override { return fallbackEditor_; }
    std::string getVpxTool() const override { return vpxTool_; }
    std::string getVbsSubCmd() const override { return vbsSubCmd_; }
    std::string getPlaySubCmd() const override { return playSubCmd_; }
    std::string getVpxtoolIndexFile() const override { return vpxtoolIndexFile_; }
    std::string getIndexerSubCmd() const override { return indexerSubCmd_; }
    std::string getDiffSubCmd() const override { return diffSubCmd_; }
    std::string getRomSubCmd() const override { return romSubCmd_; }
    std::string getImGuiConf() const override { return imGuiConf_; }

    // Unimplemented IConfigProvider methods (default to empty/zero)
    std::string getBasePath() const override { return ""; }
    std::string getTablesDir() const override { return ""; }
    std::string getStartArgs() const override { return ""; }
    std::string getCommandToRun() const override { return ""; }
    std::string getEndArgs() const override { return ""; }
    std::string getVPinballXIni() const override { return ""; }
    int getWindowWidth() const override { return 0; }
    int getWindowHeight() const override { return 0; }
    bool getEnableDPIAwareness() const override { return false; }
    float getDPIScaleFactor() const override { return 0.0f; }
    std::string getRomPath() const override { return ""; }
    std::string getAltSoundPath() const override { return ""; }
    std::string getAltColorPath() const override { return ""; }
    std::string getMusicPath() const override { return ""; }
    std::string getPupPackPath() const override { return ""; }
    std::string getWheelImage() const override { return ""; }
    std::string getTableImage() const override { return ""; }
    std::string getBackglassImage() const override { return ""; }
    std::string getMarqueeImage() const override { return ""; }
    std::string getTableVideo() const override { return ""; }
    std::string getBackglassVideo() const override { return ""; }
    std::string getDmdVideo() const override { return ""; }
    void setTablesDir(const std::string&) override {}
    void setCommandToRun(const std::string&) override {}
    void setVPinballXIni(const std::string&) override {}
    void setFirstRun(bool) override {}
    bool isFirstRun() const override { return false; }
    bool arePathsValid() const override { return false; }
    void save() override {}

private:
    std::string basePath_; // Root directory for relative paths
    std::string fallbackEditor_; // Default editor for VPX files
    std::string vpxTool_; // Path to vpxtool utility
    std::string vbsSubCmd_; // Subcommand for VBS extraction
    std::string playSubCmd_; // Subcommand for playing tables
    std::string vpxtoolIndexFile_; // Filename for vpxtool index
    std::string indexerSubCmd_; // Subcommand for indexing tables
    std::string diffSubCmd_; // Subcommand for diffing tables
    std::string romSubCmd_; // Subcommand for ROM handling
    std::string imGuiConf_; // Path to ImGui configuration file

    std::string prependBasePath(const std::string& relativePath) const; // Prepends basePath_ to relative paths
};

#endif // TOOLS_CONFIG_H