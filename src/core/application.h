#ifndef APPLICATION_H
#define APPLICATION_H

#include "config/config_manager.h"
#include "tables/table_manager.h"
#include "tools/ini_editor.h"
#include "launcher/launcher.h"
#include <SDL.h>
#include <string>

class Application {
public:
    Application(const std::string& basePath);
    ~Application();
    void run();

private:
    std::string basePath;
    ConfigManager config;
    TableManager tableManager;
    IniEditor iniEditor;
    IniEditor configEditor;
    Launcher launcher;
    bool editingIni = false;
    bool editingSettings = false;
    bool exitRequested = false;
    bool showCreateIniPrompt = false;
    bool showNoTablePopup = false;  // New
    SDL_Window* window;
    SDL_GLContext gl_context;
};

#endif // APPLICATION_H