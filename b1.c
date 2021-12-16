// Bucket sort in C

#include <stdio.h>
#include <stdlib.h>

#define NARRAY 5000   // Array size
#define NBUCKET 1000  // Number of buckets
#define INTERVAL 10  // Each bucket capacity
#define SIZE 75  // Each bucket size


void BucketSort(int arr[]);
void quick_sort(int *a, int left, int right);
void print(int arr[]);
void printBuckets(int Bucket[], int nelm);
int getBucketIndex(int value);

// Sorting function
void BucketSort(int arr[]) {
  int i, j;

  // Create buckets and allocate memory size
  int buckets [NBUCKET][SIZE] ;
  int bucketelm [NBUCKET];

  // Initialize empty buckets
  for (i = 0; i < NBUCKET; ++i) {
    bucketelm[i] = 0;
  }

  // Fill the buckets with respective elements
  for (i = 0; i < NARRAY; ++i) {
    int pos = getBucketIndex(arr[i]);
    buckets[pos][bucketelm[pos]++] = arr[i];
  }

  // Print the buckets along with their elements
  for (i = 0; i < NBUCKET; i++) {
    printf("Bucket[%d]: ", i);
    printBuckets(buckets[i],bucketelm[i]);
    printf("\n");
  }
  
  // Sort the elements of each bucket
  for (i = 0; i < NBUCKET; ++i) {
    //buckets[i] = InsertionSort(buckets[i]);
    quick_sort(buckets[i], 0, bucketelm[i]-1);
  }

  printf("-------------\n");
  printf("Bucktets after sorting\n");
  for (i = 0; i < NBUCKET; i++) {
    printf("Bucket[%d]: ", i);
    printBuckets(buckets[i],bucketelm[i]);
    printf("\n");
  }

  // Put sorted elements on arr
    for (j = 0, i = 0; i < NBUCKET; ++i) {
        int nelm = bucketelm[i];
        for (int k = 0; k < nelm ; k++) {
            arr[j++] = buckets[i][k];
        }
  }

  return;
}

// Function to sort the elements of each bucket
void quick_sort(int *a, int left, int right) {
    int i, j, x, y;
     
    i = left;
    j = right;
    x = a[(left + right) / 2];
     
    while(i <= j) {
        while(a[i] < x && i < right) {
            i++;
        }
        while(a[j] > x && j > left) {
            j--;
        }
        if(i <= j) {
            y = a[i];
            a[i] = a[j];
            a[j] = y;
            i++;
            j--;
        }
    }
     
    if(j > left) {
        quick_sort(a, left, j);
    }
    if(i < right) {
        quick_sort(a, i, right);
    }
}

int getBucketIndex(int value) {
  return value / INTERVAL;
}

void print(int ar[]) {
  int i;
  for (i = 0; i < NARRAY; ++i) {
    printf("%d ", ar[i]);
  }
  printf("\n");
}

// Print buckets
void printBuckets(int Bucket[], int nelm) {
  for(int i = 0; i < nelm; i++){
    printf("%d ", Bucket[i]);
  }
}

// Driver code
int main(void) {
  //int array[NARRAY] = {34, 42, 32, 33, 52, 9, 37, 47, 51, 0, 1, 56, 20};
  int array[NARRAY];
  for(int i=0 ; i < NARRAY ; i++) {
    array[i] = rand() % (NBUCKET*INTERVAL);
  }

  printf("Initial array: ");
  print(array);
  printf("-------------\n");

  BucketSort(array);
  printf("-------------\n");
  printf("Sorted array: ");
  print(array);
  return 0;
}
