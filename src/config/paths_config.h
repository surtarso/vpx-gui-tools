#ifndef PATHS_CONFIG_H
#define PATHS_CONFIG_H

#include "iconfig_provider.h"
#include <string>

// Manages VPinballX-related paths and first-run state
class PathsConfig : public IConfigProvider {
public:
    explicit PathsConfig(const std::string& basePath);
    void load(const std::string& configFile);

    // Core path getters
    std::string getBasePath() const override { return basePath_; }
    std::string getTablesDir() const override { return tablesDir_; }
    std::string getStartArgs() const override { return startArgs_; }
    std::string getCommandToRun() const override { return commandToRun_; }
    std::string getEndArgs() const override { return endArgs_; }
    std::string getVPinballXIni() const override { return vpinballXIni_; }

    // First-run setup methods
    void setTablesDir(const std::string& path) override { tablesDir_ = path; }
    void setCommandToRun(const std::string& path) override { commandToRun_ = path; }
    void setVPinballXIni(const std::string& path) override { vpinballXIni_ = path; }
    void setFirstRun(bool value) override { firstRun_ = value; }
    bool isFirstRun() const override { return firstRun_; }
    bool arePathsValid() const override;

    // Unimplemented IConfigProvider methods (default to empty/zero)
    std::string getFallbackEditor() const override { return ""; }
    std::string getVpxTool() const override { return ""; }
    std::string getVbsSubCmd() const override { return ""; }
    std::string getPlaySubCmd() const override { return ""; }
    std::string getVpxtoolIndexFile() const override { return ""; }
    std::string getIndexerSubCmd() const override { return ""; }
    std::string getDiffSubCmd() const override { return ""; }
    std::string getRomSubCmd() const override { return ""; }
    std::string getImGuiConf() const override { return ""; }
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
    void save() override {}

private:
    std::string basePath_; // Root directory for relative paths
    bool firstRun_; // Indicates if this is the first run
    std::string tablesDir_; // Directory for VPX tables
    std::string startArgs_; // Arguments before the command
    std::string commandToRun_; // Path to VPinballX executable
    std::string endArgs_; // Arguments after the command
    std::string vpinballXIni_; // Path to VPinballX.ini

    std::string prependBasePath(const std::string& relativePath) const; // Prepends basePath_ to relative paths
};

#endif // PATHS_CONFIG_H