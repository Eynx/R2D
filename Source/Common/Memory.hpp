/*
-------------------------------------------------------------------------------
    Filename: Common/Memory.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
// Namespace pollution (required for placement new).
#include <new>

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Memory
    {
        // Request a new memory allocation. Does nothing if size is zero. Does not fail silently if size is negative.
        // Attempting to request more memory than is available does not fail silently.
        extern Void* Request(Int size);

        // Helper function for allocating an instance of a type. Parameters are used to call a matching constructor for the type.
        template <typename Type, typename... Arguments> Type* Request(Arguments&&... arguments) { return new(Request(sizeof(Type)))Type(arguments...); }

        // Resize an existing allocation. Does nothing if the requested size is equal to or smaller than the current allocation.
        // If the pointer is null, instead requests new memory. Obeys the same rules as Request().
        extern Void* Resize(Void* handle, Int size);

        // Release an existing allocation. back for reuse. Does nothing if the pointer is null.
        // Does not fail silently if an invalid handle, i.e. a nonexistent allocation, is specified.
        extern Void Free(Void* handle);

        // Copy a block of memory from one location to another location.
        // Does nothing if a size of zero bytes is specified. Does not fail silently if size is negative.
        // Does not fail silently if either the destination or source pointers are null.
        // Behaviour is undefined if the source block overlaps the destination block. Use Memory::Move for such a situation.
        extern Void Copy(Void* destination, const Void* source, Int size);

        // Move a block of memory from one location to another location.
        // Does nothing if a size of zero bytes is specified. Does not fail silently if size is negative.
        // Does not fail silently if either the destination or source pointers are null.
        // While semantically the same as copying memory, moving memory guarantees the final destination block is an
        // uncorrupted copy of the original source block in the situation where the destination block overlaps the source block.
        extern Void Move(Void* destination, const Void* source, Int size);

        // Initialize a block of memory to a value. Does nothing if a size of zero bytes is specified.
        // Does not fail silently if the destination pointer is null or the size is negative.
        extern Void Set(Void* destination, Int value, Int size);

        // Initialize a block of memory to zero. Helper function for calling Memory::Set().
        extern Void Zero(Void* destination, Int size);
    }
}
