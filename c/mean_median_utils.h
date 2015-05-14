#ifndef MEAN_MEDIAN_UTILS_H
#define MEAN_MEDIAN_UTILS_H

unsigned char find_mean(unsigned char* data, int n)
{
  int i;
  float mean = 0.0;
  for (i = 0; i < n; i++)
    mean += (float)data[i];
  return (unsigned char)(mean / (float)n);
}

//MERGE TWO ARRAYS
void merge(unsigned char *A, int a, unsigned char *B, int b, unsigned char *C) 
{
  int i,j,k;
  i = 0; 
  j = 0;
  k = 0;
  while (i < a && j < b) {
    if (A[i] <= B[j]) {
	  /* copy A[i] to C[k] and move the pointer i and k forward */
	  C[k] = A[i];
	  i++;
	  k++;
    }
    else {
      /* copy B[j] to C[k] and move the pointer j and k forward */
      C[k] = B[j];
      j++;
      k++;
    }
  }
  /* move the remaining elements in A into C */
  while (i < a) {
    C[k]= A[i];
    i++;
    k++;
  }
  /* move the remaining elements in B into C */
  while (j < b)  {
    C[k]= B[j];
    j++;
    k++;
  }
}  

//MERGESORT
void merge_sort(unsigned char *A, int n){
	int i;
	unsigned char *A1, *A2;
	int n1, n2;
	if (n<2){
		return;
	}

	n1= n/2;
	n2=n-n1;
	A1=(unsigned char*)m_malloc(sizeof(unsigned char)*n1);
	A2=(unsigned char*)m_malloc(sizeof(unsigned char)*n2);

	for(i=0;i<n1;i++){
		A1[i]=A[i];
	}
	for(i=0;i<n2;i++){
		A2[i]=A[i+n1];
	}
	merge_sort(A1,n1);
	merge_sort(A2,n2);

	merge(A1,n1,A2,n2,A);
	m_free(A1);
	m_free(A2);
}

unsigned char find_median(unsigned char * data, int n) {
  // first, copy the input so we don't modify it
  unsigned char *array = (unsigned char*)m_malloc(sizeof(unsigned char)*n);
  int i;
  unsigned char median;
  for (i = 0; i < n; i++)
    array[i] = data[i];

  merge_sort(array, n);

	median = array[n/2];
  m_free(array);
	return median;
}

#endif