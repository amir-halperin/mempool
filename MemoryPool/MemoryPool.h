#pragma once

typedef unsigned int uint;

/**
  @brief Manages memory in fixed sized blocks.
  Not safe for multithreading.
  Object is not copyable.

  @Auther: Amir Halperin

  Usage example:

  struct MyType
  {
	int a;
	char b;
  }

  // create memory pool to maintain dynamic memory blocks for MyStruct elements.
  // Create pool with 50 elements per chunk.
  // Pool don't extends its memory during runtime, thus after 50 allocated elements no memory will provided to client.
  MemoryPool* new myPool(siezof(MyType), 50, true, false);
  // verify pool initialized properly.
  if(myPool.IsInitialized() == false)
  {
	delete myPool;
	// perform additional recovery actions ...
  }
  // get memory block
  void* p = myPool->Alloc();
  MyType* myTypeP = (MyType*)p;
  myType->a = 10;
  myType->b = 20;
  // return memory block
  myPool->Free(myTypeP);

  delete myPool;
*/
class MemoryPool 
{
public:

	/**
	  @brief ctor to create memory pool.
	  Allocate memory internaly according to user provided parameters.
	  @param objsize the size of each pool memory block (all blocks are from the same size).
	  @param noobj the number of memory blocks in each pool's memory chunks.
	  @param toAllowAtFirstNew boolean to configure whether internal pool structure is initialized on first demand.
	  @param toReAllocWhenMemFull boolean to configure whether to allocate additional memory when pool is empty.
	  MemoryPool client is reponsible to 
	*/
	MemoryPool(uint objsize, uint nobjs, bool toAllocateAtFirstNew = false, bool toReAllocateWhenMemFull = false);
	/**
	 @brief dtor frees all memory chunked allocated during life cycle of this object
	*/
    ~MemoryPool();
	/**
	  @brief Check whether construction of MemoryPool object completed successfuly.
	  @return true if ctor completes OK, and false otherwise.
	  In case false is returned, client is reponsible for roleback operation e.g. activate MemoryPool dtor.
	*/
	bool IsInitialized();
	/**
	  @brief allocate memory block.
	  Size of memory blocks is fixed and determind during pool creation.
	  @return pointer to memory block. Pointer is zero in case allocation fails.
	*/
	void * Alloc();
	/**
	  @brief return memory block to the pool.
	  @param p a pointer of pool's memory block.
	*/
	void Free(void *p);

private:

	// blocked operators
	MemoryPool();
	MemoryPool(const MemoryPool& other);
	MemoryPool& operator=(const MemoryPool& other);

	// Memory chunk that is sliced into memory blocks.
	// Chunks are dynamically allocated internaly by MemoryPool object.
	// Number of memory chunks can vary between 1..n depends on user policy.
	// Chunks are maintained as linked list.
	typedef struct TChunk 
	{
		void  *chunkBuff;	// pointer to memory chunk
		TChunk *next;       // next memory chunk (will always be zero if toReAllocateWhenMemFull == false
		TChunk(void *mp) : chunkBuff(mp), next(0) {}
	} Chunk;

	// Pool element.
	// Each element points to the next available memory blocks in the pool.
	typedef struct TMemLink{
		TMemLink *next;
	} MemLink;

    // Allocate new memory chunk and concatenate it into internal chunk's list.
	// Use AddMemoryChunk() to perform the chunk concatenation procedure.
	// Return false in case chunk allocation, or concatenation opeation failed.
	bool AllocMemoryChunk();
	// Concatenate new allocated memory chunk into a link list.
	// Reuturn false in case memory allocation of chunk's list element failed.
	bool AddMemoryChunk(void *mptr);

    // data members
	Chunk*     mHeadChunk;               // the first chunk
	char*      mNextMemBlock;            // next available pool element
	uint       mBlockSize;	             // the block sizes of this allocator
    const uint mNumOfBlocksPerChunk;     // number of blocks per chunk 
    bool       mToAllocateAtFirstNew;    // whether to allocate first chunk on demand
	bool       mToReAllocateWhenMemFull; // whether to allocate another chunk when no more memory blocks available
	bool       mIsInitializedProperly;   // flag that indicates whether ctor completed successfully
};





