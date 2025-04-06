#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "iconfig_provider.h"
#include "paths_config.h"
#include "tools_config.h"
#include "window_config.h"
#include "media_config.h"
#include <memory>

// Central config manager implementing IConfigProvider, delegating to specialized config classes
class ConfigManager : public IConfigProvider {
public:
    explicit ConfigManager(const std::string& basePath);
    void loadSettings();
    void save() override;

    // Core path getters
    std::string getBasePath() const override { return basePath_; }
    std::string getTablesDir() const override { return pathsConfig_->getTablesDir(); }
    std::string getStartArgs() const override { return pathsConfig_->getStartArgs(); }
    std::string getCommandToRun() const override { return pathsConfig_->getCommandToRun(); }
    std::string getEndArgs() const override { return pathsConfig_->getEndArgs(); }
    std::string getVPinballXIni() const override { return pathsConfig_->getVPinballXIni(); }

    // Tool-related getters
    std::string getFallbackEditor() const override { return toolsConfig_->getFallbackEditor(); }
    std::string getVpxTool() const override { return toolsConfig_->getVpxTool(); }
    std::string getVbsSubCmd() const override { return toolsConfig_->getVbsSubCmd(); }
    std::string getPlaySubCmd() const override { return toolsConfig_->getPlaySubCmd(); }
    std::string getVpxtoolIndexFile() const override { return toolsConfig_->getVpxtoolIndexFile(); }
    std::string getIndexerSubCmd() const override { return toolsConfig_->getIndexerSubCmd(); }
    std::string getDiffSubCmd() const override { return toolsConfig_->getDiffSubCmd(); }
    std::string getRomSubCmd() const override { return toolsConfig_->getRomSubCmd(); }
    std::string getImGuiConf() const override { return toolsConfig_->getImGuiConf(); }

    // Window-related getters
    int getWindowWidth() const override { return windowConfig_->getWindowWidth(); }
    int getWindowHeight() const override { return windowConfig_->getWindowHeight(); }
    bool getEnableDPIAwareness() const override { return windowConfig_->getEnableDPIAwareness(); }
    float getDPIScaleFactor() const override { return windowConfig_->getDPIScaleFactor(); }

    // Media-related getters
    std::string getRomPath() const override { return mediaConfig_->getRomPath(); }
    std::string getAltSoundPath() const override { return mediaConfig_->getAltSoundPath(); }
    std::string getAltColorPath() const override { return mediaConfig_->getAltColorPath(); }
    std::string getMusicPath() const override { return mediaConfig_->getMusicPath(); }
    std::string getPupPackPath() const override { return mediaConfig_->getPupPackPath(); }
    std::string getWheelImage() const override { return mediaConfig_->getWheelImage(); }
    std::string getTableImage() const override { return mediaConfig_->getTableImage(); }
    std::string getBackglassImage() const override { return mediaConfig_->getBackglassImage(); }
    std::string getMarqueeImage() const override { return mediaConfig_->getMarqueeImage(); }
    std::string getTableVideo() const override { return mediaConfig_->getTableVideo(); }
    std::string getBackglassVideo() const override { return mediaConfig_->getBackglassVideo(); }
    std::string getDmdVideo() const override { return mediaConfig_->getDmdVideo(); }

    // First-run setup methods
    void setTablesDir(const std::string& path) override { pathsConfig_->setTablesDir(path); save(); }
    void setCommandToRun(const std::string& path) override { pathsConfig_->setCommandToRun(path); save(); }
    void setVPinballXIni(const std::string& path) override { pathsConfig_->setVPinballXIni(path); save(); }
    void setFirstRun(bool value) override { pathsConfig_->setFirstRun(value); save(); }
    bool isFirstRun() const override { return pathsConfig_->isFirstRun(); }
    bool arePathsValid() const override { return pathsConfig_->arePathsValid(); }

private:
    std::string basePath_; // Root directory for all relative paths
    std::unique_ptr<PathsConfig> pathsConfig_; // Manages VPinballX paths and first-run state
    std::unique_ptr<ToolsConfig> toolsConfig_; // Manages tool-related settings
    std::unique_ptr<WindowConfig> windowConfig_; // Manages window and DPI settings
    std::unique_ptr<MediaConfig> mediaConfig_; // Manages media and extra folder paths
};

#endif // CONFIG_MANAGER_H