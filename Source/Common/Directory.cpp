/*
-------------------------------------------------------------------------------
    Filename: Common/Directory.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Common\Directory.hpp"
// -- //
#include "..\Common\Windows.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    String* Directory::Working = nullptr;

    // ----------------------------------------------------------------------------------------
    Void Directory::Read(const String& directory)
    {
        std::wstring wdirectory;
        // Prepare a UTF-16 path for WINAPI.
        {
            // Search for a colon symbol (that usually follows a drive symbol..) to determine if the path is absolute.
            Int pos = directory.Find(':');

            // If the symbol wasn't found..
            if(pos == directory.Length)
            {
                // Prefix the working directory to the local path and convert the string to UTF-16.
                wdirectory = widen(*Directory::Working + "\\" + directory + "*");
            }
            else
            {
                // Simply convert the absolute path to UTF-16.
                wdirectory = widen(directory + "*");
            }
        }

        WIN32_FIND_DATA ffd;
        // Find the first object in the directory.
        HANDLE hFind = FindFirstFileW(wdirectory.c_str(), &ffd);

        // Debug check
        Assert(hFind != INVALID_HANDLE_VALUE, "No such directory was found.");
        // TODO: This is ideally a warning but no method for handling warnings exists yet.

        // Continue locating and adding objects to the directory.
        do
        {
            // Translate the filename from UTF-16 to UTF-8.
            String name = narrow(ffd.cFileName);

            // Check if the last character of the filename isn't a period (to skip the "." and ".." directory operators).
            if(name[name.Length - 1] != '.')
            {
                // If the result is a directory..
                if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // Append to the folders array.
                    Folders.Reserve(1);
                    Folders.Append(Move(name));
                }
                else
                {
                    // Append to the files array.
                    Files.Reserve(1);
                    Files.Append(Move(name));
                }
            }
        } while(FindNextFileW(hFind, &ffd));

        // Release the search handle.
        FindClose(hFind);
    }
}
