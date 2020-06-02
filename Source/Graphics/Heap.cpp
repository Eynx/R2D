/*
-------------------------------------------------------------------------------
    Filename: Graphics/Heap.cpp
-------------------------------------------------------------------------------
*/

// Includes
#include "..\Graphics\Heap.hpp"
// -- //
#include "..\Graphics\Manager.hpp"
// -- //
#include "..\Graphics\D3D12.hpp"

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // ----------------------------------------------------------------------------------------
    Graphics::Descriptor::operator D3D12_CPU_DESCRIPTOR_HANDLE() const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        // Populate the descriptor handle.
        handle.ptr = Address.CPU;
        return handle;
    };

    // ----------------------------------------------------------------------------------------
    Graphics::Descriptor::operator D3D12_GPU_DESCRIPTOR_HANDLE() const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle;
        // Populate the descriptor handle.
        handle.ptr = Address.GPU;
        return handle;
    };

    // ----------------------------------------------------------------------------------------
    Graphics::Descriptor Graphics::Heap::Range::operator [] (Int index)
    {
        // Debug checks
        Assert(index >= 0, "Attempting to retrieve a descriptor with a negative index.");
        Assert(index < Capacity, "Attempting to retrieve a descriptor past the end of the range.");

        // Retrieve the heap this range corresponds to.
        Graphics::Heap& heap = *Handle;

        // Return the descriptor in the range requested.
        Graphics::Descriptor out = heap[index + Offset];
        out.Index = index;
        return out;
    };

    // ----------------------------------------------------------------------------------------
    Graphics::Descriptor Graphics::Heap::Range::operator [] (Int index) const
    {
        // Debug checks
        Assert(index >= 0, "Attempting to retrieve a descriptor with a negative index.");
        Assert(index < Capacity, "Attempting to retrieve a descriptor past the end of the range. Small heap ranges can only contain up to 64 entries.");

        // Retrieve the heap this range corresponds to.
        Graphics::Heap& heap = *Handle;

        // Return the descriptor in the range requested.
        Graphics::Descriptor out = heap[index + Offset];
        out.Index = index;
        return out;
    };

    // ----------------------------------------------------------------------------------------
    Graphics::Descriptor Graphics::Heap::operator [] (Int index)
    {
        // Debug checks
        Assert(index >= 0, "Attempting to retrieve a descriptor with a negative index.");
        Assert(index < Capacity, "Attempting to retrieve a descriptor past the end of the heap.");

        Graphics::Descriptor descriptor;
        // Populate the descriptor addresses.
        descriptor.Address.CPU = Handle->GetCPUDescriptorHandleForHeapStart().ptr + (UINT64(Stride) * index); // Always populate the CPU address for the descriptor.
        if(Byte(Flags) < 2) { descriptor.Address.GPU = Handle->GetGPUDescriptorHandleForHeapStart().ptr + (UINT64(Stride) * index); } // Only populate the GPU address if the heap is shader-visible.
        descriptor.Index = index;
        return descriptor;
    };

    // ----------------------------------------------------------------------------------------
    Graphics::Descriptor Graphics::Heap::operator [] (Int index) const
    {
        // Debug checks
        Assert(index >= 0, "Attempting to retrieve a descriptor with a negative index.");
        Assert(index < Capacity, "Attempting to retrieve a descriptor past the end of the heap.");

        Graphics::Descriptor descriptor;
        // Populate the descriptor addresses.
        descriptor.Address.CPU = Handle->GetCPUDescriptorHandleForHeapStart().ptr + (UINT64(Stride) * index); // Always populate the CPU address for the descriptor.
        if(Byte(Flags) < 2) { descriptor.Address.GPU = Handle->GetGPUDescriptorHandleForHeapStart().ptr + (UINT64(Stride) * index); } // Only populate the GPU address if the heap is shader-visible.
        descriptor.Index = index;
        return descriptor;
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Heap::Create(const Graphics::Heap::Description& description)
    {
        // Retrieve the graphics manager.
        auto graphics = Graphics::Manager::Singleton;

        // Debug checks
        Assert(description.Capacity > 0, "Cannot create a descriptor heap containing no descriptors.");
        Assert(!Handle, "Heap handle already populated. Make sure the heap isn't already created or the structure is being constructed properly.");

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        // Describe the descriptor heap.
        heapDesc.NumDescriptors = description.Capacity;
        heapDesc.Flags = (Byte(description.Flags) <= 2) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE(Byte(description.Flags) - 1);
        heapDesc.NodeMask = 0;

        // Create the descriptor heap.
        HRESULT result = graphics->D3D.Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&Handle));
        // Debug check
        Assert(SUCCEEDED(result), "There was a problem creating the descriptor heap.");

        // Set the descriptor stride.
        Stride = graphics->D3D.Device->GetDescriptorHandleIncrementSize(heapDesc.Type);

        // Store the description values.
        Capacity = description.Capacity;
        Flags = description.Flags;
    };

    // ----------------------------------------------------------------------------------------
    Void Graphics::Heap::Release()
    {
        // Release the descriptor heap object.
        if(Handle) { Handle->Release(); Handle = nullptr; }
    };
}
