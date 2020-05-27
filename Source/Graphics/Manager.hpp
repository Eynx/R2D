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

// Forward declarations.
struct ID3D12Device1;
struct IDXGIFactory4;
// -- //
struct ID3D12CommandQueue;
// -- //
struct ID3D12Fence;

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

            // Container for storing Direct3D12 interfaces.
            struct
            {
                ID3D12Device1* Device = nullptr;
                IDXGIFactory4* Factory = nullptr;
                // -- //
                ID3D12CommandQueue* Queue = nullptr;
                // -- //
                ID3D12Fence* Fence = nullptr;
                Void* FenceEvent[2] = { nullptr, nullptr };
            } D3D;

            // The application window object. Only a single window is supported and it belongs to the Graphics::Manager.
            Graphics::Window Window;

            // Static interface handle. It's not actually a singleton, but such a naming implies the intent of only a single active interface.
            static Manager* Singleton;

        public:
            // Constructors

            // Default constructor.
            Manager() : D3D(), Window() {};
            // Copy constructor.
            Manager(const Manager& other) = delete;
            // Move constructor.
            Manager(Manager&& other) : D3D(Move(other.D3D)), Window(R2D::Move(other.Window)) {};
            // Destructor.
            ~Manager() { Release(); };

            // Methods

            // Initialize the graphics manager. Call this once at the start of your application.
            Void Initialize();
            // Shutdown the graphics engine and release the memory allocated by the graphics manager.
            Void Release();

            // Prepares the frame to record commands. This synchronizes the application with the renderer, potentially blocking if more than one frame is already in flight.
            Void Begin();
            // Executes any queued commands and presents the result of the frame.
            Void Present();
        };
    }
}
