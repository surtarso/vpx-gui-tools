#ifndef APPLICATION_H
#define APPLICATION_H

#include "config/config_manager.h"
#include "tables/table_manager.h"
#include "tools/ini_editor.h"
#include "tools/first_run.h"
#include "launcher/launcher.h"
#include <SDL.h>
#include <string>
#include <thread>
#include <atomic>

class Application {
public:
    Application(const std::string& basePath);
    ~Application();
    void run();

private:
    void loadTables(); // Start the table loading process
    void drawLoadingScreen(); // Draw the "Loading..." screen

    std::string basePath;
    ConfigManager config;
    TableManager tableManager;
    IniEditor iniEditor;
    IniEditor configEditor;
    Launcher launcher;
    FirstRunDialog firstRunDialog;
    bool showFirstRunDialog;
    bool deferInitialLoad;
    bool loadingTables; // Track whether we're loading tables
    std::thread loadingThread; // Thread for background loading
    std::atomic<bool> loadingComplete; // Flag to indicate when loading is done
    bool editingIni = false;
    bool editingSettings = false;
    bool exitRequested = false;
    bool showCreateIniPrompt = false;
    bool showNoTablePopup = false;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::string imguiIniPath;
};

#endif // APPLICATION_H