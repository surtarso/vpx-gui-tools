#ifndef APPLICATION_H
#define APPLICATION_H

#include "config/config_manager.h"
#include "tables/table_manager.h"
#include "tools/first_run.h"
#include "tools/ini_editor.h"
#include "launcher/launcher.h"
#include <SDL.h>
#include <thread>

class Application {
public:
    Application(const std::string& basePath);
    ~Application();
    void run();

private:
    void applyDPIScaling();
    void loadTables();
    void drawLoadingScreen();

    std::string basePath;
    ConfigManager config;
    TableManager tableManager;
    IniEditor iniEditor;
    IniEditor configEditor;
    Launcher launcher;
    FirstRunDialog firstRunDialog;
    std::string imguiIniPath;

    bool showFirstRunDialog;
    bool deferInitialLoad;
    bool loadingTables;
    bool loadingComplete;
    bool editingIni;
    bool editingSettings;
    bool exitRequested;
    bool showCreateIniPrompt;
    bool showNoTablePopup;

    SDL_Window* window;
    SDL_Renderer* renderer;
    std::thread loadingThread;

    float dpiScale;
    bool enableDPIAwareness;
    bool needFontRebuild; // New flag to defer font changes
};

#endif // APPLICATION_H