/*
-------------------------------------------------------------------------------
    Filename: Resource.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "Common.hpp"
#include "Common\Hash.hpp"
#include "Common\String.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Resource
    {
        // ------------------------------------------------------------------------------------
        template <typename Type> struct ID
        {
        public:
            // Members

            // The internal handle the ID maps to.
            uInt Handle;

        public:
            // Constructors

            // Default constructor.
            constexpr ID() : Handle(0) {};
            // Integer constructor.
            constexpr ID(uInt id) : Handle(id) {};
            // String constructor.
            constexpr ID(const String& string) : Handle(Hash::FNV32(string.Data, string.Length)) {};
            // C-String constructor.
            template <size_t count> constexpr ID(const char(&string)[count]) : Handle(Hash::FNV32(string)) {};
            // Copy constructor.
            constexpr ID(const ID& other) : Handle(other.Handle) {};

            // Equality operators.
            Bool operator == (const ID& other) const { return Handle == other.Handle; }
            Bool operator != (const ID& other) const { return Handle != other.Handle; }
        };
    }
}
