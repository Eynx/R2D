/*
-------------------------------------------------------------------------------
    Filename: Common/Memory/Buffer.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\..\Common.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Memory
    {
        // ------------------------------------------------------------------------------------
        class Buffer
        {
        public:
            // Types

            // Seek offset specifier.
            enum class Offset : Byte
            {
                Begin = 0, // Seek offset from the first byte in the buffer.
                Current = 1, // Seek offset from the current position.
                End = 2 // Seek offset from the past-the-end position in the buffer.
            };

        public:
            // Members

            // Handle to the data allocated by the buffer.
            Byte* Data;
            // The position of the index in the buffer data will be read from or written to.
            Long Position;
            // The size of the buffer's data in bytes.
            Long Size;

        public:
            // Constructors

            // Default constructor.
            Buffer() : Data(nullptr), Position(0), Size(0) {};
            // Data and Size constructor.
            Buffer(Void* buffer, Long size) : Data((Byte*)buffer), Position(0), Size(size) {};
            // Copy constructor.
            Buffer(const Buffer& other) = delete;
            // Move constructor.
            Buffer(Buffer&& other) : Data(other.Data), Position(other.Position), Size(other.Size) { other.Data = nullptr; other.Position = 0; other.Size = 0; };
            // Destructor.
            ~Buffer() { Release(); };

            // Methods

            // Allocate a block of memory for the buffer.
            Void Create(Long size);
            // Release the memory allocated for the buffer.
            Void Release();

            // Offset the index position the specified number of bytes.
            Void Seek(Long seek, Offset offset);
            // Load data from the buffer directly into another buffer.
            Void Load(Void* buffer, Long size);

            // Read an object from the buffer without advancing the index position.
            template <typename Type> Type& Peek();

            // Read an object from the buffer and advance the index position.
            template <typename Type> Type& Read();
            // Retrieve a pointer to an array of objects in the buffer, advancing the index position by the size of the array.
            template <typename Type> Type* Read(Long count);
            // Read an array of objects from the buffer into another buffer, advancing the index position by the size of the array.
            template <typename Type> Void Read(Type* buffer, Long count);

            // Write an object to the buffer and advance the index position.
            template <typename Type> Void Write(const Type& object);
            // Write an array of objects to the buffer, advancing the index position by the size of the array.
            template <typename Type> Void Write(Type* buffer, Long count);
        };

        // ------------------------------------------------------------------------------------
        template <typename Type> Type& Buffer::Peek()
        {
            // Debug check
            Assert(Position + sizeof(Type) <= Size, "Attempting to read past end of buffer.");

            // Retrieve a the object.
            Type* object = (Type*)(Data + Position);

            return *object;
        };

        // ------------------------------------------------------------------------------------
        template <typename Type> Type& Buffer::Read()
        {
            // Debug check
            Assert(Position + sizeof(Type) <= Size, "Attempting to read past end of buffer.");

            // Retrieve a pointer to the object.
            Type* object = (Type*)(Data + Position);

            // Advance the position index.
            Position += sizeof(Type);

            return *object;
        };

        // ------------------------------------------------------------------------------------
        template <typename Type> Type* Buffer::Read(Long count)
        {
            // Debug check
            Assert(Position + sizeof(Type) * count <= Size, "Attempting to read past end of buffer.");

            // Retrieve a pointer to the array.
            Type* pointer = (Type*)(Data + Position);

            // Advance the position index.
            Position += sizeof(Type) * count;

            return pointer;
        };

        // ------------------------------------------------------------------------------------
        template <typename Type> Void Buffer::Read(Type* buffer, Long count)
        {
            // Debug check
            Assert(Position + sizeof(Type) * count <= Size, "Attempting to read past end of buffer.");

            // Copy the requested data from this buffer into the output buffer.
            Load(buffer, sizeof(Type) * count);

            // Advance the position index.
            Position += sizeof(Type) * count;
        };

        // ------------------------------------------------------------------------------------
        template <> inline Void Buffer::Read(Void* buffer, Long size)
        {
            // Debug check
            Assert(Position + size <= Size, "Attempting to read past end of buffer.");

            // Copy the requested data from this buffer into the output buffer.
            Load(buffer, size);

            // Advance the position index.
            Position += size;
        };

        // ------------------------------------------------------------------------------------
        template <typename Type> Void Buffer::Write(const Type& object)
        {
            // Debug check
            Assert(Position + sizeof(Type) <= Size, "Attempting to write past end of buffer.");

            // Copy the object to the buffer using the type's assignment operator.
            *(Type*)(Data + Position) = object;

            // Advance the position index.
            Position += sizeof(Type);
        };

        // ------------------------------------------------------------------------------------
        template <typename Type> Void Buffer::Write(Type* buffer, Long count)
        {
            // Debug check
            Assert(Position + sizeof(Type) * count <= Size, "Attempted to write past end of buffer.");

            // Loop over each object to be written.
            for(Long i = 0; i < count; i++)
            {
                // Copy the object to the buffer using the type's assignment operator.
                (Type*)(Data + Position)[i] = buffer[i];
            }
            // TODO: Maybe use a Memory::Copy() here instead? (Would be valid for only POD types)

            // Advance the position index.
            Position += sizeof(Type) * count;
        };
    }
}
