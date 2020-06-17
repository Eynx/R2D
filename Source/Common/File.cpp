/*
-----------------------------------------------------------------------------
    Filename: Common/File.cpp
-----------------------------------------------------------------------------
*/

// Includes
#include "..\Common\File.hpp"
// -- //
#include "..\Common\Directory.hpp"
// -- //
#include "..\Common\Windows.hpp"

// ----------------------------------------------------------------------------------------
namespace R2D
{
    // ------------------------------------------------------------------------------------
    Void File::Open(const String& filename, File::Mode filemode)
    {
        // Debug check
        Assert(!Handle, "Attempting to open a file that is already open.");

        // Prepare to set file the creation policies.
        DWORD fileAccessPolicy = 0, fileCreationPolicy = 0;
        {
            // If the user wants to read from the file..
            if(bool(filemode & File::Mode::Read))
            {
                fileAccessPolicy |= GENERIC_READ;
                fileCreationPolicy = OPEN_EXISTING;
            }

            // If the user wants to write to the file..
            if(bool(filemode & File::Mode::Write) || bool(filemode & File::Mode::Overwrite))
            {
                fileAccessPolicy |= GENERIC_WRITE;
                fileCreationPolicy = bool(filemode & File::Mode::Overwrite) ? CREATE_ALWAYS : OPEN_ALWAYS;
            }
        }

        // Prepare the file path.
        std::wstring path;
        {
            // Search for a colon symbol (that usually follows a drive symbol..) to determine if the path is absolute.
            Int pos = filename.Find(':');

            // If the symbol wasn't found..
            if(pos == filename.Length)
            {
                // Prefix the working directory to the local path and convert the string to UTF-16.
                path = widen(*Directory::Working + "\\" + filename);
            }
            else
            {
                // Simply convert the absolute path to UTF-16.
                path = widen(filename);
            }
        }

        // Open the file.
        Handle = CreateFileW(path.c_str(), fileAccessPolicy, FILE_SHARE_READ, nullptr, fileCreationPolicy, FILE_ATTRIBUTE_NORMAL, nullptr);

        // Debug checks
        Assert(Handle, "There was a problem opening a file.");
        Assert(Handle != INVALID_HANDLE_VALUE, "There was a problem opening a file.");

        // Retrieve the file information description.
        BY_HANDLE_FILE_INFORMATION fileInfo;
        BOOL result = GetFileInformationByHandle(Handle, &fileInfo);

        // Debug check
        Assert(result > 0, "There was a problem retrieving the properties of a file.");

        // Set file size.
        Size = Long(fileInfo.nFileSizeLow) | (Long(fileInfo.nFileSizeHigh) << 32);

        // Initialize the position index.
        Seek(0, Offset::Current);
    };

    // ------------------------------------------------------------------------------------
    Void File::Close()
    {
        // Release the file.
        if(Handle) { CloseHandle(Handle); Handle = nullptr; }
    };

    // ------------------------------------------------------------------------------------
    Long File::Seek(Long seek, Offset offset)
    {
        // Update the file pointer and position value.
        Position = SetFilePointer(Handle, seek, nullptr, (DWORD)offset);
        return Position;
    };

    // ------------------------------------------------------------------------------------
    template <> Void File::Read(Void* buffer, Long size)
    {
        // Prepare to receive the number of bytes read.
        DWORD bytesRead;

        // Read the data.
        BOOL result = ReadFile(Handle, buffer, size, &bytesRead, nullptr);
        Position += bytesRead;

        // Debug checks
        Assert(result > 0, "There was a problem reading data from a file.");
        Assert(size == bytesRead, "Attempted to read past the end file.");
    };

    // ------------------------------------------------------------------------------------
    template <> Void File::Write(const Void* buffer, Long size)
    {
        // Prepare to receive the number of bytes written.
        DWORD bytesWritten;

        // Write the data.
        BOOL result = WriteFile(Handle, buffer, size, &bytesWritten, nullptr);
        Position += bytesWritten;

        // Increase Size if the file is having new bytes written to the end of it.
        if(Position > Size) { Size = Position; }

        // Debug checks
        Assert(result > 0, "There was a problem writing data to a file.");
        Assert(size == bytesWritten, "The file had less bytes written to it than requested.");
    };

    // ------------------------------------------------------------------------------------
    Memory::Buffer File::Load(Long size)
    {
        // Debug check
        Assert((Position + size) <= Size, "Attempting to load more data from the file than it contains.");

        Memory::Buffer buffer;
        // Create and populate the buffer.
        buffer.Create(size);
        // -- //
        Read(buffer.Data, size);

        return buffer;
    };
}
