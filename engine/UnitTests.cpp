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
void PrintAllocatorStats(MemoryAllocator& allocator)
{
    MemoryAllocator::Stats localstats = *allocator.GetStats();
    std::cout << "PAGE SIZE: " << localstats.pageSize << '\n';
    std::cout << "BLOCK SIZE: " << localstats.blockSize << '\n';
    std::cout << "TOTAL PAGES: " << localstats.totalPages << '\n';
    std::cout << "ALLOCATIONS: " << localstats.Allocations << '\n';
    std::cout << "DEALLOCATIONS: " << localstats.Deallocations << '\n';
    std::cout << "CURRENT PAGES: " << localstats.currentPages << '\n';
    std::cout << "USED PAGES: " << localstats.PagesInUse << '\n';
    std::cout << "FREE OBEJCTS: " << localstats.FreeObjects << '\n';
    for (const auto& blockStats : localstats.PageStats)
    {
        unsigned page = 1;
        std::cout << "Current Page: " << page << '\n';
        for (unsigned i = 0; i < localstats.pageSize / localstats.blockSize; i++)
        {
            std::cout << "Address: " << static_cast<void*>(blockStats[i].address) << '\n';
            std::cout << "Allocated: " << blockStats[i].allocated << '\n';
        }
        page++;
    }
}
MemoryAllocator CreateAllocator(size_t pages = 3, size_t pageSize = 256, size_t blockSize = 64) {
    return MemoryAllocator(pages, pageSize, blockSize);
}

void TestAllocationAndDeallocation() {
    try {
        MemoryAllocator allocator = CreateAllocator();
        PrintAllocatorStats(allocator);
        float* floattest = static_cast<float*>(allocator.allocate());
        if (floattest == nullptr) {
            throw std::runtime_error("Allocation failed");
        }
        PrintAllocatorStats(allocator);
        allocator.deallocate(floattest);
        PrintAllocatorStats(allocator);
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
        PrintAllocatorStats(allocator);
        // Should be able to allocate 4 blocks (2 pages * 128 bytes / 64 bytes per block)
        void* blocks[4];
        for (int i = 0; i < 4; ++i) 
        {
            blocks[i] = allocator.allocate();
            if (blocks[i] == nullptr) {
                throw std::runtime_error("Allocation failed");
            }
            PrintAllocatorStats(allocator);
        }

        // Next allocation should fail
        try {
            allocator.allocate();
            PrintAllocatorStats(allocator);
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
        PrintAllocatorStats(allocator);
        void* block1 = allocator.allocate();
        PrintAllocatorStats(allocator);
        void* block2 = allocator.allocate();
        PrintAllocatorStats(allocator);

        if (block1 == nullptr || block2 == nullptr) {
            throw std::runtime_error("Allocation failed");
        }

        allocator.deallocate(block1);
        PrintAllocatorStats(allocator);
        void* block3 = allocator.allocate();
        PrintAllocatorStats(allocator);
        if (block1 != block3) {
            throw std::runtime_error("Reallocation did not reuse the deallocated block");
        }

        allocator.deallocate(block2);
        PrintAllocatorStats(allocator);
        allocator.deallocate(block3);
        PrintAllocatorStats(allocator);

        std::cout << "TestDeallocateAndReallocate passed\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestDeallocateAndReallocate failed: " << e.what() << '\n';
    }
}

void TestDoubleFreeDetection() {
    try {
        MemoryAllocator allocator = CreateAllocator();
        PrintAllocatorStats(allocator);
        void* block1 = allocator.allocate();
        PrintAllocatorStats(allocator);

        if (block1 == nullptr) {
            throw std::runtime_error("Allocation failed");
        }

        allocator.deallocate(block1);
        PrintAllocatorStats(allocator);
        // Double free should throw an exception
        try {
            allocator.deallocate(block1);
            throw std::runtime_error("Double free should have failed but did not");
            PrintAllocatorStats(allocator);
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
        PrintAllocatorStats(allocator);
        float invalidBlock;

        // Deallocating an invalid pointer should throw an exception
        try {
            allocator.deallocate(&invalidBlock);
            throw std::runtime_error("Invalid pointer deallocation should have failed but did not");
            PrintAllocatorStats(allocator);
        }
        catch (const std::runtime_error&) {
            std::cout << "TestInvalidPointerDeallocation passed\n";
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "TestInvalidPointerDeallocation failed: " << e.what() << '\n';
    }
}