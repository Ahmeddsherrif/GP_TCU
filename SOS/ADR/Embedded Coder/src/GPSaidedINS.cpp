//
// File: GPSaidedINS.cpp
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

// Include Files
#include "GPSaidedINS.h"
#include "GPSaidedINS_data.h"
#include "GPSaidedINS_types.h"
#include "mrdivide_helper.h"
#include "q2dcm.h"
#include "rt_nonfinite.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string.h>

// Function Declarations
static void binary_expand_op(double b_data[], int *b_size,
                             const struct0_T *in_data,
                             const signed char _data[], const int _size[2],
                             const double b__data[], const int *b__size);

// Function Definitions
//
// Arguments    : double b_data[]
//                int *b_size
//                const struct0_T *in_data
//                const signed char _data[]
//                const int _size[2]
//                const double b__data[]
//                const int *b__size
// Return Type  : void
//
static void binary_expand_op(double b_data[], int *b_size,
                             const struct0_T *in_data,
                             const signed char _data[], const int _size[2],
                             const double b__data[], const int *b__size)
{
  double b_in_data[6];
  int loop_ub;
  int stride_0_0;
  int stride_1_0;
  b_in_data[0] = in_data->GNSS.pos_ned[0];
  b_in_data[1] = in_data->GNSS.pos_ned[1];
  b_in_data[2] = in_data->GNSS.pos_ned[2];
  b_in_data[3] = in_data->SPEEDOMETER.speed;
  b_in_data[4] = 0.0;
  b_in_data[5] = 0.0;
  if (*b__size == 1) {
    *b_size = _size[1];
  } else {
    *b_size = *b__size;
  }
  stride_0_0 = (_size[1] != 1);
  stride_1_0 = (*b__size != 1);
  if (*b__size == 1) {
    loop_ub = _size[1];
  } else {
    loop_ub = *b__size;
  }
  for (int i{0}; i < loop_ub; i++) {
    b_data[i] = b_in_data[_data[i * stride_0_0] - 1] - b__data[i * stride_1_0];
  }
}

//
// Copy data to variables with shorter name
//
// Arguments    : const struct0_T *in_data
//                const struct4_T *settings
//                double x_h[10]
//                const double delta_u_h[6]
//                double P[225]
//                const double Q1[36]
//                const double Q2[36]
//                double t_prev
//                struct5_T *out_data
// Return Type  : void
//
void GPSaidedINS(const struct0_T *in_data, const struct4_T *settings,
                 double x_h[10], const double delta_u_h[6], double P[225],
                 const double Q1[36], const double Q2[36], double t_prev,
                 struct5_T *out_data)
{
  static const double d_a[9]{0.988,   0.154,   0.0103, -0.1472, 0.9605,
                             -0.2363, -0.0463, 0.2319, 0.9716};
  static const signed char iv1[45]{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0,
                                   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static const signed char iv4[45]{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static const signed char b_iv[36]{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                                    0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
                                    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  static const signed char iv2[36]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0,
                                   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static const signed char iv3[36]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
  static const signed char b[16]{1, 0, 0, 0, 0, 1, 0, 0,
                                 0, 0, 1, 0, 0, 0, 0, 1};
  static const signed char d_b[4]{1, 0, 0, 1};
  static const char c_b[2]{'o', 'n'};
  double F[225];
  double b_F[225];
  double G[180];
  double b_G[180];
  double b_b[144];
  double H_data[90];
  double K_data[90];
  double b_C_data[90];
  double A[36];
  double C_data[36];
  double c_C_data[36];
  double b_a[18];
  double OMEGA[16];
  double b_y[15];
  double z[15];
  double Rb2t_tmp[9];
  double b_R[9];
  double b_Ts[9];
  double b_A[6];
  double b_data[6];
  double c_a[6];
  double u_h[6];
  double y[4];
  double f_t[3];
  double OMEGA_tmp;
  double Q;
  double R;
  double Ts;
  double a;
  double b_OMEGA_tmp;
  double b_P;
  int C_size[2];
  int K_size[2];
  int b_C_size[2];
  int tmp_size[2];
  int F_tmp;
  int F_tmp_tmp;
  int aoffset;
  int b_i;
  int boffset;
  int coffset;
  int k;
  int ret;
  int trueCount;
  signed char b_I[225];
  signed char b_tmp_data[6];
  signed char c_tmp_data[6];
  signed char tmp_data[6];
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  //  function call: out_data=GPSaidedINS(in_data,settings)
  //
  //  This function integrates GNSS and IMU data. The data fusion
  //  is based upon a loosely-coupled feedback GNSS-aided INS approach.
  //
  //  Input struct:
  //  in_data.GNSS.pos_ned      GNSS-receiver position estimates in NED
  //                            coordinates [m]
  //  in_data.GNSS.t            Time of GNSS measurements [s]
  //  (in_data.GNSS.HDOP        GNSS Horizontal Dilution of Precision [-])
  //  (in_data.GNSS.VDOP        GNSS Vertical Dilution of Precision [-])
  //  in_data.IMU.acc           Accelerometer measurements [m/s^2]
  //  in_data.IMU.gyro          Gyroscope measurements [rad/s]
  //  in_data.IMU.t             Time of IMU measurements [s]
  //
  //  Output struct:
  //  out_data.x_h              Estimated navigation state vector [position;
  //  velocity; attitude] out_data.delta_u_h        Estimated IMU biases
  //  [accelerometers; gyroscopes] out_data.diag_P           Diagonal elements
  //  of the Kalman filter state
  //                            covariance matrix.
  //
  //  Edit: Isaac Skog (skog@kth.se), 2016-09-06
  //  Revised: Bo Bernhardsson, 2018-01-01
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  Initialization
  //  Initialize the navigation state
  //  Allocate memory for the output data
  // out_data.x_h(:,1)=x_h;
  // out_data.diag_P(:,1)=diag(P);
  //  Information fusion
  //  Sampling period
  Ts = in_data->IMU.t - t_prev;
  //  Calibrate the sensor measurements using current sensor bias estimate.
  u_h[0] = in_data->IMU.acc[0] + delta_u_h[0];
  u_h[3] = in_data->IMU.gyro[0] + delta_u_h[3];
  u_h[1] = in_data->IMU.acc[1] + delta_u_h[1];
  u_h[4] = in_data->IMU.gyro[1] + delta_u_h[4];
  u_h[2] = in_data->IMU.acc[2] + delta_u_h[2];
  u_h[5] = in_data->IMU.gyro[2] + delta_u_h[5];
  //  Update the INS navigation state
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //           Position and velocity        %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  //  function call: x=Nav_eq(x,u,dt)
  //
  //  Function that implements the navigation equations of an INS.
  //
  //  Inputs:
  //  x         Current navigation state [position (NED), velocity (NED),
  //  attitude (Quaternion)] u         Measured inertial quantities [Specific
  //  force (m/s^2), Angular velocity (rad/s)] dt        Sampling period (s)
  //
  //  Output:
  //  x         Updated navigation state [position (NED), velocity (NED),
  //  attitude (Quaternion)]
  //
  //  Edit: Isaac Skog (skog@kth.se), 2016-09-06
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // function x=Nav_eq(x,u,dt)
  //  Gravity vector (This should depend on the current location, but
  //  since moving in a small area it is approximatly constant).
  // g_t=gravity(59,0);
  // g_t=gravity(55,0);
  //  Subtract gravity, to obtain accelerations in tangent plane coordinates
  //  state space model matrices
  for (boffset = 0; boffset < 36; boffset++) {
    A[boffset] = b_iv[boffset];
  }
  A[18] = Ts;
  A[25] = Ts;
  A[32] = Ts;
  a = Ts * Ts / 2.0;
  //  Position and velocity update
  q2dcm(*(double(*)[4]) & x_h[6], Rb2t_tmp);
  R = u_h[0];
  OMEGA_tmp = u_h[1];
  Q = u_h[2];
  for (boffset = 0; boffset < 3; boffset++) {
    signed char i;
    i = iv[3 * boffset];
    b_a[6 * boffset] = a * static_cast<double>(i);
    b_a[6 * boffset + 3] = Ts * static_cast<double>(i);
    i = iv[3 * boffset + 1];
    b_a[6 * boffset + 1] = a * static_cast<double>(i);
    b_a[6 * boffset + 4] = Ts * static_cast<double>(i);
    i = iv[3 * boffset + 2];
    b_a[6 * boffset + 2] = a * static_cast<double>(i);
    b_a[6 * boffset + 5] = Ts * static_cast<double>(i);
    f_t[boffset] =
        ((Rb2t_tmp[boffset] * R + Rb2t_tmp[boffset + 3] * OMEGA_tmp) +
         Rb2t_tmp[boffset + 6] * Q) -
        settings->gravity[boffset];
  }
  for (boffset = 0; boffset < 6; boffset++) {
    R = 0.0;
    for (F_tmp_tmp = 0; F_tmp_tmp < 6; F_tmp_tmp++) {
      R += A[boffset + 6 * F_tmp_tmp] * x_h[F_tmp_tmp];
    }
    b_A[boffset] = R;
    c_a[boffset] = (b_a[boffset] * f_t[0] + b_a[boffset + 6] * f_t[1]) +
                   b_a[boffset + 12] * f_t[2];
  }
  for (boffset = 0; boffset < 6; boffset++) {
    x_h[boffset] = b_A[boffset] + c_a[boffset];
  }
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //         Attitude Quaternion            %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  Quaternion algorithm according to Farrel, pp 49.
  b_P = u_h[3] * Ts;
  Q = u_h[4] * Ts;
  R = u_h[5] * Ts;
  OMEGA[0] = 0.0;
  OMEGA[4] = 0.5 * R;
  OMEGA_tmp = 0.5 * -Q;
  OMEGA[8] = OMEGA_tmp;
  OMEGA[12] = 0.5 * b_P;
  b_OMEGA_tmp = 0.5 * -R;
  OMEGA[1] = b_OMEGA_tmp;
  OMEGA[5] = 0.0;
  OMEGA[9] = 0.5 * b_P;
  OMEGA[13] = 0.5 * Q;
  OMEGA[2] = 0.5 * Q;
  b_P = 0.5 * -b_P;
  OMEGA[6] = b_P;
  OMEGA[10] = 0.0;
  OMEGA[14] = 0.5 * R;
  OMEGA[3] = b_P;
  OMEGA[7] = OMEGA_tmp;
  OMEGA[11] = b_OMEGA_tmp;
  OMEGA[15] = 0.0;
  b_P = 3.3121686421112381E-170;
  Q = std::abs(u_h[3]);
  if (Q > 3.3121686421112381E-170) {
    OMEGA_tmp = 1.0;
    b_P = Q;
  } else {
    R = Q / 3.3121686421112381E-170;
    OMEGA_tmp = R * R;
  }
  Q = std::abs(u_h[4]);
  if (Q > b_P) {
    R = b_P / Q;
    OMEGA_tmp = OMEGA_tmp * R * R + 1.0;
    b_P = Q;
  } else {
    R = Q / b_P;
    OMEGA_tmp += R * R;
  }
  Q = std::abs(u_h[5]);
  if (Q > b_P) {
    R = b_P / Q;
    OMEGA_tmp = OMEGA_tmp * R * R + 1.0;
    b_P = Q;
  } else {
    R = Q / b_P;
    OMEGA_tmp += R * R;
  }
  OMEGA_tmp = b_P * std::sqrt(OMEGA_tmp);
  b_P = OMEGA_tmp * Ts;
  if (b_P != 0.0) {
    a = std::cos(b_P / 2.0);
    b_OMEGA_tmp = 2.0 / b_P * std::sin(b_P / 2.0);
    for (boffset = 0; boffset < 16; boffset++) {
      OMEGA[boffset] =
          a * static_cast<double>(b[boffset]) + b_OMEGA_tmp * OMEGA[boffset];
    }
    R = x_h[6];
    OMEGA_tmp = x_h[7];
    Q = x_h[8];
    b_P = x_h[9];
    for (boffset = 0; boffset < 4; boffset++) {
      y[boffset] = ((OMEGA[boffset] * R + OMEGA[boffset + 4] * OMEGA_tmp) +
                    OMEGA[boffset + 8] * Q) +
                   OMEGA[boffset + 12] * b_P;
    }
    x_h[6] = y[0];
    x_h[7] = y[1];
    x_h[8] = y[2];
    x_h[9] = y[3];
  }
  //  Get state space model matrices
  //  Convert quaternion to DCM
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //                           SUB-FUNCTIONS                                %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //        Init filter          %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //   Init navigation state     %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //   State transition matrix   %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  q2dcm(*(double(*)[4]) & x_h[6], Rb2t_tmp);
  //  Transform measured force to force in
  //  the tangent plane coordinate system.
  R = u_h[0];
  OMEGA_tmp = u_h[1];
  Q = u_h[2];
  for (boffset = 0; boffset < 3; boffset++) {
    f_t[boffset] = (Rb2t_tmp[boffset] * R + Rb2t_tmp[boffset + 3] * OMEGA_tmp) +
                   Rb2t_tmp[boffset + 6] * Q;
  }
  //  Only the standard errors included
  //  Approximation of the discret
  //  time state transition matrix
  std::memset(&b_I[0], 0, 225U * sizeof(signed char));
  b_Ts[0] = Ts * 0.0;
  b_Ts[3] = Ts * -f_t[2];
  b_Ts[6] = Ts * f_t[1];
  b_Ts[1] = Ts * f_t[2];
  b_Ts[4] = Ts * 0.0;
  b_Ts[7] = Ts * -f_t[0];
  b_Ts[2] = Ts * -f_t[1];
  b_Ts[5] = Ts * f_t[0];
  b_Ts[8] = Ts * 0.0;
  for (k = 0; k < 15; k++) {
    b_I[k + 15 * k] = 1;
    F[15 * k] =
        static_cast<double>(b_I[15 * k]) + Ts * static_cast<double>(iv1[3 * k]);
    F_tmp = 15 * k + 1;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) +
               Ts * static_cast<double>(iv1[3 * k + 1]);
    F_tmp = 15 * k + 2;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) +
               Ts * static_cast<double>(iv1[3 * k + 2]);
  }
  for (boffset = 0; boffset < 3; boffset++) {
    F_tmp = 15 * boffset + 3;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    ret = 15 * (boffset + 3);
    F[ret + 3] = static_cast<double>(b_I[ret + 3]) + Ts * 0.0;
    aoffset = 15 * (boffset + 6);
    F[aoffset + 3] = static_cast<double>(b_I[aoffset + 3]) + b_Ts[3 * boffset];
    R = Rb2t_tmp[3 * boffset];
    F_tmp_tmp = 15 * (boffset + 9);
    F[F_tmp_tmp + 3] = static_cast<double>(b_I[F_tmp_tmp + 3]) + Ts * R;
    coffset = 15 * (boffset + 12);
    F[coffset + 3] = static_cast<double>(b_I[coffset + 3]) + Ts * 0.0;
    F_tmp = 15 * boffset + 6;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F[ret + 6] = static_cast<double>(b_I[ret + 6]) + Ts * 0.0;
    F[aoffset + 6] = static_cast<double>(b_I[aoffset + 6]) + Ts * 0.0;
    F[F_tmp_tmp + 6] = static_cast<double>(b_I[F_tmp_tmp + 6]) + Ts * 0.0;
    F[coffset + 6] = static_cast<double>(b_I[coffset + 6]) + Ts * -R;
    F_tmp = 15 * boffset + 4;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F[ret + 4] = static_cast<double>(b_I[ret + 4]) + Ts * 0.0;
    F_tmp = 3 * boffset + 1;
    F[aoffset + 4] = static_cast<double>(b_I[aoffset + 4]) + b_Ts[F_tmp];
    R = Rb2t_tmp[F_tmp];
    F[F_tmp_tmp + 4] = static_cast<double>(b_I[F_tmp_tmp + 4]) + Ts * R;
    F[coffset + 4] = static_cast<double>(b_I[coffset + 4]) + Ts * 0.0;
    F_tmp = 15 * boffset + 7;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F[ret + 7] = static_cast<double>(b_I[ret + 7]) + Ts * 0.0;
    F[aoffset + 7] = static_cast<double>(b_I[aoffset + 7]) + Ts * 0.0;
    F[F_tmp_tmp + 7] = static_cast<double>(b_I[F_tmp_tmp + 7]) + Ts * 0.0;
    F[coffset + 7] = static_cast<double>(b_I[coffset + 7]) + Ts * -R;
    F_tmp = 15 * boffset + 5;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F[ret + 5] = static_cast<double>(b_I[ret + 5]) + Ts * 0.0;
    F_tmp = 3 * boffset + 2;
    F[aoffset + 5] = static_cast<double>(b_I[aoffset + 5]) + b_Ts[F_tmp];
    R = Rb2t_tmp[F_tmp];
    F[F_tmp_tmp + 5] = static_cast<double>(b_I[F_tmp_tmp + 5]) + Ts * R;
    F[coffset + 5] = static_cast<double>(b_I[coffset + 5]) + Ts * 0.0;
    F_tmp = 15 * boffset + 8;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F[ret + 8] = static_cast<double>(b_I[ret + 8]) + Ts * 0.0;
    F[aoffset + 8] = static_cast<double>(b_I[aoffset + 8]) + Ts * 0.0;
    F[F_tmp_tmp + 8] = static_cast<double>(b_I[F_tmp_tmp + 8]) + Ts * 0.0;
    F[coffset + 8] = static_cast<double>(b_I[coffset + 8]) + Ts * -R;
  }
  for (boffset = 0; boffset < 15; boffset++) {
    F_tmp = 15 * boffset + 9;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F_tmp = 15 * boffset + 12;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F_tmp = 15 * boffset + 10;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F_tmp = 15 * boffset + 13;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F_tmp = 15 * boffset + 11;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
    F_tmp = 15 * boffset + 14;
    F[F_tmp] = static_cast<double>(b_I[F_tmp]) + Ts * 0.0;
  }
  //  Noise gain matrix
  for (boffset = 0; boffset < 12; boffset++) {
    G[15 * boffset] = Ts * 0.0;
    G[15 * boffset + 1] = Ts * 0.0;
    G[15 * boffset + 2] = Ts * 0.0;
  }
  for (boffset = 0; boffset < 3; boffset++) {
    R = Rb2t_tmp[3 * boffset];
    G[15 * boffset + 3] = Ts * R;
    ret = 15 * (boffset + 3);
    G[ret + 3] = Ts * 0.0;
    aoffset = 15 * (boffset + 6);
    G[aoffset + 3] = Ts * 0.0;
    F_tmp_tmp = 15 * (boffset + 9);
    G[F_tmp_tmp + 3] = Ts * 0.0;
    G[15 * boffset + 6] = Ts * 0.0;
    G[ret + 6] = Ts * -R;
    G[aoffset + 6] = Ts * 0.0;
    G[F_tmp_tmp + 6] = Ts * 0.0;
    R = Rb2t_tmp[3 * boffset + 1];
    G[15 * boffset + 4] = Ts * R;
    G[ret + 4] = Ts * 0.0;
    G[aoffset + 4] = Ts * 0.0;
    G[F_tmp_tmp + 4] = Ts * 0.0;
    G[15 * boffset + 7] = Ts * 0.0;
    G[ret + 7] = Ts * -R;
    G[aoffset + 7] = Ts * 0.0;
    G[F_tmp_tmp + 7] = Ts * 0.0;
    R = Rb2t_tmp[3 * boffset + 2];
    G[15 * boffset + 5] = Ts * R;
    G[ret + 5] = Ts * 0.0;
    G[aoffset + 5] = Ts * 0.0;
    G[F_tmp_tmp + 5] = Ts * 0.0;
    G[15 * boffset + 8] = Ts * 0.0;
    G[ret + 8] = Ts * -R;
    G[aoffset + 8] = Ts * 0.0;
    G[F_tmp_tmp + 8] = Ts * 0.0;
  }
  for (boffset = 0; boffset < 12; boffset++) {
    G[15 * boffset + 9] = Ts * static_cast<double>(iv2[3 * boffset]);
    G[15 * boffset + 12] = Ts * static_cast<double>(iv3[3 * boffset]);
    ret = 3 * boffset + 1;
    G[15 * boffset + 10] = Ts * static_cast<double>(iv2[ret]);
    G[15 * boffset + 13] = Ts * static_cast<double>(iv3[ret]);
    ret = 3 * boffset + 2;
    G[15 * boffset + 11] = Ts * static_cast<double>(iv2[ret]);
    G[15 * boffset + 14] = Ts * static_cast<double>(iv3[ret]);
  }
  //  Time update of the Kalman filter state covariance.
  std::memset(&b_b[0], 0, 144U * sizeof(double));
  for (boffset = 0; boffset < 6; boffset++) {
    for (F_tmp_tmp = 0; F_tmp_tmp < 6; F_tmp_tmp++) {
      ret = F_tmp_tmp + 6 * boffset;
      b_b[F_tmp_tmp + 12 * boffset] = Q1[ret];
      b_b[(F_tmp_tmp + 12 * (boffset + 6)) + 6] = Q2[ret];
    }
  }
  for (boffset = 0; boffset < 15; boffset++) {
    for (F_tmp_tmp = 0; F_tmp_tmp < 15; F_tmp_tmp++) {
      R = 0.0;
      for (ret = 0; ret < 15; ret++) {
        R += F[boffset + 15 * ret] * P[ret + 15 * F_tmp_tmp];
      }
      b_F[boffset + 15 * F_tmp_tmp] = R;
    }
    for (F_tmp_tmp = 0; F_tmp_tmp < 12; F_tmp_tmp++) {
      R = 0.0;
      for (ret = 0; ret < 12; ret++) {
        R += G[boffset + 15 * ret] * b_b[ret + 12 * F_tmp_tmp];
      }
      b_G[boffset + 15 * F_tmp_tmp] = R;
    }
  }
  for (boffset = 0; boffset < 15; boffset++) {
    for (F_tmp_tmp = 0; F_tmp_tmp < 15; F_tmp_tmp++) {
      ret = boffset + 15 * F_tmp_tmp;
      P[ret] = 0.0;
      for (aoffset = 0; aoffset < 15; aoffset++) {
        P[ret] += b_F[boffset + 15 * aoffset] * F[F_tmp_tmp + 15 * aoffset];
      }
    }
    for (F_tmp_tmp = 0; F_tmp_tmp < 15; F_tmp_tmp++) {
      R = 0.0;
      for (ret = 0; ret < 12; ret++) {
        R += b_G[boffset + 15 * ret] * G[F_tmp_tmp + 15 * ret];
      }
      b_F[boffset + 15 * F_tmp_tmp] = R;
    }
  }
  for (boffset = 0; boffset < 225; boffset++) {
    P[boffset] += b_F[boffset];
  }
  //  Defualt measurement observation matrix  and measurement covariance
  //  matrix
  a = settings->sigma_gps * settings->sigma_gps;
  b_OMEGA_tmp = settings->sigma_non_holonomic * settings->sigma_non_holonomic;
  // ---> Hint: error standard deviations are in settings.sigma_speed and
  // settings.sigma_non_holonomic
  for (b_i = 0; b_i < 6; b_i++) {
    u_h[b_i] = 0.0;
  }
  //  index vector, describing available measurements
  //  Check if GNSS measurement is available
  if ((in_data->IMU.t >= in_data->GNSS.t) &&
      (in_data->IMU.t < in_data->GNSS.t + 0.5)) {
    boolean_T guard1{false};
    guard1 = false;
    if ((in_data->IMU.t < settings->outagestart) ||
        (in_data->IMU.t > settings->outagestop)) {
      guard1 = true;
    } else {
      ret = memcmp(&settings->gnss_outage[0], &c_b[0], 2);
      if (ret != 0) {
        guard1 = true;
      }
    }
    if (guard1) {
      u_h[0] = 1.0;
      u_h[1] = 1.0;
      u_h[2] = 1.0;
    }
    //  Update GNSS data counter
  }
  if ((in_data->IMU.t >= in_data->SPEEDOMETER.t) &&
      (in_data->IMU.t < in_data->SPEEDOMETER.t + 0.5)) {
    ret = memcmp(&settings->speed_aiding[0], &c_b[0], 2);
    if (ret == 0) {
      u_h[3] = 1.0;
    }
  }
  ret = memcmp(&settings->non_holonomic[0], &c_b[0], 2);
  if (ret == 0) {
    u_h[4] = 1.0;
    u_h[5] = 1.0;
  }
  trueCount = 0;
  ret = 0;
  for (b_i = 0; b_i < 6; b_i++) {
    if (u_h[b_i] != 0.0) {
      trueCount++;
      tmp_data[ret] = static_cast<signed char>(b_i + 1);
      ret++;
    }
  }
  for (boffset = 0; boffset < 3; boffset++) {
    R = d_a[boffset];
    OMEGA_tmp = d_a[boffset + 3];
    Q = d_a[boffset + 6];
    for (F_tmp_tmp = 0; F_tmp_tmp < 3; F_tmp_tmp++) {
      b_Ts[boffset + 3 * F_tmp_tmp] =
          (R * Rb2t_tmp[F_tmp_tmp] + OMEGA_tmp * Rb2t_tmp[F_tmp_tmp + 3]) +
          Q * Rb2t_tmp[F_tmp_tmp + 6];
    }
  }
  for (boffset = 0; boffset < 15; boffset++) {
    K_data[6 * boffset] = iv4[3 * boffset];
    K_data[6 * boffset + 1] = iv4[3 * boffset + 1];
    K_data[6 * boffset + 2] = iv4[3 * boffset + 2];
  }
  for (boffset = 0; boffset < 3; boffset++) {
    K_data[6 * boffset + 3] = 0.0;
    F_tmp_tmp = 6 * (boffset + 3);
    K_data[F_tmp_tmp + 3] = b_Ts[3 * boffset];
    K_data[6 * boffset + 4] = 0.0;
    K_data[F_tmp_tmp + 4] = b_Ts[3 * boffset + 1];
    K_data[6 * boffset + 5] = 0.0;
    K_data[F_tmp_tmp + 5] = b_Ts[3 * boffset + 2];
  }
  for (boffset = 0; boffset < 9; boffset++) {
    F_tmp_tmp = 6 * (boffset + 6);
    K_data[F_tmp_tmp + 3] = 0.0;
    K_data[F_tmp_tmp + 4] = 0.0;
    K_data[F_tmp_tmp + 5] = 0.0;
  }
  for (boffset = 0; boffset < 15; boffset++) {
    for (F_tmp_tmp = 0; F_tmp_tmp < trueCount; F_tmp_tmp++) {
      H_data[F_tmp_tmp + trueCount * boffset] =
          K_data[(tmp_data[F_tmp_tmp] + 6 * boffset) - 1];
    }
  }
  //  Calculate the Kalman filter gain.
  aoffset = 0;
  ret = 0;
  for (b_i = 0; b_i < 6; b_i++) {
    if (u_h[b_i] != 0.0) {
      aoffset++;
      b_tmp_data[ret] = static_cast<signed char>(b_i + 1);
      ret++;
    }
  }
  if (0 <= aoffset - 1) {
    std::copy(&b_tmp_data[0], &b_tmp_data[aoffset], &tmp_data[0]);
  }
  K_size[0] = 15;
  K_size[1] = trueCount;
  for (ret = 0; ret < trueCount; ret++) {
    coffset = ret * 15;
    for (b_i = 0; b_i < 15; b_i++) {
      b_P = 0.0;
      for (k = 0; k < 15; k++) {
        b_P += P[k * 15 + b_i] * H_data[k * trueCount + ret];
      }
      K_data[coffset + b_i] = b_P;
    }
  }
  for (ret = 0; ret < 15; ret++) {
    coffset = ret * trueCount;
    boffset = ret * 15;
    for (b_i = 0; b_i < trueCount; b_i++) {
      b_P = 0.0;
      for (k = 0; k < 15; k++) {
        b_P += H_data[k * trueCount + b_i] * P[boffset + k];
      }
      b_C_data[coffset + b_i] = b_P;
    }
  }
  C_size[0] = trueCount;
  C_size[1] = trueCount;
  for (ret = 0; ret < trueCount; ret++) {
    coffset = ret * trueCount;
    for (b_i = 0; b_i < trueCount; b_i++) {
      b_P = 0.0;
      for (k = 0; k < 15; k++) {
        b_P += b_C_data[k * trueCount + b_i] * H_data[k * trueCount + ret];
      }
      C_data[coffset + b_i] = b_P;
    }
  }
  if ((trueCount == aoffset) && (aoffset == trueCount)) {
    for (boffset = 0; boffset < 3; boffset++) {
      A[6 * boffset] = a * static_cast<double>(iv[3 * boffset]);
      ret = 6 * (boffset + 3);
      A[ret] = 0.0;
      A[6 * boffset + 1] = a * static_cast<double>(iv[3 * boffset + 1]);
      A[ret + 1] = 0.0;
      A[6 * boffset + 2] = a * static_cast<double>(iv[3 * boffset + 2]);
      A[ret + 2] = 0.0;
      A[6 * boffset + 3] = 0.0;
    }
    A[21] = settings->sigma_speed * settings->sigma_speed;
    A[27] = 0.0;
    A[33] = 0.0;
    for (boffset = 0; boffset < 4; boffset++) {
      A[6 * boffset + 4] = 0.0;
      A[6 * boffset + 5] = 0.0;
    }
    A[28] = b_OMEGA_tmp;
    A[29] = b_OMEGA_tmp * 0.0;
    A[34] = b_OMEGA_tmp * 0.0;
    A[35] = b_OMEGA_tmp;
    b_C_size[0] = trueCount;
    b_C_size[1] = trueCount;
    for (boffset = 0; boffset < trueCount; boffset++) {
      for (F_tmp_tmp = 0; F_tmp_tmp < trueCount; F_tmp_tmp++) {
        c_C_data[F_tmp_tmp + trueCount * boffset] =
            C_data[F_tmp_tmp + trueCount * boffset] +
            A[(tmp_data[F_tmp_tmp] + 6 * (tmp_data[boffset] - 1)) - 1];
      }
    }
    coder::internal::mrdiv(K_data, K_size, c_C_data, b_C_size);
  } else {
    binary_expand_op(K_data, K_size, C_data, C_size, a, iv, settings,
                     b_OMEGA_tmp, d_b, tmp_data, &aoffset);
  }
  //  Update the perturbation state estimate.
  ret = trueCount - 1;
  for (b_i = 0; b_i <= ret; b_i++) {
    b_P = 0.0;
    for (k = 0; k < 6; k++) {
      boffset = k * trueCount + b_i;
      b_P += H_data[boffset % trueCount + trueCount * (boffset / trueCount)] *
             x_h[k];
    }
    b_A[b_i] = b_P;
  }
  aoffset = 0;
  tmp_size[0] = 1;
  ret = 0;
  for (b_i = 0; b_i < 6; b_i++) {
    if (u_h[b_i] != 0.0) {
      aoffset++;
      c_tmp_data[ret] = static_cast<signed char>(b_i + 1);
      ret++;
    }
  }
  tmp_size[1] = aoffset;
  if (trueCount == aoffset) {
    u_h[0] = in_data->GNSS.pos_ned[0];
    u_h[1] = in_data->GNSS.pos_ned[1];
    u_h[2] = in_data->GNSS.pos_ned[2];
    u_h[3] = in_data->SPEEDOMETER.speed;
    u_h[4] = 0.0;
    u_h[5] = 0.0;
    for (boffset = 0; boffset < aoffset; boffset++) {
      b_data[boffset] = u_h[c_tmp_data[boffset] - 1] - b_A[boffset];
    }
  } else {
    binary_expand_op(b_data, &ret, in_data, c_tmp_data, tmp_size, b_A,
                     &trueCount);
  }
  ret = K_size[1];
  std::memset(&b_y[0], 0, 15U * sizeof(double));
  for (k = 0; k < ret; k++) {
    aoffset = k * 15;
    for (b_i = 0; b_i < 15; b_i++) {
      b_y[b_i] += K_data[aoffset + b_i] * b_data[k];
    }
  }
  std::copy(&b_y[0], &b_y[9], &z[0]);
  //  Correct the navigation states using current perturbation estimates.
  for (b_i = 0; b_i < 6; b_i++) {
    z[b_i + 9] = delta_u_h[b_i] + b_y[b_i + 9];
    x_h[b_i] += z[b_i];
  }
  //  Convert quaternion to DCM
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //      Error correction of quaternion    %%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  Construct skew symetric matrx
  //  Correct the DCM matrix
  q2dcm(*(double(*)[4]) & x_h[6], Rb2t_tmp);
  b_Ts[0] = 1.0;
  b_Ts[1] = 0.0 - z[8];
  b_Ts[2] = 0.0 - (-z[7]);
  b_Ts[3] = 0.0 - (-z[8]);
  b_Ts[4] = 1.0;
  b_Ts[5] = 0.0 - z[6];
  b_Ts[6] = 0.0 - z[7];
  b_Ts[7] = 0.0 - (-z[6]);
  b_Ts[8] = 1.0;
  for (boffset = 0; boffset < 3; boffset++) {
    R = b_Ts[boffset];
    OMEGA_tmp = b_Ts[boffset + 3];
    Q = b_Ts[boffset + 6];
    for (F_tmp_tmp = 0; F_tmp_tmp < 3; F_tmp_tmp++) {
      b_R[boffset + 3 * F_tmp_tmp] = (R * Rb2t_tmp[3 * F_tmp_tmp] +
                                      OMEGA_tmp * Rb2t_tmp[3 * F_tmp_tmp + 1]) +
                                     Q * Rb2t_tmp[3 * F_tmp_tmp + 2];
    }
  }
  //  Calculte the corrected quaternions
  //
  //  function q=dcm2q(R)
  //  Function for transformation from directional cosine matrix to quaternions
  //  From Farrel pp 42.
  //  Edit: Isaac Skog, 2007-05-24
  b_P = 0.5 * std::sqrt(((b_R[0] + b_R[4]) + b_R[8]) + 1.0);
  x_h[6] = (b_R[5] - b_R[7]) / (4.0 * b_P);
  x_h[7] = (b_R[6] - b_R[2]) / (4.0 * b_P);
  x_h[8] = (b_R[1] - b_R[3]) / (4.0 * b_P);
  x_h[9] = b_P;
  for (b_i = 0; b_i < 6; b_i++) {
    out_data->delta_u_h[b_i] = z[b_i + 9];
  }
  //  Update the Kalman filter state covariance.
  std::memset(&b_I[0], 0, 225U * sizeof(signed char));
  ret = K_size[1];
  for (k = 0; k < 15; k++) {
    b_I[k + 15 * k] = 1;
    coffset = k * 15;
    boffset = k * trueCount;
    std::memset(&F[coffset], 0, 15U * sizeof(double));
    for (F_tmp_tmp = 0; F_tmp_tmp < ret; F_tmp_tmp++) {
      aoffset = F_tmp_tmp * 15;
      b_P = H_data[boffset + F_tmp_tmp];
      for (b_i = 0; b_i < 15; b_i++) {
        F_tmp = coffset + b_i;
        F[F_tmp] += K_data[aoffset + b_i] * b_P;
      }
    }
  }
  for (boffset = 0; boffset < 225; boffset++) {
    F[boffset] = static_cast<double>(b_I[boffset]) - F[boffset];
  }
  for (boffset = 0; boffset < 15; boffset++) {
    for (F_tmp_tmp = 0; F_tmp_tmp < 15; F_tmp_tmp++) {
      R = 0.0;
      for (ret = 0; ret < 15; ret++) {
        R += F[boffset + 15 * ret] * P[ret + 15 * F_tmp_tmp];
      }
      b_F[boffset + 15 * F_tmp_tmp] = R;
    }
  }
  std::copy(&b_F[0], &b_F[225], &P[0]);
  //  Save the data to the output data structure
  std::copy(&x_h[0], &x_h[10], &out_data->x_h[0]);
  for (k = 0; k < 15; k++) {
    out_data->diag_P[k] = P[k + 15 * k];
  }
}

//
// File trailer for GPSaidedINS.cpp
//
// [EOF]
//
