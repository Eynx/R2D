/*
-------------------------------------------------------------------------------
    Filename: Graphics/Manager.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// -- //
#include "..\Graphics.hpp"
#include "..\Graphics\Window.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Graphics
    {
        // ------------------------------------------------------------------------------------
        class Manager
        {
        public:
            // Members

            // The application window object. Only a single window is supported and it belongs to the Graphics::Manager.
            Graphics::Window Window;

            // Static interface handle. It's not actually a singleton, but such a naming implies the intent of only a single active interface.
            static Manager* Singleton;

        public:
            // Constructors

            // Default constructor.
            Manager() : Window() {};
            // Copy constructor.
            Manager(const Manager& other) = delete;
            // Move constructor.
            Manager(Manager&& other) : Window(R2D::Move(other.Window)) {};
            // Destructor.
            ~Manager() { Release(); };

            // Methods

            // Initialize the graphics manager. Call this once at the start of your application.
            Void Initialize();
            // Shutdown the graphics engine and release the memory allocated by the graphics manager.
            Void Release();
        };
    }
}
