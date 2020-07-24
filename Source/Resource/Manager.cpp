/*
-------------------------------------------------------------------------------
    Filename: Resource/Manager.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Resource\Manager.hpp"
// -- //
#include "..\Common\Directory.hpp"
#include "..\Common\File.hpp"
// -- //
#include "..\Resource\Loader.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Resource::Manager* Resource::Manager::Singleton = nullptr;

    // ----------------------------------------------------------------------------------------
    Void Resource::Manager::Initialize()
    {
        // Reserve a set amount of slots for the resources.
        Materials.Expand(64);
        Shaders.Expand(64);
    }

    // ----------------------------------------------------------------------------------------
    Void Resource::Manager::Release()
    {
        // Release all the material resources.
        for(auto iterator = Materials.First(); !iterator.Last(); iterator.Next())
        {
            Materials.Data[iterator.Index].Release();
        }
        // -- //
        Materials.Release();

        // Release all the shader resources.
        for(auto iterator = Shaders.First(); !iterator.Last(); iterator.Next())
        {
            Shaders.Data[iterator.Index].Release();
        }
        // -- // 
        Shaders.Release();
    };

    // ----------------------------------------------------------------------------------------
    Void Resource::Manager::InitializeResourceLocation(const String& directory)
    {
        Directory folder;
        // Retrieve the names of all the files and folders in the directory.
        folder.Read(directory);

        // Iterate over the files in the directory.
        for(Int i = 0; i < folder.Files.Count; i++)
        {
            // Helper
            String& filename = folder.Files[i];

            // Check if the filename contains a file extension.
            Int pos = filename.rFind('.');
            // -- //
            if(pos < filename.Length)
            {
                // If the length of the filename is three characters long..
                if((filename.Length - pos) == 4)
                {
                    // Extract and reverse the byte-order of the file extension substring (for readability purposes).
                    Int extension = (filename[pos + 1] << 16) + (filename[pos + 2] << 8) + (filename[pos + 3]);

                    // Parse the extension.
                    switch(extension)
                    {
                        // Resource Definition Tags resources file.
                        // TODO: This is currently .txt to convey its plaintext contents. Considering using .rdf in the future.
                        case 'txt':
                        {
                            File file;
                            // Open the file and load its contents.
                            file.Open(directory + filename, File::Mode::Read);
                            Memory::Buffer buffer = file.Load();
                            file.Close();

                            Resource::Loader::TXT txt;
                            // Load and parse the TXT data.
                            txt.Load(buffer);
                            txt.Parse(directory);

                            break;
                        }
                    }
                }
            }
        }

        // Initialize all subfolders contained in the directory.
        for(Int i = 0; i < folder.Folders.Count; i++)
        {
            InitializeResourceLocation(directory + folder.Folders[i] + "/");
        }
    }
}
