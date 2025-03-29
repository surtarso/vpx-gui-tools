#ifndef APPLICATION_H
#define APPLICATION_H

#include "config_manager.h"
#include "table_manager.h"
#include "ini_editor.h"
#include "launcher.h"
#include <SDL.h>

class Application {
public:
    Application();
    ~Application();
    void run();

private:
    ConfigManager config;
    TableManager tableManager;
    IniEditor iniEditor;
    Launcher launcher;
    bool editingIni = false;
    bool exitRequested = false;

    SDL_Window* window;
    SDL_GLContext gl_context;
};

#endif // APPLICATION_H