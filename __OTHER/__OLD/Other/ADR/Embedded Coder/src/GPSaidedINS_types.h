//
// File: GPSaidedINS_types.h
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

#ifndef GPSAIDEDINS_TYPES_H
#define GPSAIDEDINS_TYPES_H

// Include Files
#include "rtwtypes.h"

// Type Definitions
struct struct1_T {
  double acc[3];
  double gyro[3];
  double t;
};

struct struct2_T {
  double speed;
  double t;
};

struct struct3_T {
  double pos_ned[3];
  double t;
};

struct struct0_T {
  struct1_T IMU;
  struct2_T SPEEDOMETER;
  struct3_T GNSS;
};

struct struct5_T {
  double x_h[10];
  double diag_P[15];
  double delta_u_h[6];
};

struct struct4_T {
  char gnss_outage[2];
  double outagestart;
  double outagestop;
  char non_holonomic[2];
  char speed_aiding[2];
  double init_heading;
  double sigma_acc;
  double sigma_gyro;
  double sigma_acc_bias;
  double sigma_gyro_bias;
  double sigma_gps;
  double sigma_speed;
  double sigma_non_holonomic;
  double factp[7];
  double gravity[3];
};

#endif
//
// File trailer for GPSaidedINS_types.h
//
// [EOF]
//
