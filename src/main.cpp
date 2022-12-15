#include "Application.h"

int main() {
    Application* app = new Application();

    app->Load();
    app->Initialize();
    app->Run();
    app->Unload();

    delete app;
}
