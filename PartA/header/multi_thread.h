#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <assert.h>


using namespace std;

struct multi_thread_data
{
    int N;
    int th_n;
    int *output;
    int *matA;
    int *matB;
};


void* thread_function(void* arg){
    struct multi_thread_data *th_data = (struct multi_thread_data *)arg;
    int N = th_data->N;
    int th_n = th_data->th_n;
    int *mat_temp0;
    int *mat_temp1;
    int *mat_temp2;
    int *mat_temp3;
    //even even
    if(th_n==0){
        mat_temp0 = (int *)malloc((N*N/4)*sizeof(int));
        for(int i = 0,ii=0; i<N,ii<(N/2); i+=2,ii++){
            for(int j = 0,jj=0; j<N,jj<N/2;j+=2,jj++){
                int sum = 0;
                for(int k = 0;k<N;k++){
                    sum += (th_data->matA[(N*i)+k]*th_data->matB[(N*k)+j]);
                }
                mat_temp0[((N/2)*ii)+jj] += sum;
            }
        }
        pthread_exit((void*) mat_temp0);
    }
    // even odd
    else if (th_n==1)
    {
        mat_temp1 = (int *)malloc((N*N/4)*sizeof(int));
        for(int i = 0,ii=0; ii<N,ii<(N/2); i+=2,ii++){
            for(int j = 1,jj=0; j<N,jj<N/2;j+=2,jj++){
                int sum = 0;
                for(int k = 0;k<N;k++){
                    sum += (th_data->matA[(N*i)+k]*th_data->matB[(N*k)+j]);
                }
                mat_temp1[((N/2)*ii)+jj] += sum;
            }
        } 
        pthread_exit((void*) mat_temp1);
    }
    // odd odd
    else if (th_n==2)
    {
        mat_temp2 = (int *)malloc((N*N/4)*sizeof(int));
        for(int i = 1,ii=0; i<N,ii<N,ii<(N/2); i+=2,ii++){
            for(int j = 1,jj=0; j<N,jj<N/2;j+=2,jj++){
               int sum = 0;
                for(int k = 0;k<N;k++){
                    sum += (th_data->matA[(N*i)+k]*th_data->matB[(N*k)+(j)]);
                }
                mat_temp2[((N/2)*ii)+jj] += sum;
            }
        }
        pthread_exit((void*) mat_temp2);
    }
    // odd even
    else{
        mat_temp3 = (int *)malloc((N*N/4)*sizeof(int));
        for(int i = 1,ii=0; i<N,ii<N/2; i+=2,ii++){
            for(int j = 0,jj=0; j<N,jj<N/2;j+=2,jj++){
                int sum = 0;
                for(int k = 0;k<N;k++){
                    sum += (th_data->matA[(N*i)+k]*th_data->matB[(N*k)+j]);
                }
                mat_temp3[((N/2)*ii)+jj] += sum;
            }
        }
        pthread_exit((void*) mat_temp3);
    }
}

// Fill in this function
void multiThread(int N, int *matA, int *matB, int *output)
{
    pthread_t th[4];
    struct multi_thread_data th_data[4];
    for(int i = 0; i<4; i++){
        struct multi_thread_data args;
        args.N = N;
        args.matA = matA;
        args.matB = matB;
        args.output = output;
        args.th_n = i;
        th_data[i] = args;
    }

    for(int i = 0; i< 4;i++){
        int error_code = pthread_create(&th[i],NULL,thread_function ,(void*)&th_data[i]);
        if(error_code != 0){
            cout<<"Thread Creation Fail with error code  "<< error_code <<endl;
        }
    }

    int* mat_block[4];
    for(int c = 0;c<4;c++){
        int* mat_block_t;
        int error = pthread_join(th[c],(void**)&mat_block_t);
        if(error != 0){
            cout<<"Thread join fail and error code is : "<<error<<endl;
        }
        mat_block[c] = (int *) malloc(((N*N)/4)*sizeof(int));
        for(int j=0;j<(N*N)/4;j++){
            mat_block[c][j]=mat_block_t[j];
        }
        cout<<endl;
        free(mat_block_t);
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<((N*N)/4);j++){
            output[j]+=mat_block[i][j];
        }
    }
}