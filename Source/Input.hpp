/*
-------------------------------------------------------------------------------
    Filename: Input.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "Common.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Input
    {
        // Key descriptor containing the frames the key most recently registered as being pressed and released.
        struct State
        {
            // The most recent frame the key registered being pressed down.
            Int Pressed = -1;
            // The most recent frame the key registered being released.
            Int Released = -1;
        };

        // Enum matching common key names to their corresponding key code.
        enum Key : uByte
        {
            LeftMouse = 0x01, LMB = 0x01,
            RightMouse = 0x02, RMB = 0x02,
            Cancel = 0x03, // Not a mouse button
            MiddleMouse = 0x04, MMB = 0x04,
            MouseA = 0x05,
            MouseB = 0x06,
            // 0x07: Unassigned
            Backspace = 0x08,
            Tab = 0x09,
            // -- //
            Clear = 0x0C,
            Return = 0x0D, Enter = 0x0D,
            // -- //
            LeftShift = 0x10, Shift = 0x10,
            LeftControl = 0x11, Control = 0x11,
            Menu = 0x12,
            Pause = 0x13,
            CapsLock = 0x14, Capital = 0x14,
            // -- //
            Escape = 0x1B,
            Convert = 0x1C,
            NonConvert = 0x1D,
            Accept = 0x1E,
            Mode = 0x1F,
            // -- //
            Space = 0x20,
            Prior = 0x21,
            Next = 0x22,
            End = 0x23,
            Home = 0x24,
            // Arrow keys
            Left = 0x25,
            Up = 0x26,
            Right = 0x27,
            Down = 0x28,
            // -- //
            Select = 0x29,
            Print = 0x2A,
            Execute = 0x2B,
            Snapshot = 0x2C,
            Insert = 0x2D,
            Delete = 0x2E,
            Help = 0x2F,
            // ASCII '0' - '9' (0x30 - 0x39)
            // 0x40 : unassigned
            // ASCII 'A' - 'Z' (0x41 - 0x5A)
            LWindows = 0x5B,
            RWindows = 0x5C,
            Apps = 0x5D,
            // -- //
            Sleep = 0x5F,
            // Keyboard numpad keys.
            Numpad0 = 0x60,
            Numpad1 = 0x61,
            Numpad2 = 0x62,
            Numpad3 = 0x63,
            Numpad4 = 0x64,
            Numpad5 = 0x65,
            Numpad6 = 0x66,
            Numpad7 = 0x67,
            Numpad8 = 0x68,
            Numpad9 = 0x69,
            NumpadMultiply = 0x6A,
            NumpadAdd = 0x6B,
            NumpadSeparator = 0x6C,
            NumpadSubtract = 0x6D,
            NumpadDecimal = 0x6E,
            NumpadForwardSlash = 0x6F,
            // Keyboard function keys.
            F1 = 0x70,
            F2 = 0x71,
            F3 = 0x72,
            F4 = 0x73,
            F5 = 0x74,
            F6 = 0x75,
            F7 = 0x76,
            F8 = 0x77,
            F9 = 0x78,
            F10 = 0x79,
            F11 = 0x7A,
            F12 = 0x7B,
            F13 = 0x7C,
            F14 = 0x7D,
            F15 = 0x7E,
            F16 = 0x7F,
            F17 = 0x80,
            F18 = 0x81,
            F19 = 0x82,
            F20 = 0x83,
            F21 = 0x84,
            F22 = 0x85,
            F23 = 0x86,
            F24 = 0x87,
            // -- //
            Numlock = 0x90,
            Scroll = 0x91,
            // -- //
            LShift = 0xA0,
            RShift = 0xA1,
            LControl = 0xA2,
            RControl = 0xA3,
            LMenu = 0xA4,
            RMenu = 0xA5,
            // Browser keys
            BrowserBack = 0xA6,
            BrowserForward = 0xA7,
            BrowserRefresh = 0xA8,
            BrowserStop = 0xA9,
            BrowserSearch = 0xAA,
            BrowserFavorites = 0xAB,
            BrowserHome = 0xAC,
            // Media keys
            VolumeMute = 0xAD,
            VolumeDown = 0xAE,
            VolumeUp = 0xAF,
            MediaNext = 0xB0,
            MediaPrevious = 0xB1,
            MediaStop = 0xB2,
            MediaPlay = 0xB3,
            LaunchMail = 0xB4,
            MediaSelect = 0xB5,
            LaunchApp1 = 0xB6,
            LaunchApp2 = 0xB7,
            // -- //
            OEM1 = 0xBA,      // ';:' for US
            OEMPlus = 0xBB,   // '+' any country
            OEMComma = 0xBC,  // ',' any country
            OEMMinus = 0xBD,  // '-' any country
            OEMPeriod = 0xBE, // '.' any country
            OEM2 = 0xBF,      // '/?' for US
            OEM3 = 0xC0,      // '`~' for US
            // -- //
            OEM4 = 0xDB,  // '[{' for US
            OEM5 = 0xDC,  // '\|' for US
            OEM6 = 0xDD,  // ']}' for US
            OEM7 = 0xDE,  // ''"' for US
            OEM8 = 0xDF
        };
    }
}
