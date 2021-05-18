// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"

void initA(elem_t m[DIM][DIM]) {
    for (size_t i = 0; i < DIM; i++)
        for (size_t j = 0; j < DIM; j++)
            m[i][j] = (2*i)&(~0x3);
}

void initB(elem_t m[DIM][DIM]) {
    for (size_t i = 0; i < DIM; i++)
        for (size_t j = 0; j < DIM; j++)
            m[i][j] = (2*j)&(~0x3);
}

void computeRef(elem_t a[DIM][DIM],
        elem_t b[DIM][DIM],
        elem_t r[DIM][DIM]) {
    for (size_t i = 0; i < DIM; i++)
        for(size_t j = 0; j < DIM; j++)
            r[i][j] = (a[i][j]+b[i][j])&(~0x3);
}

int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

  //printf("Flush Gemmini TLB of stale virtual addresses\n");
  gemmini_flush(0);

  //printf("Initialize our input and output matrices in main memory\n");
  elem_t A[DIM][DIM];
  elem_t B[DIM][DIM];
  elem_t Out[DIM][DIM];
  elem_t Ref[DIM][DIM];
  initA(A);
  initB(B);
  computeRef(A, B, Ref);

  tiled_resadd_auto(DIM, DIM, 0, (elem_t*)A, (elem_t*)B, (elem_t*)Out, false, WS);
//void tiled_resadd_auto(const size_t I, const size_t J,
//        const int A_shift,
//        const elem_t * A,
//        const elem_t * B,
//        elem_t * C,
//        bool relu,
//        enum tiled_matmul_type_t matadd_type) {

  //printf("Check whether \"Ref\" and \"Out\" matrices are identical\n");
  if (!is_equal(Ref, Out)) {
    printf("Ref and Out matrices are different!\n");
    printf("\"Ref\" matrix:\n");
    printMatrix(Ref);
    printf("\"Out\" matrix:\n");
    printMatrix(Out);
    printf("\n");

    exit(1);
  }
  //printf("Reference and output matrices are identical, as expected\n");
  exit(0);
}

