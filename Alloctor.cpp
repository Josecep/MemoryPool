#include "Alloctor.h"
#include "MemoryMgr.h"

void* operator new(size_t size)
{
	return MemoryMgr::Instance().allocMem(size);
}

void operator delete(void* p)
{
	MemoryMgr::Instance().freeMem(p);
}


void* operator new[](size_t size)
{
	return MemoryMgr::Instance().allocMem(size);
}

void operator delete[](void* p)
{
	MemoryMgr::Instance().freeMem(p);
}

void* mem_alloc(size_t size)
{
	return malloc(size);
}

void mem_dree(void* p)
{
	free(p);
}
