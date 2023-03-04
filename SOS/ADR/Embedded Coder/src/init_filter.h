//
// File: init_filter.h
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

#ifndef INIT_FILTER_H
#define INIT_FILTER_H

// Include Files
#include "GPSaidedINS_types.h"
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
extern void init_filter(const struct4_T *settings, double P[225], double Q1[36],
                        double Q2[36], double R[9], double H[45]);

#endif
//
// File trailer for init_filter.h
//
// [EOF]
//
