/*
-------------------------------------------------------------------------------
    Filename: Graphics/Manager.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Graphics\Manager.hpp"
// -- //
#include "..\Common\Time.hpp"
// -- //
#include "..\Graphics\D3D12.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Graphics::Manager* Graphics::Manager::Singleton = nullptr;

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Initialize()
    {
        UINT factoryFlags = 0;
        // Enable the debug layer. This is done first as enabling the debug layer after device creation will invalidate the active device.
        {
#ifdef _DEBUG
            ID3D12Debug* debugController;
            // Query for the debug interface.
            HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

            // Debug check
            Assert(SUCCEEDED(result), "There was a problem initializing the debug layer.");

            // Enable the debug layer.
            debugController->EnableDebugLayer();
            // Enable additional debug layers.
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG
        }

        // Initialize the factory.
        {
            // Create the factory.
            HRESULT result = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&D3D.Factory));
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the D3D12 factory.");
        }

        // Initialize the device.
        {
            IDXGIAdapter1* adapter = nullptr;
            // Retrieve the first adapter that supports Direct3D12.
            for(UINT adapterIndex = 0;; adapterIndex++)
            {
                // Locate an adapter.
                if(D3D.Factory->EnumAdapters1(adapterIndex, &adapter) == DXGI_ERROR_NOT_FOUND)
                {
                    // No more adapters to enumerate.
                    Assert(adapter, "Unable to find an adapter/GPU that supports Direct3D12.");
                }

                // Query the adapter to check if it supports Direct3D12 (not actually creating the device).
                if(SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device1), nullptr)))
                {
                    break;
                }

                // Clean up.
                adapter->Release();
            }

            // Create the device.
            HRESULT result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3D.Device));
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the D3D12 device.");
        }

        // Initialize the scheduler resources.
        {
            D3D12_COMMAND_QUEUE_DESC queueDesc;
            // Describe the command queue.
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            queueDesc.NodeMask = 0;

            // Create the command queue.
            HRESULT result = D3D.Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&D3D.Queue));
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the command queue.");

            // Create fence for signaling the frame events.
            result = D3D.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3D.Fence));
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the fence.");

            // Create the per-frame resources.
            for(Int i = 0; i < 2; i++)
            {
                // Create an event for the fence to signal when frames are ready.
                D3D.FenceEvent[i] = CreateEventW(nullptr, TRUE, TRUE, nullptr);
                // Debug check
                Assert(D3D.FenceEvent[i], "There was a problem creating the fence event.");
            }
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Release()
    {
        HRESULT result;
        // Ensure the GPU is done using everything and destroy the fence events.
        if(D3D.FenceEvent[0]) { result = WaitForSingleObjectEx(D3D.FenceEvent[0], 1000, FALSE); CloseHandle(D3D.FenceEvent[0]); D3D.FenceEvent[0] = nullptr; }
        if(D3D.FenceEvent[1]) { result = WaitForSingleObjectEx(D3D.FenceEvent[1], 1000, FALSE); CloseHandle(D3D.FenceEvent[1]); D3D.FenceEvent[1] = nullptr; }
        // Release the fence.
        if(D3D.Fence) { D3D.Fence->Release(); D3D.Fence = nullptr; }

        // Release the application window.
        Window.Release();

        // Release the command queue.
        if(D3D.Queue) { D3D.Queue->Release(); D3D.Queue = nullptr; }
        // Release the device.
        if(D3D.Device) { D3D.Device->Release(); D3D.Device = nullptr; }
        // Release the factory.
        if(D3D.Factory) { D3D.Factory->Release(); D3D.Factory = nullptr; }
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Begin()
    {
        // Compute the index to use for determining frame resources.
        // Frame resources are determined on an even/odd basis of the current frame being rendered.
        Int frame = Time::Manager::Singleton->Frame & 1;

        // If the frame is still doing work, wait for the work to finish before its command allocator is reset.
        // The wait interval of 1000ms is for debug purposes and should be disabled when not debugging.
        DWORD wait = WaitForSingleObjectEx(D3D.FenceEvent[frame], 1000, FALSE);
        // Debug check
        Assert(wait == WAIT_OBJECT_0, "The wait event has timed out.");
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Present()
    {
        // Helper
        auto time = Time::Manager::Singleton;

        // Compute the index to use for determining frame resources.
        // Frame resources are determined on an even/odd basis of the current frame being rendered.
        Int frame = time->Frame & 1;

        // Present the frame.
        HRESULT result = Window.D3D.SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING | DXGI_PRESENT_DO_NOT_WAIT);
        // Debug check
        Assert(SUCCEEDED(result), "There was a problem presenting the frame.");

        // Reset the frame event to denote that the frame has begun doing work.
        ResetEvent(D3D.FenceEvent[frame]);

        // Queue the fence to signal this frame's synchronization event when the frame is complete.
        D3D.Fence->SetEventOnCompletion(time->Frame, D3D.FenceEvent[frame]);
        D3D.Queue->Signal(D3D.Fence, time->Frame);
    };
}
