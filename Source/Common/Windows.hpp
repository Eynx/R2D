/*
-------------------------------------------------------------------------------
    Filename: Common/Windows.hpp
-------------------------------------------------------------------------------
    Encapsulates including Windows.h into source files. Please don't
    include it into headers for reasons relating to namespace pollution.
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"

// Prepare and include Windows.h
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
 // -- //
#include <Windows.h>

// Deal with Windows.h here later as conflicts arise (they are bound to)
#undef near // Common name for variables
#undef far // Common name for variables
