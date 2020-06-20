/*
-------------------------------------------------------------------------------
    Filename: Input/Manager.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Time.hpp"
// -- //
#include "..\Input.hpp"
#include "..\Input\Keyboard.hpp"
#include "..\Input\Mouse.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Input
    {
        // ------------------------------------------------------------------------------------
        class Manager
        {
        public:
            // Members

            // The mouse input object.
            Input::Mouse Mouse;
            // The keyboard input object.
            Input::Keyboard Keyboard;
            // Array of input states for each code.
            State States[256];

            // Static interface handle.
            static Manager* Singleton;

        public:
            // Constructors

            // Default constructor.
            Manager() : Mouse(), Keyboard(), States() {};
            // Copy constructor.
            Manager(const Manager& other) = delete;
            // Move constructor.
            Manager(Manager&& other) : Mouse(Move(other.Mouse)), Keyboard(Move(other.Keyboard))
            {
                for(Int i = 0; i < 256; i++)
                {
                    // Copy the other state.
                    States[i] = other.States[i];
                    // 
                    other.States[i] = Input::State();
                }
            };
            // Destructor.
            ~Manager() { Release(); };

            // Methods

            // Initialize the input objects.
            Void Initialize() {}; // Empty for now.
            // Release the input manager and input objects.
            Void Release() {}; // Empty for now.

            // Update the input module, which only updates the mouse for now.
            Void Update();
            // Message handler for parsing window messages for input.
            Bool Handler(Int message, uLong wparam, uLong lparam);

            // Query if a button is currently down.
            Bool Held(Input::Key key) const { const Input::State state = States[key]; return state.Pressed > state.Released; };
            // Query if a button was just pressed.
            Bool Pressed(Input::Key key) const { return States[key].Pressed == Time::Manager::Singleton->Frame; };
            // Query if a button was just released.
            Bool Released(Input::Key key) const { return States[key].Released == Time::Manager::Singleton->Frame; };

        };
    }
}
