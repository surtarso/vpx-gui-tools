#ifndef APPLICATION_H
#define APPLICATION_H

#include "config/config_manager.h"
#include "tables/table_manager.h"
#include "tools/ini_editor.h"
#include "launcher/launcher.h"
#include <SDL.h>

class Application {
public:
    Application();
    ~Application();
    void run();

private:
    ConfigManager config;
    TableManager tableManager;
    IniEditor iniEditor;      // For VPinballX.ini
    IniEditor configEditor;   // Added for settings.ini
    Launcher launcher;
    bool editingIni = false;
    bool editingSettings = false;
    bool exitRequested = false;

    SDL_Window* window;
    SDL_GLContext gl_context;
};

#endif // APPLICATION_H