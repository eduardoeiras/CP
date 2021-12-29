#include<omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "papi.h"

#define NARRAY 126   // Array size
#define NBUCKET 32  // Number of buckets
#define INTERVAL 16  // Each bucket capacity
#define SIZE 64  // Each bucket size

// PAPI events to monitor
#define NUM_EVENTS 4
int Events[NUM_EVENTS] = { PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_DCM };
// PAPI counters' values
long long values[NUM_EVENTS], min_values[NUM_EVENTS];
int retval, EventSet=PAPI_NULL;

// number of times the function is executed and measured
#define NUM_RUNS 5

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
  /*for (i = 0; i < NBUCKET; i++) {
    printf("Bucket[%d]: ", i);
    printBuckets(buckets[i],bucketelm[i]);
    printf("\n");
  }*/

  // Sort the elements of each bucket
  for (i = 0; i < NBUCKET; ++i) {
    quick_sort(buckets[i], 0, bucketelm[i]-1);
  }

  /*printf("-------------\n");
  printf("Bucktets after sorting\n");
  for (i = 0; i < NBUCKET; i++) {
    printf("Bucket[%d]: ", i);
    printBuckets(buckets[i],bucketelm[i]);
    printf("\n");
  }*/

  // Put sorted elements on arr
    for (j = 0, i = 0; i < NBUCKET; ++i) {
        int nelm = bucketelm[i];
        int k = 0;
        for (k = 0; k < nelm ; k++) {
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
  int i;
  for(i = 0; i < nelm; i++){
    printf("%d ", Bucket[i]);
  }
}

// Driver code
int main(void) {
  long long start_usec, end_usec, elapsed_usec, min_usec=0L;
  int num_hwcntrs = 0;
  int run;

  fprintf (stdout, "\nSetting up PAPI...");
  // Initialize PAPI
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
      fprintf(stderr,"PAPI library init error!\n");
      return 0;
  }

  /* create event set */
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
      fprintf(stderr,"PAPI create event set error\n");
      return 0;
  }


  /* Get the number of hardware counters available */
  if ((num_hwcntrs = PAPI_num_hwctrs()) <= PAPI_OK)  {
      fprintf (stderr, "PAPI error getting number of available hardware counters!\n");
      return 0;
  }
  fprintf(stdout, "done!\nThis system has %d available counters.\n\n", num_hwcntrs);

  int array[NARRAY];
  int i;
  for(i=0 ; i < NARRAY ; i++) {
     array[i] = rand() % (NBUCKET*INTERVAL);
  }

  // We will be using at most NUM_EVENTS counters
  if (num_hwcntrs >= NUM_EVENTS) {
      num_hwcntrs = NUM_EVENTS;
  } else {
      fprintf (stderr, "Error: there aren't enough counters to monitor %d events!\n", NUM_EVENTS);
      return 0;
  }

  if (PAPI_add_events(EventSet,Events,NUM_EVENTS) != PAPI_OK)  {
      fprintf(stderr,"PAPI library add events error!\n");
      return 0;
  }

  // warmup caches
  fprintf (stdout, "Warming up caches...");
  BucketSort(array);
  fprintf (stdout, "done!\n");

  for (run=0 ; run < NUM_RUNS ; run++) {
        fprintf (stdout, "run=%d - Sorting Array...", run);

        // use PAPI timer (usecs) - note that this is wall clock time
        // for process time running in user mode -> PAPI_get_virt_usec()
        // real and virtual clock cycles can also be read using the equivalent
        // PAPI_get[real|virt]_cyc()
        start_usec = PAPI_get_real_usec();

        /* Start counting events */
        if (PAPI_start(EventSet) != PAPI_OK) {
            fprintf (stderr, "PAPI error starting counters!\n");
            return 0;
        }

        //printf("Initial array: ");
        //print(array);
        //printf("-------------\n");

        BucketSort(array);
        printf("-------------\n");
        printf("Sorted array: ");
        print(array);
        printf("\n-------------\n");

        /* Stop counting events */
        if (PAPI_stop(EventSet,values) != PAPI_OK) {
            fprintf (stderr, "PAPI error stoping counters!\n");
            return 0;
        }

        end_usec = PAPI_get_real_usec();
        fprintf (stdout, "done!\n");

        elapsed_usec = end_usec - start_usec;

        if ((run==0) || (elapsed_usec < min_usec)) {
            min_usec = elapsed_usec;
            for (i=0 ; i< NUM_EVENTS ; i++) min_values[i] = values [i];
        }

  } // end runs
  fprintf (stdout,"\nWall clock time: %lld usecs\n", min_usec);

  // output PAPI counters' values
  for (i=0 ; i< NUM_EVENTS ; i++) {
        char EventCodeStr[PAPI_MAX_STR_LEN];

        if (PAPI_event_code_to_name(Events[i], EventCodeStr) == PAPI_OK) {
            fprintf (stdout, "%s = %lld\n", EventCodeStr, min_values[i]);
        } else {
            fprintf (stdout, "PAPI UNKNOWN EVENT = %lld\n", min_values[i]);
        }
  }
  return 0;
}