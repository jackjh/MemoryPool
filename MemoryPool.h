#pragma once
#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <stdio.h>

#define _ALIGNMENT 4
#define DEFAULT_UNITSIZE 4
#define DEFAULT_INITSIZE 4096
#define DEFAULT_GROWSIZE 1024

typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

// 内存块
struct MemoryBlock {
	ushort _size;
	ushort numFree;
	ushort first;			// 存储当前内存块中第一个空闲单元的前两个字节作为编号
	MemoryBlock* next;
	char data[1];

	static void* operator new(size_t, ushort, ushort);
	static void operator delete(void* p, size_t);

	MemoryBlock(ushort n, ushort unitsize);
	~MemoryBlock(){ }
};

// 内存池
class MemoryPool {
public:
	MemoryPool(ushort _unitsize=DEFAULT_UNITSIZE, ushort _initsize=DEFAULT_INITSIZE, ushort _growsize=DEFAULT_GROWSIZE, bool _prealloc=false);
	~MemoryPool();
	void* Alloc();
	void Free(void* p);
private:
	ushort unitsize;
	ushort initsize;
	ushort growsize;
	bool prealloc;
	MemoryBlock* pMem;
};

#endif