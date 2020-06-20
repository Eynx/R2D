/*
-------------------------------------------------------------------------------
    Filename: Input/Mouse.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// -- //
#include "..\Input.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Input
    {
        // ------------------------------------------------------------------------------------
        class Mouse
        {
        public:
            // Types

            // Helper enum describing the mouse buttons. Corresponds to the Input::Key enum.
            enum Button : uByte
            {
                Left = 0x01,
                Right = 0x02,
                Middle = 0x04,
                // Extra mouse buttons
                A = 0x05,
                B = 0x06
            };

        public:
            // Members

            // The position of the mouse on the screen relative to the window.
            // If the mouse is locked, then Position is how much the mouse has moved since the previous update.
            Int2 Position;
            // The correction offset to bring the mouse from display coordinates into window coordinates.
            // i.e. the top-left position of the window.
            Int2 Offset;
            // The position the mouse will be set to when locked relative to the window.
            Int2 LockedPosition;
            // The locked state of the mouse.
            Bool Locked;
            // The requested visibility state of the mouse. Does not reflect the true visibility of the cursor.
            Bool Visible;

        public:
            // Constructors

            // Default constructor.
            Mouse() : Position(), Offset(), LockedPosition(), Locked(false), Visible(true) {};
            // Copy constructor.
            Mouse(const Mouse& other) = delete;
            // Move constructor.
            Mouse(Mouse&& other) : Position(Move(other.Position)), Offset(Move(other.Offset)), LockedPosition(Move(other.LockedPosition)), Locked(other.Locked), Visible(other.Visible) { other.Locked = false; other.Visible = true; };
            // Destructor.
            ~Mouse() { Release(); };

            // Methods

            // Release the mouse object.
            Void Release() { Unlock(); Show(); };

            // Query if a button is currently down.
            Bool Held(Mouse::Button button) const;
            // Query if a button was just pressed.
            Bool Pressed(Mouse::Button button) const;
            // Query if a button was just released.
            Bool Released(Mouse::Button button) const;

            // Set the position of the cursor or update its locked position if it is locked.
            Void SetPosition(const Float2& position);
            // -- //
            Void Show();
            Void Hide();
            // -- //
            Void Lock();
            Void Unlock();
        };
    }
}
