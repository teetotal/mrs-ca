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
namespace MRSCA{
#define BIT32	4
#define BIT64	8
#define MAX_ALLOC_SIZE_32	( 0x7FFEFFFF - 0x00010000 ) //User partition size (32bit os)
#define MAX_REF_COUNT	255
		
	struct _IndexInfo
	{
		bool mIsAllocated;
		unsigned char mRefCount;
	};

	class MemoryAllocator
	{		
	public:		
		MemoryAllocator(void);
		virtual ~MemoryAllocator(void);				
		_IndexInfo * GetIndexInfo(void)
		{
			return mIndex;
		};

	protected:
		struct _AllocatorInfo
		{
			size_t mSegmentSize, mMaxSegmentsCount;		
			DWORD mPageSize;
			unsigned __int64 mCountOfPage, mTotalAllocSize;
			size_t mCurrentIndex;
			size_t mHeaderSize;
			size_t mAllocCount;
		};		

		bool Init(size_t headerSize, size_t segmentSize, size_t maxSegmentsCount, TCHAR * szCacheName, TCHAR * szSpecificPath);
		void Fin();
		void * Alloc();
		void * Alloc(size_t & index);
		void Free(size_t index);
		bool IsExistMemoryMappedFile();
		bool SetSharedMemory(void ** p, INT64 totalAllocSize);
		void * GetMemory(size_t index);		
		void DeleteMemoryMappedFile(void);
		size_t FindLRUAbandonedNode(void);

		void * mAllocIndex, *mDataIndex, *mHeaderIndex;
		_IndexInfo * mIndex;		
		HANDLE	mFileHandle, mMemoryMapHandle;		
		TCHAR mCacheName[MAX_PATH];
		TCHAR mSpecificPath[MAX_PATH];		
		volatile _AllocatorInfo * mAllocatorInfo;
		bool mIsExistMemoryMappedFile;		
	};


}