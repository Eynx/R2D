/*
-------------------------------------------------------------------------------
    Filename: Common.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "Common\Types.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // Initialize the framework and allocate the module managers.
    extern Void Initialize();

    // Update all the modules.
    extern Bool Update();

    // Prompt the framework to end the main application loop and shutdown.
    extern Void Quit();

    // Shuts down and deallocates all of the modules.
    extern Void Shutdown();

    // Temporarily suspend execution of the program.
    extern Void Sleep(Int milliseconds);
    // TODO: Located here until a Time module or something is implemented maybe?

    // Helper function; Shorthand for casting an lvalue to an xvalue.
    template <typename Type> inline Type&& Move(Type& object) { return static_cast<Type&&>(object); };

    // TODO: Temporary namespace containing common debugging utilties?
    namespace Debug
    {
        // Print text to debugger console.
        extern Void Print(const wchar_t* pOutputString);
    }
}

// Debug-Mode utility that triggers a breakpoint if the asserted expression is false.
#define Assert(expression, message) if(!(expression)) { __debugbreak(); }

// Debug-Mode utlity that displays the contained message if the expression is false. Use only if the program state is still valid.
#define Warning(expression, message) if(!(expression)) { R2D::Debug::Print(L##"WARNING: "##message"\n"); }
