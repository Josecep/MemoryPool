#include "MemoryMgr.h"
#include <assert.h>


#ifdef _DEBUG
#include <cstdio>
	#define xPrintf(...) printf(__VA_ARGS__)
#else
	#define xPrintf(xxx)  
#endif

MemoryBlock::MemoryBlock()
{

}

MemoryBlock::~MemoryBlock()
{
}

MemoryAlloc::MemoryAlloc()
{
	_pBuf = nullptr;
	_pHeader = nullptr;
	_nSize = 0;
	_nBlockSize = 0;
}

MemoryAlloc::~MemoryAlloc()
{
	if (_pBuf)
	{
		free(_pBuf);
	}
}

void* MemoryAlloc::allocMemory(size_t nSize)
{
	if (!_pBuf)
	{
		initMemory();
	}

	MemoryBlock* pReturn = nullptr;
	if (!_pHeader)
	{
		pReturn = (MemoryBlock*)malloc(nSize+sizeof(MemoryBlock));
		pReturn->bPool = false;
		pReturn->nID = -1;
		pReturn->nRef = 1;
		pReturn->pAlloc = nullptr;
		pReturn->pNext = nullptr;
	}
	else
	{
		pReturn = _pHeader;
		_pHeader = _pHeader->pNext;
		assert(0 == pReturn->nRef);
		pReturn->nRef = 1;
	}

	xPrintf("allocMemory: %llx,id=%d,size=%d\n", pReturn, pReturn->nID, nSize);

	return ((char*)pReturn + sizeof(MemoryBlock));
}

void MemoryAlloc::freeMemory(void* pMem)
{
	MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
	xPrintf("freeMemory: %llx,id=%d\n", pBlock, pBlock->nID);
	assert(1 == pBlock->nRef);

	if (--pBlock->nRef != 0)
	{
		return;
	}
	if (pBlock->bPool)
	{
		pBlock->pNext = _pHeader;
		_pHeader = pBlock;
	}else{
		free(pBlock);
	}

}

void MemoryAlloc::initMemory()
{
	//断言
	assert(nullptr == _pBuf);

	if (_pBuf)
		return;

	//计算内存池的大小
	size_t realSize = _nSize + sizeof(MemoryBlock);
	size_t bufSize = realSize * _nBlockSize;

	//向系统申请池的内存
	_pBuf = (char *)malloc(bufSize);

	//初始化内存池
	_pHeader = (MemoryBlock*)_pBuf;
	_pHeader->bPool = true;
	_pHeader->nID = 0;
	_pHeader->nRef = 0;
	_pHeader->pAlloc = this;
	_pHeader->pNext = nullptr;
	

	//遍历内存块并初始化
	MemoryBlock* pTemp1 = _pHeader;
	
	for (size_t i = 1; i < _nBlockSize; i++)
	{
		MemoryBlock* pTemp2 = (MemoryBlock * )(_pBuf + (i * _nSize + sizeof(MemoryBlock)));
		pTemp2->bPool = true;
		pTemp2->nID = i;
		pTemp2->nRef = 0;
		pTemp2->pAlloc = this;
		pTemp2->pNext = nullptr;
		pTemp1->pNext = pTemp2;
		pTemp1 = pTemp2;	
	}
}

MemoryMgr::MemoryMgr()
{
	init(0, 64, &_mem64);
}

MemoryMgr::~MemoryMgr()
{
}

void MemoryMgr::init(int nBegin, int nEnd, MemoryAlloc* pMemA)
{
	for (int i = nBegin; i <= nEnd; i++)
	{
		_szAlloc[i] = pMemA;
	}
}

MemoryMgr& MemoryMgr::Instance()
{
	static MemoryMgr mgr;
	return mgr;
}


void* MemoryMgr::allocMem(size_t nSize)
{

	if (nSize <= MAX_MEMORY_SIZE)
	{
		return _szAlloc[nSize]->allocMemory(nSize);
	}
	else
	{
		MemoryBlock*  pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
		pReturn->bPool = false;
		pReturn->nID = -1;
		pReturn->nRef = 1;
		pReturn->pAlloc = nullptr;
		pReturn->pNext = nullptr;

		xPrintf("allocMem: %llx,id=%d,size=%d\n", pReturn, pReturn->nID, nSize);

		return ((char*)pReturn + sizeof(MemoryBlock));
	}
	
}

void MemoryMgr::freeMem(void* pMem)
{
	MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
	xPrintf("freeMem: %llx,id=%d\n", pBlock, pBlock->nID);

	if (pBlock->bPool)
	{
		pBlock->pAlloc->freeMemory(pMem);
	}
	else
	{
		if(--pBlock->nRef == 0)
			free(pBlock);
	}
}

void MemoryMgr::addRef(void* pMem)
{
	MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
	++pBlock->nRef;
}
