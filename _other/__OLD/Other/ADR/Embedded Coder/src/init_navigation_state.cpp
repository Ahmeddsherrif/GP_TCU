//
// File: init_navigation_state.cpp
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

// Include Files
#include "init_navigation_state.h"
#include "GPSaidedINS_types.h"
#include "rt_nonfinite.h"
#include "rt_defines.h"
#include <cmath>
#include <string.h>

// Function Declarations
static double rt_atan2d_snf(double u0, double u1);

// Function Definitions
//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_atan2d_snf(double u0, double u1)
{
  double y;
  if (std::isnan(u0) || std::isnan(u1)) {
    y = rtNaN;
  } else if (std::isinf(u0) && std::isinf(u1)) {
    int b_u0;
    int b_u1;
    if (u0 > 0.0) {
      b_u0 = 1;
    } else {
      b_u0 = -1;
    }
    if (u1 > 0.0) {
      b_u1 = 1;
    } else {
      b_u1 = -1;
    }
    y = std::atan2(static_cast<double>(b_u0), static_cast<double>(b_u1));
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = std::atan2(u0, u1);
  }
  return y;
}

//
// Calculate the roll and pitch
//
// Arguments    : const double u[1200]
//                const struct4_T *settings
//                double x_h[10]
// Return Type  : void
//
void init_navigation_state(const double u[1200], const struct4_T *settings,
                           double x_h[10])
{
  double f[6];
  double Rb2t_tmp;
  double absxk;
  double cp;
  double cr;
  double q_idx_3;
  double scale;
  double sr;
  double t;
  double y;
  int b_i;
  int i;
  for (i = 0; i < 6; i++) {
    f[i] = u[i % 6 + 6 * (i / 6)];
  }
  for (int k{0}; k < 99; k++) {
    int xoffset;
    xoffset = (k + 1) * 6;
    for (i = 0; i < 6; i++) {
      b_i = xoffset + i;
      f[i] += u[b_i % 6 + 6 * (b_i / 6)];
    }
  }
  for (b_i = 0; b_i < 6; b_i++) {
    f[b_i] /= 100.0;
  }
  scale = 3.3121686421112381E-170;
  absxk = std::abs(f[1]);
  if (absxk > 3.3121686421112381E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.3121686421112381E-170;
    y = t * t;
  }
  absxk = std::abs(f[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }
  //  Initial coordinate rotation matrix
  absxk = rt_atan2d_snf(-f[1], -f[2]);
  t = rt_atan2d_snf(f[0], scale * std::sqrt(y));
  //  function for calculation of the rotation matrix for
  //  rotaion from tangent frame to body frame.
  //  function R=Rt2b[roll,pitch,yaw];
  //  v_b=[u v d]'
  //  v_t=[N E D]'
  cr = std::cos(absxk);
  sr = std::sin(absxk);
  cp = std::cos(t);
  absxk = std::sin(t);
  t = std::cos(settings->init_heading);
  scale = std::sin(settings->init_heading);
  y = t * absxk;
  Rb2t_tmp = scale * absxk;
  //  Calculate quaternions
  //
  //  function q=dcm2q(R)
  //  Function for transformation from directional cosine matrix to quaternions
  //  From Farrel pp 42.
  //  Edit: Isaac Skog, 2007-05-24
  q_idx_3 =
      0.5 * std::sqrt(((t * cp + (t * cr + Rb2t_tmp * sr)) + cp * cr) + 1.0);
  //  Initial navigation state vector
  for (i = 0; i < 6; i++) {
    x_h[i] = 0.0;
  }
  x_h[6] = (cp * sr - (-t * sr + Rb2t_tmp * cr)) / (4.0 * q_idx_3);
  x_h[7] = ((scale * sr + y * cr) - (-absxk)) / (4.0 * q_idx_3);
  x_h[8] = (scale * cp - (-scale * cr + y * sr)) / (4.0 * q_idx_3);
  x_h[9] = q_idx_3;
}

//
// File trailer for init_navigation_state.cpp
//
// [EOF]
//
