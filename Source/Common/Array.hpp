/*
-------------------------------------------------------------------------------
    Filename: Common/Array.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Memory.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    template <typename Type> class Array
    {
    public:
        // Members

        // Handle to the contiguous array of entries.
        Type* Data;
        // The number of active entries currently in the array.
        Int Count;
        // The maximum number of entries that can be currently contained in the array.
        Int Capacity;

    public:
        // Constructors

        // Default constructor.
        Array() : Data(nullptr), Count(0), Capacity(0) {};
        // Copy constructor.
        Array(const Array<Type>& other) = delete;
        // Move constructor.
        Array(Array<Type>&& other) : Data(other.Data), Count(other.Count), Capacity(other.Capacity) { other.Data = nullptr; other.Count = 0; other.Capacity = 0; };
        // Destructor
        ~Array() { Release(); };

        // Operators

        // Array access operator.
        Type& operator [] (Int index)
        {
            // Debug checks
            Assert(index >= 0, "Tried to access an element with a negative index.");
            Assert(index < Capacity, "Tried to access an element past the end of the array.");

            return Data[index];
        };
        // Const array access operator.
        const Type& operator [] (Int index) const
        {
            // Debug checks
            Assert(index >= 0, "Tried to access an element with a negative index.");
            Assert(index < Capacity, "Tried to access an element past the end of the array.");

            return Data[index];
        };

        // Methods

        // foreach loop begin hook.
        Type* begin() const
        {
            return Data;
        };
        // foreach loop end hook.
        Type* end() const
        {
            return Data + Count;
        };

        // The size of the array's active entries block in bytes.
        Int Size() const
        {
            return sizeof(Type) * Count;
        };

        // Increase the capacity of the array. Does nothing if count is zero. Does not fail silently if count is negative.
        // The new entries are created in an uninitialized state and need to be constructed.
        // Use Expand() if you need to default construct the new entries.
        Void Reserve(Int count)
        {
            // Debug check
            Assert(count >= 0, "Attempting to increase the capacity of the array by a negative amount.");

            // Allocate more data for the array.
            Data = (Type*)(Memory::Resize(Data, sizeof(Type) * (Capacity + count)));

            // Update the capacity.
            Capacity += count;
        };

        // Increase the capacity of the array and construct the new elements.
        // Obeys the same rules as Reserve().
        Void Expand(Int count)
        {
            // Append and construct the new entries.
            Reserve(count);
            // -- //
            for(Int i = 0; i < count; i++)
            {
                new(Data + (Count + i))Type();
            }

            // Update the count
            Count += count;
        };

        // Releases the data allocated by this container. Does not destruct the entries contained in the array.
        Void Release()
        {
            // Deallocate the memory.
            if(Data) { Memory::Free(Data); Data = nullptr; }
            // Reset the members.
            Count = 0;
            Capacity = 0;
        };

        // Shifts a section of the array around in memory, overwriting any overlapping entries.
        Void Shift(Int begin, Int count, Int offset)
        {
            // Debug checks
            Assert((Count + offset) < Capacity, "Array isn't large enough to contain shifted data.");
            Assert((begin + offset) >= 0, "Attempting to move a section past the beginning of the array.");
            Assert(count >= 0, "Attempting to move a negative number of entries.");

            // Only attempt to move entries if there actually any to move and an amount to move them.
            if(count && offset)
            {
                // Move the memory by the offset. The block most likely overlaps with itself so Memory::Copy() would be invalid here.
                Memory::Move(Data + (begin + offset), Data + begin, sizeof(Type) * count);
            }
        };

        // Destruct an entry in the array, shifting everything under it up one location.
        Void Delete(Int slot)
        {
            // Debug checks
            Assert(slot < Capacity, "Tried to remove an entry past the end of the array.");
            Assert(slot >= 0, "Tried to delete an entry with a negative index.");

            // Call the destructor on the object.
            Data[slot++].~Type();

            // Shift the entries under it up one slot and decrement the count.
            Shift(slot, Count - slot, -1);
            Count--;
        };

        // Construct an entry in-place at the end of the array.
        template <typename... Arguments> Int Append(Arguments&&... arguments)
        {
            // Debug check
            Assert(Count < Capacity, "Attempting to add an entry to the array that is full.");

            // Retrieve a slot index at the end of the array.
            Int slot = Count++;

            // Construct the object in-place.
            new(Data + slot)Type(arguments...);

            // Return the slot index.
            return slot;
        };

        // Construct an entry in-place at the specified index. Does not increment the Count.
        template <typename... Arguments> Type& Set(Int slot, Arguments&&... arguments)
        {
            // Debug check
            Assert(slot < Capacity, "Tried to construct an entry past the end of the array.");
            Assert(slot >= 0, "Tried to construct an entry with a negative index.");

            // Construct the object in-place
            new(Data + slot)Type(arguments...);

            // Return the object
            return Data[slot];
        };

        // Insert an entry at the specificed location in the array, moving everything under it down one location and constructing it in-place.
        template <typename... Arguments> Type& Insert(Int slot, Arguments&&... arguments)
        {
            // Debug check
            Assert(slot < Capacity, "Tried to insert an entry past the end of the array.");
            Assert(slot >= 0, "Tried to insert an entry at a negative index.");

            // Shift down everything under the slot
            if(slot < Count) { Shift(slot + 1, Count - slot, 1); }

            // Construct the object in-place
            new(Data + slot)Type(arguments...);

            // Return the object
            return Data[slot];
        };
    };
}
