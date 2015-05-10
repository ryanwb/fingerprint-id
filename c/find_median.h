//MERGE TWO ARRAYS
void merge (int *A, int a, int *B, int b, int *C) 
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
void mergesort(float * A, int n){
	int i;
	int *A1, *A2;
	int n1, n2;
	if (n<2){
		return;
	}
	n1= n/2;
	n2=n-n1;
	A1=(float*)malloc(sizeof(float)*n1);
	A1=(float*)malloc(sizeof(float)*n2);

	for(i=0;i<n1;i++){
		A1[i]=A[i];
	}
	for(i=0;i<n2;i++){
		A2[i]=A[i+n1];
	}
	mergesort(A1,n1);
	mergesort(A2,n2);

	merge(A1,n1,A2,n2,A);
	free(A1);
	free(A2);
}

float find_median(float * array, int n){
	mergesort(array, n);
	return array[n/2];
}
