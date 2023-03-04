//
// File: mrdivide_helper.h
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

#ifndef MRDIVIDE_HELPER_H
#define MRDIVIDE_HELPER_H

// Include Files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
struct struct4_T;

// Function Declarations
void binary_expand_op(double K_data[], int K_size[2], const double C_data[],
                      const int C_size[2], double a, const signed char b[9],
                      const struct4_T *settings, double b_a,
                      const signed char b_b[4], const signed char B_tmp_data[],
                      const int *B_tmp_size);

namespace coder {
namespace internal {
void mrdiv(double A_data[], int A_size[2], const double B_data[],
           const int B_size[2]);

}
} // namespace coder

#endif
//
// File trailer for mrdivide_helper.h
//
// [EOF]
//
