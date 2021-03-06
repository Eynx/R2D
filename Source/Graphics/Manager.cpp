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

        // Initialize the descriptor heap.
        {
            // Setup the heap ranges.
            Heap.RTVs.Offset = 0;
            Heap.RTVs.Capacity = 64;

            Heap::Description heapDesc;
            // Create the RTV heap.
            heapDesc.Capacity = Heap.RTVs.Capacity;
            heapDesc.Flags = Heap::Flag::RTV;
            // -- //
            Heaps[0].Create(heapDesc);

            // Assign the heap ranges to their respective heaps.
            Heap.RTVs.Handle = &Heaps[0];
        }

        // Initialize the root signature.
        {
            // Prepare to describe the root signature.
            D3D12_ROOT_PARAMETER1 parameters[1];

            // Describe the root constants.
            parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            parameters[0].Constants.ShaderRegister = 0; // b0
            parameters[0].Constants.RegisterSpace = 1; // space1
            parameters[0].Constants.Num32BitValues = 4;
            parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            // Deny uneccessary access to certain pipeline stages. The Input-Assembler's Input-Layout also isn't enabled here as it's not needed or used.
            D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
            // Describe the root signature.
            rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
            rootSignatureDesc.Desc_1_1.NumParameters = _countof(parameters);
            rootSignatureDesc.Desc_1_1.pParameters = parameters;
            rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0; //_countof(samplers);
            rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr; //samplers;
            rootSignatureDesc.Desc_1_1.Flags = flags;

            // Prepare the blobs.
            ID3DBlob* signature = nullptr; ID3DBlob* error = nullptr;

            // Compile the root signature.
            HRESULT result = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem serializing the root signature.");

            // Create the root signature.
            result = D3D.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&D3D.Root));
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the root signature.");
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
                // Create a command allocator.
                result = D3D.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&D3D.Allocator[i]));
                // Debug check
                Assert(SUCCEEDED(result), "There was a problem creating the command allocator.");

                // Create an event for the fence to signal when frames are ready.
                D3D.FenceEvent[i] = CreateEventW(nullptr, TRUE, TRUE, nullptr);
                // Debug check
                Assert(D3D.FenceEvent[i], "There was a problem creating the fence event.");
            }

            // Compute the index to use for determining frame resources.
            // Frame resources are determined on an even/odd basis of the current frame being rendered.
            Int frame = Time::Manager::Singleton->Frame & 1;

            // Create the command list.
            result = D3D.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D.Allocator[frame], nullptr, IID_PPV_ARGS(&D3D.Commands));
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the command list.");

            // Close the command list since Begin() expects it to be closed for when it resets it.
            result = D3D.Commands->Close();
            // Debug check
            Assert(SUCCEEDED(result), "There was a problem closing the command list.");
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
        // Release the command list.
        if(D3D.Commands) { D3D.Commands->Release(); D3D.Commands = nullptr; }
        // Release the command allocators.
        if(D3D.Allocator[0]) { D3D.Allocator[0]->Release(); D3D.Allocator[0] = nullptr; }
        if(D3D.Allocator[1]) { D3D.Allocator[1]->Release(); D3D.Allocator[1] = nullptr; }

        // Release the RTV descriptor heap.
        Heaps[0].Release();
        // Release the application window.
        Window.Release();

        // Release the command queue.
        if(D3D.Queue) { D3D.Queue->Release(); D3D.Queue = nullptr; }
        // Release the root signature.
        if(D3D.Root) { D3D.Root->Release(); D3D.Root = nullptr; }
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

        // Reset the command allocator.
        HRESULT result = D3D.Allocator[frame]->Reset();
        // Debug check
        Assert(SUCCEEDED(result), "There was a problem resetting the command allocator.");
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Upload()
    {
        // Compute the index to use for determining frame resources.
        // Frame resources are determined on an even/odd basis of the current frame being rendered.
        Int frame = Time::Manager::Singleton->Frame & 1;

        // Open the command list for recording draw commands.
        HRESULT result = D3D.Commands->Reset(D3D.Allocator[frame], nullptr);
        // Debug check
        Assert(SUCCEEDED(result), "There was a problem opening the command list for recording draw commands.");

        // Setup the initial GPU state.
        D3D.Commands->SetGraphicsRootSignature(D3D.Root);
        D3D.Commands->SetComputeRootSignature(D3D.Root);
        D3D.Commands->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        Int4 zeroes = Int4(0, 0, 0, 0);
        // Initialize the root constants to zero (for now).
        D3D.Commands->SetGraphicsRoot32BitConstants(0, 4, &zeroes, 0);

        D3D12_RESOURCE_BARRIER barrier;
        // SwapChain Present-to-RenderTarget transition.
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = Window.D3D.RenderTargets[frame];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        // -- //
        D3D.Commands->ResourceBarrier(1, &barrier);

        D3D12_DISCARD_REGION discard;
        // Describe the entire resource for the discard.
        discard.NumRects = 0;
        discard.pRects = nullptr;
        discard.FirstSubresource = 0;
        discard.NumSubresources = 1;

        // Let the GPU know that the previous contents of the render target don't matter.
        D3D.Commands->DiscardResource(Window.D3D.RenderTargets[frame], &discard);

        // Retrieve the render target descriptor.
        auto rtv = (D3D12_CPU_DESCRIPTOR_HANDLE)Heap.RTVs[Window.D3D.RTVs[frame]];

        Float4 color(0.01f, 0.01f, 0.01f, 0.0f);
        // Clear the render target to a default colour.
        D3D.Commands->ClearRenderTargetView(rtv, color.m, 0, nullptr);

        // Bind the backbuffer as a render target.
        D3D.Commands->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        // Set the viewport and scissor.
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, Float(Window.Size.x), Float(Window.Size.y), 0.0f, 1.0f };
        D3D12_RECT rect = { 0, 0, Window.Size.x, Window.Size.y };
        // -- //
        D3D.Commands->RSSetViewports(1, &viewport);
        D3D.Commands->RSSetScissorRects(1, &rect);
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Manager::Present()
    {
        // Helper
        auto time = Time::Manager::Singleton;

        // Compute the index to use for determining frame resources.
        // Frame resources are determined on an even/odd basis of the current frame being rendered.
        Int frame = time->Frame & 1;

        D3D12_RESOURCE_BARRIER barrier;
        // SwapChain Present-to-RenderTarget transition.
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = Window.D3D.RenderTargets[frame];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        // -- //
        D3D.Commands->ResourceBarrier(1, &barrier);

        // Close the command list.
        HRESULT result = D3D.Commands->Close();
        // Debug check
        Assert(SUCCEEDED(result), "There was a problem finalizing the command list to draw the frame.");

        ID3D12CommandList* commands[] = { D3D.Commands };
        // Execute the draw commands.
        D3D.Queue->ExecuteCommandLists(1, commands);

        // Present the frame. DO_NOT_WAIT is used here as the frame should already be synchronized (and something has gone wrong if it has to block).
        result = Window.D3D.SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING | DXGI_PRESENT_DO_NOT_WAIT);
        // Debug check
        Assert(SUCCEEDED(result), "There was a problem presenting the frame.");

        // Reset the frame event to denote that the frame has begun doing work.
        ResetEvent(D3D.FenceEvent[frame]);

        // Queue the fence to signal this frame's synchronization event when the frame is complete.
        D3D.Fence->SetEventOnCompletion(time->Frame, D3D.FenceEvent[frame]);
        D3D.Queue->Signal(D3D.Fence, time->Frame);

        // Move to the next frame.
        time->Frame++;
        // TODO: Signal the next frame rather than the current frame (as signaling the first frame, frame 0, triggers instantly as the fence is already 0).
    };
}
