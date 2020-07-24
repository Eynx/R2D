/*
-------------------------------------------------------------------------------
    Filename: Resource/Manager.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Map.hpp"
#include "..\Common\String.hpp"
// -- //
#include "..\Resource.hpp"
#include "..\Resource\Material.hpp"
#include "..\Resource\Shader.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Resource
    {
        // ------------------------------------------------------------------------------------
        class Manager
        {
        public:
            // Members

            // Map contain material IDs linked to their material resource.
            Map<ID<Material>, Material> Materials;
            // Map containing shader IDs linked to their shader resource.
            Map<ID<Shader>, Shader> Shaders;

            // Static interface handle.
            static Manager* Singleton;

        public:
            // Constructors

            // Default constructor.
            Manager() :Materials(), Shaders() {};
            // Copy constructor.
            Manager(const Manager& other) = delete;
            // Move constructor.
            Manager(Manager&& other) : Materials(Move(other.Materials)), Shaders(Move(other.Shaders)) {};
            // Destructor.
            ~Manager() { Release(); };

            // Methods

            // Initialize the resource manager.
            Void Initialize();
            // Release all of the resources and uninitialize the resource manager.
            Void Release();

            // Scan a directory and its subfolders for resource descriptions to load.
            Void InitializeResourceLocation(const String& directory);
        };
    }
}
