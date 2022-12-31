#ifndef _MEMORY_MGR_H_
#define _MEMORY_MGR_H_
#include <cstdlib>


#define MAX_MEMORY_SIZE 64

class MemoryAlloc;
//内存块
class MemoryBlock
{
public:
	MemoryBlock();
	~MemoryBlock();
public:
	//内存块编号
	int nID;

	//引用次数
	int nRef;

	//所属的内存池
	MemoryAlloc *pAlloc;

	//下一块的位置
	MemoryBlock* pNext;

	//是否在内存池中
	bool bPool;
	

private:
	//预留
	char c1;
	char c2;
	char c3;
};

//内存池
class MemoryAlloc
{
public:
	MemoryAlloc();
	~MemoryAlloc();

	//申请内存
	void* allocMemory(size_t nSize);

	//释放内存
	void freeMemory(void* p);

	//初始化
	void initMemory();

protected:
	//内存池地址
	char* _pBuf;
	//头部内存单元
	MemoryBlock* _pHeader;
	//内存单元的大小
	size_t _nSize;
	//内存单元的数量
	size_t _nBlockSize;
};

//便于声明成员变量时初始化MemoryAlloc
template<size_t nSize,size_t nBloockSize>
class MemoryAlloctor:public MemoryAlloc
{
public:
	MemoryAlloctor();
};

//内存管理工具
class MemoryMgr
{
private:
	MemoryMgr();
	~MemoryMgr();
private:
	//初始化内存池映射数组
	void init(int nBegin, int nEnd, MemoryAlloc* pMemA);

public:
	static MemoryMgr& Instance();

	//申请内存
	void* allocMem(size_t nSize);

	//释放内存
	void freeMem(void* p);

	//增加内存池的引用计数
	void addRef(void* pMem);
private:
	MemoryAlloctor<MAX_MEMORY_SIZE,10> _mem64;
	MemoryAlloc* _szAlloc[MAX_MEMORY_SIZE + 1];

};



#endif

template<size_t nSize, size_t nBloockSize>
inline MemoryAlloctor<nSize, nBloockSize>::MemoryAlloctor()
{
	const size_t n = sizeof(void*);

	_nSize = (nSize/n)*n+(nSize % n ? n : 0);
	_nBlockSize = nBloockSize;
}
