/*
-------------------------------------------------------------------------------
    Filename: Graphics/State.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Graphics\State.hpp"
// -- //
#include "..\Graphics\Manager.hpp"
// -- //
#include "..\Graphics\D3D12.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void Graphics::State::Create(const Graphics::State::Description& description)
    {
        // Helper
        auto& D3D = Graphics::Manager::Singleton->D3D;

        // If the material is for the graphics pipeline.. (All graphics materials must contain a vertex shader)
        if(description.VS.Handle)
        {
            // Configure default pipeline states
            D3D12_BLEND_DESC blendDesc;
            D3D12_RASTERIZER_DESC rasterizerDesc;
            D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
            // -- //
            {
                // Describe the blend state
                blendDesc.AlphaToCoverageEnable = FALSE;
                blendDesc.IndependentBlendEnable = FALSE;
                // -- //
                D3D12_RENDER_TARGET_BLEND_DESC defaultBlendDesc;
                defaultBlendDesc.BlendEnable = FALSE;
                defaultBlendDesc.LogicOpEnable = FALSE;
                defaultBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
                defaultBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
                defaultBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                defaultBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
                defaultBlendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
                defaultBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
                defaultBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
                defaultBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
                // -- //
                blendDesc.RenderTarget[0] = defaultBlendDesc;
                blendDesc.RenderTarget[1] = defaultBlendDesc;
                blendDesc.RenderTarget[2] = defaultBlendDesc;
                blendDesc.RenderTarget[3] = defaultBlendDesc;
                blendDesc.RenderTarget[4] = defaultBlendDesc;
                blendDesc.RenderTarget[5] = defaultBlendDesc;
                blendDesc.RenderTarget[6] = defaultBlendDesc;
                blendDesc.RenderTarget[7] = defaultBlendDesc;

                // Debug check
                Assert((description.Flags & 24) != 24, "Both CullFront and CullBack are enabled. There currently isn't support for enabling both culling modes");

                // Describe the rasterizer state
                rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
                rasterizerDesc.CullMode = description.CullFront ? D3D12_CULL_MODE_FRONT : D3D12_CULL_MODE_BACK;
                rasterizerDesc.FrontCounterClockwise = TRUE;
                rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
                rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
                rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
                rasterizerDesc.DepthClipEnable = FALSE;
                rasterizerDesc.MultisampleEnable = FALSE;
                rasterizerDesc.AntialiasedLineEnable = FALSE;
                rasterizerDesc.ForcedSampleCount = 0;
                rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

                // Describe the depth-stencil buffer
                depthStencilDesc.DepthEnable = description.DepthEnable ? TRUE : FALSE;
                depthStencilDesc.DepthWriteMask = (description.DepthWrite) ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL;
                depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
                depthStencilDesc.StencilEnable = FALSE;
                depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
                depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
                // -- //
                D3D12_DEPTH_STENCILOP_DESC defaultStencilOp;
                defaultStencilOp.StencilFailOp = D3D12_STENCIL_OP_KEEP;
                defaultStencilOp.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
                defaultStencilOp.StencilPassOp = D3D12_STENCIL_OP_KEEP;
                defaultStencilOp.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
                // -- //
                depthStencilDesc.FrontFace = defaultStencilOp;
                depthStencilDesc.BackFace = defaultStencilOp;
            }

            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
            // Describe the graphics pipeline state object (PSO).
            psoDesc.pRootSignature = D3D.Root;
            psoDesc.VS.pShaderBytecode = description.VS.Handle;
            psoDesc.VS.BytecodeLength = description.VS.Size;
            psoDesc.GS.pShaderBytecode = description.GS.Handle;
            psoDesc.GS.BytecodeLength = description.GS.Size;
            psoDesc.PS.pShaderBytecode = description.PS.Handle;
            psoDesc.PS.BytecodeLength = description.PS.Size;
            psoDesc.BlendState = blendDesc;
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.RasterizerState = rasterizerDesc;
            psoDesc.DepthStencilState = depthStencilDesc;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1; // Hardcoded to a single render target for the time being.
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT; // ..along with its format.
            psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            psoDesc.SampleDesc.Count = 1;
            psoDesc.NodeMask = 1;

            // Create the PSO.
            HRESULT result = D3D.Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&Handle));

            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the PSO.");
        }
        else // Otherwise the material is for the compute pipeline. (Materials that don't have a vertex shader must contain a compute shader).
        {
            D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
            // Describe the compute pipeline state object.
            psoDesc.pRootSignature = D3D.Root;
            psoDesc.CS.pShaderBytecode = description.CS.Handle;
            psoDesc.CS.BytecodeLength = description.CS.Size;
            psoDesc.NodeMask = 1;

            // Create the PSO.
            HRESULT result = D3D.Device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&Handle));

            // Debug check
            Assert(SUCCEEDED(result), "There was a problem creating the PSO.");
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::State::Release()
    {
        if(Handle)
        {
            // Release the state.
            Handle->Release();
            Handle = nullptr;
        }
    };
}
