#include "Allocator.h"
#include <stdexcept>

MemoryAllocator::MemoryAllocator()
{
    Allocations = 0;
    Deallocations = 0;
    PagesInUse = 0;
    FreeObjects = 0;

    // Initialize memory pages
    for (int i = 0; i < DEFAULT_NUM_PAGES; ++i)
    {
        Page* page = new Page(DEFAULT_PAGE_SIZE, DEFAULT_BLOCK_SIZE);
        FreeList.push_back(page);
        PagesInUse++;
    }
}

// Destructor
MemoryAllocator::~MemoryAllocator()
{
    // Free memory pages
    for (Page* page : FreeList)
    {
        delete page;
    }
    FreeList.clear();

    for (Page* page : partial)
    {
        delete page;
    }
    partial.clear();

    for (Page* page : full)
    {
        delete page;
    }
    full.clear();
}

void* MemoryAllocator::allocate()
{
    // Find the page with the best fit
    Page* bestFitPage = nullptr;
    size_t minFragmentation = SIZE_MAX;

    for (Page* page : FreeList)
    {
        size_t freeSpace = 0;
        for (unsigned i = 0; i < page->numBlocks; ++i)
        {
            if (!page->metadata[i].allocated)
                freeSpace++;
        }

        // Calculate fragmentation
        size_t fragmentation = (page->numBlocks - freeSpace) * page->BlockSize;

        // If the page has enough space and fragmentation is less than current minFragmentation
        if (freeSpace > 0 && fragmentation < minFragmentation)
        {
            bestFitPage = page;
            minFragmentation = fragmentation;
        }
    }

    // If a page with free space is found
    if (bestFitPage)
    {
        // Allocate a block in the best fit page
        for (unsigned i = 0; i < bestFitPage->numBlocks; ++i)
        {
            if (!bestFitPage->metadata[i].allocated)
            {
                bestFitPage->metadata[i].allocated = true;
                FreeObjects--;
                return bestFitPage->Data + i * bestFitPage->BlockSize;
            }
        }
    }

    // No suitable free space found
    return nullptr;
}

// Deallocate memory
void MemoryAllocator::deallocate(void* ptr)
{
    // Iterate through all pages to find the page containing the pointer
    for (Page* page : FreeList)
    {
        // Check if the pointer falls within the range of this page's memory
        if (ptr >= page->Data && ptr < page->Data + page->PageSize)
        {
            // Calculate the index of the block in the page
            size_t blockIndex = (reinterpret_cast<char*>(ptr) - page->Data) / page->BlockSize;

            // Mark the corresponding block as deallocated
            if (blockIndex < page->numBlocks && page->metadata[blockIndex].allocated)
            {
                page->metadata[blockIndex].allocated = false;
                FreeObjects++;
                return;
            }
        }
    }

    // If the pointer doesn't belong to any page or the block is already deallocated
    // Handle error or do nothing (depending on your error handling strategy)
}

MemoryAllocator::Page::Page(size_t pageSize, size_t blockSize)
{
    PageSize = pageSize;
    BlockSize = blockSize;
    numBlocks = PageSize / BlockSize;
    metadata = new BlockInfo[numBlocks];
    Data = new char[PageSize];
}

// Destructor for Page struct
MemoryAllocator::Page::~Page()
{
    delete[] metadata;
    delete[] Data;
}