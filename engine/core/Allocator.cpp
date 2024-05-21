#include "Allocator.h"
#include <stdexcept>

MemoryAllocator::MemoryAllocator()
{
    allocatorStats.blockSize = DEFAULT_BLOCK_SIZE; 
    allocatorStats.pageSize = DEFAULT_PAGE_SIZE;  
    allocatorStats.totalPages = DEFAULT_NUM_PAGES;
    allocatorStats.Allocations = 0;
    allocatorStats.Deallocations = 0;
    allocatorStats.PagesInUse = 0; 
    allocatorStats.FreeObjects = 0,
    allocatorStats.currentPages = 0;
    // Initialize memory pages
    for (int i = 0; i < DEFAULT_NUM_PAGES; ++i)
    {
        AllocateNewPage();
    }
}

MemoryAllocator::MemoryAllocator(size_t Pages, size_t PageSize, size_t BlockSize)
{
    allocatorStats.blockSize = BlockSize;
    allocatorStats.pageSize = PageSize;
    allocatorStats.totalPages = Pages;
    allocatorStats.Allocations = 0;
    allocatorStats.Deallocations = 0;
    allocatorStats.PagesInUse = 0;
    allocatorStats.FreeObjects = 0,
    allocatorStats.currentPages = 0;
    // Initialize memory pages
    for (int i = 0; i < Pages; ++i)
    {
        AllocateNewPage();
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
void MemoryAllocator::AllocateNewPage()
{
    if (allocatorStats.currentPages >= allocatorStats.totalPages)
    {
        throw std::runtime_error("Memory Allocator limit reached!");
        return;
    }
    Page* page = new Page(allocatorStats.pageSize, allocatorStats.blockSize);
    FreeList.push_back(page);
    allocatorStats.FreeObjects += allocatorStats.pageSize / allocatorStats.blockSize;
    allocatorStats.currentPages += 1;
    allocatorStats.PageStats.push_back(page->metadata);
}

void* MemoryAllocator::allocate()
{
    bool freeListMoveFlag = false;
    Page* current = nullptr;

    if (!partial.empty())
    {
        current = partial.front();
    }

    if (current == nullptr && !FreeList.empty())
    {
        current = FreeList.front();
        freeListMoveFlag = true;
    }
    //return type
    void* returnPtr = nullptr;

    if (current == nullptr)
    {
        throw std::runtime_error("No pages left!");
    }

    for (int i = 0; i < current->numBlocks; i++)
    {
        if (current->metadata[i].allocated == false)
        {
            returnPtr = &current->Data[i * allocatorStats.blockSize];
            allocatorStats.Allocations++;
            allocatorStats.FreeObjects--;
            current->FreeObjects--;
            current->metadata[i].allocated = true;
            break;
        }
    }
    //now that we have the pointer selected, if the memory block is now full, let us move it
    //if it was part of partial
    if (current->FreeObjects == 0)
    {
        full.push_back(current);
        partial.erase(std::remove(partial.begin(), partial.end(), current), partial.end());
    }
    else if (freeListMoveFlag)
    {
        partial.push_back(current);
        FreeList.erase(std::remove(FreeList.begin(), FreeList.end(), current), FreeList.end());
    }

    //if, somehow, the return type is still invalid
    if (returnPtr == nullptr)
    {
        throw std::runtime_error("Unable to Allocate memory!");
    }
    //then return if memory allocator is full
    return returnPtr;
}

// Deallocate memory
void MemoryAllocator::deallocate(void* ptr)
{
    auto deallocateFromList = [&](std::vector<Page*>& pageList) -> bool {
        for (auto it = pageList.begin(); it != pageList.end(); ++it)
        {
            Page* page = *it;
            // Check if the pointer belongs to this page
            if (ptr >= page->Data && ptr < (page->Data + (page->numBlocks * allocatorStats.blockSize)))
            {
                // Calculate the block index
                int blockIndex = (static_cast<char*>(ptr) - page->Data) / allocatorStats.blockSize;

                if (page->metadata[blockIndex].allocated)
                {
                    page->metadata[blockIndex].allocated = false;
                    page->FreeObjects++;
                    allocatorStats.Deallocations++;
                    allocatorStats.FreeObjects++;

                    // Move the page if necessary
                    if (pageList == full && page->FreeObjects > 0)
                    {
                        partial.push_back(page);
                        full.erase(it);
                    }
                    else if (pageList == partial && page->FreeObjects == page->numBlocks)
                    {
                        FreeList.push_back(page);
                        partial.erase(it);
                    }

                    return true;
                }
                else
                {
                    throw std::runtime_error("Double free detected!");
                }
            }
        }
        return false;
        };

    if (deallocateFromList(partial) || deallocateFromList(full))
    {
        return;
    }

    throw std::runtime_error("Pointer not allocated by this allocator!");
}

MemoryAllocator::Page::Page(size_t pageSize, size_t blockSize)
{
    PageSize = pageSize;
    BlockSize = blockSize;
    numBlocks = PageSize / BlockSize;
    FreeObjects = PageSize / BlockSize;
    metadata = new BlockInfo[numBlocks];
    Data = new char[PageSize];
    for (unsigned i = 0; i < PageSize / BlockSize; i++)
    {
        metadata[i].address = &Data[i * BlockSize];
    }
}

// Destructor for Page struct
MemoryAllocator::Page::~Page()
{
    delete[] metadata;
    delete[] Data;
}

const MemoryAllocator::Stats* MemoryAllocator::GetStats() const
{
    return &allocatorStats;
}