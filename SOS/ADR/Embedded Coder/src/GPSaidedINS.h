//
// File: GPSaidedINS.h
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

#ifndef GPSAIDEDINS_H
#define GPSAIDEDINS_H

// Include Files
#include "GPSaidedINS_types.h"
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
extern void GPSaidedINS(const struct0_T *in_data, const struct4_T *settings,
                        double x_h[10], const double delta_u_h[6],
                        double P[225], const double Q1[36], const double Q2[36],
                        double t_prev, struct5_T *out_data);

#endif
//
// File trailer for GPSaidedINS.h
//
// [EOF]
//
