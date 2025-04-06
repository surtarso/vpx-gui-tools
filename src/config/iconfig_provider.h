#ifndef ICONFIG_PROVIDER_H
#define ICONFIG_PROVIDER_H

#include <string>

// Abstract interface for configuration providers, defining all config access methods
class IConfigProvider {
public:
    virtual ~IConfigProvider() = default;

    // Base and core path getters
    virtual std::string getBasePath() const = 0;
    virtual std::string getTablesDir() const = 0;
    virtual std::string getStartArgs() const = 0;
    virtual std::string getCommandToRun() const = 0;
    virtual std::string getEndArgs() const = 0;
    virtual std::string getVPinballXIni() const = 0;

    // Tool-related getters
    virtual std::string getFallbackEditor() const = 0;
    virtual std::string getVpxTool() const = 0;
    virtual std::string getVbsSubCmd() const = 0;
    virtual std::string getPlaySubCmd() const = 0;
    virtual std::string getVpxtoolIndexFile() const = 0;
    virtual std::string getIndexerSubCmd() const = 0;
    virtual std::string getDiffSubCmd() const = 0;
    virtual std::string getRomSubCmd() const = 0;
    virtual std::string getImGuiConf() const = 0;

    // Window-related getters
    virtual int getWindowWidth() const = 0;
    virtual int getWindowHeight() const = 0;
    virtual bool getEnableDPIAwareness() const = 0;
    virtual float getDPIScaleFactor() const = 0;

    // Media-related getters
    virtual std::string getRomPath() const = 0;
    virtual std::string getAltSoundPath() const = 0;
    virtual std::string getAltColorPath() const = 0;
    virtual std::string getMusicPath() const = 0;
    virtual std::string getPupPackPath() const = 0;
    virtual std::string getWheelImage() const = 0;
    virtual std::string getTableImage() const = 0;
    virtual std::string getBackglassImage() const = 0;
    virtual std::string getMarqueeImage() const = 0;
    virtual std::string getTableVideo() const = 0;
    virtual std::string getBackglassVideo() const = 0;
    virtual std::string getDmdVideo() const = 0;

    // First-run setup methods
    virtual void setTablesDir(const std::string& path) = 0;
    virtual void setCommandToRun(const std::string& path) = 0;
    virtual void setVPinballXIni(const std::string& path) = 0;
    virtual void setFirstRun(bool value) = 0;
    virtual bool isFirstRun() const = 0;
    virtual bool arePathsValid() const = 0;
    virtual void save() = 0;
};

#endif // ICONFIG_PROVIDER_H