
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

non-volatile cache algorithm.
A memory restoration supportable cache algorithm.

Features¶

A memory restoration supportable cache algorithm 

This algorithm library can 

- make cache memory dump file. 

- reload a cache after system(or binary) restart. 

- share cache with another process. 

Version¶

- 0.0.1 (beta) 2013.08 

Beta version is thread-unsafe. 

If you want to use this algorithm in multi-thread application, you have to add the lock (ex. criticalsection). 

Requirements¶

- Minimum supported Windows XP, Windows Server 2003 

kor¶

이 알고리즘 라이브러리는 

- 캐시 메모리를 파일 덤프로 남길 수 있습니다. 

- 시스템 혹은 바이너리가 재 가동 될때 캐시를 리로드 할 수 있습니다. 

- 다른 프로세스와 캐시를 공유할 수 있습니다. 
