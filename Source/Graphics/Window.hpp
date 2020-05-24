/*
-------------------------------------------------------------------------------
    Filename: Graphics/Window.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// -- //
#include "..\Graphics.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Graphics
    {
        // ------------------------------------------------------------------------------------
        class Window
        {
        public:
            // Nested Types

            // Description struct for passing window parameters to the Create() method.
            struct Description
            {
                // The width of the client area of the window, specified in pixels.
                Int Width = 0;
                // The height of the client area of the window, specified in pixels.
                Int Height = 0;
            };

        public:
            // Members

            // [HWND] Abstraction; The Win32 handle to the application window.
            Void* Handle;

        public:
            // Constructors

            // Default constructor.
            Window() : Handle(nullptr) {};
            // Copy constructor.
            Window(const Window& other) = delete;
            // Move constructor.
            Window(Window&& other) : Handle(other.Handle) { other.Handle = nullptr; };
            // Destructor.
            ~Window() { Release(); };

            // Methods

            // Create the window. Call this only after initializing the graphics manager.
            Void Create(const Description& description);
            // Release the window, destroying the Win32 window.
            Void Release();
        };
    }
}
