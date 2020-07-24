/*
-------------------------------------------------------------------------------
    Filename: Resource/Loader.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Array.hpp"
#include "..\Common\Memory\Buffer.hpp"
#include "..\Common\String.hpp"
// -- //
#include "..\Resource.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Resource
    {
        // ------------------------------------------------------------------------------------
        namespace Loader
        {
            // --------------------------------------------------------------------------------
            class TXT
            {
            public:
                // Types

                // A tag value, containing either a String or a Number.
                struct Value
                {
                    // A string value. Only valid if Data is actually allocated, otherwise the Value is actually a Number.
                    String String;
                    // A number value. This is only valid if the Data member is nullptr.
                    Long Number = 0;
                };
                // A tag, containing an ID and zero or more values.
                struct Tag
                {
                    // Pointers to children tags whom the tag is their parent tag.
                    Tag* Children = nullptr;
                    // Pointer to the tag values the tag contains.
                    Value* Values = nullptr;
                    // The hash of the tag's name.
                    Int ID = 0;
                    // The number of child tags under the tag.
                    Int ChildrenCount = 0;
                    // The number of vtag values the tag contains.
                    Int ValueCount = 0;
                };

            public:
                // Members

                // Array of the tags contained in the description object.
                // The first tag, tag index 0, contains the tags at root level as its children.
                Array<Tag> Tags;
                // Array of the values contained by the tags in the description object.
                Array<Value> Values;

            public:
                // Constructors

                // Default constructor.
                TXT() : Tags(), Values() {};
                // Copy constructor.
                TXT(const TXT& other) = delete;
                // Move constructor.
                TXT(TXT&& other) : Tags(Move(other.Tags)), Values(Move(other.Values)) {};
                // Destructor.
                ~TXT() { Release(); };

                // Methods

                // Parse the resource definition descriptions and store the tag structure.
                Void Load(Memory::Buffer& buffer);
                // Parse the loaded tag structure and add the described resources to the resource graph.
                // The directory is used for resolving local pathnames located in the descriptions.
                Void Parse(const String& directory);
                // Release the memory allocated by the resource definitions and reset the object.
                Void Release();
            };
        };
    }
}
