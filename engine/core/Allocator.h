#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include <utility>
#define DEFAULT_NUM_PAGES 10
#define DEFAULT_PAGE_SIZE 4096
#define DEFAULT_BLOCK_SIZE 64


class MemoryAllocator
{
public:
	struct BlockInfo
	{
		bool allocated;
		char* address;
		BlockInfo() { allocated = false; }
	};
	struct Page
	{
		size_t PageSize, BlockSize;
		unsigned numBlocks;
		unsigned FreeObjects;
		BlockInfo* metadata;
		char* Data;
		Page(size_t pageSize, size_t blockSize);
		~Page();
	};
	struct Stats
	{
		size_t pageSize;
		size_t blockSize;
		size_t totalPages;
		unsigned Allocations;
		unsigned Deallocations;
		unsigned currentPages;
		unsigned PagesInUse;
		unsigned FreeObjects;
		std::vector<BlockInfo*> PageStats;
	};
	MemoryAllocator();
	MemoryAllocator(size_t Pages, size_t PageSize, size_t BlockSize);
	~MemoryAllocator();
	 void* allocate();
	void deallocate(void*);
	const Stats* GetStats() const;
private:
	
	std::vector<Page*> FreeList;
	std::vector<Page*> partial;
	std::vector<Page*> full;
	
	void AllocateNewPage();

	Stats allocatorStats;
};

