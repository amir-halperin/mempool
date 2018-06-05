#pragma once

#include "MemoryPool.h"

/**
  @brief Template class that provide memory pool for specific type of class.
  This class enforce matching between class type the pool manages and pointer types that are passed
  via Alloc and Free operations.
 */
template<class T>
class TypedMemoryPool : public MemoryPool
{
public:
	TypedMemoryPool(unsigned int numOfObjs) : MemoryPool(sizeof(T), numOfObjs, false, false) {}
	~TypedMemoryPool(void) {}

	/**
		@brief Allocate memory according the sizet of class and activate default ctore.
		@return a pointer to a T object.
	*/
	T* Alloc() 
	{
		// Allocate memory buffer for T
		void* p = MemoryPool::Alloc();
		// placement new to activate T ctor on buffer p
		return new (p) T();
	} 
	/**
		@brief Activate dtor of received T object and return its memory to the pool.
		@param Pointer to T object.
	*/
	void Free(T* p) 
	{
		// call T dtor
		p->~T();
		// return its memory buffer to the pool
		MemoryPool::Free((void*)p);
	}
};