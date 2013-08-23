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
namespace MRSCA
{
	class _HashData
	{
	public:		
		virtual _int64 GetKey(void) = 0;
	};

	template <class c>
	class Hashmap :
		public MemoryAllocator
	{
#define HASHMAP_STACK_NAMING	TEXT("%d.%s")
	protected:
		Stack<c>* mHashtable;
		size_t mHashSize; 		
	public :
		Hashmap(void){};
		virtual ~Hashmap(void){};

		bool Initialize(size_t hashSize, size_t maxSegmentsCount, TCHAR * szCacheName, TCHAR * szSpecificPath, bool isLRU)
		{
			mHashSize = hashSize;
			mHashtable = new Stack<c>[mHashSize];

			for(size_t n = 0; n < mHashSize; n++)
			{
				TCHAR sz[MAX_PATH] = {0,};
				_stprintf_s(sz, _countof(sz), HASHMAP_STACK_NAMING, n, szCacheName);
				if(!mHashtable[n].Initialize(maxSegmentsCount, sz, szSpecificPath, isLRU))
				{
					Finalize();
					return false;				
				}
			}
			return true;
		};

		void Finalize()
		{
			for(size_t n = 0; n < mHashSize; n++)
			{
				mHashtable[n].Finalize();			
			}

			delete[] mHashtable;
			
		};

		Stack<c> * GetStack(size_t stackIndex)
		{
			if(stackIndex > mHashSize -1)			
				return NULL;
			
			return &mHashtable[stackIndex];
		};

		size_t GetHashKey(_int64 key)
		{			
			return ((size_t)key % mHashSize); // simple way			
			/*
			//more complicated way
			//key = key ^ 0;
			//return ((size_t)key % mHashSize);
			*/
		};

		size_t Size()
		{
			size_t sum = 0;
			for(int n=0; n < mHashSize; n++)
			{
				sum += mHashtable[n].Size();
			}
			return sum;
		};

		bool Insert(c & data)
		{			
			if(!mHashtable[GetHashKey(data.GetKey())].Push(data))
				return false;

			
			return true;
		};	

		bool Find(_int64 key, c & pRet)
		{
			size_t index;
			return Find(key, pRet, index);
		};

		bool Find(_int64 key, c & pRet, size_t & nodeIndex)
		{
			size_t hashkey = GetHashKey(key);
			
			c * p = mHashtable[hashkey].Front(nodeIndex);
			for(size_t n = 0; n < mHashtable[hashkey].Size(); n++)
			{
				if(p==NULL)
					return false;

				char* dst = (char*)&pRet + sizeof(_HashData);
				char* src = (char*)p + sizeof(_HashData);
				size_t size = sizeof(c) - sizeof(_HashData);

				::memcpy_s(dst, size, src, size);
				if(pRet.GetKey() == key)
				{
					if(mHashtable[hashkey].GetIndexInfo()[nodeIndex].mRefCount < MAX_REF_COUNT)
						mHashtable[hashkey].GetIndexInfo()[nodeIndex].mRefCount++;
					return true;
				}
				
				p = mHashtable[hashkey].GetNext(p, nodeIndex);
			}
			return false;
		};

		c * Find(_int64 key)
		{
			size_t hashkey = GetHashKey(key);
			size_t nodeIndex = 0;
			c * p = mHashtable[hashkey].Front(nodeIndex);
			c pRet;
			for(size_t n = 0; n < mHashtable[hashkey].Size(); n++)
			{
				if(p==NULL)
					return NULL;

				char* dst = (char*)&pRet + sizeof(_HashData);
				char* src = (char*)p + sizeof(_HashData);
				size_t size = sizeof(c) - sizeof(_HashData);

				::memcpy_s(dst, size, src, size);
				if(pRet.GetKey() == key)
				{
					if(mHashtable[hashkey].GetIndexInfo()[nodeIndex].mRefCount < MAX_REF_COUNT)
						mHashtable[hashkey].GetIndexInfo()[nodeIndex].mRefCount++;
					return p;
				}

				p = mHashtable[hashkey].GetNext(p, nodeIndex);
			}
			return NULL;
		};

		void Remove(_int64 key)
		{
			c temp;
			size_t index;
			if(!Find(key, temp, index))
				return;

			size_t hashkey = GetHashKey(key);
			mHashtable[hashkey].Remove(index);
		};

		void Destroy(void)
		{
			for(size_t n = 0; n < mHashSize; n++)
			{							
				mHashtable[n].Destroy();
			}

			delete[] mHashtable;
			
		};
	};
}