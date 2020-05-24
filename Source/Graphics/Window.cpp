/*
-------------------------------------------------------------------------------
    Filename: Graphics/Window.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Graphics\Window.hpp"
// -- //
#include "..\Common\Windows.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void Graphics::Window::Create(const Graphics::Window::Description& description)
    {
        // Debug check
        Assert(!Handle, "Window handle already populated. Check if the window has already been created or make sure the Graphics::Manager is being properly constructed.");

        // Create and initialize the application window.
        {
            // Get the application name
            LPCWSTR applicationName = L"R2D";

            Int x, y, width, height;
            // Prepare the size and position of the window.
            {
                RECT rect;
                // Adjust the window size parameters to satisfy the requested client area size.
                rect.left = 0;
                rect.top = 0;
                rect.right = description.Width;
                rect.bottom = description.Height;
                // -- //
                AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);
                // -- //
                width = rect.right - rect.left;
                height = rect.bottom - rect.top;

                // Place the window in the middle of the screen.
                x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
                y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
            }

            // Create the window and store the handle to it.
            Handle = CreateWindowExW(WS_EX_APPWINDOW, applicationName, applicationName, WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

            // Debug checks
            Assert(Handle, "There was a problem creating the window.");
            Assert(Handle != INVALID_HANDLE_VALUE, "There was a problem creating the window.");
        }

        auto handle = (HWND)Handle;
        // Finally, setting the window visibility last ensures that the user doesn't see the temporary stages the window goes through during creation (looks like flickering/lag).
        ShowWindow(handle, SW_SHOW);
        SetForegroundWindow(handle);
        SetFocus(handle);
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Window::Release()
    {
        // Destroy the window if one exists.
        if(Handle) { DestroyWindow((HWND)Handle); Handle = nullptr; }
    };
}
