/*
-------------------------------------------------------------------------------
    Filename: Graphics/Window.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Graphics\Window.hpp"
// -- //
#include "..\Graphics\Manager.hpp"
// -- //
#include "..\Graphics\D3D12.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void Graphics::Window::Create(const Graphics::Window::Description& description)
    {
        // Debug check
        Assert(!Handle, "Window handle already populated. Check if the window has already been created or make sure the Graphics::Manager is being properly constructed.");

        // Create and initialize the application window.
        {
            // Get the application name
            LPCWSTR applicationName = L"R2D";

            Int x, y, width, height;
            // Prepare the size and position of the window.
            {
                RECT rect;
                // Adjust the window size parameters to satisfy the requested client area size.
                rect.left = 0;
                rect.top = 0;
                rect.right = description.Width;
                rect.bottom = description.Height;
                // -- //
                AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);
                // -- //
                width = rect.right - rect.left;
                height = rect.bottom - rect.top;

                // Place the window in the middle of the screen.
                x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
                y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
            }

            // Create the window and store the handle to it.
            Handle = CreateWindowExW(WS_EX_APPWINDOW, applicationName, applicationName, WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

            // Debug checks
            Assert(Handle, "There was a problem creating the window.");
            Assert(Handle != INVALID_HANDLE_VALUE, "There was a problem creating the window.");
        }

        // Create and initialize the swap-chain.
        {
            // Retreive the graphics manager.
            auto graphics = Graphics::Manager::Singleton;

            // Initialize the swap-chain.
            {
                // Retrieve the factory.
                auto factory = graphics->D3D.Factory;

                DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
                // Describe the swap-chain.
                swapChainDesc.Width = description.Width;
                swapChainDesc.Height = description.Height;
                swapChainDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
                swapChainDesc.Stereo = FALSE;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.SampleDesc.Quality = 0;
                swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
                swapChainDesc.BufferCount = 2;
                swapChainDesc.Scaling = DXGI_SCALING_NONE;
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
                // Create the swap-chain.
                HRESULT result = factory->CreateSwapChainForHwnd(graphics->D3D.Queue, (HWND)Handle, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&D3D.SwapChain);
                // Debug check
                Assert(SUCCEEDED(result), "There was a problem creating the swap-chain.");

                // Disable fullscreen transitions.
                result = factory->MakeWindowAssociation((HWND)Handle, DXGI_MWA_NO_ALT_ENTER);
                // Debug check
                Warning(SUCCEEDED(result), "There was a problem disabling fullscreen transitions.");

                UINT support;
                // Retrieve the colour profile.
                result = D3D.SwapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709, &support);

                // Set the colour profile.
                result = D3D.SwapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709);
                // Note: DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709 is the only supported colour profile for 16-bit swap-chains.

                // Debug check
                Assert(SUCCEEDED(result), "There was a problem setting the colour profile.");
            }

            // Initialize the swap-chain render targets.
            for(Int i = 0; i < 2; i++)
            {
                // Retrieve a pointer to the backbuffer.
                HRESULT result = D3D.SwapChain->GetBuffer(i, IID_PPV_ARGS(&D3D.RenderTargets[i]));
                // Debug check
                Assert(SUCCEEDED(result), "There was a problem retrieving the handle to the swap-chain backbuffers.");

                D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
                // Create an RTV for the render target.
                rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = 0;
                rtvDesc.Texture2D.PlaneSlice = 0;
                // -- //
                Graphics::Descriptor descriptor = graphics->Heap.RTVs.Request();
                D3D.RTVs[i] = descriptor.Index;
                // -- //
                graphics->D3D.Device->CreateRenderTargetView(D3D.RenderTargets[i], &rtvDesc, descriptor);
            }
        }

        // Store the window size.
        Width = description.Width;
        Height = description.Height;

        auto handle = (HWND)Handle;
        // Finally, setting the window visibility last ensures that the user doesn't see the temporary stages the window goes through during creation (looks like flickering/lag).
        ShowWindow(handle, SW_SHOW);
        SetForegroundWindow(handle);
        SetFocus(handle);
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Window::Release()
    {
        // Release the render targets.
        if(D3D.RenderTargets[0]) { D3D.RenderTargets[0]->Release(); D3D.RenderTargets[0] = nullptr; }
        if(D3D.RenderTargets[1]) { D3D.RenderTargets[1]->Release(); D3D.RenderTargets[1] = nullptr; }
        // Release the swap chain.
        if(D3D.SwapChain) { D3D.SwapChain->Release(); D3D.SwapChain = nullptr; }

        // Destroy the window if one exists.
        if(Handle) { DestroyWindow((HWND)Handle); Handle = nullptr; }
    };
}
