#ifndef FIRST_RUN_H
#define FIRST_RUN_H

#include "utils/logging.h"
#include "config/iconfig_provider.h"
#include <ImGuiFileDialog.h>
#include <imgui.h>
#include <string>

class FirstRunDialog {
public:
    FirstRunDialog(IConfigProvider& config);
    bool show(); // Returns true if the dialog is still active, false if it can be closed

private:
    IConfigProvider& config;
    std::string tablePath;
    std::string vpxExecutable;
    std::string vpinballXIni;
    bool pathsValid;
    bool validatePaths();
};

#endif // FIRST_RUN_H