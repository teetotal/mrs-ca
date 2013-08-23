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
#include "pch.h"

namespace MRSCA
{
	MemoryAllocator::MemoryAllocator(void) : 		
		mIndex(NULL),
		mAllocIndex(NULL), 
		mDataIndex(NULL), 
		mFileHandle(INVALID_HANDLE_VALUE),
		mMemoryMapHandle(INVALID_HANDLE_VALUE),
		mAllocatorInfo(NULL),
		mIsExistMemoryMappedFile(false)
	{		
		mCacheName[0]		= 0;
		mSpecificPath[0]	= 0;
	}


	MemoryAllocator::~MemoryAllocator(void)
	{
	}

	bool MemoryAllocator::Init(size_t headerSize, size_t segmentSize, size_t maxSegmentsCount, TCHAR * szCacheName, TCHAR * szSpecificPath)
	{		
		_tcscpy_s(mCacheName, _countof(mCacheName), szCacheName);
		_tcscpy_s(mSpecificPath, _countof(mSpecificPath), szSpecificPath);

		if(IsExistMemoryMappedFile())
		{
			DWORD read = 0; 
			char buffer[sizeof(_AllocatorInfo)] = {0};
			if(::ReadFile(mFileHandle, buffer, sizeof(_AllocatorInfo), &read, NULL) == FALSE)
			{
				Fin();
				return false;
			}
			
			if(!SetSharedMemory(&mAllocIndex, ((_AllocatorInfo * )buffer)->mTotalAllocSize))
			{
				DeleteMemoryMappedFile();
				return false;
			}

			mAllocatorInfo = (_AllocatorInfo * )mAllocIndex;
			mHeaderIndex = (char*)mAllocIndex + sizeof(_AllocatorInfo);
			mIndex = (_IndexInfo*)((char*)mHeaderIndex + mAllocatorInfo->mHeaderSize);
			mDataIndex = (char*)mIndex + (mAllocatorInfo->mMaxSegmentsCount * sizeof(_IndexInfo));

		}
		else
		{
			_AllocatorInfo AllocatorInfo; //temp variable
			::memset(&AllocatorInfo, 0, sizeof(_AllocatorInfo));
			SYSTEM_INFO info;
			GetSystemInfo(&info);
			AllocatorInfo.mPageSize = info.dwPageSize;
			AllocatorInfo.mHeaderSize = headerSize;

			/*
			Memory Map
			-----------------------------------------
			|_AllocatorInfo | Header | Index | Data |
			-----------------------------------------
			*/
			INT64 nTotal = (segmentSize * maxSegmentsCount) + sizeof(_AllocatorInfo) + AllocatorInfo.mHeaderSize; //Total memory alloc size
			size_t indexSize = maxSegmentsCount * sizeof(_IndexInfo);				//Index Area
		
			AllocatorInfo.mCountOfPage = (nTotal + indexSize) / AllocatorInfo.mPageSize;	// Count of SystemPage
			if(((nTotal + indexSize) % AllocatorInfo.mPageSize) != 0 )
				AllocatorInfo.mCountOfPage ++;		
		
			AllocatorInfo.mTotalAllocSize  = (AllocatorInfo.mCountOfPage  * AllocatorInfo.mPageSize);	//Alloc size	

			switch(sizeof(LONG_PTR))
			{
			case (BIT32):	
				//32bit
				if(AllocatorInfo.mTotalAllocSize > MAX_ALLOC_SIZE_32)
					return false;
				break;
			case (BIT64): 
				//64bit
				break;
			default:
				return false;
			}
		
			AllocatorInfo.mSegmentSize		= segmentSize;
			AllocatorInfo.mMaxSegmentsCount	= maxSegmentsCount;		

			if(!SetSharedMemory(&mAllocIndex, AllocatorInfo.mTotalAllocSize))
				return false;
			
			::memcpy_s(mAllocIndex, sizeof(_AllocatorInfo), &AllocatorInfo, sizeof(_AllocatorInfo));			
			mAllocatorInfo = (_AllocatorInfo * )mAllocIndex;
			mHeaderIndex = (char*)mAllocIndex + sizeof(_AllocatorInfo);
			mIndex = (_IndexInfo*)((char*)mHeaderIndex + mAllocatorInfo->mHeaderSize);
			::memset(mIndex , 0, indexSize);
			mDataIndex = (char*)mIndex + indexSize;			
		}
		return true;
	}

	bool MemoryAllocator::IsExistMemoryMappedFile()
	{
		TCHAR path[MAX_PATH] = {0};
		_stprintf_s(path, _countof(path), TEXT("%s\\%s"), mSpecificPath, mCacheName);

		mFileHandle = CreateFile(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(mFileHandle == INVALID_HANDLE_VALUE)
		{
			//assert();			
		}

		if(::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			mIsExistMemoryMappedFile = true;
			return true;
		}
		
		return false;
	}

	bool MemoryAllocator::SetSharedMemory(void ** p, INT64 totalAllocSize)
	{
		if(totalAllocSize <= 0)
			return false;

		DWORD size[2];
		::memcpy_s(size, sizeof(size), &totalAllocSize, sizeof(totalAllocSize));
		
		TCHAR sz[MAX_PATH] = {0,};
		_stprintf_s(sz, _countof(sz), TEXT("Global\\%s"), mCacheName);
		if(mIsExistMemoryMappedFile)
		{
			mMemoryMapHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sz);
			if(mMemoryMapHandle == NULL)
				mMemoryMapHandle = CreateFileMapping(mFileHandle, NULL, PAGE_READWRITE, size[1], size[0], sz);//windows is little-endian				
		}
		else
			mMemoryMapHandle = CreateFileMapping(mFileHandle, NULL, PAGE_READWRITE, size[1], size[0], sz);//windows is little-endian
		
		if(mMemoryMapHandle == NULL)
			return false;
		
		*p = MapViewOfFile(mMemoryMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if(*p == NULL)		
			return false;

		return true;
	}

	void MemoryAllocator::Fin()
	{		
		if(mAllocIndex)
			UnmapViewOfFile(mAllocIndex);
		if(mMemoryMapHandle != NULL && mMemoryMapHandle != INVALID_HANDLE_VALUE)
			CloseHandle(mMemoryMapHandle);
		if(mFileHandle != NULL && mFileHandle != INVALID_HANDLE_VALUE)
			CloseHandle(mFileHandle);

		mAllocIndex			= NULL;
		mMemoryMapHandle	= INVALID_HANDLE_VALUE;
		mFileHandle			= INVALID_HANDLE_VALUE;
	}

	void MemoryAllocator::DeleteMemoryMappedFile(void)
	{		
		if(mCacheName[0] == 0)
			return;

		TCHAR szPath[MAX_PATH];
		_stprintf_s(szPath, _countof(szPath), TEXT("%s\\%s"), mSpecificPath, mCacheName);
		Fin();
		::DeleteFile(szPath);
	}

	void * MemoryAllocator::GetMemory(size_t index)
	{
		return (void*)((char*)mDataIndex + (index * mAllocatorInfo->mSegmentSize));
	}

	void * MemoryAllocator::Alloc()
	{		
		size_t index;
		return Alloc(index);
	}

	size_t MemoryAllocator::FindLRUAbandonedNode()
	{
		size_t abandonedNode = 1;
		unsigned char ref = mIndex[abandonedNode].mRefCount;
		for(size_t n = 2; n < mAllocatorInfo->mMaxSegmentsCount; n++)
		{
			if(ref > mIndex[n].mRefCount)
			{
				abandonedNode = n;
				ref = mIndex[n].mRefCount;
			}
		}
		return abandonedNode;
		
	};

	void * MemoryAllocator::Alloc(size_t & index)
	{
		if(mAllocatorInfo->mAllocCount >= mAllocatorInfo->mMaxSegmentsCount)
			return NULL;				
		else
		{
			while(mIndex[mAllocatorInfo->mCurrentIndex].mIsAllocated)					
				mAllocatorInfo->mCurrentIndex++;			
		}
		
		mIndex[mAllocatorInfo->mCurrentIndex].mIsAllocated = true;
		mIndex[mAllocatorInfo->mCurrentIndex].mRefCount++;
		void * p = (char*)mDataIndex + (mAllocatorInfo->mCurrentIndex * mAllocatorInfo->mSegmentSize);
		index = mAllocatorInfo->mCurrentIndex;
		mAllocatorInfo->mCurrentIndex++;	
		mAllocatorInfo->mAllocCount++;
		return p;		
	}

	void MemoryAllocator::Free(size_t index)
	{
		mIndex[index].mIsAllocated	= false;
		mIndex[index].mRefCount		= 0;
		mAllocatorInfo->mCurrentIndex = index;
		mAllocatorInfo->mAllocCount--;
	}
}