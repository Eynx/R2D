/*
-------------------------------------------------------------------------------
    Filename: Input/Manager.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Input\Manager.hpp"
// -- //
#include "..\Graphics\Window.hpp"
#include "..\Graphics\Manager.hpp"
// -- //
#include "..\Common\Windows.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Input::Manager* Input::Manager::Singleton = nullptr;

    // ----------------------------------------------------------------------------------------
    Void Input::Manager::Update()
    {
        // Helpers
        auto graphics = Graphics::Manager::Singleton;
        auto& window = graphics->Window;

        POINT point{ 0, 0 };
        // Get top-left position of the application window.
        BOOL result = ClientToScreen((HWND)window.Handle, &point);
        // Debug check
        Assert(result > 0, "There was a problem retrieving the current position of the window.");
        // TODO: Let the window update its own position and move this code to there.

        // Set the mouse's offset to the the top-left window coordinate. This used for the window-local coordinates.
        Mouse.Offset = Int2(point.x, point.y);

        // Set the locked position of the mouse to the center of the window. (Rely on the user to set this later.)
        Mouse.LockedPosition = window.Size / 2;

        // Get the position of the cursor on the display.
        GetCursorPos(&point);

        // Update the position of the mouse.
        Mouse.Position = Int2(point.x, point.y) - Mouse.Offset;

        // If the mouse is locked..
        if(Mouse.Locked)
        {
            // Set the mouse's position to its delta from the locked position.
            Mouse.Position = Mouse.Position - Mouse.LockedPosition;

            // Set the position of the mouse to its locked position.
            SetCursorPos(Mouse.LockedPosition.x + Mouse.Offset.x, Mouse.LockedPosition.y + Mouse.Offset.y);
        }
    };

    // ----------------------------------------------------------------------------------------
    Bool Input::Manager::Handler(Int message, uLong wparam, uLong lparam)
    {
        // Helper
        auto time = Time::Manager::Singleton;

        Bool result = true;
        switch(message)
        {
            // Keyboard input.
            case WM_KEYDOWN:
            {
                Input::Manager::Singleton->States[wparam].Pressed = time->Frame;
                break;
            }
            case WM_KEYUP:
            {
                Input::Manager::Singleton->States[wparam].Released =  time->Frame;
                break;
            }

            // Mouse input.
            case WM_LBUTTONDOWN:
            {
                Input::Manager::Singleton->States[Input::Key::LMB].Pressed =  time->Frame;
                break;
            }
            case WM_RBUTTONDOWN:
            {
                Input::Manager::Singleton->States[Input::Key::RMB].Pressed =  time->Frame;
                break;
            }
            case WM_MBUTTONDOWN:
            {
                Input::Manager::Singleton->States[Input::Key::MMB].Pressed =  time->Frame;
                break;
            }
            case WM_XBUTTONDOWN:
            {
                switch(GET_XBUTTON_WPARAM(wparam))
                {
                    case XBUTTON1:
                    {
                        Input::Manager::Singleton->States[Input::Key::MouseA].Pressed =  time->Frame;
                        break;
                    }
                    case XBUTTON2:
                    {
                        Input::Manager::Singleton->States[Input::Key::MouseB].Pressed =  time->Frame;
                        break;
                    }
                    default:
                    {
                        result = false;
                    }
                }
                break;
            }
            case WM_LBUTTONUP:
            {
                Input::Manager::Singleton->States[Input::Key::LMB].Released =  time->Frame;
                break;
            }
            case WM_RBUTTONUP:
            {
                Input::Manager::Singleton->States[Input::Key::RMB].Released =  time->Frame;
                break;
            }
            case WM_MBUTTONUP:
            {
                Input::Manager::Singleton->States[Input::Key::MMB].Released =  time->Frame;
                break;
            }
            case WM_XBUTTONUP:
            {
                switch(GET_XBUTTON_WPARAM(wparam))
                {
                    case XBUTTON1:
                    {
                        Input::Manager::Singleton->States[Input::Key::MouseA].Released =  time->Frame;
                        break;
                    }
                    case XBUTTON2:
                    {
                        Input::Manager::Singleton->States[Input::Key::MouseB].Released =  time->Frame;
                        break;
                    }
                    default:
                    {
                        result = false;
                    }
                }
                break;
            }

            // Pass other messages through.
            default:
            {
                result = false;
            }
        }
        return result;
    };
}
