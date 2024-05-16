#include "UnitTests.h"
#include "Allocator.h"
#include <stdexcept>
#include <iostream>
void TestAllocationAndDeallocation();
void TestAllocationUntilExhausted();
void TestDeallocateAndReallocate();
void TestDoubleFreeDetection();
void TestInvalidPointerDeallocation();

void RunUnitTests()
{
    try {
        TestAllocationAndDeallocation();
        TestAllocationUntilExhausted();
        TestDeallocateAndReallocate();
        TestDoubleFreeDetection();
        TestInvalidPointerDeallocation();
        std::cout << "All tests passed!\n";
    }
    catch (const std::exception& e) {
        std::cerr << "A test failed: " << e.what() << '\n';
    }
}

MemoryAllocator CreateAllocator(size_t pages = 10, size_t pageSize = 1024, size_t blockSize = 64) {
    return MemoryAllocator(pages, pageSize, blockSize);
}

void TestAllocationAndDeallocation() {
    try {
        MemoryAllocator allocator = CreateAllocator();
        float* floattest = static_cast<float*>(allocator.allocate());
        if (floattest == nullptr) {
            throw std::runtime_error("Allocation failed");
        }

        allocator.deallocate(floattest);
        std::cout << "TestAllocationAndDeallocation passed\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestAllocationAndDeallocation failed: " << e.what() << '\n';
    }
}

void TestAllocationUntilExhausted() {
    try {
        size_t pages = 2;
        size_t pageSize = 128; // 2 pages * 128 bytes = 256 bytes total
        size_t blockSize = 64; // Each block is 64 bytes

        MemoryAllocator allocator = CreateAllocator(pages, pageSize, blockSize);

        // Should be able to allocate 4 blocks (2 pages * 128 bytes / 64 bytes per block)
        void* blocks[4];
        for (int i = 0; i < 4; ++i) {
            blocks[i] = allocator.allocate();
            if (blocks[i] == nullptr) {
                throw std::runtime_error("Allocation failed");
            }
        }

        // Next allocation should fail
        try {
            allocator.allocate();
            throw std::runtime_error("Allocation should have failed but did not");
        }
        catch (const std::runtime_error&) {
            std::cout << "TestAllocationUntilExhausted passed\n";
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestAllocationUntilExhausted failed: " << e.what() << '\n';
    }
}

void TestDeallocateAndReallocate() {
    try {
        MemoryAllocator allocator = CreateAllocator();
        void* block1 = allocator.allocate();
        void* block2 = allocator.allocate();

        if (block1 == nullptr || block2 == nullptr) {
            throw std::runtime_error("Allocation failed");
        }

        allocator.deallocate(block1);
        void* block3 = allocator.allocate();
        if (block1 != block3) {
            throw std::runtime_error("Reallocation did not reuse the deallocated block");
        }

        allocator.deallocate(block2);
        allocator.deallocate(block3);

        std::cout << "TestDeallocateAndReallocate passed\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestDeallocateAndReallocate failed: " << e.what() << '\n';
    }
}

void TestDoubleFreeDetection() {
    try {
        MemoryAllocator allocator = CreateAllocator();
        void* block1 = allocator.allocate();

        if (block1 == nullptr) {
            throw std::runtime_error("Allocation failed");
        }

        allocator.deallocate(block1);

        // Double free should throw an exception
        try {
            allocator.deallocate(block1);
            throw std::runtime_error("Double free should have failed but did not");
        }
        catch (const std::runtime_error&) {
            std::cout << "TestDoubleFreeDetection passed\n";
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestDoubleFreeDetection failed: " << e.what() << '\n';
    }
}

void TestInvalidPointerDeallocation() {
    try {
        MemoryAllocator allocator = CreateAllocator();
        float invalidBlock;

        // Deallocating an invalid pointer should throw an exception
        try {
            allocator.deallocate(&invalidBlock);
            throw std::runtime_error("Invalid pointer deallocation should have failed but did not");
        }
        catch (const std::runtime_error&) {
            std::cout << "TestInvalidPointerDeallocation passed\n";
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestInvalidPointerDeallocation failed: " << e.what() << '\n';
    }
}