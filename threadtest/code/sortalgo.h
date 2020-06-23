


void swap(int *x, int *y) { 
    int temp = *x; 
    *x = *y; 
    *y = temp; 
} 
  
void selectionSort(int arr[], int n) { 
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

void bubble_sort(int a[], int n) {
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

int partition (int arr[], int low, int high) { 
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
  
void quickSort(int arr[], int low, int high) { 
    if (low < high) 
    { 
        int pi = partition(arr, low, high); 
  
        // separately sort values before partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
} 

void insertionSort(int arr[], int n) { 
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
