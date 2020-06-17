/*
-------------------------------------------------------------------------------
    Filename: Graphics/State.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// -- //
#include "..\Graphics.hpp"

// Forward declarations.
struct ID3D12PipelineState;

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Graphics
    {
        // ------------------------------------------------------------------------------------
        class State
        {
        public:
            // Types

            // Description struct for creating pipeline states.
            struct Description
            {
                // Description of a memory buffer containing shader bytecode.
                struct Shader
                {
                    // Handle to the shader bytecode.
                    Void* Handle = nullptr;
                    // Size of the bytecode.
                    Long Size = 0;
                };

                // Vertex shader to be bound to this state.
                Shader VS;
                // Geometry shader to be bound to this state.
                Shader GS;
                // Pixel shader to be bound to this state.
                Shader PS;
                // Compute shader to be bound to this state.
                Shader CS;
                // Material creation flags.
                union
                {
                    struct
                    {
                        // Set to true to enable the depth buffer. Otherwise disable depth clipping and writing when disabled.
                        // Must be set to false if no depth buffer is bound.
                        // Default is true.
                        Byte DepthEnable : 1;
                        // Set to true to enable writing to the depth buffer. Otherwise depth testing will still occur as normal, but the depth buffer will not be updated by this material.
                        // DEPTH_WRITE must be enabled for the depth buffer if this is set to true.
                        // Default is true.
                        Byte DepthWrite : 1;
                        // Set to true to use a counterclockwise winding mode for the front of a triangle. If set to false, a clockwise winding order will be used to denote the front instead.
                        // Default is true.
                        Byte FrontCounterClockwise : 1;
                        // Set to true to cull pixels on the front of the triangle.
                        // Default is false.
                        Byte CullFront : 1;
                        // Set to true to cull pixels on the back of the triangle.
                        // Default is true.
                        Byte CullBack : 1;
                    };
                    // Helper byte containing the material flags.
                    Byte Flags = 23;
                };
            };

        public:
            // Members

            // D3D12 handle to the pipeline state object. 
            ID3D12PipelineState* Handle;

        public:
            // Constructors

            // Default constructor.
            State() : Handle(nullptr) {};
            // Copy constructor.
            State(const State& other) = delete;
            // Move constructor.
            State(State&& other) : Handle(other.Handle) { other.Handle = nullptr; };
            // Destructor.
            ~State() { Release(); };

            // Methods

            // Create a Graphics Pipeline State object.
            Void Create(const Description& description);
            // Release this D3D12 resource.
            Void Release();
        };
    }
}
