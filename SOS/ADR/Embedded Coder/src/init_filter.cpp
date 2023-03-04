//
// File: init_filter.cpp
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

// Include Files
#include "init_filter.h"
#include "GPSaidedINS_data.h"
#include "GPSaidedINS_types.h"
#include "rt_nonfinite.h"
#include <cstring>
#include <string.h>

// Function Definitions
//
// Kalman filter state matrix
//
// Arguments    : const struct4_T *settings
//                double P[225]
//                double Q1[36]
//                double Q2[36]
//                double R[9]
//                double H[45]
// Return Type  : void
//
void init_filter(const struct4_T *settings, double P[225], double Q1[36],
                 double Q2[36], double R[9], double H[45])
{
  double d[9];
  double y[9];
  double a;
  double b_a;
  int P_tmp;
  int i;
  signed char i1;
  std::memset(&P[0], 0, 225U * sizeof(double));
  a = settings->factp[0] * settings->factp[0];
  b_a = settings->factp[1] * settings->factp[1];
  for (i = 0; i < 3; i++) {
    i1 = iv[3 * i];
    P[15 * i] = a * static_cast<double>(i1);
    P_tmp = 15 * (i + 3);
    P[P_tmp + 3] = b_a * static_cast<double>(i1);
    i1 = iv[3 * i + 1];
    P[15 * i + 1] = a * static_cast<double>(i1);
    P[P_tmp + 4] = b_a * static_cast<double>(i1);
    i1 = iv[3 * i + 2];
    P[15 * i + 2] = a * static_cast<double>(i1);
    P[P_tmp + 5] = b_a * static_cast<double>(i1);
  }
  std::memset(&d[0], 0, 9U * sizeof(double));
  d[0] = settings->factp[2];
  d[4] = settings->factp[3];
  d[8] = settings->factp[4];
  for (P_tmp = 0; P_tmp < 9; P_tmp++) {
    a = d[P_tmp];
    y[P_tmp] = a * a;
  }
  a = settings->factp[5] * settings->factp[5];
  b_a = settings->factp[6] * settings->factp[6];
  for (i = 0; i < 3; i++) {
    int b_P_tmp;
    int c_P_tmp;
    int d_P_tmp;
    P_tmp = 15 * (i + 6);
    P[P_tmp + 6] = y[3 * i];
    i1 = iv[3 * i];
    b_P_tmp = 15 * (i + 9);
    P[b_P_tmp + 9] = a * static_cast<double>(i1);
    c_P_tmp = 15 * (i + 12);
    P[c_P_tmp + 12] = b_a * static_cast<double>(i1);
    d_P_tmp = 3 * i + 1;
    P[P_tmp + 7] = y[d_P_tmp];
    i1 = iv[d_P_tmp];
    P[b_P_tmp + 10] = a * static_cast<double>(i1);
    P[c_P_tmp + 13] = b_a * static_cast<double>(i1);
    d_P_tmp = 3 * i + 2;
    P[P_tmp + 8] = y[d_P_tmp];
    i1 = iv[d_P_tmp];
    P[b_P_tmp + 11] = a * static_cast<double>(i1);
    P[c_P_tmp + 14] = b_a * static_cast<double>(i1);
  }
  //  Process noise covariance
  std::memset(&Q1[0], 0, 36U * sizeof(double));
  a = settings->sigma_acc * settings->sigma_acc;
  b_a = settings->sigma_gyro * settings->sigma_gyro;
  for (i = 0; i < 3; i++) {
    Q1[6 * i] = a;
    P_tmp = 6 * (i + 3);
    Q1[P_tmp + 3] = b_a;
    Q1[6 * i + 1] = a;
    Q1[P_tmp + 4] = b_a;
    Q1[6 * i + 2] = a;
    Q1[P_tmp + 5] = b_a;
  }
  std::memset(&Q2[0], 0, 36U * sizeof(double));
  a = settings->sigma_acc_bias * settings->sigma_acc_bias;
  b_a = settings->sigma_gyro_bias * settings->sigma_gyro_bias;
  for (i = 0; i < 3; i++) {
    i1 = iv[3 * i];
    Q2[6 * i] = a * static_cast<double>(i1);
    P_tmp = 6 * (i + 3);
    Q2[P_tmp + 3] = b_a * static_cast<double>(i1);
    i1 = iv[3 * i + 1];
    Q2[6 * i + 1] = a * static_cast<double>(i1);
    Q2[P_tmp + 4] = b_a * static_cast<double>(i1);
    i1 = iv[3 * i + 2];
    Q2[6 * i + 2] = a * static_cast<double>(i1);
    Q2[P_tmp + 5] = b_a * static_cast<double>(i1);
  }
  //  GNSS-receiver position measurement noise
  a = settings->sigma_gps * settings->sigma_gps;
  //  Observation matrix
  for (i = 0; i < 9; i++) {
    R[i] = a * static_cast<double>(iv[i]);
    d[i] = 0.0;
  }
  d[0] = 1.0;
  d[4] = 1.0;
  d[8] = 1.0;
  for (i = 0; i < 3; i++) {
    H[3 * i] = d[3 * i];
    P_tmp = 3 * i + 1;
    H[P_tmp] = d[P_tmp];
    P_tmp = 3 * i + 2;
    H[P_tmp] = d[P_tmp];
  }
  for (i = 0; i < 12; i++) {
    P_tmp = 3 * (i + 3);
    H[P_tmp] = 0.0;
    H[P_tmp + 1] = 0.0;
    H[P_tmp + 2] = 0.0;
  }
}

//
// File trailer for init_filter.cpp
//
// [EOF]
//
