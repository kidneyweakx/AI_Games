#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <thread>
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



void mergesort(int arr1[], int arr2[], int a1, int a2) {// a1 and a2 as total of numbers contain in its array
	int currenta = 0, currentb = 0, currentc = 0;

	while (currenta != a1 || currentb != a2)// while index doesn't equal to its max
	{
		if (currenta == a1 && currentb != a2)// when index of arr1 reach its max
		{
			for (int i = currentb; i < a2; i++)// sort for value in arr2
			{
				sortarr[currentc] = arr2[currentb];
				currentb = currentb + 1;
				currentc = currentc + 1;
			}
		}
		else if (currenta != a1 && currentb == a2)// when index of arr2 reach its max
		{
			for (int i = currenta; i < a1; i++)// sort for value in arr1
			{
				sortarr[currentc] = arr1[currenta];
				currenta = currenta + 1;
				currentc = currentc + 1;
			}
		}
		else
		{
			if (arr1[currenta] < arr2[currentb])// if value in arr1 smaller than arr2
			{
				sortarr[currentc] = arr1[currenta];// put smallest value into a new array from arr1 and 2
				currenta = currenta + 1;// move to next index
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

void selectionSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	selectionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
}

void bubbleSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	bubble_sort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);

}

void quickSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	quickSort(&data[parameter2->startindex], 0, parameter2->endindex - parameter2->startindex);
}

void insertionSortthread(void* parameter)
{
	arrayindex* parameter2 = (arrayindex*)parameter;
	insertionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
}

void mergesortthread(void* parameter)
{
	arraystruct* ptr = (arraystruct*)parameter;

	mergesort(
		&data[ptr->arr1.startindex],
		&data[ptr->arr2.startindex],
		ptr->arr1.endindex - ptr->arr1.startindex + 1,
		ptr->arr2.endindex - ptr->arr2.startindex + 1);
}

int main()
{
	int datanum = N;
	double START, END;
	FILE* fptr;
	fptr = fopen ("input.txt","r");
	for(int i=0;i<datanum;i++)
	{
		fscanf(fptr,"%d\n",&data[i]);
	}
	fclose(fptr);

	printf("std::thread progressing...");

	// DWORD threadid,threadid2;
	// HANDLE threadhandle[2];

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
	std::thread t1(quickSortthread, (void*)&thread1index);
	std::thread t2(insertionSortthread, &thread2index);
	
	
	t1.join();

	t2.join();;
	// using insert sort in thread 1

	
	arraystruct array12;
	//use arraystruct to keep start and end index of both thread 0 and 1
	array12.arr1.startindex = thread1index.startindex;
	array12.arr1.endindex = thread1index.endindex;
	array12.arr2.startindex = thread2index.startindex;
	array12.arr2.endindex = thread2index.endindex;

	//using merge sort in thread 3
	std::thread t3(mergesortthread, &array12);
	t3.join();

	END = clock();
	double Sorttime = (END - START) / 1000.0;
	printf("Sorted Data Done!!\n");

	printf("\n");;
	printf("std thread exclusive time %fsec\n", Sorttime);
	system("pause");
}


