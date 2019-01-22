#include <iostream>
#include "MemoryPool.h"

using namespace std;

/*
** 内存块类构造函数
** 参数n: 内存块中内存单元的数量，unitsize_: 每个内存单元的大小
*/
MemoryBlock::MemoryBlock(ushort n, ushort unitsize_) : 
				block_size(n * unitsize_), 
				nums_free(n - 1), 
				first_free_unit(1), 
				next(NULL) {
	char* pdata = data;
	/*
	**将内存块中的每个存储单元的前两个字节初始化为下一个存储单元的编号，最后一个存储单元不设值
	*/
	for (ushort i = 1; i < n; i++) {
		*(ushort *)pdata = i;
		pdata += unitsize_;
	}
}

/*
** MemoryBlock中的new操作符重载
** 参数size_: 内存块中存储单元的数量，unitsize_: 存储单元的大小
*/
void* MemoryBlock::operator new(size_t, ushort size_, ushort unitsize_) {
	return ::operator new(sizeof(MemoryBlock) + size_ * unitsize_);
}

/*
** MemoryBlock中的delete操作符重载
** 参数: 将要释放的指针
*/
void MemoryBlock::operator delete(void* p) {
	::operator delete(p);
}

/*
** 内存池构造函数
** 参数: unitsize_: 内存块中的存储单元大小，initsize_: 初始化分配的大小（存储单元数量），prealloc_: 预分配标志
*/
MemoryPool::MemoryPool(ushort unitsize_, ushort unitnums_, bool prealloc_) : 
				pMem(NULL), 
				unitsize(unitsize_), 
				unitnums(unitnums_), 
				prealloc(prealloc_) {
	// 对unitsize进行调整
	if (unitsize_ > 4) {
		unitsize = (unitsize_ + ALIGNMENT - 1) &~(ALIGNMENT - 1);	// 将_unitsize调整至4(_ALIGNMENT)的倍数
	}
	else if (unitsize_ <= 2) {
		unitsize = 2;
	}
	else {
		unitsize = 4;
	}

	// 根据预分配标志是否进行预分配内存
	if (prealloc_) {
		Alloc();
		pMem->first_free_unit = 0;
		pMem->nums_free = unitnums;
	}
}

/*
** 内存池析构函数，清除内存池所分配的所有内存块
*/
MemoryPool::~MemoryPool() {
	while (pMem) {
		MemoryBlock* ptmp = pMem;
		pMem = pMem->next;
		delete ptmp;
	}
}

/*
** 功能：内存池分配函数，分配大小固定的内存单元
** 返回值：分配内存的指针
*/
void* MemoryPool::Alloc() {
	if (!pMem) {	// 尚未分配内存块时
		pMem = new(unitnums, unitsize) MemoryBlock(unitnums, unitsize);
		if (!pMem)
			return NULL;
		return pMem->data;	// 返回新开内存块的第一个存储单元的指针
	}

	MemoryBlock* ptmp = pMem;

	// 当前内存块ptmp无空闲单元，跳到下一块
	while (ptmp && !ptmp->nums_free) {
		ptmp = ptmp->next;
	}

	if (ptmp) {	// 找到内存块——当前内存块非空且有空闲单元
		char* pfree = ptmp->data + ptmp->first_free_unit * unitsize;
		ptmp->first_free_unit = *(ushort *)pfree;	// 更行内存块中first的值为pfree的前两个字节，即下一个空闲单元
		ptmp->nums_free--;
		return (void *)pfree;	// 返回内存块中第一个空闲单元的指针
	}
	else {	// 之前的几块内存块都满了，需要重新开一个MemoryBlock
		ptmp = new(unitnums, unitsize) MemoryBlock(unitnums, unitsize);

		if (!ptmp)
			return NULL;

		// 将新开的内存块放在链表的开头，提高下次分配的效率
		ptmp->next = pMem;
		pMem = ptmp;
		return (void *)ptmp->data;
	}
}

/*
** 功能：内存池释放内存函数
** 参数：待释放内存单元的指针
*/
void MemoryPool::Free(void* pfree) {
	MemoryBlock* prev = pMem;
	MemoryBlock* ptmp = pMem;

	// 找到属于哪个内存块
	while (ptmp && ((ulong)pfree < (ulong)ptmp || (ulong)pfree >= (ulong)(ptmp->data) + ptmp->block_size)) {
		prev = ptmp;
		ptmp = ptmp->next;
	}

	if (!ptmp)
		return;

	ptmp->nums_free++;	// 空闲单元数量++

	*(ushort *)pfree = ptmp->first_free_unit;	// 将当前内存块中的空闲单元的编号赋给pfree

	// 根据pfree在内存块中的偏移，计算first的位置
	ptmp->first_free_unit = ((ulong)pfree - (ulong)ptmp->data) / unitsize;

	if (ptmp->nums_free * unitsize == ptmp->block_size) {	// 如果找到的内存块全是空闲单元的情况下，则删除该内存块
		if (ptmp == pMem) {	// 在链表头
			pMem = pMem->next;
			delete ptmp;
		}
		else {
			prev->next = ptmp->next;
			delete ptmp;
		}
	}
	else {
		if (ptmp != pMem) {	// 不是链表头节点的情况下，需要把当前内存块节点放到头节点，否则不需要动
			prev->next = ptmp->next;
			ptmp->next = pMem;
			pMem = ptmp;
		}
	}
}