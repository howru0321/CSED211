/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int a,b,c;
    int n1,n2,n3,n4,n5,n6,n7,n8;
    if(M==32){
        for(a=0;a<32;a+=8){
            for(b=0;b<32;b+=8){
                for(c=0;c<8;c++){
                    n1=A[a+c][b];
                    n2=A[a+c][b+1];
                    n3=A[a+c][b+2];
                    n4=A[a+c][b+3];
                    n5=A[a+c][b+4];
                    n6=A[a+c][b+5];
                    n7=A[a+c][b+6];
                    n8=A[a+c][b+7];

                    B[b][a+c]=n1;
                    B[b+1][a+c]=n2;
                    B[b+2][a+c]=n3;
                    B[b+3][a+c]=n4;
                    B[b+4][a+c]=n5;
                    B[b+5][a+c]=n6;
                    B[b+6][a+c]=n7;
                    B[b+7][a+c]=n8;
                }
            }
        }
    }

    //int num[64];

    else if(M==64){
        for(a=0;a<64;a+=8){
            for(b=0;b<64;b+=8){
                for(c=0;c<4;c++){
                    n1=A[a+c][b];
                    n2=A[a+c][b+1];
                    n3=A[a+c][b+2];
                    n4=A[a+c][b+3];
                    n5=A[a+c][b+4];
                    n6=A[a+c][b+5];
                    n7=A[a+c][b+6];
                    n8=A[a+c][b+7];

                    B[b][a+c]=n1;
                    B[b+1][a+c]=n2;
                    B[b+2][a+c]=n3;
                    B[b+3][a+c]=n4;
                    B[b][a+c+4]=n5;
                    B[b+1][a+c+4]=n6;
                    B[b+2][a+c+4]=n7;
                    B[b+3][a+c+4]=n8;
                }
                for(c=0;c<4;c++){
                    n1=B[b+c][a+4];
                    n2=B[b+c][a+5];
                    n3=B[b+c][a+6];
                    n4=B[b+c][a+7];

                    n5=A[a+4][b+c];
                    n6=A[a+5][b+c];
                    n7=A[a+6][b+c];
                    n8=A[a+7][b+c];

                    B[b+c][a+4]=n5;
                    B[b+c][a+5]=n6;
                    B[b+c][a+6]=n7;
                    B[b+c][a+7]=n8;

                    B[b+c+4][a]=n1;
                    B[b+c+4][a+1]=n2;
                    B[b+c+4][a+2]=n3;
                    B[b+c+4][a+3]=n4;
                }
                for(c=0;c<4;c++){
                    n1=A[a+c+4][b+4];
                    n2=A[a+c+4][b+5];
                    n3=A[a+c+4][b+6];
                    n4=A[a+c+4][b+7];

                    B[b+4][a+c+4]=n1;
                    B[b+5][a+c+4]=n2;
                    B[b+6][a+c+4]=n3;
                    B[b+7][a+c+4]=n4;
                }
            }
        }
    }
    else{
        for(a=0;a<64;a+=8){
            for(b=0;b<56;b+=8){
                for(c=0;c<8;c++){
                    n1=A[a+c][b];
                    n2=A[a+c][b+1];
                    n3=A[a+c][b+2];
                    n4=A[a+c][b+3];
                    n5=A[a+c][b+4];
                    n6=A[a+c][b+5];
                    n7=A[a+c][b+6];
                    n8=A[a+c][b+7];

                    B[b][a+c]=n1;
                    B[b+1][a+c]=n2;
                    B[b+2][a+c]=n3;
                    B[b+3][a+c]=n4;
                    B[b+4][a+c]=n5;
                    B[b+5][a+c]=n6;
                    B[b+6][a+c]=n7;
                    B[b+7][a+c]=n8;
                }
            }
        }
        for (a = 0; a < 56; a += 8) {
            for (b = 0; b < 3; b++) {
                n1 = A[64 + b][a];
                n2 = A[64 + b][a + 1];
                n3 = A[64 + b][a + 2];
                n4 = A[64 + b][a + 3];
                n5 = A[64 + b][a + 4];
                n6 = A[64 + b][a + 5];
                n7 = A[64 + b][a + 6];
                n8 = A[64 + b][a + 7];

                B[a + 0][64 + b] = n1;
                B[a + 1][64 + b] = n2;
                B[a + 2][64 + b] = n3;
                B[a + 3][64 + b] = n4;
                B[a + 4][64 + b] = n5;
                B[a + 5][64 + b] = n6;
                B[a + 6][64 + b] = n7;
                B[a + 7][64 + b] = n8;
            }
        }
        for (a = 0; a < 67; a++) {
            n1 = A[a][56];
            n2 = A[a][57];
            n3 = A[a][58];
            n4 = A[a][59];
            n5 = A[a][60];

            B[56][a] = n1;
            B[57][a] = n2;
            B[58][a] = n3;
            B[59][a] = n4;
            B[60][a] = n5;
        }
    }


}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}