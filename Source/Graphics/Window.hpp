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

// Forward declarations.
struct IDXGISwapChain3;
struct ID3D12Resource;

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

            // Container for storing Direct3D12 interfaces.
            struct D3D12
            {
                IDXGISwapChain3* SwapChain = nullptr;
                ID3D12Resource* RenderTargets[2] = { nullptr, nullptr };
            } D3D;
            // [HWND] Abstraction; The Win32 handle to the application window.
            Void* Handle;
            // Window size and helper variables.
            union
            {
                // Window size as separate members.
                struct
                {
                    // The width of the window's client area in pixels.
                    Int Width;
                    // The height of the window's client area in pixels.
                    Int Height;
                };
                // 2D vector containing the size of the window's client area.
                Int2 Size;
            };

        public:
            // Constructors

            // Default constructor.
            Window() : D3D(), Handle(nullptr), Width(0), Height(0) {};
            // Copy constructor.
            Window(const Window& other) = delete;
            // Move constructor.
            Window(Window&& other) : D3D(Move(other.D3D)), Handle(other.Handle), Size(Move(other.Size)) { other.Handle = nullptr; };
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
