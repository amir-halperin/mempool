#include "MemoryPool.h"
#include <stdio.h>

// public
// Construct the the memory pool according to user inputs
// Makes sure that minimal memory block >= 4 bytes.
// Makes sure memory blocked size is alignment to 4 bytes.
// Call to internal method to initialize first chunk of memory blocks.
MemoryPool::MemoryPool(uint objsize, uint nobjs, bool toAllocateAtFirstNew, bool toReAllocateWhenMemFull)
	: mBlockSize(sizeof(MemLink) > objsize ? sizeof(MemLink) : objsize),
      mNumOfBlocksPerChunk(nobjs),
	  mHeadChunk(0),
	  mNextMemBlock(0),
      mToAllocateAtFirstNew(toAllocateAtFirstNew),
	  mToReAllocateWhenMemFull(toReAllocateWhenMemFull),
	  mIsInitializedProperly(false)
{
	// enforce alignment to 32bit address
	if( mBlockSize % 4 != 0 )
	{
		mBlockSize += 4 - (mBlockSize % 4);
	}
	// user request to allocate the memory block upon creation of the memory pool.
	if (mToAllocateAtFirstNew == false)
	{
		// allocate and initialized first memory chunk
		// if alloc failed false is returned i.e. mIsInitializedProperly == false
		mIsInitializedProperly = AllocMemoryChunk();
	} // end if (mToAllocateAtFirstNew == false)
	else
	{
		// this flag is used to indicate whether ctor completed successfuly or not.
		// ctor can fail only if AlloMemoryChunk() failes.
		// in that case it is not activated therefore ctor is succeeded by default.
		mIsInitializedProperly = true;
	}
}

// public
// dealloactes all memory chunks dynamically allocated during the life cycle of this object.
MemoryPool::~MemoryPool()
{
	// iterate all chunks in list
	while (mHeadChunk) 
	{
    	Chunk *c = mHeadChunk;
		// memory blocks per chunk were allocated using new[]
		// delete memory blocks vector
		delete [] mHeadChunk->chunkBuff;
		// iterate to the next chunk
		mHeadChunk = mHeadChunk->next;
		// delete chunk structure
		delete c;
    }
}

// public
bool MemoryPool::IsInitialized()
{
	return mIsInitializedProperly;
}

// public
// alloc single memory block from a memory chunk
void* MemoryPool::Alloc()
{
	// all memory blocks in this memory chunk runs out
	if (mNextMemBlock == 0)
	{
		// in case pool was configured with dynamic chunks allocation
		if (mToReAllocateWhenMemFull == true)
		{
			// allocate and initialized additional memory chunk to extend the pool
			AllocMemoryChunk();
			if(mNextMemBlock == 0)
			{
			  // Out of memory after Realloc is generated!!!
			  return 0;
			}
		}
		else
		{
			// Out of memory
			return 0;
		}
	}
	// cast to get the available memory block, need to update the embedded link list of available blocks.
	MemLink *link = (MemLink*)mNextMemBlock;
	// save the current available memory block to be delivered to the caller
	char* tmpBlock = mNextMemBlock;
	// points the address of the next available memory block to be the first element in the embedded link list
	mNextMemBlock = (char*)link->next;
// return pointer to available memory block
return tmpBlock; 
}

// public
// free an object linked it into the block
void MemoryPool::Free(void *myBlock)
{
	// cast to update the embedded link list of available blocks.
	MemLink *link = (MemLink*)myBlock;
	// retured memory block is points to the available memory blocks link list
	link->next = (MemLink*)mNextMemBlock;
	// the returned memory block becomes next available block in the embedded link list
    mNextMemBlock = (char*)myBlock;
}

// private
// allocate new memory chunk.
// return true if allocation succeeded and false otherwise.
bool MemoryPool::AllocMemoryChunk()
{
// allocate memory chunk
	mNextMemBlock = new char[mBlockSize * mNumOfBlocksPerChunk];
	// memory chunk allocation failed
	if(mNextMemBlock == 0)
	{
		return false;
	}
	// memory chunk allocation succeeded, continue with memory blocks initialization
	char* blockMem = mNextMemBlock;
	MemLink *link;
	// transform the flat memory chunk into a linked list of memory blocks
	for (uint i=0; i<mNumOfBlocksPerChunk; i++) 
	{
		link = (MemLink*)blockMem;
		// in case this is the last memory block in the chunk
		if (i == mNumOfBlocksPerChunk -1)
		{
			// last memory block in the linked list shall point to NULL
			link->next = (MemLink*)0;
		}
		else // in case this memory block is not the last in the chunk
		{
			// the next available mem blocks points to the start address of the allocated memory chunk
			link->next = (MemLink*)(blockMem + mBlockSize);
		}
		// move on to the next memory block in the chunk
		blockMem += mBlockSize;
	}// end for
	// concatenate memory chunk into the chunk list
	if(AddMemoryChunk(mNextMemBlock) == false)
	{
		// in case chunk addition fails delete chunk memory and indicate failure
		delete[]mNextMemBlock;
		return false;
	}

// memory chunk allocation succeeded
return true;
}

// private
// addMemBlock - adds a mem block onto the chain
bool MemoryPool::AddMemoryChunk(void *newChunkBuff)
{
// create new chunk that points to the new allocated memory chunk
Chunk *cp = new Chunk(newChunkBuff);

	// failed to allocate memory
	if(cp == 0) {
		return false;
	}
	// concatenate new memory chunk into the chunk list
	cp->next = mHeadChunk;
	mHeadChunk = cp;

return true;
}