/*
-------------------------------------------------------------------------------
    Filename: Graphics/Heap.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once
// Includes
#include "..\Common.hpp"
#include "..\Common\Set.hpp"
// -- //
#include "..\Graphics.hpp"

// Forward declarations.
struct ID3D12DescriptorHeap;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    namespace Graphics
    {
        // ------------------------------------------------------------------------------------
        struct Descriptor
        {
            // Descriptor flags specifying the type of the descriptor and its data capabilities.
            enum class Flag : Byte
            {
                None = 0,
                // Descriptor types. Not bitwise since a descriptor can only be one type.
                SRV = 1, // The descriptor is a Shader Resource View.
                CBV = 2, // The descriptor is a Constant Buffer View.
                UAV = 3, // The descriptor is an Unordered-Access View.
                RTV = 4, // The descriptor is a Render-Target View.
                DSV = 5, // The descriptor is a Depth-Stencil View.
                Sampler = 6, // The descriptor is a Texture Sampler.
                // Data capabilities. Also not bitwise since a descriptor can only have one data type.
                Static = 8, // The descriptor references a static resource.
                Dynamic = 9, // The descriptor references a dynamic resource.
                Volatile = 10 // the descriptor references a volatile resource.
            };

            // Container for storing the descriptor addresses.
            struct
            {
                // The descriptor's CPU address handle value.
                uLong CPU = 0;
                // The descriptor's GPU address handle value.
                uLong GPU = 0;
            } Address;
            // The array index of the resource the descriptor in its specified category.
            Int Index;
            // Flags detailing the type of the descriptor and its root signature promises.
            Flag Flags;

            // Operators

            // Conversion operator for retrieving the CPU address of the descriptor.
            operator D3D12_CPU_DESCRIPTOR_HANDLE() const;
            // Conversion operator for retrieving the GPU address of the descriptor.
            operator D3D12_GPU_DESCRIPTOR_HANDLE() const;
        };

        // --------------------------------------------------------------------------------
        class Heap
        {
        public:
            // Nested types

            // Descriptor heap flags specifying the capabilities of the heap.
            enum class Flag : Byte
            {
                None = 0,
                // Shader-visible flags.
                SRV = 1, // The descriptor heap contains Shader Resource Views. This heap can also contain CBVs and UAVs. The heap is shader visible.
                CBV = 1, // The descriptor heap contains Constant Buffer Views. This heap can also contain SRVs and UAVs. The heap is shader visible.
                UAV = 1, // The descriptor heap contains Unordered-Access Views. This heap can also contain SRVs and CBVs. The heap is shader visible.
                Sampler = 2, // The descriptor heap contains Samplers. This heap can only contain Samplers and is shader visible.
                // Non-shader-visible flags.
                RTV = 3, // The descriptor heap contains Render-Target Views. This heap can only contain RTVs and isn't shader visible.
                DSV = 4  // The descriptor heap contains Depth-Stencil Views. This heap can only contain DSVs and isn't shader visible.
            };

            // Descriptor struct for passing heap parameters to the Create() method.
            struct Description
            {
                // The maximum number of descriptors the heap will be able to contain.
                Int Capacity = 0;
                // Flags specifiying the capabilities of the heap to create. Refer to Heap::Flag::* for information regarding each flag.
                Flag Flags = Flag::None;
            };

            class Range
            {
            public:
                // Members

                // Handle to the heap the range is allocated inside.
                Heap* Handle = nullptr;
                // Registry for indexing up to 64 entries.
                BitSet Index = BitSet();
                // Offset from the start of the heap to where the range begins.
                Int Offset = 0;
                // The number of descriptors inside the range.
                Int Capacity = 0;

            public:

                // Operators

                // Array access operator.
                Descriptor operator [] (Int index);
                // Const array access operator.
                Descriptor operator [] (Int index) const;

                // Methods

                // Request a free descriptor slot from the range and set it as occupied.
                Descriptor Request() { return (*this)[Index.Request()]; };
                // Request a specific descriptor slot from the range and set it as occupied. 
                Descriptor Set(Int index) { Assert(!Index.Get(index), "Attempting to set a slot that is already set."); Index.Set(index); return (*this)[index]; };
            };

            // Graphics::Manager interface for dealing with multiple descriptor heaps.
            class Manager;

        public:
            // Members

            // D3D12 handle to the descriptor heap object.
            ID3D12DescriptorHeap* Handle;
            // The maximum number of descriptors the heap can contain.
            Int Capacity;
            // The size of a single descriptor entry in the heap.
            Int Stride;
            // Flags detailing the capabilities of the heap.
            Flag Flags;

        public:
            // Constructors

            // Default constructor.
            Heap() : Handle(nullptr), Capacity(0), Stride(0), Flags(Flag::None) {};
            // Copy constructor.
            Heap(const Heap& other) = delete;
            // Move constructor.
            Heap(Heap&& other) : Handle(other.Handle), Capacity(other.Capacity), Stride(other.Stride), Flags(other.Flags) { other.Handle = nullptr; other.Capacity = 0; other.Stride = 0; other.Flags = Flag::None; };
            // Destructor.
            ~Heap() { Release(); };

            // Operators

            // Array access operator.
            Descriptor operator [] (Int index);
            // Const array access operator.
            Descriptor operator [] (Int index) const;

            // Methods

            // Create the descriptor heap.
            Void Create(const Description& description);
            // Release the descriptor heap.
            Void Release();
        };
    }
}

// Define operators for the commonly used scoped enum[s].
DEFINE_ENUM_OPERATORS(R2D::Graphics::Heap::Flag);
