/*
-------------------------------------------------------------------------------
    Filename: Common/Directory.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Array.hpp"
#include "..\Common\String.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    class Directory
    {
    public:
        // Members

        // Array containing the names of the folders located in this directory.
        Array<String> Folders;
        // Array containing the names of the files located in this directory.
        Array<String> Files;

        // Static; The current working directory of the executable.
        static String* Working;

    public:
        // Constructors

        // Default constructor.
        Directory() : Folders(), Files() {};
        // Copy constructor.
        Directory(const Directory& other) = delete;
        // Move constructor.
        Directory(Directory&& other) : Folders(Move(other.Folders)), Files(Move(other.Files)) {};
        // Destructor.
        ~Directory() { Folders.Release(); Files.Release(); };

        // Methods

        // Populate the directory object with the names of a path's contents.
        Void Read(const String& path);
    };
}
