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

// TODO: Assess the validity of the Set classes as they haven't been fully tested or implemented yet.

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // Useful functions

    // Count Leading Zeroes.
    Int CLZ(uInt value);
    // Population Count.
    Int POPCNT(uInt value);

    // ----------------------------------------------------------------------------------------
    class BitSet
    {
    public:
        // Types

        // Iterator helper object.
        class Iterator
        {
        public:
            // Members

            // The iterator's bitmask.
            uLong Mask;

        public:
            // Constructors

            // Default constructor.
            Iterator() : Mask(0) {};
            // Mask constructor.
            Iterator(uLong mask) : Mask(mask) {};
            // Copy constructor.
            Iterator(const Iterator& other) : Mask(other.Mask) {};
            // Destructor.
            ~Iterator() {};

            // Methods

            // Retrieve the number of active bits.
            Int Count() const;

            // Retrieve the index of the next active bit without setting it to inactive. Return negative one if one isn't found.
            Int Peek() const;
            // Retrieve the index of the next active bit and set it to inactive. Return negative one if one isn't found.
            Int Next();
        };

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
        // Move constructor.
        BitSet(BitSet&& other) : Mask(other.Mask) { other.Mask = 0; };
        // Destructor.
        ~BitSet() {};

        // Methods

        // Retrieve the number of active bits.
        Int Count() const;

        // Retrieve the state of a bit.
        Bool Get(Int index) const;
        // Set a bit to active
        Void Set(Int index);
        // Set a bit to inactive.
        Void Reset(Int index);
        // Reset all of the bits to inactive.
        Void Reset();

        // Retrieve the index of first inactive bit without setting the bit if one is found.
        // Returns negative one if one isn't found.
        Int Query() const;
        // Retrieve the index of the lowest inactive bit for a group of adjacent inactive bits.
        // Returns negative one if one isn't found.
        Int Query(Int count) const;
        // Retrieve the index of the first inactive bit and set it to active. Use Query() if you don't want to set the bit.
        // Returns negative one if one isn't found.
        Int Request();

        // Returns an iterator object for querying the positions of all the active bits.
        Iterator Iterate() const { return Iterator(Mask); };
    };

    // ----------------------------------------------------------------------------------------
    template <typename Key, Int Reciprocal = 3> class Set
    {
    public:
        // Types

        // Container for the bitsets detailing the state of a group of keys in the set.
        struct Bucket
        {
            // Bitset detailing which slots contain valid keys.
            BitSet Active = BitSet();
            // Bitset detailing which slots have been hit with a collision.
            BitSet Collided = BitSet();
        };

        // Helper object for iterating through the active keys in the set.
        class Iterator
        {
        public:
            // Members

            // Handle to the set the iterator references.
            Set const* Handle = nullptr;
            // The current slot of the iterator.
            Int Index = 0;

            // Attempt to compute the index of the next active key in the set. Use Last() to determine if the index is valid or not.
            Int Next()
            {
                // There isn't a next key if the end of the set has already been reached.
                if(Last()) { return Index; }

                // Attempt to query the next key in the current bucket.
                Int index = BitSet((~Handle->Buckets[Index >> 6].Active.Mask) | ((2ULL << (Index & 63)) - 1)).Query();

                // If the current bucket doesn't contain anymore keys..
                if(index < 0)
                {
                    // Attempt to locate the next bucket that contains keys.
                    Int bucket = BitSet(~(Handle->Index.Mask) | ((2ULL << (Index >> 6)) - 1)).Query();

                    // If there aren't anymore buckets with active keys..
                    if(bucket < 0)
                    {
                        // Set the index to the end of the set.
                        Index = Handle->Capacity;
                    }
                    else
                    {
                        // Compute the index of the first active key in the bucket.
                        Index = (bucket * 64) + BitSet(~Handle->Buckets[bucket].Active.Mask).Query();
                    }
                }
                else
                {
                    // Offset the current index to the next key.
                    Index += index - (Index & 63);
                }
                // -- //
                return Index;
            };
            // Check if the iterator has reached past-the-end of the set. No longer contains an index to a valid key if true is returned.
            Bool Last()
            {
                // We've reached the end of the set once the iterator index is equal to the set's capacity.
                return Index == Handle->Capacity;
            };
        };

    public:
        // Members

        // Array of keys in the set. Also serves has the handle to the memory used by all the buffers in the set.
        Key* Data;
        // Array of buckets describing groups of keys in the set.
        Bucket* Buckets;
        // Index denoting which buckets contain active keys.
        BitSet Index;
        // Number of keys contained in the set.
        Int Count;
        // Maximum number of keys the set can contain.
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

        // Increase the capacity of the set.
        Void Reserve(Int count);
        // Increase the capacity of the set and reset the new keys.
        Void Expand(Int count)
        {
            // Debug check
            Assert(count >= 0, "Tried to expand the capacity of the set by a negative amount.");

            // Prepare another set to contain the previous data (resetting the current set).
            Set old(R2D::Move(*this));

            // Compute the total number of keys the set will contain.
            Capacity = old.Capacity + count;
            // Debug check
            Assert(POPCNT(Capacity) == 1, "Cannot create sets whose capacities aren't a power of 2.");

            Int size[2];
            // Compute the size of the new buffer as a combination of the two data arrays.
            size[0] = sizeof(Key) * Capacity;
            size[1] = sizeof(Bucket) * (((Capacity - 1) / 64) + 1);
            // Allocate one bucket for every 64 keys.

            // Request the new memory and assign the pointers.
            Byte* memory = (Byte*)Memory::Resize(Data, size[0] + size[1]);
            // -- //
            Data = (Key*)(memory);
            Buckets = (Bucket*)(memory + size[0]);

            // Construct all of the Key objects.
            for(Int i = 0; i < Capacity; i++)
            {
                // Call the default constructor.
                new(Data + i)Key();
            }
            // Construct all of the Bucket objects.
            for(Int i = 0; i < (((Capacity - 1) / 64) + 1); i++)
            {
                // Call the default constructor.
                new(Buckets + i)Bucket();
            }

            // Iterate through the old set and add all of its keys to this new set.
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
            // Construct the iterator and set its index to the first key in the set if it contains any.
            iterator.Handle = this;
            // -- //
            if(Count)
            {
                // Locate the first key's index and start the iterator there.
                Int bucket = BitSet(~Index.Mask).Query();
                iterator.Index = (bucket * 64) + BitSet(~Buckets[bucket].Active.Mask).Query();
                return iterator;
            }
            else
            {
                // Set the iterator to past-the-end of the set, denoting no more keys.
                iterator.Index = Capacity;
                return iterator;
            }
        };
        // Retrieve a forward iterator that already contains an index to the past-the-end value of the set.
        Iterator Last() const
        {
            Iterator iterator;
            // Simply set the iterator to the past-the-end index of the set.
            iterator.Handle = this;
            iterator.Index = Capacity;
        };

        // Construct a new key in-place with the specified key.
        Int Add(const Key& key)
        {
            // Allocate memory for the set if none has been allocated yet.
            if(!Data)
            {
                // Initialize with a default capacity of 64 keys.
                Expand(64);
            }

            // Check if the set's free capacity is less than the reciprocal.
            if((Capacity / (Capacity - Count)) > Reciprocal)
            {
                // Double the capacity of the set and rehash.
                //Expand(Capacity);
            }

            // Mask the hashed key to compute the slot index and bucket index.
            Int slot = Hash::FNV32(key) & (Capacity - 1);
            Int bucket = slot >> 6;

            // Linear probing only needs to be done if the bucket isn't empty.
            if(Index.Get(bucket))
            {
                // If the current slot already contains an active key..
                while(Buckets[bucket].Active.Get(slot & 63))
                {
                    // Debug check
                    Assert(key != Data[slot], "Cannot add the key. A key with that value already exists.");

                    // Collide the key.
                    Buckets[bucket].Collided.Set(slot & 63);

                    // Move to the next slot and update the indexes;
                    bucket = (++slot & (Capacity - 1)) >> 6;
                }
            }

            // Increment the number of keys in the set.
            Count++;

            // Flag the bucket as containing keys.
            Index.Set(bucket);
            // Flag the key in the bucket as active.
            Buckets[bucket].Active.Set(slot & 63);

            // Copy the key and return its index.
            Data[slot] = key;
            return slot;
        };

        // Remove a key from the set.
        Void Delete(const Key& key);

        // Lookup a key and return its index if one is found. Returns -1 if one wasn't found.
        Int Find(const Key& key) const
        {
            // Return if the set isn't allocated yet.
            if(!Data) { return -1; }

            // Mask the hashed key to compute the slot index and bucket index.
            Int slot = Hash::FNV32(key) & (Capacity - 1);
            Int bucket = slot >> 6;

            // Loop through (potentially) every key in the set.
            for(Int i = 0; i < Capacity; i++)
            {
                // If the slot contains an active key..
                if(Buckets[bucket].Active.Get(slot & 63))
                {
                    // If the key matches..
                    if(key == Data[slot])
                    {
                        // A matching key was found.
                        return slot;
                    }
                }

                // If the slot isn't collided..
                if(!Buckets[bucket].Collided.Get(slot & 63))
                {
                    // An empty slot was reached without finding a matching key.
                    break;
                }

                // Move to the next key.
                bucket = (++slot & (Capacity - 1)) >> 6;
            }

            // A matching key wasn't found.
            return -1;
        };

        // Update the key return the index of its new location.
        Int Move(const Key& oldKey, const Key& newKey);
    };
}
