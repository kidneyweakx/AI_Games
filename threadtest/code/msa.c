#include <stdio.h> 
#include <windows.h>
#include <time.h>
#define N 50

int data[N];
int sortarr[N];

typedef struct{
	int startindex;
	int endindex;
}arrayindex;

typedef struct{
	arrayindex arr1;
	arrayindex arr2;
}arraystruct;

void mergesort(int arr1[],int arr2[],int a1,int a2){// a1 and a2 as total of numbers contain in its array
	int currenta=0,currentb=0,currentc=0;
	
	while(currenta!=a1||currentb!=a2)// while index doesn't equal to its max
	{
		if(currenta==a1&&currentb!=a2)// when index of arr1 reach its max
		{
			for(int i=currentb;i<a2;i++)// sort for value in arr2
			{
				sortarr[currentc]=arr2[currentb];
				currentb=currentb+1;
				currentc=currentc+1;
			}
		}
		else if(currenta!=a1&&currentb==a2)// when index of arr2 reach its max
		{
			for(int i=currenta;i<a1;i++)// sort for value in arr1
			{
				sortarr[currentc]=arr1[currenta];
				currenta=currenta+1;
				currentc=currentc+1;
			}
		}
		else
		{
			if(arr1[currenta]<arr2[currentb])// if value in arr1 smaller than arr2
			{
				sortarr[currentc]=arr1[currenta];// put smallest value into a new array from arr1 and 2
				currenta=currenta+1;// move to next index
				currentc=currentc+1;
			}
			else
			{
				sortarr[currentc]=arr2[currentb];
				currentb=currentb+1;
				currentc=currentc+1;
			}
		} 
	}
}

void swap(int *x, int *y)
{ 
    int temp = *x; 
    *x = *y; 
    *y = temp; 
} 
  
void selectionSort(int arr[], int n)
{ 
    int i, j, min_index; 
  
    for (i = 0; i < n; i++) 
    { 
        min_index = i; 
        for (j = i+1; j < n; j++) 
          if (arr[j] < arr[min_index])// if next value smaller than current value, swap
            min_index = j;// keep index
        swap(&arr[min_index], &arr[i]); 
    } 
}

void bubble_sort(int a[], int n)
{
   int i = 0, j = 0;
   for (i = 0; i < n; i++) {   
       for (j = 0; j < n - i - 1; j++) 
	   {     
            if (a[j] > a[j + 1])// if current value larger than next value, swap
			{ 
               swap(&a[j],&a[j+1]);
           }
       }
   }
}

int partition (int arr[], int low, int high) 
{ 
    int pivot = arr[high];// set last value as pivot 
    int i = (low - 1); // index of smaller value
  
    for (int j = low; j <= high- 1; j++) 
    { 
        //if current value is smaller than the pivot, move it to the left
        if (arr[j] < pivot) 
        { 
            i++;// increment index of smaller value 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]);// move pivot to the middle
    return (i + 1);
} 
  
void quickSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        int pi = partition(arr, low, high); 
  
        // separately sort values before partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
} 

void insertionSort(int arr[], int n) 
{ 
    int i, key, j; 
    for (i = 1; i < n; i++) { 
        key = arr[i]; 
        j = i - 1; 
  
        while (j >= 0 && arr[j] > key) {// while current value larger than next
            arr[j + 1] = arr[j];// swap
            j = j - 1; 
        } 
        arr[j + 1] = key; 
    } 
} 

DWORD WINAPI selectionSortthread(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*) parameter;
	selectionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
}

DWORD WINAPI bubbleSortthread(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*) parameter;
	bubble_sort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1); 
  
}

DWORD WINAPI quickSortthread(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*) parameter;
	quickSort(&data[parameter2->startindex], 0, parameter2->endindex - parameter2->startindex);
}

DWORD WINAPI insertionSortthread(VOID* parameter)
{
	arrayindex* parameter2 = (arrayindex*) parameter;
	insertionSort(&data[parameter2->startindex], parameter2->endindex - parameter2->startindex + 1);
}

DWORD WINAPI mergesortthread(VOID* parameter)
{
	arraystruct* ptr = (arraystruct*) parameter;
	
	mergesort(
	&data[ptr->arr1.startindex],
	&data[ptr->arr2.startindex],
	ptr->arr1.endindex - ptr->arr1.startindex + 1,
	ptr->arr2.endindex - ptr->arr2.startindex + 1);
}

int main()
{
	int datanum;
	datanum=N;
	
	srand(time(NULL));
	
	for(int i=0;i<datanum;i++)// generate an array contains 50 numbers 
	{
		data[i]=rand();
	} 	

	printf("Original Data¡G\n");
	for(int i =0;i<datanum;i++)
	{
		printf("%d \n",data[i]);
	}
	printf("\n\n");
	
	DWORD threadid,threadid2;
	HANDLE threadhandle[2];
	
	arrayindex thread1index,thread2index;
	
	if(datanum%2==0)// if total of data set is even
	{
		thread1index.startindex=0;
		thread1index.endindex=datanum/2-1;
		thread2index.startindex=datanum/2;
		thread2index.endindex=datanum-1;
	}
	 
	else if(datanum%2==1)// if total of data set is odd
	{
		thread1index.startindex=0;
		thread1index.endindex=datanum/2;
		thread2index.startindex=datanum/2+1;
		thread2index.endindex=datanum-1;
	}
	
	// using selection sort in thread 0
	threadhandle[0] = CreateThread(
    NULL,
    0,
    quickSortthread,
    &thread1index,
    0,
    &threadid);
    
    // using quick sort in thread 1
    threadhandle[1] = CreateThread(
    NULL,
    0,
    insertionSortthread,
    &thread2index,
    0,
    &threadid2);
    
    //wait until thread 0 and 1 finished sorting
    WaitForMultipleObjects(2,threadhandle,TRUE,INFINITE);
    CloseHandle(threadhandle);
    
    DWORD threadid3;
	HANDLE mergehandle;
	arraystruct array12;
	
	//use arraystruct to keep start and end index of both thread 0 and 1
	array12.arr1.startindex=thread1index.startindex;
	array12.arr1.endindex=thread1index.endindex;
	array12.arr2.startindex=thread2index.startindex;
	array12.arr2.endindex=thread2index.endindex;
	
	//using merge sort in thread 3
	mergehandle = CreateThread(
    NULL,
    0,
    mergesortthread,
    &array12,
    0,
    &threadid3);
	
	//wait until thread 3 finished sorting
	WaitForSingleObject(mergehandle,INFINITE);
	
	printf("Sorted Data¡G\n");
	for(int i=0;i<datanum;i++)
	{
		printf("%d: %d \n",i,sortarr[i]);
	}
	printf("\n");

}
