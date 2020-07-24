/*
-------------------------------------------------------------------------------
    Filename: Resource/Material.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Resource\Material.hpp"
// -- //
#include "..\Resource\Manager.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void Resource::Material::Create(const Resource::Material::Description& description)
    {
        // Helper
        Resource::Manager* resources = Resource::Manager::Singleton;

        Graphics::State::Description stateDesc;
        // Prepare the graphics state description.
        stateDesc.Flags = description.Flags;

        // If the material references a vertex shader... (denoting its for the rasterization pipeline)
        if(description.VS.Handle)
        {
            // If the material references a pixel shader... (denoting its used for drawing to render targets)
            if(description.PS.Handle)
            {
                // Locate the shader resources.
                auto vs = resources->Shaders.Find(description.VS.Handle);
                auto ps = resources->Shaders.Find(description.PS.Handle);

                // Debug checks
                Assert(vs, "Unable to create the material: No vertex shader with the resource ID was found.");
                Assert(ps, "Unable to create the material: No pixel shader with the resource ID was found.");

                // Populate the graphics state description.
                stateDesc.VS.Handle = vs->Data;
                stateDesc.VS.Size = vs->Size;
                // -- //
                stateDesc.PS.Handle = ps->Data;
                stateDesc.PS.Size = ps->Size;
            }
            else // Otherwise the material is VS-only and is for depth-only rendering or readback materials.
            {
                // Locate the shader resource.
                auto vs = resources->Shaders.Find(description.VS.Handle);

                // Debug check
                Assert(vs, "Unable to create the material: No vertex shader with the resource ID was found.");

                // Populate the graphics state description.
                stateDesc.VS.Handle = vs->Data;
                stateDesc.VS.Size = vs->Size;
            }
        }
        else // Otherwise the material is for the compute shader pipeline.
        {
            // Locate the shader resource.
            auto cs = resources->Shaders.Find(description.CS.Handle);

            // Debug check
            Assert(cs, "Unable to create the material: No compute shader with the resource ID was found.");

            // Populate the state description.
            stateDesc.CS.Handle = cs->Data;
            stateDesc.CS.Size = cs->Size;
        }

        // Create the graphics state
        Resource.Create(stateDesc);
    };
}
