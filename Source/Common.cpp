/*
-------------------------------------------------------------------------------
    Filename: Common.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "Common.hpp"
#include "Common\Directory.hpp"
#include "Common\Memory.hpp"
#include "Common\Time.hpp"
// -- //
#include "Graphics.hpp"
#include "Graphics\Manager.hpp"
// -- //
#include "Input.hpp"
#include "Input\Manager.hpp"
// -- //
#include "Common\Windows.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // Forward the declare the procedure function
    LRESULT CALLBACK Procedure(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

    // ----------------------------------------------------------------------------------------
    Void Initialize()
    {
        // Register the application.
        {
            WNDCLASSEX wc;
            // Setup the window class with default settings.
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
            wc.lpfnWndProc = Procedure;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = GetModuleHandleW(nullptr);
            wc.hIcon = LoadIconW(nullptr, IDI_WINLOGO);
            wc.hIconSm = wc.hIcon;
            wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
            wc.lpszMenuName = nullptr;
            wc.lpszClassName = L"R2D";
            wc.cbSize = sizeof(WNDCLASSEX);

            // Register the window class.
            RegisterClassExW(&wc);
        }

        // Allocate the time manager.
        Time::Manager::Singleton = Memory::Request<Time::Manager>();

        // Allocate the graphics manager.
        Graphics::Manager::Singleton = Memory::Request<Graphics::Manager>();

        // Allocate the input manager.
        Input::Manager::Singleton = Memory::Request<Input::Manager>();

        // Initialize the working directory.
        {
            // Temporary buffer to hold the string data.
            wchar_t pathBuffer[MAX_PATH];

            // Load the directory.
            GetCurrentDirectoryW(MAX_PATH, pathBuffer);

            // Translate from UTF-16 to UTF-8.
            Directory::Working = Memory::Request<String>(narrow(std::wstring(pathBuffer)));
        }
    };

    // ----------------------------------------------------------------------------------------
    Bool Update()
    {
        // Prepare a flag to denote if the program should end.
        Bool running = true;

        MSG msg;
        // Continuously pull messages until none remain.
        while(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Preemptively look at the message to parse before passing it to the message handler.
            switch(msg.message)
            {
                // If windows signals to end the application then end the main loop.
                case WM_QUIT:
                {
                    // End the application.
                    running = false;
                }

                // Disable the Alt-key menu mode.
                case WM_SYSKEYDOWN:
                {
                    // Simply do nothing.
                    continue;
                }

                default:
                {
                    // Translate keypresses and send the message to the message handler.
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        }

        return running;
    };

    // ----------------------------------------------------------------------------------------
    Void Quit()
    {
        // Simply send WM_QUIT to the message pump.
        PostQuitMessage(0);
    };

    // ----------------------------------------------------------------------------------------
    Void Shutdown()
    {
        // Release the input manager.
        if(Input::Manager::Singleton)
        {
            Input::Manager::Singleton->Release();
            Memory::Free(Input::Manager::Singleton);
            Input::Manager::Singleton = nullptr;
        }

        // Release the graphics manager.
        if(Graphics::Manager::Singleton)
        {
            Graphics::Manager::Singleton->Release();
            Memory::Free(Graphics::Manager::Singleton);
            Graphics::Manager::Singleton = nullptr;
        }

        // Release the time manager.
        if(Time::Manager::Singleton)
        {
            //Time::Manager::Singleton->Release();
            Memory::Free(Time::Manager::Singleton);
            Time::Manager::Singleton = nullptr;
        }

        // Unregister the application class.
        UnregisterClassW(L"R3D", GetModuleHandleW(nullptr));
    };

    // ----------------------------------------------------------------------------------------
    Void Sleep(Int milliseconds)
    {
        // Call the WINAPI Sleep() directly from the global namespace to avoid an ambiguous call.
        ::Sleep(milliseconds);
    };

    // ----------------------------------------------------------------------------------------
    LRESULT CALLBACK Procedure(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
    {
        // Parse the message or send it off to the default procedure to handle it.
        switch(umessage)
        {
            // End the application loop if the window requests it.
            case WM_DESTROY: // The window is being destroyed.
            case WM_CLOSE: // The window had its close button pressed
            {
                Quit();
                return S_OK;
            }

            default:
            {
                // Allow the input manager to attempt to parse the message.
                if(Input::Manager::Singleton->Handler(umessage, wparam, lparam))
                {
                    return S_OK;
                }
                // All other messages are passed to the default message handler.
                else
                {
                    return DefWindowProcW(hwnd, umessage, wparam, lparam);
                }
            }
        }
    };

    // ----------------------------------------------------------------------------------------
    namespace Debug
    {
        // ------------------------------------------------------------------------------------
        Void Print(const wchar_t* pOutputString)
        {
            // Print the text to Visual Studio's console.
            OutputDebugStringW(pOutputString);
        };
    }
}
