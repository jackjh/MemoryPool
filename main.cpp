#include "MemoryPool.h"
#include <iostream>
#include <queue>
#include <time.h>

#define TEST_MEM

using namespace std;

class Test {
public:
	Test() :val(0), cnt(0) { }
	~Test() { }
#ifdef TEST_MEM
	static void* operator new(size_t) {
		if (mPool == NULL)
			return NULL;

		return mPool->Alloc();
	}
	static void operator delete(void* p, size_t) {
		mPool->Free(p);
	}
	static MemoryPool* mPool;
#endif

private:
	int val;
	int cnt;
};

#ifdef TEST_MEM
MemoryPool* Test::mPool = new MemoryPool(sizeof(Test), 1024, 512, false);
#endif // TEST_MEM

int main() {
	queue<Test*> q;
	time_t start;
	time(&start);
#ifdef TEST_MEM
	for (int i = 0; i < 3000; i++) {
		for (int j = 0; j < 3000; j++) {
			q.push(new Test());
		}
		while (!q.empty()) {
			delete q.front();
			q.pop();
		}
	}
	cout << "using MemoryPool..." << endl;	// 21s
#else
	for (int i = 0; i < 3000; i++) {
		for (int j = 0; j < 3000; j++) {
			q.push(new Test());
		}
		while (!q.empty()) {
			delete q.front();
			q.pop();
		}
	}
	cout << "using standard new..." << endl;	// 23s
#endif
	time_t end;
	time(&end);

	cout << "start time: " << start << endl;
	cout << "end time: " << end << endl;
	cout << "total time: " << end - start << endl;

	return 0;
}