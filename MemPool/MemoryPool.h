#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <stdio.h>

#define ALIGNMENT 4
#define DEFAULT_UNITSIZE 4096
#define DEFAULT_UNITNUMS 4

typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

// 内存块
struct MemoryBlock {
	ushort block_size;				// 内存块的大小
	ushort nums_free;				// 内存块中存储单元的数量
	ushort first_free_unit;			// 存储当前内存块中第一个空闲单元的前两个字节作为编号
	MemoryBlock* next;
	char data[1];					// 变长数组

	static void* operator new(size_t, ushort, ushort);
	static void operator delete(void* p);

	MemoryBlock(ushort n, ushort unitsize);
	~MemoryBlock(){ }
};

// 内存池
class MemoryPool {
public:
	MemoryPool(ushort unitsize_=DEFAULT_UNITSIZE, ushort unitnums_=DEFAULT_UNITNUMS, bool _prealloc=false);
	~MemoryPool();
	void* Alloc();
	void Free(void* p);
private:
	ushort unitsize;
	ushort unitnums;
	//ushort growsize;
	bool prealloc;
	MemoryBlock* pMem;
};

#endif