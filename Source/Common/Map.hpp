/*
-------------------------------------------------------------------------------
    Filename: Common/Map.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Hash.hpp"
#include "..\Common\Memory.hpp"
#include "..\Common\Set.hpp"

// TODO: Refactor Map as it has been lazily imported without change.

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    template <typename Key, typename Value> class Map
    {
    public:
        // Types

        // Container for the bitsets detailing the state of a group of entries in the map.
        struct Bucket
        {
            // Bitmask denoting which slots contain valid entries.
            BitSet Active = BitSet();
            // Bitmask denoting which slots have been hit with a collision.
            BitSet Collided = BitSet();
        };

        // Helper object for iterating through the active entries in a map.
        class Iterator
        {
        public:
            // Members

            // Handle to the map the iterator references.
            Map const* Handle = nullptr;
            // The current slot of the iterator.
            Int Index = 0;

            // Attempt to compute the index of the next active entry in the map. Use Last() to determine if the index is valid or not.
            Void Next()
            {
                // Do nothing if the end of the map has been reached.
                if(Last()) { return; }

                // Attempt to query the next entry in the current bucket.
                Int index = BitSet((~Handle->Buckets[Index >> 6].Active.Mask) | ((2ULL << (Index & 63)) - 1)).Query();

                // If the current bucket doesn't contain anymore entries..
                if(index < 0)
                {
                    // Attempt to locate the next bucket that contains entries.
                    Int bucket = BitSet(~(Handle->Index.Mask) | ((2ULL << (Index >> 6)) - 1)).Query();

                    // If there aren't anymore buckets with active entries..
                    if(bucket < 0)
                    {
                        // Set the index to the end of the map.
                        Index = Handle->Capacity;
                    }
                    else
                    {
                        // Compute the index of the first active entry in the bucket.
                        Index = (bucket * 64) + BitSet(~Handle->Buckets[bucket].Active.Mask).Query();
                    }
                }
                else
                {
                    // Offset the current index to the next entry.
                    Index += index - (Index & 63);
                }
            };
            // Check if the iterator has reached past-the-end of the map. No longer contains an index to a valid entry if true is returned.
            Bool Last()
            {
                // The iterator has reached the end of the map once the iterator's index is equal to the map's capacity.
                return Index == Handle->Capacity;
            };
        };

    public:
        // Members

        // Array of entries in the map. Also serves has the handle to the memory used by all the buffers in the map.
        Value* Data;
        // Array of keys corresponding to the entries in the map.
        Key* Keys;
        // Array of buckets describing groups of entries in the map.
        Bucket* Buckets;
        // Index denoting which buckets contain active entries.
        BitSet Index;
        // Number of entries contained in the map.
        Int Count;
        // Maximum number of entries the map can contain.
        Int Capacity;

    public:
        // Constructors

        // Default constructor.
        Map() : Data(nullptr), Keys(nullptr), Buckets(nullptr), Index(), Count(0), Capacity(0) {};
        // Copy constructor.
        Map(const Map& other) = delete;
        // Move constructor.
        Map(Map&& other) : Data(other.Data), Keys(other.Keys), Buckets(other.Buckets), Index(R2D::Move(other.Index)), Count(other.Count), Capacity(other.Capacity) { other.Data = nullptr; other.Keys = nullptr; other.Buckets = nullptr; other.Count = 0; other.Capacity = 0; };
        // Destructor.
        ~Map() { Release(); };

        // Methods

        // Increase the capacity of the map.
        Void Reserve(Int count);
        // Increase the capacity of the map and reset the new keys.
        Void Expand(Int count)
        {
            // Debug check
            Assert(count >= 0, "Tried to reserve a negative amount of additional entries for the map.");

            // Prepare another map to contain the previous data and reset the current map.
            Map old(R2D::Move(*this));

            // Precompute the total number of entries the map will contain.
            Capacity = old.Capacity + count;
            // Debug check
            Assert(POPCNT(Capacity) == 1, "Cannot create maps whose capacities aren't a power of 2.");

            Int size[3];
            // Compute the size of the new buffer as a combination of the three data arrays.
            size[0] = sizeof(Value) * Capacity;
            size[1] = sizeof(Key) * Capacity;
            size[2] = sizeof(Bucket) * (((Capacity - 1) / 64) + 1);
            // Allocate one bucket for every 64 entries.

            // Request the new memory and assign the pointers.
            Byte* memory = (Byte*)Memory::Resize(Data, size[0] + size[1] + size[2]);
            // -- //
            Data = (Value*)(memory);
            Keys = (Key*)(memory + size[0]);
            Buckets = (Bucket*)(memory + size[0] + size[1]);

            // Construct all of the Value objects.
            for(Int i = 0; i < Capacity; i++)
            {
                // Call the default constructor.
                new(Data + i)Value();
            }
            // Construct all of the Key objects.
            for(Int i = 0; i < Capacity; i++)
            {
                // Call the default constructor.
                new(Keys + i)Key();
            }
            // Construct all of the Bucket objects.
            for(Int i = 0; i < (((Capacity - 1) / 64) + 1); i++)
            {
                // Call the default constructor.
                new(Buckets + i)Bucket();
            }

            // Iterate through the old map and add all of its entries to this new map.
            for(auto iterator = old.First(); !iterator.Last(); iterator.Next())
            {
                // Add the old entry to the new map.
                //Add(old.Keys[iterator.Index], R2D::Move(old.Data[iterator.Index]));
                auto& data = Add(old.Keys[iterator.Index]);
                //data = R2D::Move(old.Data[iterator.Index]);
                new(&data)Value(R2D::Move(old.Data[iterator.Index]));
            }

            // Release the old map.
            old.Release();
        };
        // Release the data allocated by the map.
        Void Release()
        {
            // Free the memory allocated by the map.
            if(Data) { Memory::Free(Data); Data = nullptr; }
            // -- //
            Keys = nullptr;
            Buckets = nullptr;
            Count = 0;
            Capacity = 0;
        };

        // Retrieve a forward iterator that already contains an index to the first active entry in the map (if the map has entries).
        Iterator First() const
        {
            Iterator iterator;
            // Construct the iterator and set its index to the first entry in the map if it contains any.
            iterator.Handle = this;
            // -- //
            if(Count)
            {
                // Locate the first entry's index and start the iterator there.
                Int bucket = BitSet(~Index.Mask).Query();
                iterator.Index = (bucket * 64) + BitSet(~Buckets[bucket].Active.Mask).Query();
                return iterator;
            }
            else
            {
                // Set the iterator to past the end of the map, denoting no more entries.
                iterator.Index = Capacity;
                return iterator;
            }
        };
        // Retrieve a forward iterator that already contains an index to the past-the-end value of the map.
        Iterator Last() const
        {
            Iterator iterator;
            // Simply set the iterator to the past-the-end point of the map.
            iterator.Handle = this;
            iterator.Index = Capacity;
        };

        // Compute the health of the map, as in the percentage of entries that aren't collided.
        Float Health() const;
        // Heal the map, potentially reducing the number of collided entries by cleaning up collided slots that lead to nowhere and placing collided entries closer to their true (uncollided) slot.
        // A badly damaged map with low health will suffer increased lookup times on badly collided entries and usually suffers from clumping due to the linear addressing.
        // Healing is most effective when going from a mostly full map to a relatively empty one, when collided entries can reclaim their true slot or at least be positioned closer to it.
        // There isn't a benefit to healing a map multiple times in succession without removing entries.
        Void Heal();

        // Construct a new entry in-place with the specified key.
        template <typename... Arguments> Value& Add(const Key& key, Arguments&&... arguments)
        {
            // Allocate memory for the map if none has been allocated yet.
            if(!Data)
            {
                // Initialize a default capacity of 64 entries.
                Expand(64);
            }

            // Check if the map's free capacity is less than the reciprocal.
            //if((Capacity / (Capacity - Count)) > Reciprocal)
            //{
                // Double the capacity of the map and rehash.
                //Expand(Capacity);
            //}

            // Mask the hashed key to compute the slot index and bucket index.
            Int slot = Hash::FNV32(key) & (Capacity - 1);
            Int bucket = slot >> 6;

            // Linear probing only needs to be done if the bucket isn't empty.
            if(Index.Get(bucket))
            {
                // If the current slot already contains an active entry..
                while(Buckets[bucket].Active.Get(slot & 63))
                {
                    // Debug check
                    Assert(key != Keys[slot], "Cannot add the entry. An entry with that key already exists.");

                    // Collide the entry.
                    Buckets[bucket].Collided.Set(slot & 63);

                    // Move to the next slot and update the indexes;
                    bucket = (++slot & (Capacity - 1)) >> 6;
                }
            }

            // Increment the number of entries in the map.
            Count++;

            // Flag the bucket as containing entries.
            Index.Set(bucket);
            // Flag the entry in the bucket as active.
            Buckets[bucket].Active.Set(slot & 63);

            // Copy the key and construct the value.
            Keys[slot] = key;
            new(Data + slot)Value(arguments...);
            return Data[slot];
        };

        // Remove an entry from the map.
        Void Delete(const Key& key);

        // Lookup an entry and return a pointer to its payload if one is found.
        Value* Find(const Key& key) const
        {
            // Return if the map isn't allocated yet.
            if(!Data) { return nullptr; }

            // Mask the hashed key to compute the slot index and bucket index.
            Int slot = Hash::FNV32(key) & (Capacity - 1);
            Int bucket = slot >> 6;

            // Loop through (potentially) every entry in the map.
            for(Int i = 0; i < Capacity; i++)
            {
                // If the slot contains an active entry..
                if(Buckets[bucket].Active.Get(slot & 63))
                {
                    // If the entry has a matching key..
                    if(key == Keys[slot])
                    {
                        // A matching entry was found.
                        return Data + slot;
                    }
                }

                // If the slot isn't collided..
                if(!Buckets[bucket].Collided.Get(slot & 63))
                {
                    // An empty slot was reached without finding an active entry with a matching key.
                    break;
                }

                // Move to the next entry.
                bucket = (++slot & (Capacity - 1)) >> 6;
            }

            // No entry with a matching key was found.
            return nullptr;
        };

        // Update the key to an entry and return the pointer to its new location.
        Value& Move(const Key& oldKey, const Key& newKey);
    };
}
