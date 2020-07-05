/*
-------------------------------------------------------------------------------
    Filename: Common/Set.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Common\Set.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Int BitSet::Count() const
    {
        // Return the number of active bits.
        return POPCNT(Mask);
    };

    // ----------------------------------------------------------------------------------------
    Bool BitSet::Get(Int index) const
    {
        return (Mask >> index) & 1;
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Set(Int index, Bool state)
    {
        // Disable the bit and then set it to the requested state.
        Mask ^= ~(1ULL << index);
        Mask |= uLong(state) << index;
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Set(Int index)
    {
        Mask |= uLong(1) << index;
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Reset(Int index)
    {
        Mask &= ~(1ULL << index);
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Reset()
    {
        // Simply set the mask to zero.
        Mask = 0;
    }

    // ----------------------------------------------------------------------------------------
    Int BitSet::Query() const
    {
        // Return the index of the lowest zero bit.
        return BitScanForward(~Mask);
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Peek() const
    {
        // Return the index of the lowest active bit.
        return BitScanForward(Mask);
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Query(Int index) const
    {
        // Invert the mask and disable all of the bits at and below the index and then search for the lowest active bit.
        return BitScanForward(~Mask & (~1ULL << index));
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Peek(Int index) const
    {
        // Disable all of the bits at and below the index and search for the lowest active bit.
        return BitScanForward(Mask & (~1ULL << index));
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Request()
    {
        // Get the index of the lowest zero bit.
        Int index = Query();

        // Set the bit to active if one was found.
        if(index > -1) { Set(index); }

        // Return the index.
        return index;
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Pop()
    {
        // Get the index of the lowest active bit.
        Int index = Peek();

        // Reset the bit if one was found.
        if(index > -1) { Reset(index); }

        // Return the index.
        return index;
    };
}
