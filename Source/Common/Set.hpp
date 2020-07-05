/*
-------------------------------------------------------------------------------
    Filename: Common/Set.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Hash.hpp"
#include "..\Common\Memory.hpp"

// TODO: Set can only contain a maximum of 4,096 entries due to having only a single BitSet for the bucket index hardcoded. Considering using a doubly-linked list instead.
// TODO: Probing doesn't take advantage of the internal acceleration structures. Use them maybe, somehow?

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    class BitSet
    {
    public:
        // Members

        // The set's bitmask.
        uLong Mask;

    public:
        // Constructors

        // Default constructor.
        BitSet() : Mask(0) {};
        // Mask constructor. Initialize the bitmask directly.
        BitSet(uLong mask) : Mask(mask) {};
        // Copy constructor.
        BitSet(const BitSet& other) : Mask(other.Mask) {};

        // Methods

        // Retrieve the number of active bits.
        Int Count() const;
        // Retrieve the state of a bit.
        Bool Get(Int index) const;

        // Set the state of a bit.
        Void Set(Int index, Bool active);
        // Set a bit to active
        Void Set(Int index);
        // Set a bit to inactive.
        Void Reset(Int index);
        // Reset all of the bits to inactive.
        Void Reset();

        // Retrieve the index of first inactive bit.
        Int Query() const;
        // Retrieve the index of the first active bit.
        Int Peek() const;

        // Retrieve the index of the first inactive bit starting from the specified bit.
        Int Query(Int index) const;
        // Retrieve the index of the first active bit starting from the specified bit.
        Int Peek(Int index) const;

        // Retrieve the index of the first inactive bit and set it to active.
        Int Request();
        // Retrieve the index of the first active bit and set it inactive.
        Int Pop();
    };

    // ----------------------------------------------------------------------------------------
    template <typename Key> class Set
    {
    public:
        // Types

        // Container for bitsets detailing the state of a group of keys.
        struct Bucket
        {
            // Bitset detailing which slots contain valid keys.
            BitSet Active = BitSet();
            // Bitset detailing which slots have been hit with a collision.
            BitSet Collided = BitSet();
        };

        // Helper object for iterating through the active keys in a set.
        class Iterator
        {
        public:
            // Members

            // Handle to the set being iterated.
            const Set* Handle = nullptr;
            // The current slot of the iterator.
            Int Index = -1;

            // Attempt to compute the index of the next active key in the set. Use Last() to determine if the index is valid or not.
            Int Next()
            {
                // There isn't a next key if the end of the set has already been reached.
                if(Last()) { return -1; }

                // Prepare some indexing values.
                Int bucket = Index >> 6;
                Int slot = Index & 63;

                // Attempt to locate the next active key in the current bucket.
                Int index = Handle->Buckets[bucket].Active.Peek(slot);

                // If a key was found in the current bucket...
                if(index >= 0)
                {
                    // Offset the current index to the next key.
                    Index += index - slot;
                }
                else
                {
                    // Attempt to locate the next bucket that contains keys.
                    bucket = Handle->Index.Mask.Peek(slot);

                    // If there aren't anymore buckets with active keys..
                    if(bucket < 0)
                    {
                        // Invalidate the index.
                        Index = -1;
                    }
                    else
                    {
                        // Compute the index of the first active key in the bucket.
                        Index = (bucket * 64) + Handle->Buckets[bucket].Active.Peek();
                    }
                }
                // -- //
                return Index;
            };
            // Check if the iterator is done parsing the set.
            Bool Last()
            {
                // The end of the set has been reached once the iterator's index is equal to -1.
                return Index == -1;
            };
        };

    public:
        // Members

        // Array of keys in the set. Also serves has the handle to the memory used by all the buffers in the set.
        Key* Data;
        // Array of buckets describing groups of keys in the set.
        Bucket* Buckets;
        // Index denoting which buckets contain active keys. Being hardcoded to a single BitSet like this limits this to only 4,096 entries.
        BitSet Index;
        // The number of active keys in the set.
        Int Count;
        // The maximum number of keys the set can contain.
        Int Capacity;

    public:
        // Constructors

        // Default constructor.
        Set() : Data(nullptr), Buckets(nullptr), Index(), Count(0), Capacity(0) {};
        // Copy constructor.
        Set(const Set& other) = delete;
        // Move constructor.
        Set(Set&& other) : Data(other.Data), Buckets(other.Buckets), Index(R2D::Move(other.Index)), Count(other.Count), Capacity(other.Capacity) { other.Data = nullptr; other.Buckets = nullptr; other.Count = 0; other.Capacity = 0; };
        // Destructor.
        ~Set() { Release(); };

        // Methods

        // Increase the capacity of the set. The final capacity of the set must be a power of two.
        // Does nothing if count is zero. Does not fail silently if count is negative.
        Void Reserve(Int count)
        {
            // Debug check
            Assert(count >= 0, "Tried to expand the capacity of the set by a negative amount.");

            // Move the current set into an old one to reset this one and use it to contain the expanded data.
            Set old(R2D::Move(*this));

            // Compute the new capacity of the set.
            Capacity = old.Capacity + count;
            // Debug check
            Assert(POPCNT(Capacity) == 1, "Cannot create sets whose capacities aren't a power of two.");

            // Allocate one bucket for every 64 keys.
            Int buckets = ((Capacity - 1) / 64) + 1;

            Int size[2];
            // Compute the memory footprints of the key buffer and bucket buffer.
            size[0] = sizeof(Key) * Capacity;
            size[1] = sizeof(Bucket) * buckets;

            // Request the new memory and assign the pointers.
            Byte* memory = (Byte*)Memory::Resize(Data, size[0] + size[1]);
            // -- //
            Data = (Key*)(memory);
            Buckets = (Bucket*)(memory + size[0]);

            // Iterate through the old set and add all of its active keys to this new set.
            for(auto iterator = old.First(); !iterator.Last(); iterator.Next())
            {
                // Add the old key to the new set.
                Add(old.Data[iterator.Index]);
            }

            // Release the old set.
            old.Release();
        };
        // Increase the capacity of the set and default construct the new keys.
        // Obeys the same rules as Reserve().
        Void Expand(Int count)
        {
            // Debug check
            Assert(count >= 0, "Tried to expand the capacity of the set by a negative amount.");

            // Move the current set into an old one to reset this one and use it to contain the expanded data.
            Set old(R2D::Move(*this));

            // Compute the new capacity of the set.
            Capacity = old.Capacity + count;
            // Debug check
            Assert(POPCNT(Capacity) == 1, "Cannot create sets whose capacities aren't a power of two.");

            // Allocate one bucket for every 64 keys.
            Int buckets = ((Capacity - 1) / 64) + 1;

            Int size[2];
            // Compute the memory footprints of the key buffer and bucket buffer.
            size[0] = sizeof(Key) * Capacity;
            size[1] = sizeof(Bucket) * buckets;

            // Request the new memory and assign the pointers.
            Byte* memory = (Byte*)Memory::Resize(Data, size[0] + size[1]);
            // -- //
            Data = (Key*)(memory);
            Buckets = (Bucket*)(memory + size[0]);

            // Default construct all of the Key objects.
            for(Int i = 0; i < Capacity; i++)
            {
                // Call the default constructor.
                new(Data + i)Key();
            }
            // Default construct all of the Bucket objects.
            for(Int i = 0; i < buckets; i++)
            {
                // Call the default constructor.
                new(Buckets + i)Bucket();
            }

            // Iterate through the old set and add all of its active keys to this new set.
            for(auto iterator = old.First(); !iterator.Last(); iterator.Next())
            {
                // Add the old key to the new set.
                Add(old.Data[iterator.Index]);
            }

            // Release the old set.
            old.Release();
        };
        // Release the data allocated by the set and reset it back to its defualt state.
        Void Release()
        {
            // Free the memory allocated by the set.
            if(Data) { Memory::Free(Data); Data = nullptr; }
            // -- //
            Buckets = nullptr;
            Index.Reset();
            Count = 0;
            Capacity = 0;
        };

        // Retrieve a forward iterator that already contains an index to the first active key in the set (if the set has keys).
        Iterator First() const
        {
            Iterator iterator;
            // Construct the iterator.
            iterator.Handle = this;

            // If the set even contains any keys...
            if(Count)
            {
                // Locate the first key's index and start the iterator there.
                Int bucket = Index.Peek();
                iterator.Index = (bucket * 64) + Buckets[bucket].Active.Peek();
            }
            else
            {
                // Invalidate iterator's index since there's nothing in the set.
                iterator.Index = -1;
            }
            // -- //
            return iterator;
        };

        // Construct a new key in-place with the specified key.
        Int Add(const Key& key)
        {
            // Debug checks
            Assert(Data, "Attempting to insert a key into an unallocated set.");
            Assert(Count < Capacity, "Set is full and cannot contain anymore keys.");
                
            // Mask the hashed key to compute the bucket and slot index.
            Int slot = Hash::FNV32(key) & (Capacity - 1);
            Int bucket = slot >> 6;

            // Linear probing only needs to be done if the bucket isn't empty.
            if(Index.Get(bucket))
            {
                // Linearly probe the set if the current slot already contains an active key.
                while(Buckets[bucket].Active.Get(slot & 63))
                {
                    // Debug check
                    Assert(key != Data[slot], "Cannot add the key. A key with that value already exists.");

                    // Collide the key.
                    Buckets[bucket].Collided.Set(slot & 63);

                    // Move to the next slot and update the bucket index;
                    bucket = (++slot & (Capacity - 1)) >> 6;
                }
            }

            // Increment the number of keys in the set.
            Count++;

            // Flag the bucket as containing keys.
            Index.Set(bucket);
            // Flag the key in the bucket as active.
            Buckets[bucket].Active.Set(slot & 63);

            // Copy the key and return the index.
            Data[slot] = key;
            return slot;
        };

        // Lookup a key and return its index if one is found. Returns -1 if one wasn't found.
        Int Find(const Key& key) const
        {
            // Debug check
            Warning(Data, "Caught a lookup attempt in an unallocated set.");

            // Return an invalid index if the set is unallocated.
            if(!Data) { return -1; }

            // Mask the hashed key to compute the slot index and bucket index.
            Int slot = Hash::FNV32(key) & (Capacity - 1);
            Int bucket = slot >> 6;

            // Loop through (potentially) every key in the set to locate a matching key.
            for(Int i = 0; i < Capacity; i++)
            {
                // If the slot contains an active key...
                if(Buckets[bucket].Active.Get(slot & 63))
                {
                    // If the key matches...
                    if(key == Data[slot])
                    {
                        // A matching key was found.
                        return slot;
                    }
                }

                // If the slot isn't collided...
                if(!Buckets[bucket].Collided.Get(slot & 63))
                {
                    // An empty slot was reached without finding a matching key.
                    break;
                }

                // Move to the next key and update the bucket index.
                bucket = (++slot & (Capacity - 1)) >> 6;
            }

            // A matching key wasn't found.
            return -1;
        };

        // Remove a key from the set and return its index if one was removed. Returns -1 if nothing happened.
        Int Delete(const Key& key)
        {
            // Search for the key.
            Int index = Find(key);

            // If a key was found...
            if(index >= 0)
            {
                Int bucket = index >> 6;
                // Deactivate the key.
                Buckets[bucket].Active.Reset(index & 63);
                // Deactivate the bucket if it's now empty.
                if(Buckets[bucket].Active.Mask == 0) { Index.Reset(bucket); }

                // Reconstruct the key.
                Data[index] = Key();
                // Decrement the number of keys in the set.
                Count--;
            }

            // Return the index.
            return index;
        };

        // Update an existing key and return the index of its new location. Returns -1 if no existing key was found.
        Int Move(const Key& oldKey, const Key& newKey)
        {
            // Attempt to remove the old key if there is one.
            Int index = Delete(oldKey);

            // If the old key was removed...
            if(index >= 0)
            {
                // Add the new key and return its index.
                index =  Add(newKey);
            }
            // -- //
            return index;
        };
    };
}
