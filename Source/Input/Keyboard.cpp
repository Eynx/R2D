/*
-------------------------------------------------------------------------------
    Filename: Input/Keyboard.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Input\Keyboard.hpp"
// -- //
#include "..\Input\Manager.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Bool Input::Keyboard::Held(Input::Keyboard::Key key) const
    {
        return Input::Manager::Singleton->Held(Input::Key(key));
    };

    // ----------------------------------------------------------------------------------------
    Bool Input::Keyboard::Pressed(Input::Keyboard::Key key) const
    {
        return Input::Manager::Singleton->Pressed(Input::Key(key));
    };

    // ----------------------------------------------------------------------------------------
    Bool Input::Keyboard::Released(Input::Keyboard::Key key) const
    {
        return Input::Manager::Singleton->Released(Input::Key(key));
    };
}
