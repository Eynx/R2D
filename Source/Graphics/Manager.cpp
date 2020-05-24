/*
-------------------------------------------------------------------------------
    Filename: Graphics/Manager.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Graphics\Manager.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Graphics::Manager* Graphics::Manager::Singleton = nullptr;

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Initialize()
    {
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Release()
    {
        // Release the application window.
        Window.Release();
    };
}
