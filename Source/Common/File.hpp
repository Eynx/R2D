/*
-------------------------------------------------------------------------------
    Filename: Common/File.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Memory\Buffer.hpp"
#include "..\Common\String.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    class File
    {
    public:
        // Types

        // Filemode flags; Privileges to request when opening a file.
        enum class Mode : Byte
        {
            // Open the file with read access. Sets the file pointer to the beginning of the file.
            Read = 1,
            // Open the file with write access. Sets the file pointer to the end of the file (one position past the last byte).
            // This overrides the read access file pointer offset.
            Write = 2,
            // Open the file with write access. Sets the file pointer to the beginning of the file.
            Overwrite = 4
        };
        // Seek offset modes.
        enum class Offset : Byte
        {
            Begin = 0, // Seek offset starting from the first byte in the file.
            Current = 1, // Seek offset from the current position in the file.
            End = 2 // Seek offset from the end of the file.
        };

    public:
        // Members

        // Handle to the file object.
        Void* Handle;
        // The size of the file's contents in bytes.
        Long Size;
        // The position of the file pointer used for the next read/write operation.
        // The end of the file is reached when this is equal to the file's size.
        Long Position;

    public:
        // Constructors

        // Default constructor.
        File() : Handle(nullptr), Size(0), Position(0) {};
        // Copy constructor.
        File(const File& other) = delete;
        // Move constructor.
        File(File&& other) : Handle(other.Handle), Size(other.Size), Position(other.Position) { other.Handle = nullptr; other.Size = 0; other.Position = 0; };
        // Destructor.
        ~File() { Close(); }

        // Methods

        // Open a file.
        Void Open(const String& filename, Mode filemode);
        // Close the current handle.
        Void Close();

        // Move the file read/write index. To be implemented later.
        Long Seek(Long seek, Offset offset);

        // Load and return a memory buffer containing the specified amount of data from the file.
        Memory::Buffer Load(Long size);
        // Load and return a memory buffer containing the remaining data in a file.
        Memory::Buffer Load() { return Load(Size - Position); };

        // Read an object from the file. Preferably, only call this for POD types that don't contain pointers.
        template <typename Type> Type Read()
        {
            Type object;
            // Call the internal Void specialization to read the bytes into the object.
            Read((Void*)&object, sizeof(Type));
            return object;
        };
        // Read an array of objects from the file into the specified buffer.
        template <typename Type> Void Read(Type* buffer, Long count)
        {
            // Call the internal Void specialization to read the bytes into the buffer.
            Read((Void*)buffer, sizeof(Type) * count);
        };

        // Write an object to the file.
        template <typename Type> Void Write(const Type& object)
        {
            // Call the Void specialization write the object's bytes to the file.
            Write((Void*)&object, sizeof(Type));
        };
        // Write an array of objects to the file.
        template <typename Type> Void Write(const Type* buffer, Long count)
        {
            // Call the Void specialization to write the buffer to the file.
            Write((Void*)buffer, sizeof(Type) * count);
        };

        // Write a string to the file.
        template <> Void Write(const String& string)
        {
            // Call the Void specialization to write the string's data to the file.
            Write(string.Data, string.Length);
        };
    };

    // ------------------------------------------------------------------------
    extern template Void File::Read(Void* buffer, Long size);

    // ------------------------------------------------------------------------
    extern template Void File::Write(const Void* buffer, Long size);
}

// Helper operators for dealing with the file privileges.
DEFINE_ENUM_OPERATORS(R2D::File::Mode);
