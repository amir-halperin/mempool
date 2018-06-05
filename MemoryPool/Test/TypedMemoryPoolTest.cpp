#include "../TypedMemoryPool.h"
#include "gtest/gtest.h"
#include <iostream>

// private shall not be called out of this module
namespace {

class Client
{
public:
	Client() : mData(0) {}
	Client(int data) : mData(data) {}
	~Client() {}

	int Get() {return mData;}

private:
	int mData;
};

typedef TypedMemoryPool<Client> ClientMemPool;

// pool configuration:
//	- create memory chunk in ctor
//	- don't allocate memory when no pool have no free blocks
//	- 1 block per chunk
// test second alloc return NULL
TEST(TypedMemoryPool, checkNoMemoryToAlloc)
{
ClientMemPool* mp = 0;
Client* clientP1 = 0;
Client* clientP2 = 0;

	// pool that manages Client objects configured with 1 object in the pool
	mp = new ClientMemPool(1);
	
	// expect that ctor failed to allocate memory
	EXPECT_EQ(true, mp->IsInitialized());
	// first alloc shall succeeded
	clientP1 = mp->Alloc();
	EXPECT_NE((void*)NULL, (void*)clientP1);
	// second allocation shall fail
	clientP2 = mp->Alloc();
	EXPECT_EQ((void*)NULL, (void*)clientP2);

	mp->Free(clientP1);

	// test cleanup
	delete mp;
}

}; // end of namespace MemoryPoolTesting
