/*
-------------------------------------------------------------------------------
    Filename: Common/Hash.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Hash
    {
        // 32-bit FNV-1a (Fowler/Noll/Vo) hash function.
        inline uInt FNV32(const Void* data, Int size)
        {
            // Helper
            auto bytes = (const Byte*)data;

            // Starting hash offset basis.
            uInt hash = 2166136261;

            // Loop over every byte in the type and perform the hashing operation.
            for(Int i = 0; i < size; i++)
            {
                // FNV-1 performs the multiplication with the hash value and xor with the magic constant.
                // FNV-1a reverses the order, performing the xor with the hash value and multiplication with the prime constant.
                hash = (hash ^ bytes[i]) * 16777619;
            }
            // -- //
            return hash;
        };

        // 32-bit FNV-1a (Fowler/Noll/Vo) hash function. Can be used during compile-time with C-strings. Doesn't include the null byte.
        template <size_t count> constexpr uInt FNV32(const char(&string)[count])
        {
            // Starting hash offset basis.
            uInt hash = 2166136261;

            // Loop over every byte in the type and perform the hashing operation.
            for(Int i = 0; i < (count - 1); i++)
            {
                // FNV-1 performs the multiplication with the hash value and xor with the magic constant.
                // FNV-1a reverses the order, performing the xor with the hash value and multiplication with the prime constant.
                hash = (hash ^ string[i]) * 16777619;
            }
            // -- //
            return hash;
        };

        // 32-bit FNV-1a (Fowler/Noll/Vo) hash function. Helper interface for cleanly hashing POD types.
        template <typename Type> inline uInt FNV32(const Type& object)
        {
            // Parse some type information and simply call the normal FNV32 function.
            return FNV32(&object, sizeof(Type));
        };
    }
}
