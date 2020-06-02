/*
-------------------------------------------------------------------------------
    Filename: Common/Set.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Common\Set.hpp"
// -- //
#include <intrin.h>

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Int CLZ(uInt value)
    {
        // Return number of zeroes until an active bit scanned from the left
        return __lzcnt(value);
    };

    // ----------------------------------------------------------------------------------------
    Int POPCNT(uInt value)
    {
        return __popcnt(value);
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Iterator::Count() const
    {
        // Return the number of active bits
        return Int(__popcnt64(Mask));
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Iterator::Peek() const
    {
        // Return the index of the lowest active bit
        return Int(63 - __lzcnt64(Mask & -Long(Mask)));
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Iterator::Next()
    {
        // Get the lowest active bit
        Int index = Peek();

        // If one was found
        if(index > -1)
        {
            // Set the bit to inactive
            Mask = Mask & ~(uLong(1) << index);
        }

        // Return the index
        return index;
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Count() const
    {
        // Return the number of active bits
        return Int(__popcnt64(Mask));
    };

    // ----------------------------------------------------------------------------------------
    Bool BitSet::Get(Int index) const
    {
        // Return the state of the bit
        return (Mask & (uLong(1) << index)) > 0;
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Set(Int index)
    {
        // Return the state of the bit
        Mask |= (uLong(1) << index);
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Reset(Int index)
    {
        // Set the bit to inactive
        Mask &= ~(uLong(1) << index);
    };

    // ----------------------------------------------------------------------------------------
    Void BitSet::Reset()
    {
        // Simply set the mask to zero
        Mask = 0;
    }

    // ----------------------------------------------------------------------------------------
    Int BitSet::Query() const
    {
        // Return the index of the lowest zero bit
        return Int(63 - __lzcnt64(~Mask & -Long(~Mask)));
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Query(Int count) const
    {
        // TODO: Finish implementing this.
        Assert(false, "Finish implmenting BitSet::Query(Int).");

        // Prepare the iterator
        auto iterator = Iterate();

        // Reverse the active-inactive bits
        iterator.Mask = iterator.Mask;

        // Loop through all the inactive bits (which are now the active bits)
        while(iterator.Count())
        {
        }

        // Return the index of the lowest zero bit
        return Int(63 - __lzcnt64(~Mask & -Long(~Mask)));
    };

    // ----------------------------------------------------------------------------------------
    Int BitSet::Request()
    {
        // Get the index of the lowest zero bit
        Int index = Query();

        // If one was found
        if(index > -1)
        {
            // Set the bit to active
            Mask = Mask | (uLong(1) << index);
        }

        // Return the index
        return index;
    };
}
