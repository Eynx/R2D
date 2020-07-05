/*
-------------------------------------------------------------------------------
    Filename: Input/Mouse.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Input\Mouse.hpp"
// -- //
#include "..\Input\Manager.hpp"
// -- //
#include "..\Common\Windows.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Bool Input::Mouse::Held(Input::Mouse::Button button) const
    {
        return Input::Manager::Singleton->Held(Input::Key(button));
    };

    // ----------------------------------------------------------------------------------------
    Bool Input::Mouse::Pressed(Input::Mouse::Button button) const
    {
        return Input::Manager::Singleton->Pressed(Input::Key(button));
    };

    // ----------------------------------------------------------------------------------------
    Bool Input::Mouse::Released(Input::Mouse::Button button) const
    {
        return Input::Manager::Singleton->Released(Input::Key(button));
    };

    // ----------------------------------------------------------------------------------------
    Void Input::Mouse::SetPosition(const Int2& position)
    {
        // Set the position of the hardware cursor.
        SetCursorPos(position.x + Offset.x, position.y + Offset.y);

        // If the cursor is locked..
        if(Locked)
        {
            // Update the mouse's locked position to the specified position.
            LockedPosition = position;

            // Set the mouse's delta to zero.
            Position = Int2();
        }
        else
        {
            // Update the mouse's position to the specified position.
            Position = position;
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Input::Mouse::Show()
    {
        // If the mouse isn't visible..
        if(!Visible)
        {
            // Increment the cursor's visibility count.
            ShowCursor(true);

            // Update the mouse's visibility.
            Visible = true;
        }
    };
    // ----------------------------------------------------------------------------------------
    Void Input::Mouse::Hide()
    {
        // If the mouse is visible..
        if(Visible)
        {
            // Decrement the cursor's visibility count.
            ShowCursor(false);

            // Update the mouse's visibility.
            Visible = false;
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Input::Mouse::Lock()
    {
        // If the mouse is unlocked..
        if(!Locked)
        {
            // Update the mouse's locked state.
            Locked = true;

            // Set the position to its locked position.
            SetPosition(LockedPosition);
        }
    };
    // ----------------------------------------------------------------------------------------
    Void Input::Mouse::Unlock()
    {
        // If the mouse is locked..
        if(Locked)
        {
            // Update the mouse's locked state.
            Locked = false;
        }
    };
}
