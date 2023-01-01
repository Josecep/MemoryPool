#ifndef _MEMORY_MGR_H_
#define _MEMORY_MGR_H_
#include <cstdlib>
#include <mutex>

#define MAX_MEMORY_SIZE 1024

class MemoryAlloc;
//�ڴ��
class MemoryBlock
{
public:
	MemoryBlock();
	~MemoryBlock();
public:
	//�ڴ����
	int nID;

	//���ô���
	int nRef;

	//�������ڴ��
	MemoryAlloc *pAlloc;

	//��һ���λ��
	MemoryBlock* pNext;

	//�Ƿ����ڴ����
	bool bPool;
	

private:
	//Ԥ��
	char c1;
	char c2;
	char c3;
};

//�ڴ��
class MemoryAlloc
{
public:
	MemoryAlloc();
	~MemoryAlloc();

	//�����ڴ�
	void* allocMemory(size_t nSize);

	//�ͷ��ڴ�
	void freeMemory(void* p);

	//��ʼ��
	void initMemory();

protected:
	//�ڴ�ص�ַ
	char* _pBuf;
	//ͷ���ڴ浥Ԫ
	MemoryBlock* _pHeader;
	//�ڴ浥Ԫ�Ĵ�С
	size_t _nSize;
	//�ڴ浥Ԫ������
	size_t _nBlockSize;

	std::mutex _mutex;
};

//����������Ա����ʱ��ʼ��MemoryAlloc
template<size_t nSize,size_t nBloockSize>
class MemoryAlloctor:public MemoryAlloc
{
public:
	MemoryAlloctor();
};

//�ڴ������
class MemoryMgr
{
private:
	MemoryMgr();
	~MemoryMgr();
private:
	//��ʼ���ڴ��ӳ������
	void init_szAlloc(int nBegin, int nEnd, MemoryAlloc* pMemA);

public:
	static MemoryMgr& Instance();

	//�����ڴ�
	void* allocMem(size_t nSize);

	//�ͷ��ڴ�
	void freeMem(void* p);

	//�����ڴ�ص����ü���
	void addRef(void* pMem);
private:
	MemoryAlloctor<64,1000000> _mem64;
	MemoryAlloctor<128, 1000000> _mem128;
	MemoryAlloctor<256, 1000000> _mem256;
	MemoryAlloctor<512, 1000000> _mem512;
	MemoryAlloctor<1024, 1000000> _mem1024;
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
