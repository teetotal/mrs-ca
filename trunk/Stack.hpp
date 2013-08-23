/*
http://code.google.com/p/mrs-ca/

Copyright (c) 2013, James Jung (teetotal@gmail.com)
 All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
namespace MRSCA
{
	template <class c>
	class Stack :
		public MemoryAllocator
	{
	protected:
		struct Header
		{
			size_t mHeadIndex;
		};
		Header * mHeader;		
		
		MemoryAllocator * mMemoryAllocator;
		bool mIsLRU;

	public:
		struct StackNode : public c
		{
			size_t mId;
			size_t mNext;
			size_t mPre;
			bool mEnd;
		};
		StackNode * mTop;

		Stack(void)
		{};

		virtual ~Stack(void) 
		{};

		void Destroy(void)
		{
			DeleteMemoryMappedFile();
		};

		bool SetHead()
		{
			size_t index;
			mTop = (StackNode*)Alloc(index);
			if(mTop == NULL)
				return false;
			mTop->mEnd = true;
			mTop->mId = index;
			mHeader->mHeadIndex = index;			
			return true;
		};

		bool Reload(void)
		{
			if(!mIndex[mHeader->mHeadIndex].mIsAllocated)
			{
				if(!SetHead())
					return false;
			}
			else
				mTop = (StackNode*)GetMemory(mHeader->mHeadIndex);

			return true;
		};

		bool Initialize(size_t maxSegmentsCount, TCHAR * szCacheName, TCHAR * szSpecificPath)
		{
			return Initialize(maxSegmentsCount, szCacheName, szSpecificPath, false);
		};

		bool Initialize(size_t maxSegmentsCount, TCHAR * szCacheName, TCHAR * szSpecificPath, bool isLRU)
		{
			if(maxSegmentsCount < 1)
				return false;

			mIsLRU = isLRU;

			if(!Init(sizeof(Header), sizeof(StackNode), maxSegmentsCount, szCacheName, szSpecificPath))
				return false;
			
			mHeader = (Header * )mHeaderIndex;			

			if(mIsExistMemoryMappedFile)
			{
				if(!Reload())
					return false;
			}
			else
			{
				if(!SetHead())
					return false;
			}
			
			return true;
		};

		void Finalize(void)
		{
			Fin();
		};

		bool Push(c& node)
		{
			size_t index;
			StackNode * p = (StackNode*)Alloc(index);
			if(p == NULL)
			{
				if(mIsLRU)
				{
					size_t abandonedNode = FindLRUAbandonedNode();
					Remove(abandonedNode);
					p = (StackNode*)Alloc(index);
				}
				else
					return false;
			}

			::memcpy_s(p, sizeof(StackNode), &node, sizeof(c));
			p->mEnd = false;			
			p->mNext = mTop->mId;
			mTop->mPre = index;
			p->mId = index;
			mHeader->mHeadIndex = index;			
			mTop = p;
			return true;
		};
		void Pop()
		{			
			Free(mTop->mId);
			mTop = (StackNode *)GetMemory(mTop->mNext);
			mHeader->mHeadIndex = mTop->mId;			
		};

		c* Front()
		{
			size_t index;
			return Front(index);
		};

		c* Front(size_t & index)
		{
			if(mHeader->mHeadIndex != mTop->mId)
			{
				Reload();
			}
			if(mTop->mEnd)
				return NULL;
			index = mTop->mId;
			return (c*)mTop;
		};

		size_t Size()
		{
			return mAllocatorInfo->mAllocCount - 1;
		};

		c* GetNext(c* p)
		{
			size_t index;
			return GetNext(p, index);
		};

		c* GetNext(c* p, size_t& nodeIndex)
		{
			StackNode * node = (StackNode *)p;
			node = (StackNode * )GetMemory(node->mNext);
			if(node->mEnd)
				return NULL;

			nodeIndex = node->mId;
			return  (c*)node;
		};

		void Remove(size_t id)
		{			
			void * p = GetMemory(id);
			StackNode * node = (StackNode *)p;
			if(node->mEnd)
				return;

			if(node == mTop)
			{
				Pop();
				return;
			}

			StackNode * pNext	= (StackNode * )GetMemory(node->mNext);
			pNext->mPre = node->mPre;

			if(node != mTop)
			{
				StackNode * pPre	= (StackNode * )GetMemory(node->mPre);
				pPre->mNext = node->mNext;
			}
			
			size_t temp = mAllocatorInfo->mCurrentIndex;
			if(node->mId < mAllocatorInfo->mCurrentIndex )
				temp = node->mId;
			Free(node->mId);

			mAllocatorInfo->mCurrentIndex = temp;
			
		};
	};
}