/*
 * @Description : Project Versus Thread Performance
 * @Author : kidneyweakx
 *
 */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma comment(lib, "pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <windows.h>
#include <thread>

#include <cstdio>
#include <cstdlib>
#include <time.h>


#include "sortalgo.h"


#define N 200000 

int data[N];
int sortarr[N];
typedef struct {
	int startindex;
	int endindex;
}arrayindex;

typedef struct {
	arrayindex arr1;
	arrayindex arr2;
}arraystruct;

void mergesort(int arr1[], int arr2[], int a1, int a2)
{ // a1 and a2 as total of numbers contain in its array
	int currenta = 0, currentb = 0, currentc = 0;
	// while index doesn't equal to its max
	while (currenta != a1 || currentb != a2)
	{	// when index of arr1 reach its max
		if (currenta == a1 && currentb != a2)
		{	// sort for value in arr2
			for (int i = currentb; i < a2; i++)
			{
				sortarr[currentc] = arr2[currentb];
				currentb = currentb + 1;
				currentc = currentc + 1;
			}
		}
		// when index of arr2 reach its max
		else if (currenta != a1 && currentb == a2)
		{// sort for value in arr1
			for (int i = currenta; i < a1; i++)
			{
				sortarr[currentc] = arr1[currenta];
				currenta = currenta + 1;
				currentc = currentc + 1;
			}
		}
		else
		{// if value in arr1 smaller than arr2
			if (arr1[currenta] < arr2[currentb])
			{// put smallest value into a new array from arr1 and 2
				sortarr[currentc] = arr1[currenta];
				// move to next index
				currenta = currenta + 1;
				currentc = currentc + 1;
			}
			else
			{
				sortarr[currentc] = arr2[currentb];
				currentb = currentb + 1;
				currentc = currentc + 1;
			}
		}
	}
}

void* selectionSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	selectionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
	return 0;
}

void* bubbleSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	bubble_sort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
	return 0;

}

void* quickSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	quickSort(&data[parameter2->startindex], 0, parameter2->endindex - parameter2->startindex);
	return 0;
}

void* insertionSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	insertionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
	return 0;
}

void* mergesortthread(void* parameter)
{
	arraystruct* ptr = (arraystruct*)parameter;

	mergesort(
		&data[ptr->arr1.startindex],
		&data[ptr->arr2.startindex],
		ptr->arr1.endindex - ptr->arr1.startindex + 1,
		ptr->arr2.endindex - ptr->arr2.startindex + 1);
	return 0;
}


DWORD WINAPI selectionSortthread1(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	selectionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
	return 0;
}

DWORD WINAPI bubbleSortthread1(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	bubble_sort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
	return 0;

}

DWORD WINAPI quickSortthread1(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	quickSort(&data[parameter2->startindex], 0, parameter2->endindex - parameter2->startindex);
	return 0;
}

DWORD WINAPI insertionSortthread1(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	insertionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
	return 0;
}
DWORD WINAPI mergesortthread1(VOID* parameter)
{
	arraystruct* ptr = (arraystruct*)parameter;

	mergesort(
		&data[ptr->arr1.startindex],
		&data[ptr->arr2.startindex],
		ptr->arr1.endindex - ptr->arr1.startindex + 1,
		ptr->arr2.endindex - ptr->arr2.startindex + 1);
	return 0;
}

int main(){
	int datanum = N;
	double START, END;
	// read Random data
	FILE* fptr;
	fptr = fopen("input.txt", "r");
	for (int i = 0; i < datanum; i++)
	{
		fscanf(fptr, "%d\n", &data[i]);
	}
	fclose(fptr);

	int tempdata[N] = {};
	memcpy(tempdata,data,N);
	printf("pthread progressing...\n");

	pthread_t t[3];
	arrayindex thread1index, thread2index;

	if (datanum % 2 == 0)// if total of data set is even
	{
		thread1index.startindex = 0;
		thread1index.endindex = datanum / 2 - 1;
		thread2index.startindex = datanum / 2;
		thread2index.endindex = datanum - 1;
	}
	else if (datanum % 2 == 1)// if total of data set is odd
	{
		thread1index.startindex = 0;
		thread1index.endindex = datanum / 2;
		thread2index.startindex = datanum / 2 + 1;
		thread2index.endindex = datanum - 1;
	}
	START = clock();
	// using quick sort in thread 0
	pthread_create(&t[0], NULL, quickSortthread, &thread1index);
	pthread_create(&t[1], NULL, insertionSortthread, &thread2index);
	pthread_join(t[0], NULL);
	pthread_join(t[1], NULL);
	// using insert sort in thread 1
	arraystruct posixarr;
	//use arraystruct to keep start and end index of both thread 0 and 1
	posixarr.arr1.startindex = thread1index.startindex;
	posixarr.arr1.endindex = thread1index.endindex;
	posixarr.arr2.startindex = thread2index.startindex;
	posixarr.arr2.endindex = thread2index.endindex;
	//using merge sort in thread 3
	pthread_create(&t[2], NULL, mergesortthread, &posixarr);
	pthread_join(t[2], NULL);
	END = clock();

	double Sorttime = (END - START) / 1000.0;
	printf("Sorted Data Done!!\n");;
	printf("pthread exclusive time %fsec\n\n", Sorttime);


	/*=====================================================*/

	memcpy(data, tempdata, N);
	printf("winapi progressing...\n");

	DWORD threadid, threadid2;
	HANDLE threadhandle[2];

	if (datanum % 2 == 0)// if total of data set is even
	{
		thread1index.startindex = 0;
		thread1index.endindex = datanum / 2 - 1;
		thread2index.startindex = datanum / 2;
		thread2index.endindex = datanum - 1;
	}

	else if (datanum % 2 == 1)// if total of data set is odd
	{
		thread1index.startindex = 0;
		thread1index.endindex = datanum / 2;
		thread2index.startindex = datanum / 2 + 1;
		thread2index.endindex = datanum - 1;
	}
	START = clock();
	// using quick sort in thread 0
	threadhandle[0] = CreateThread(NULL,0,quickSortthread1,&thread1index,0,&threadid);
	// using insert sort in thread 1
	threadhandle[1] = CreateThread(NULL,0,insertionSortthread1,&thread2index,0,&threadid2);
	//wait until thread 0 and 1 finished sorting
	WaitForMultipleObjects(2, threadhandle, TRUE, INFINITE);
	//CloseHandle(threadhandle);

	DWORD threadid3;
	HANDLE mergehandle;
	arraystruct winapiarr;
	//use arraystruct to keep start and end index of both thread 0 and 1
	winapiarr.arr1.startindex = thread1index.startindex;
	winapiarr.arr1.endindex = thread1index.endindex;
	winapiarr.arr2.startindex = thread2index.startindex;
	winapiarr.arr2.endindex = thread2index.endindex;

	//using merge sort in thread 3
	mergehandle = CreateThread(	NULL,0,mergesortthread1,&winapiarr,0,&threadid3);
	//wait until thread 3 finished sorting
	WaitForSingleObject(mergehandle, INFINITE);
	END = clock();
	Sorttime = (END - START) / 1000.0;
	printf("Sorted Data Done!!\n");

	printf("Winapi exclusive time %fsec\n\n", Sorttime);
/*===================================================*/

	memcpy(data, tempdata, N);
	printf("std::thread progressing...\n");

	if (datanum % 2 == 0)// if total of data set is even
	{
		thread1index.startindex = 0;
		thread1index.endindex = datanum / 2 - 1;
		thread2index.startindex = datanum / 2;
		thread2index.endindex = datanum - 1;
	}

	else if (datanum % 2 == 1)// if total of data set is odd
	{
		thread1index.startindex = 0;
		thread1index.endindex = datanum / 2;
		thread2index.startindex = datanum / 2 + 1;
		thread2index.endindex = datanum - 1;
	}

	START = clock();
	// using quick sort in thread 0
	std::thread t1(quickSortthread, &thread1index);
	std::thread t2(insertionSortthread, &thread2index);

	t1.join();
	t2.join();;
	// using insert sort in thread 1
	arraystruct cpparr;
	//use arraystruct to keep start and end index of both thread 0 and 1
	cpparr.arr1.startindex = thread1index.startindex;
	cpparr.arr1.endindex = thread1index.endindex;
	cpparr.arr2.startindex = thread2index.startindex;
	cpparr.arr2.endindex = thread2index.endindex;

	//using merge sort in thread 3
	std::thread t3(mergesortthread, &cpparr);
	t3.join();

	END = clock();
	printf("Sorted Data Done!!\n");
	Sorttime = (END - START) / 1000.0;


	printf("std thread exclusive time %fsec\n", Sorttime);


	system("pause");
}


