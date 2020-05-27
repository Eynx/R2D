/*
-------------------------------------------------------------------------------
    Filename: Common/Time.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"

namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Time
    {
        class Manager
        {
        public:
            // Members

            // The current frame being processed.
            Int Frame;

            // Static interface handle.
            static Manager* Singleton;

        public:
            // Constructors

            // Default constructor.
            Manager() : Frame(0) {};
            // Copy constructor.
            Manager(const Manager& other) = delete;
            // Move constructor.
            Manager(Manager&& other) : Frame(other.Frame) { other.Frame = 0; };
            // Destructor.
            ~Manager() {};
        };
    };
}
