/*
-------------------------------------------------------------------------------
    Filename: Resource/Shader.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Memory.hpp"
// -- //
#include "..\Resource.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Resource
    {
        // ------------------------------------------------------------------------------------
        class Shader
        {
        public:
            // Members

            // Pointer to the shader's bytecode data.
            Void* Data;
            // The size in bytes of the shader's bytecode data.
            Int Size;

        public:
            // Constructors

            // Default constructor.
            Shader() : Data(nullptr), Size(0) {};
            // Copy constructor.
            Shader(const Shader& other) = delete;
            // Move constructor.
            Shader(Shader&& other) : Data(other.Data), Size(other.Size) { other.Data = nullptr; other.Size = 0; };
            // Destructor.
            ~Shader() { Release(); };

            // Methods

            // Allocate the shader and copy the supplied bytecode to it.
            // If data is a nullptr, only allocate memory for the shader bytecode.
            Void Create(const Void* data, Int size) { Assert(!Data, "Attempting to initialize a shader that has already has data initialized."); Data = Memory::Request(size); if(data) { Memory::Copy(Data, data, size); } Size = size; }
            // Release the memory allocated for the bytecode.
            Void Release() { if(Data) { Memory::Free(Data); Data = nullptr; } Size = 0; };
        };
    }
}
