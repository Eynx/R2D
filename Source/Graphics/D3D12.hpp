/*
-------------------------------------------------------------------------------
    Filename: Graphics/D3D12.hpp
-------------------------------------------------------------------------------
    Helper header for including Direct3D12 headers into a source file. As
    with the Windows.hpp helper, please don't include this into headers.
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once

// Includes
#include "..\Common.hpp"
// Windows prerequisite.
#include "..\Common\Windows.hpp"

// Redefine necessary windows macros.
#define near
#define far

// Include the D3D12 headers.
#include <d3d12.h>
#ifdef _DEBUG
#include <d3d12sdklayers.h>
#endif
#include <dxgi1_4.h>

// Clean up some namespace pollution.
#undef near
#undef far

// Link the D3D12 libraries.
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
