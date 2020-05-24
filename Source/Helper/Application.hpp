/*
-------------------------------------------------------------------------------
    Filename: Helper/Application.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// -- //
#include "..\Graphics.hpp"
#include "..\Graphics\Manager.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    template <typename Type> class Application
    {
    public:
        // Members

        // The object containing the user entrypoint and program logic.
        Type User;

    public:
        // Constructors

        // Default constructor.
        Application() : User() {};
        // Copy constructor.
        Application(const Application& other) = delete;
        // Move constructor.
        Application(Application&& other) : User(R2D::Move(other.User)) {};
        // Destructor.
        ~Application() { Shutdown(); };

        // Methods

        // Create the entry point object and initialize the framework and singleton objects.
        Void Initialize()
        {
            // Initialize the R2D framework.
            R2D::Initialize();

            // Initialize the graphics manager.
            Graphics::Manager::Singleton->Initialize();

            // Call the user entrypoint.
            User.Create();
        };

        // Begin main application loop.
        Void Run()
        {
            // Prepare the infinite loop.
            Bool running = true;

            // Main application loop.
            while(running)
            {
                // Prepare the frame.
                running = R2D::Update();

                // Call the user update function.
                User.Update();

                // Call the user draw function.
                User.Draw();

                // Sleep for now, to keep the CPU utilization down.
                R2D::Sleep(16);
            }
        };

        // Release and delete singleton objects.
        Void Shutdown()
        {
            // Let the user do their cleanup first.
            User.Shutdown();

            // Shutdown the framework.
            R2D::Shutdown();
        };
    };
}
