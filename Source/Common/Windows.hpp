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

// Include STL for std::wstring support
#include <string>
//#include <codecvt>

// Deal with Windows.h here later as conflicts arise (they are bound to)
#undef near // Common name for variables
#undef far // Common name for variables
#undef BitScanForward // Naming conflict with R2D::BitScanForward()
#undef BitScanReverse // Naming conflict with R2D::BitScanReverse()

// String utilities for dealing UTF-16
#include "..\Common\String.hpp"
// -- //
namespace R2D
{
    // UTF-8 string to UTF-16 string conversion
    inline std::wstring widen(const String& string)
    {
        // Declare temporary storage to contain the converted string data.
        auto temp = new wchar_t[string.Size()];

        // Convert the UTF-8 string to UTF-16.
        Int result = MultiByteToWideChar(CP_UTF8, 0, string.Data, string.Size(), temp, string.Size());
        // Debug check
        Assert(result > 0, "There was a problem converting the string.");

        // Construct and return the heap-allocated string.
        return std::wstring(temp, result);

        //return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(string.Data);
    };

    // UTF-16 string to UTF-8 string conversion
    inline String narrow(const std::wstring& wstring)
    {
        // Declare temporary storage to contain the converted string data.
        auto temp = new char[wstring.size()];

        // Convert the UTF-16 string to UTF-8.
        Int result = WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), int(wstring.length()), temp, int(wstring.size()), NULL, NULL);
        // Debug check
        Assert(result > 0, "There was a problem converting the string.");

        // Construct and return the heap-allocated string.
        return String(temp, result);

        //auto string = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstring);
        //return String(string.c_str(), Int(string.length()));
    };
}
