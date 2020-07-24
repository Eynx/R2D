/*
-------------------------------------------------------------------------------
    Filename: Common/String.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Memory.hpp"
// TODO: Move the Memory namespace calls to a source file, maybe?

// TODO: Refactor the String class as I think it looks messy. Also rename Reserve to Resize (and add another function that actually Reserves capacity instead of resizing it).

// --------------------------------------------------------------------------------------------
namespace R2D
{
    class String
    {
    public:
        // The memory allocated for the string. Contains the characters and the null-byte at the end of the string.
        Byte* Data;
        // The number of characters in the string (not including the null-byte). Should always be less than Capacity, never equal to it, due to the null-byte at the end.
        Int Length;
        // The amount of memory allocated for the string. Includes the extra byte at the end of the buffer that is the null-byte.
        Int Capacity;

    public:
        // Constructors

        // Default constructor.
        constexpr String() : Data(nullptr), Length(0), Capacity(0) {};

        // Single character constructor.
        explicit String(char character) : Data(nullptr), Length(1), Capacity(0) { Reserve(1); Data[0] = character; Data[1] = 0; };

        // C-string constructor. Copies the string directly, including the null-byte.
        template <size_t size> String(const char(&string)[size]) : Data(nullptr), Length(size - 1), Capacity(0)
        {
            // Copy the contents of the other string without regard for parameter safety.
            Reserve(Length);
            Memory::Copy(Data, string, size);
        };

        // C-string pointer constructor. Length does not include the null-byte at the end of the string, only the number of characters.
        String(const char* string, Int length) : Data(nullptr), Length(length), Capacity(0)
        {
            // Only allocate the string if there's actual characters to copy over.
            if(length > 0)
            {
                // Debug check
                Assert(string, "Attempting to copy data from an invalid memory location.");

                // Copy over the contents from the other string.
                Reserve(length);
                Memory::Copy(Data, string, length);
                // Explicitly initialize the null-byte.
                Data[length] = 0;
            }
        };

        // Copy constructor.
        String(const String& other) : Data(nullptr), Length(other.Length), Capacity(0)
        {
            // Only attempt to copy data from the other string if it even has data.
            if(other.Data)
            {
                // Ensure that the string is large enough to contain the other string's contents.
                Reserve(Length);
                // Copy over the other string's contents (including its implicit null-byte).
                Memory::Copy(Data, other.Data, Length + 1);
            }
        };

        // Move constructor.
        String(String&& other) : Data(other.Data), Length(other.Length), Capacity(other.Capacity) { other.Data = nullptr; other.Length = 0; other.Capacity = 0; };

        ~String() { Release(); };

        // Operators

        // Array access operator.
        Byte& operator [] (Int index)
        {
            // Debug checks
            Assert(Data, "Tried to access characters in an empty string.");
            Assert(index >= 0, "Tried to access characters with a negative index.");
            Assert(index < Length, "Tried to access characters past the end of the string.");

            return Data[index];
        };
        // Const array access operator.
        const Byte& operator [] (Int index) const
        {
            // Debug checks
            Assert(Data, "Tried to access characters in an empty string.");
            Assert(index >= 0, "Tried to access characters with a negative index.");
            Assert(index < Length, "Tried to access characters past the end of the string.");

            return Data[index];
        };

        // Copy assignment operator.
        String& operator = (const String& other)
        {
            // Release the existing string
            if(Data) { Memory::Free(Data); }

            // Copy the other string
            new(this)String(other);

            return *this;
        };

        // Move assignment operator.
        String& operator = (String&& other)
        {
            // Release the existing string
            if(Data) { Memory::Free(Data); }

            // Assimilate the other string
            new(this)String(other);

            return *this;
        };

        // String concatenation assignment operator.
        String& operator += (const String& other)
        {
            // If the other string has data
            if(other.Data)
            {
                // Debug check
                Assert(other.Length >= 0, "Tried to concatenate a string with a negative length to this string.");

                // Expand the string
                Reserve(Length + other.Length);

                // Append the other string
                Memory::Copy(Data + Length, other.Data, other.Length);
                Length += other.Length;

                // Update the null-byte
                Data[Length] = 0;
            }

            return *this;
        };

        // String concatenation assignment operator.
        template <size_t size> String& operator += (const char(&string)[size])
        {
            // If the other string has data (other than the null-byte).
            if(size > 1)
            {
                // Expand the string (while removing the null-byte).
                Reserve(Length + (size - 1));

                // Append the other string (including the null-byte).
                Memory::Copy(Data + Length, string, size);
                Length += size - 1;
            }

            return *this;
        };

        // Character concatenation assignment operator.
        String& operator += (char character)
        {
            // Expand the string.
            Reserve(Length + 1);

            // Append the character.
            Data[Length] = character;

            // Increase the length of the string.
            Length += 1;

            // Update the null-byte.
            Data[Length] = 0;

            return *this;
        };

        // String concatenation operator.
        String operator + (const String& other) const
        {
            // If this string has data (concatenation target)..
            if(Data)
            {
                // If the other string has data for concatenation..
                if(other.Data)
                {
                    String string;
                    // Allocate a string for containing both strings.
                    string.Reserve(Length + other.Length);

                    // Append the strings.
                    Memory::Copy(string.Data, Data, Length);
                    Memory::Copy(string.Data + Length, other.Data, other.Length);

                    //  Set the length of the string.
                    string.Length = Length + other.Length;

                    // Set the null-byte.
                    string.Data[string.Length] = 0;

                    return string;
                }
                else
                {
                    // Just return a copy of this string since the other string doesn't have any data.
                    return String(Data, Length);
                }
            }
            else
            {
                // If the other string has data..
                if(other.Data)
                {
                    // Just return a copy of the other string since this string doesn't have any data.
                    return String(other.Data, other.Length);
                }
                else
                {
                    // Return an empty string since neither strings contain data.
                    return String();
                }
            }
        };

        // Methods

        // The size of the string in bytes (including the null-delimiter).
        Int Size() const
        {
            // Number of character plus the null-delimiter if the string is allocated
            return Data ? Length + 1 : 0;
        };

        // Release the contents and memory allocated by the string.
        Void Release()
        {
            if(Data) { Memory::Free(Data); Data = nullptr; }
            Length = 0;
            Capacity = 0;
        };

        // Reserve capacity for the string. Null-byte is implicit and shouldn't be reserved.
        // Does nothing if size is zero. Does nothing if size is equal to or less than Capacity.
        Void Reserve(Int size)
        {
            // Debug check
            Assert(size >= 0, "Attempting to reserve an invalid number of characters.");

            // Only reallocate if size is greater than zero and less than the string's current capacity.
            if((size > 0) && (size > Capacity))
            {
                // Reallocate the string.
                Data = (Byte*)Memory::Resize(Data, size + 1);

                // Set the new capacity.
                Capacity = size;
            }
        };

        // Iterate through the string to locate a character. Returns Length if a match isn't found.
        // An optional offset can be applied to specify the character index to start the search from.
        Int Find(Byte character, Int offset = 0) const
        {
            // Debug check
            Assert(offset >= 0, "Cannot begin string search using a negative offset.");

            // Loop through the characters, starting at offset.
            for(; offset < Length; offset++)
            {
                // Return the index if the characters match.
                if(Data[offset] == character) { return offset; }
            }

            // Return Length to signify a match wasn't found (offset should be equal to Length here).
            return offset;
        };

        // Iterate through the string in reverse to locate a character. Returns Length if a match isn't found.
        // The offset is expected to be a character index, rather than a relative offset from the end of the string.
        // For example, specifying an offset of 2 means starting the search from Data[2], moving backwards from there.
        Int rFind(Byte character, Int offset) const
        {
            // Loop through the characters in reverse, starting at offset.
            for(; offset >= 0; offset--)
            {
                // Return the index if the characters match.
                if(Data[offset] == character) { return offset; }
            }

            // Return Length to signify a match wasn't found (offset should be equal to Length here).
            return offset;
        };

        // Iterate through the string in reverse to locate a character. Returns Length if a match isn't found.
        Int rFind(Byte character) const
        {
            // Simply a helper for calling the offset-based rFind but starting at the end of the string.
            return rFind(character, Length - 1);
        }
    };
}
