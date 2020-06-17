/*
-------------------------------------------------------------------------------
    Filename: Common/Directory.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\String.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    class Directory
    {
    public:
        // Members

        // Static; The current working directory of the executable.
        static String* Working;
    };
}
