#include "../MemoryPool.h"
#include "gtest/gtest.h"

// private shall not be called out of this module
namespace {

class Client
{
public:
	Client() : mData(0) {}
	Client(int data) : mData(data) {}
	int Get() {return mData;}

	// initialized internal private MemoryPool
	// returns false in case mem pool creation fails, and true otherwise.
	static bool InitPrivateMemPool(int numOfClient, bool allocOnDemand, bool supportRealloc)
	{
		msPrivateMemPool = new MemoryPool(sizeof(Client), numOfClient, allocOnDemand, supportRealloc);
		if(msPrivateMemPool == 0) 
		{
			return false;
		}
		
		return msPrivateMemPool->IsInitialized();
	}
	// wrap new Client to alloc memory from internal private pool
	void* operator new(size_t)
	{
		return msPrivateMemPool->Alloc();
	}
	// wrap delete Client to free memory to internal private pool
    void operator delete(void* p)
	{
		msPrivateMemPool->Free(p);
	}

private:
	int mData;

	static MemoryPool* msPrivateMemPool;
};

// static member initialiazion
MemoryPool* Client::msPrivateMemPool = 0;

// pool configuration:
//	- create memory chunk in ctor
//	- don't allocate memory when no pool have no free blocks
//	- 1 block per chunk
// test second alloc return NULL
TEST(MemoryPool, checkNoMemoryToAlloc)
{
MemoryPool* mp = 0;
void* ptr = 0;

	// pool that manages 4 bytes blocks, with 1 block free
	mp = new MemoryPool(4, 1);
	
	// expect that ctor failed to allocate memory
	EXPECT_EQ(true, mp->IsInitialized());
	// first alloc shall succeeded
	ptr = mp->Alloc();
	EXPECT_NE((void*)NULL, (void*)ptr);
	// second allocation shall fail
	ptr = mp->Alloc();
	EXPECT_EQ((void*)NULL, (void*)ptr);

	// test cleanup
	delete mp;
}

// pool configuration:
//	- create memory chunk in ctor
//	- allocate memory when no pool have no free blocks
//	- 1 block per chunk
// test second alloc return valid pointer
TEST(MemoryPool, checkRealloc)
{
MemoryPool* mp = 0;
void* ptr = 0;

	// pool that manages 4 bytes blocks, with 1 block free
	mp = new MemoryPool(sizeof(int), 1, false, true);
	
	// expect that ctor failed to allocate memory
	EXPECT_EQ(true, mp->IsInitialized());
	// first alloc shall succeeded
	ptr = mp->Alloc();
	EXPECT_NE((void*)NULL, (void*)ptr);
	// second allocation shall trigger internal reallocation
	ptr = mp->Alloc();
	EXPECT_NE((void*)NULL, (void*)ptr);

	// test cleanup
	delete mp;
}

// test pool have no memory to allocat
TEST(MemoryPool, poolFailToAllow)
{
MemoryPool* mp = 0;
Client* clientP = 0;

	// create pool with 50 elements per chunk, first chunk is created on demand, no dynamic chunks allocation
	Client::InitPrivateMemPool(50, true, false);
	clientP = new Client(123);
	EXPECT_EQ((void*)NULL, (void*)clientP);
}

TEST(MemoryPool, alloc)
{
MemoryPool* mp = 0;
Client* clientP = 0;

	// create pool with 50 elements per chunk, first chunk is created on demand, no dynamic chunks allocation
	EXPECT_EQ(true, Client::InitPrivateMemPool(50, false, false));
	clientP = new Client(123);
	EXPECT_NE((void*)NULL, (void*)clientP);
	delete clientP;
}



}; // end of namespace MemoryPoolTesting
