
#include "pch.h"
using namespace MRSCA;

struct st
{
	char sz[100];
};

class hashdata : public _HashData
{
public :
	_int64 key;
	char value[100];
	virtual _int64 GetKey(void)
	{
		return key;
	};	
};

int StackSample(size_t maxStackSize)
{
	Stack<st> stack;
	if(!stack.Initialize(maxStackSize, L"stack", L"D:\\Dev"))
	{		
		printf("Check your permission. Error = %d", ::GetLastError());
		return 0;
	}
	st * p;
	st temp;
	int nRemoveIndex = 0;	
	printf("i : push, d : pop, l : show all, s : size, q : quit, k :  destroy, r : remove \r\n");
	while(true)
	{	
		char c;
		printf(">>");
		scanf_s("%c", &c);
		switch(c)
		{
		case 'r':
			printf("remove index = ");
			scanf("%d", &nRemoveIndex);
			size_t id;
			p = stack.Front(id);
			for(int n = 0; n < stack.Size(); n++)
			{
				if(p==NULL)
					break;
				if(n == nRemoveIndex)
				{
					printf("value : %s \r\n", p->sz);
					stack.Remove(id);
				}
				p = stack.GetNext(p, id);
			}
			break;
		case 'k':
			stack.Destroy();
			goto q;			
		case 's':
			printf("size = %d \r\n", stack.Size());
			break;
		case 'i':			
			printf("value(char[100]) = ");
			scanf("%s", temp.sz);
			stack.Push(temp);
			break;
		case 'd':			
			p = stack.Front();
			if(p ==NULL)
				break;
			printf("Front = %s \r\n", p->sz);
			stack.Pop();
			break;
		case 'l':
			p = stack.Front();
			for(int n = 0; n < stack.Size(); n++)
			{
				if(p==NULL)
					break;
				printf("index = %d,  value : %s \r\n",n, p->sz);
				p = stack.GetNext(p);
			}
			break;
		case 'q':
			stack.Finalize();
			goto q;
		default:
			break;
		}
	}

q:
	return 0;
};


int HashmapSample(size_t hashsize, size_t maxStackSize)
{
	Hashmap<hashdata> hash;	
	if(!hash.Initialize(hashsize, maxStackSize, TEXT("hash"), TEXT("D:\\Dev"), true))
	{		
		printf("Check your permission. Error = %d", ::GetLastError());		
		return 0;
	}

	hashdata temp;
	hashdata * p = NULL;
	_int64 key = 0;
	printf("i : insert, f : Find, l : show all, s : size, q : quit, k :  destroy, r : remove, m : Modify \r\n");
	while(true)
	{			
		char c;
		printf(">>");
		scanf_s("%c", &c);
		switch(c)
		{
		case 'm':
			printf("key = ");
			temp.key = 0;
			scanf("%d", &temp.key);
			printf("Value = ");			
			scanf("%s", temp.value);

			p = hash.Find(temp.key);
			::memcpy_s(p->value, sizeof(p->value), temp.value, sizeof(temp.value));
			break;
		case 'f' :
			printf("find key = ");
			scanf("%d", &key);
			if(!hash.Find(key, temp))
				printf("no data. \r\n");
			else
				printf("key = %d,  value : %s \r\n",temp.key, temp.value);

			break;
		case 'r':
			printf("remove key = ");
			scanf("%d", &key);
			hash.Remove(key);
			break;
		case 'k':
			hash.Destroy();
			goto q;			
		case 's':
			printf("size = %d \r\n", hash.Size());
			break;
		case 'i':			
			printf("Key(int64) = ");
			temp.key = 0;
			scanf("%d", &temp.key);
			printf("value(char[100]) = ");
			scanf("%s", temp.value);
			hash.Insert(temp);
			break;		
		case 'l':
			for(int i=0; i< hashsize; i++)
			{
				Stack<hashdata> * stack = hash.GetStack(i);
				hashdata * p = stack->Front();
				for(int n = 0; n < stack->Size(); n++)
				{
					if(p==NULL)
						break;
					printf("key = %d,  value : %s \r\n",p->key, p->value);
					p = stack->GetNext(p);
				}
			}
			break;
		case 'q':
			hash.Finalize();
			goto q;
		default:
			break;
		}
	}

q:
	return 0;
};


int _tmain(int argc, _TCHAR* argv[])
{
	//StackSample();
	HashmapSample(4,20); //LRU mode
	return 0;	
}

