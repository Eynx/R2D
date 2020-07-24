/*
-------------------------------------------------------------------------------
    Filename: R2D.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once

// Includes

// Common module
#include "Common.hpp"
#include "Common\Array.hpp"
#include "Common\Directory.hpp"
#include "Common\File.hpp"
#include "Common\Hash.hpp"
#include "Common\Map.hpp"
#include "Common\Memory.hpp"
#include "Common\Memory\Buffer.hpp"
#include "Common\Set.hpp"
#include "Common\String.hpp"
#include "Common\Time.hpp"
#include "Common\Types.hpp"
//#include "Common\Windows.hpp" // Only include into source files, not headers.

// Graphics module
#include "Graphics.hpp"
#include "Graphics\Heap.hpp"
#include "Graphics\State.hpp"
#include "Graphics\Window.hpp"
#include "Graphics\Manager.hpp"
//#include "Graphics\D3D12.hpp" // Only include into source files, not headers.

// Input module
#include "Input.hpp"
#include "Input\Keyboard.hpp"
#include "Input\Mouse.hpp"
#include "Input\Manager.hpp"

// Resource module
#include "Resource.hpp"
//#include "Resource\Loader.hpp" // Optional
#include "Resource\Material.hpp"
#include "Resource\Shader.hpp"
#include "Resource\Manager.hpp"

// Scene module
#include "Scene.hpp"

// Helper module
//#include "Helper\Application.hpp" // Optional and only necessary to include for the program entrypoint.
