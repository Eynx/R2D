/*
-------------------------------------------------------------------------------
    Filename: Common/Memory.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Common\Memory.hpp"
// -- //
#include "..\Common\Windows.hpp"
#include <malloc.h>

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void* Memory::Request(Long size)
    {
        // Debug check
        Assert(size >= 0, "Attempting to request an invalid amount of memory.");

        Void* pointer = nullptr;
        // Only allocate memory if size is larger than zero.
        if(size > 0)
        {
            // Addresses are guaranteed (and assumed) to be 16-byte aligned by default.
            pointer = _aligned_malloc(size, 16);
        }

        return pointer;
    };

    // ----------------------------------------------------------------------------------------
    Void* Memory::Resize(Void* handle, Long size)
    {
        // Debug check
        Assert(size >= 0, "Attempting to request an invalid amount of memory.");

        Void* pointer = handle;
        // Only allocate memory if size is larger than zero.
        if(size > 0)
        {
            // Addresses are guaranteed (and assumed) to be 16-byte aligned by default.
            pointer = _aligned_realloc(handle, size, 16);
        }

        return pointer;
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Free(Void* handle)
    {
        // Debug check
        Warning(handle, "Attempting to free memory from a nullptr.");

        // Manually verify if the pointer is null to ensure the no-op case when a nullptr is specified.
        if(handle) { _aligned_free(handle); }
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Copy(Void* destination, const Void* source, Long size)
    {
        // Debug check
        Assert(size >= 0, "Attempting to copy an invalid amount of memory.");

        // Only copy memory if size is larger than zero.
        if(size > 0)
        {
            // Debug checks
            Assert(destination, "Attempting to copy data into an invalid memory buffer.");
            Assert(source, "Attempting to read data from an invalid memory buffer.");

            // Simply call memcpy
            memcpy(destination, source, size);
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Move(Void* destination, const Void* source, Long size)
    {
        // Debug check
        Assert(size >= 0, "Attempting to move an invalid amount of memory.");

        // Only move memory if size is larger than zero.
        if(size > 0)
        {
            // Debug checks
            Assert(destination, "Attempting to move data into an invalid memory buffer.");
            Assert(source, "Attempting to read data from an invalid memory buffer.");

            // Simply call memmove
            memmove(destination, source, size);
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Set(Void* destination, Int value, Long size)
    {
        // Debug check
        Assert(size >= 0, "Attempting to move an invalid amount of memory.");

        // Only initialize memory if size is larger than zero.
        if(size > 0)
        {
            // Debug checks
            Assert(destination, "Attempting to initialize an invalid memory buffer.");

            // Simply call memset
            memset(destination, value, size);
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Zero(Void* destination, Long size)
    {
        // Internally just a helper for calling Memory::Set(). Could potentially be more efficient than Set, however.
        Memory::Set(destination, 0, size);
    };
}
