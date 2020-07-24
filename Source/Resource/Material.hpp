/*
-------------------------------------------------------------------------------
    Filename: Resource/Material.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// -- //
#include "..\Graphics.hpp"
#include "..\Graphics\State.hpp"
// -- //
#include "..\Resource.hpp"
#include "..\Resource\Shader.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Resource
    {
        // ------------------------------------------------------------------------------------
        class Material
        {
        public:
            // Types

            // Description struct used for creating materials.
            struct Description
            {
                // The resource ID of the vertex shader for the material.
                ID<Shader> VS = 0;
                // The resource ID of the pixel shader for the material.
                ID<Shader> PS = 0;
                // The resource ID of the compute shader for the material.
                ID<Shader> CS = 0;
                // Material creation flags. The same as in a Graphics::State::Description object.
                Graphics::State::Description::Options Flags;
            };

        public:
            // Members

            // The GPU resource the material owns.
            Graphics::State Resource;

        public:
            // Constructors.

            // Default constructor.
            Material() : Resource() {};
            // Copy constructor.
            Material(const Material& other) = delete;
            // Move constructor.
            Material(Material&& other) : Resource(Move(other.Resource)) {};
            // Destructor.
            ~Material() { Release(); };

            // Methods

            // Create a graphics state for the material.
            Void Create(const Description& description);
            // Release the graphics state owned by the material.
            Void Release() { Resource.Release(); };
        };
    }
}
