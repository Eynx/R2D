/*
-------------------------------------------------------------------------------
    Filename: Resource/Loader.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Resource\Loader.hpp"
// -- //
#include "..\Common\File.hpp"
// -- //
#include "..\Resource\Manager.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Void Resource::Loader::TXT::Release()
    {
        // Release the tags.
        Tags.Release();
        // Release the values.
        Values.Release();
    };

    // ----------------------------------------------------------------------------------------
    Void Resource::Loader::TXT::Load(Memory::Buffer& buffer)
    {
        // Local functions
        struct Local
        {
            // Test if a character is a letter.
            static Bool Alphabetical(Byte character)
            {
                return ((character > 64) && (character < 91)) || ((character > 96) && (character < 123));
            };
            // Test if a character is a number.
            static Bool Numerical(Byte character)
            {
                return (character > 47) && (character < 58);
            };
            // Test if a character is a letter or a number.
            static Bool Alphanumeric(Byte character)
            {
                return Alphabetical(character) || Numerical(character);
            };
        };

        // Debug checks
        Assert(Tags.Data == nullptr, "Attempting to parse resource definitions with a parser that has already been used.");
        Assert(Values.Data == nullptr, "Attempting to parse resource definitions with a parser that has already been used.");

        // The intermediate tag structure for facilitating building the flattened tags array in the TXT structure. 
        // X is the hash of the tag's name.
        // Y is the ID of the scope assigned to the tag, if it has one.
        // Z is the index of the first value in the tag's value array.
        // W is the number of values in the tag's value array.
        Array<Array<Int4>> tags;

        // Parse the memory buffer and store the tags in the intermediate tags structure.
        {
            // Prepare the indirection structure for tag scopes (a stack).
            Int scopes[4];
            Int scope = 0;
            Int recent = 0;
            Int tag = -1;

            // Prepare the root scope.
            scopes[scope] = 0;
            tags.Expand(1);

            // TODO: Plaintext data is assumed to either be zero-size or end with a newline. Parsing text whose last character is not a newline results in undefined behaviour. Not really worth fixing?
            // TODO: Use exponential reservation (i.e. Reserve(Capacity)) instead of reserving a single entry at a time.
            // TODO: Prepare a root tag so that values may be declared for it.
            // TODO: String-building and Number-building loops read the next character before dropping it. This fine for formatting plaintext but is incorrect when terminating using quotations or braces.

            // Begin parsing the plaintext.
            while(true)
            {
                // Terminate if the end of the buffer has been reached.
                if(buffer.Position == buffer.Size) { break; }

                // Read in the next character.
                Byte character = buffer.Read<Byte>();

                // Comments begin with a hash and end at the next newline.
                if(character == '#')
                {
                    while(true)
                    {
                        character = buffer.Read<Byte>();
                        // Ignore further characters until the next newline.
                        if(character == 10) { break; }
                    }

                    // Skip this character and begin working on the next one.
                    continue;
                }

                // Tokens that begin with a letter are recognized to be tags.
                if(Local::Alphabetical(character))
                {
                    // Prepare a string containing the first character.
                    // TODO: Probably slow to allocate a string with length 1. Consider reserving capacity here.
                    String name(character);
                    // Parse in the rest of the tag's name.
                    while(true)
                    {
                        // Read in the next character.
                        character = buffer.Read<Byte>();

                        // Stop parsing the tag if the next letter isn't a letter or a number.
                        if(!Local::Alphanumeric(character)) { break; }

                        // Append the character to the end of the name.
                        name += character;
                    }

                    // Add the tag to the intermediate structure.
                    tags[scopes[scope]].Reserve(1);
                    tag = tags[scopes[scope]].Append(Hash::FNV32(name.Data, name.Length), -1, Values.Count, 0);
                    // Update the most recently used scope.
                    recent = scopes[scope];
                }

                // Tokens that begin with a number are recognized to be number values.
                if(Local::Numerical(character))
                {
                    // Initialize the number and begin parsing in the rest of its characters.
                    Long number = Long(character) - '0';
                    // -- //
                    while(true)
                    {
                        // Read in the next character.
                        character = buffer.Read<Byte>();

                        // Stop parsing the tag if the next letter isn't a number.
                        if(!Local::Numerical(character)) { break; }

                        // Append the character to the end of the number.
                        number = (number * 10) + (Long(character) - '0');
                    }

                    // Add the value to the values array.
                    Values.Reserve(1);
                    Int index = Values.Append();
                    Values[index].Number = number;

                    // Increase the number of values owned by the tag.
                    tags[recent][tag].w += 1;
                }

                // Tokens that begin with a double quotation mark are recognized as string values.
                if(character == 34) // ASCII code for " is 34
                {
                    String string;
                    // Begin parsing in the rest of the string's contents.
                    while(true)
                    {
                        // Read in the next character.
                        character = buffer.Read<Byte>();

                        // Stop parsing the string at the next double quotation mark.
                        if(character == 34) { break; }

                        // Append the character to the end of the string.
                        string += character;
                    }

                    // Add the value to the values array.
                    Values.Reserve(1);
                    Int index = Values.Append();
                    Values[index].String = Move(string);

                    // Increase the number of values owned by the tag.
                    tags[recent][tag].w += 1;
                }

                // Braces denote the tag hierarchy and are used for traversing tag scopes.
                if(character == '{')
                {
                    // Debug check
                    Assert(scope < 3, "Cannot parse description that contains a tag scope depth greater than three.");
                    // TODO: Limiting tag scope depths to three is arbitrary. Consider using an Array for the scope indirection structure.

                    // Allocate and assign the new scope to the current tag.
                    tags.Reserve(1);
                    // -- //
                    tags[scopes[scope++]][tag].y = tags.Append();

                    // Add the new scope to the stack.
                    scopes[scope] = tags[scopes[scope - 1]][tag].y;

                    // Go to the next character.
                    continue;
                }
                // -- //
                if(character == '}')
                {
                    // Debug check
                    Assert(scope > 0, "Cannot go down a scope from the root scope.");

                    // Go to the previous scope.
                    scope--;
                    // Go to the next character.
                    continue;
                }
            }
        }

        Int count = 0;
        // Count the number of tags to reserve in the TXT structure.
        for(Int i = 0; i < tags.Count; i++) { count += tags[i].Count; }
        // -- //
        Tags.Reserve(count + 1);

        // Reset the count to use it to track tag allocation.
        count = 1;

        // Initialize the root tag.
        {
            // Add the root tag.
            Int index = Tags.Append();

            // If the tag has children..
            if(tags[0].Count > 0)
            {
                // Initialize the children members.
                Tags[index].Children = Tags.Data + count;
                Tags[index].ChildrenCount = tags[0].Count;
                // Increase the amount of allocated tags.
                count += tags[0].Count;
            }
        }

        // Copy the parsed tags to the TXT structure.
        for(Int i = 0; i < tags.Count; i++)
        {
            for(Int j = 0; j < tags[i].Count; j++)
            {
                // Helper
                Int4& tag = tags[i][j];

                // Add the tag.
                Int index = Tags.Append();

                // Assign the tag hash.
                Tags[index].ID = tag.x;
                // Retrieve its scope index.
                Int scope = tag.y;

                // If the tag has a scope assigned to it..
                if(scope > 0)
                {
                    // If the scope contains tags..
                    if(tags[scope].Count > 0)
                    {
                        // Initialize the children members.
                        Tags[index].Children = Tags.Data + count;
                        Tags[index].ChildrenCount = tags[scope].Count;
                        // Increase the amount of allocated tags.
                        count += tags[scope].Count;
                    }
                }

                // If the tag has values..
                if(tag.w > 0)
                {
                    // Initialize the values members.
                    Tags[index].Values = Values.Data + tag.z;
                    Tags[index].ValueCount = tag.w;
                }
            }
        }
    };

    // ----------------------------------------------------------------------------------------
    Void Resource::Loader::TXT::Parse(const String& directory)
    {
        // Helper
        #define hash(string) Hash::FNV32(string)

        // Loop through all of the root-level tags in the TXT structure.
        for(Int i = 0; i < Tags[0].ChildrenCount; i++)
        {
            // Helper
            auto& tag = Tags[0].Children[i];

            // Parse the tag.
            switch(tag.ID)
            {
                // Material resource defintion.
                case hash("material"):
                {
                    // TODO: Write comment explaining how material definitons are parsed.

                    Resource::Material::Description materialDesc;
                    // Prepare a material description.
                    materialDesc.Flags.DepthEnable = false; // Disable depth for now.

                    // Iterate through the tags in the material.
                    for(Int i = 0; i < tag.ChildrenCount; i++)
                    {
                        // Parse the tag.
                        switch(tag.Children[i].ID)
                        {
                            // Vertex shader resource ID.
                            case hash("vshader"):
                            {
                                // Debug check
                                Assert(tag.Children[i].Values[0].String.Data, "Unexpected tag type for the recognized key.");

                                // Assign the vertex shader resource ID.
                                materialDesc.VS = tag.Children[i].Values[0].String;
                                break;
                            }

                            // Pixel shader resource ID.
                            case hash("pshader"):
                            {
                                // Debug check
                                Assert(tag.Children[i].Values[0].String.Data, "Unexpected tag type for the recognized key.");

                                // Assign the pixel shader resource ID.
                                materialDesc.PS = tag.Children[i].Values[0].String;
                                break;
                            }

                            // Compute shader resource ID.
                            case hash("cshader"):
                            {
                                // Debug check
                                Assert(tag.Children[i].Values[0].String.Data, "Unexpected tag type for the recognized key.");

                                // Assign the pixel shader resource ID.
                                materialDesc.CS = tag.Children[i].Values[0].String;
                                break;
                            }
                        }
                    }

                    // Add the material to the resource manager.
                    Resource::Material& material = Resource::Manager::Singleton->Materials.Add(tag.Values[0].String);

                    // Initialize the material using the parsed description.
                    material.Create(materialDesc);

                    break;
                }

                // Shader resource definition.
                case hash("shader"):
                {
                    // Hardcoded shader definition structure.
                    // tag.ID = "shader"
                    // tag.Values[0].String = <resource name>
                    // tag.Children:
                    // [0] tag.ID = "source"
                    // [0] tag.Values[0].String = <filepath>

                    // Debug check
                    Assert(tag.Children[0].ID == hash("source"), "No source tag was found for the resource definition.");

                    File file;
                    // Open the shader data file.
                    file.Open(directory + tag.Children[0].Values[0].String, File::Mode::Read);

                    // Add the shader to the resource manager.
                    Resource::Shader& shader = Resource::Manager::Singleton->Shaders.Add(tag.Values[0].String);

                    // Initialize the shader with bytecode from the data file.
                    shader.Create(nullptr, file.Size);
                    file.Read(shader.Data, file.Size);
                    file.Close();

                    break;
                }
            }
        }
    }
}
