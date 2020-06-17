/*
-------------------------------------------------------------------------------
    Filename: Common/Memory/Buffer.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\..\Common\Memory\Buffer.hpp"
// -- //
#include "..\..\Common\Memory.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void Memory::Buffer::Create(Long size)
    {
        // Debug check
        Assert(!Data, "Attempting to allocate memory with a buffer that already has memory.");

        // Allocate the requested amount of memory.
        Data = (Byte*)Memory::Request(size);
        Size = size;
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Buffer::Release()
    {
        // Deallocate the memory.
        if(Data) { Memory::Free(Data); Data = nullptr; }
        // Reset the members.
        Position = 0;
        Size = 0;
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Buffer::Seek(Long seek, Offset offset)
    {
        // Add the seek based on offset mode.
        switch(offset)
        {
            case Offset::Begin: { Position = seek; break; };
            case Offset::Current: { Position += seek; break; };
            case Offset::End: { Position = Size + seek; break; };
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Memory::Buffer::Load(Void* buffer, Long size)
    {
        // Copy the data into the buffer.
        Memory::Copy(buffer, Data + Position, size);
    };
}
