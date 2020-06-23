#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma comment(lib, "pthreadVC2.lib")
#include <cstdio>
#include <cstdlib>
#include <time.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include "sortalgo.h"
#define N 200000 

int data[N];
int sortarr[N];

typedef struct {
	int start;
	int end;
}arrayindex;

typedef struct {
	arrayindex arr1;
	arrayindex arr2;
}arraystruct;

void mergesort(int arr1[],int arr2[],int a1,int a2) {
	// a1 and a2 as total of numbers contain in its array
	int cnta=0,cntb=0,cntc=0;
	
	// while index doesn't equal to its max
	while(cnta != a1 || cntb != a2)	{
		// when index of arr1 reach its max
		if(cnta == a1 && cntb !=a2) {
			// sort for value in arr2
			for(int i = cntb ; i< a2 ;i++) {
				sortarr[cntc]=arr2[cntb];
				cntb=cntb+1;
				cntc=cntc+1;
			}
		}
		// when index of arr2 reach its max
		
		else if(cnta != a1&& cntb==a2) {
			// sort for value in arr1
			for(int i= cnta; i<a1; i++) {
				sortarr[cntc]=arr1[cnta];
				cnta=cnta+1;
				cntc=cntc+1;
			}
		}
		else {
			// if value in arr1 smaller than arr2
			if(arr1[cnta]<arr2[cntb]) {
				// put smallest value into a new array from arr1 and 2
				sortarr[cntc]=arr1[cnta];
				// move to next index
				cnta=cnta+1;
				cntc=cntc+1;
			}
			else {
				sortarr[cntc]=arr2[cntb];
				cntb=cntb+1;
				cntc=cntc+1;
			}
		} 
	}
}

void* selectionSortthread(void* parameter) {
	arrayindex* parameter2 = (arrayindex*) parameter;
	selectionSort(&data[parameter2->start], parameter2->end - parameter2->start + 1);
	return 0;
}

void* bubbleSortthread(void* parameter) {
	arrayindex* parameter2 = (arrayindex*) parameter;
	bubble_sort(&data[parameter2->start], parameter2->end - parameter2->start + 1);
	return 0;
  
}

void* quickSortthread(void* parameter) {
	arrayindex* parameter2 = (arrayindex*) parameter;
	quickSort(&data[parameter2->start], 0, parameter2->end - parameter2->start);
	return 0;
}

void* insertionSortthread(void* parameter) {
	arrayindex* parameter2 = (arrayindex*) parameter;
	insertionSort(&data[parameter2->start], parameter2->end - parameter2->start + 1);
	return 0;
}

void* mergesortthread(void* parameter) {
	arraystruct* ptr = (arraystruct*) parameter;
	
	mergesort(
	&data[ptr->arr1.start],
	&data[ptr->arr2.start],
	ptr->arr1.end - ptr->arr1.start + 1,
	ptr->arr2.end - ptr->arr2.start + 1);
	return 0;
}

int main() {
	int dataN=N;
	double START,END;
	// random number
	srand(time(NULL));
	for (int i = 0; i < dataN; i++)
		data[i] = rand();

	FILE* fptr;
	fptr = fopen ("input.txt","r");
	for(int i=0;i<dataN;i++)
	{
		fscanf(fptr,"%d\n",&data[i]);
	}
	fclose(fptr);

	printf("pthread progressing...");
	
    pthread_t t[3];
	
	arrayindex thread1index,thread2index;
	// if total of data set is even
	if(dataN%2==0) {
		thread1index.start=0;
		thread1index.end=dataN/2-1;
		thread2index.start=dataN/2;
		thread2index.end=dataN-1;
	}
	// if total of data set is odd
	else if(dataN%2==1) {
		thread1index.start=0;
		thread1index.end=dataN/2;
		thread2index.start=dataN/2+1;
		thread2index.end=dataN-1;
	}
	START = clock();
	// using quick sort in thread 0
    pthread_create(&t[0], NULL,quickSortthread ,&thread1index);
	// using insert sort in thread 1
    pthread_create(&t[1], NULL,insertionSortthread ,&thread2index);
	pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    

	arraystruct finalarr;
	
	//use arraystruct to keep start and end index of both thread 0 and 1
	finalarr.arr1.start=thread1index.start;
	finalarr.arr1.end=thread1index.end;
	finalarr.arr2.start=thread2index.start;
	finalarr.arr2.end=thread2index.end;
	
	//using merge sort in thread 3
	pthread_create(&t[2], NULL,mergesortthread ,&finalarr);
    pthread_join(t[2], NULL);

	END = clock();
	double Sorttime = (END - START) / 1000.0;
	printf("Sorted Data Done!!\n");
	for (int i=0; i < dataN; i++)
		printf("%d ", sortarr[i]);
	printf("\n");
	printf("pthread exclusive time %fsec\n",Sorttime);
	system("pause");
}


