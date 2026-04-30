#include "editor/EditorApp.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        IsoForge::EditorApp app;
        app.Run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Fatal error: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
