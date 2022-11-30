#include <assert.h>
#include <immintrin.h>
void singleThread(int N, int *matA, int *matB, int *output)
{
  assert( N>=4 and N == ( N &~ (N-1)));
  int *temp_mat = new int[N*N];
  //set all element zero to get rid of grabage value
  for(int i=0;i<N;i++){
    for(int j = 0;j<N;j++){
      temp_mat[N*i + j] = 0;
    }
  }
  __m256i mul_temp = _mm256_setzero_si256(); // store result of multiplication of one vector
  __m256i single_element = _mm256_setzero_si256(); // to store single element of matrix A
  __m256i matB_row = _mm256_setzero_si256(); // to store 8 element of row of matrix B
  // __m256i mat_mul = _mm256_setzero_si256();
  // __m256i mul_sum = _mm256_setzero_si256();

  for(int rowA = 0; rowA < N; rowA++) {
    for(int colB = 0; colB < N; colB++ ){
     
      int element = matA[(N*rowA) + colB];
      single_element = _mm256_set1_epi32(element); // get one element and store same element in all eight position

      for(int iter = 0; iter < N; iter += 8) 
      {
        matB_row = _mm256_loadu_si256((__m256i*)&matB[(N*colB) + iter]); // store eight element of row of matrix B
        // mat_mul = _mm256_mullo_epi32(single_element,matB_row);    
        mul_temp = _mm256_loadu_si256((__m256i*)&temp_mat[(N*rowA)+iter]); // load the pre-exists value of tamp_mat
        mul_temp = _mm256_add_epi32(mul_temp, _mm256_mullo_epi32(single_element,matB_row)); // first multiply the single element and matB_row and then sum this in temp_mat
        _mm256_storeu_si256((__m256i*)&temp_mat[(N*rowA)+iter],mul_temp); s//tore the final result into temp_mat
      }
    }
  }
  for(int rowA = 0; rowA < N; rowA++){
    for(int colB = 0; colB < N; colB++){
      output[(rowA>>1)*(N>>1) + (colB>>1)] += temp_mat[(rowA*N) + colB];
    }
  }
}