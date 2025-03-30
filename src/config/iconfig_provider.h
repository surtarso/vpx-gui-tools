#ifndef ICONFIG_PROVIDER_H
#define ICONFIG_PROVIDER_H

#include <string>

class IConfigProvider {
public:
    virtual ~IConfigProvider() = default;

    // Launcher settings
    virtual std::string getTablesDir() const = 0;
    virtual std::string getStartArgs() const = 0;
    virtual std::string getCommandToRun() const = 0;
    virtual std::string getEndArgs() const = 0;
    virtual std::string getVPinballXIni() const = 0;
    virtual std::string getFallbackEditor() const = 0;
    virtual std::string getVpxTool() const = 0;
    virtual std::string getVbsSubCmd() const = 0;
    virtual std::string getPlaySubCmd() const = 0;

    // Application settings
    virtual int getWindowWidth() const = 0;
    virtual int getWindowHeight() const = 0;

    // TableManager settings
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

    // Future JSON hooks (placeholders)
    virtual std::string getVpxtoolIndexFile() const = 0; // Path to JSON
    virtual std::string getIndexerSubCmd() const = 0;
    virtual std::string getDiffSubCmd() const = 0;
    virtual std::string getRomSubCmd() const = 0;
    // Add getTableDataJson() later when JSON is ready
};

#endif // ICONFIG_PROVIDER_H