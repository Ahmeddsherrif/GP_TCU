//
// File: q2dcm.cpp
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

// Include Files
#include "q2dcm.h"
#include "rt_nonfinite.h"
#include <string.h>

// Function Definitions
//
// function R=q2dcm(q)
//  Function for transformation from quaternions to directional cosine matrix
//  Farell pp.41
//  Edit: Isaac Skog, 2007-05-24
//
// Arguments    : const double q[4]
//                double R[9]
// Return Type  : void
//
void q2dcm(const double q[4], double R[9])
{
  double p_idx_0;
  double p_idx_1;
  double p_idx_2;
  double p_idx_4;
  double p_idx_5;
  p_idx_0 = q[0] * q[0];
  p_idx_1 = q[1] * q[1];
  p_idx_2 = q[2] * q[2];
  p_idx_4 = p_idx_1 + p_idx_2;
  p_idx_5 = (p_idx_0 + q[3] * q[3]) + p_idx_4;
  if (p_idx_5 != 0.0) {
    p_idx_5 = 2.0 / p_idx_5;
  } else {
    p_idx_5 = 0.0;
  }
  R[0] = 1.0 - p_idx_4 * p_idx_5;
  R[4] = 1.0 - p_idx_5 * (p_idx_0 + p_idx_2);
  R[8] = 1.0 - p_idx_5 * (p_idx_0 + p_idx_1);
  p_idx_0 = q[0] * p_idx_5;
  p_idx_1 = q[1] * p_idx_5;
  p_idx_4 = q[2] * p_idx_5 * q[3];
  p_idx_5 = p_idx_0 * q[1];
  R[3] = p_idx_5 - p_idx_4;
  R[1] = p_idx_4 + p_idx_5;
  p_idx_4 = p_idx_1 * q[3];
  p_idx_5 = p_idx_0 * q[2];
  R[6] = p_idx_4 + p_idx_5;
  R[2] = p_idx_5 - p_idx_4;
  p_idx_4 = p_idx_0 * q[3];
  p_idx_5 = p_idx_1 * q[2];
  R[7] = p_idx_5 - p_idx_4;
  R[5] = p_idx_4 + p_idx_5;
  //  R(1,2)=2*(q(1)*q(2)-q(3)*q(4));
  //  R(1,3)=2*(q(1)*q(3)+q(2)*q(4));
  //
  //  % Row 2
  //  R(2,1)=2*(q(1)*q(2)+q(3)*q(4));
  //  R(2,2)=q(2)^2+q(4)^2-q(1)^2-q(3)^2;
  //  R(2,3)=2*(q(2)*q(3)-q(1)*q(4));
  //
  //  % Row 3
  //  R(3,1)=2*(q(1)*q(3)-q(2)*q(4));
  //  R(3,2)=2*(q(2)*q(3)+q(1)*q(4));
  //  R(3,3)=q(3)^2+q(4)^2-q(1)^2-q(2)^2;
  //
  //  % Okej, men kan förbätrras, se pp 41 Farrel
}

//
// File trailer for q2dcm.cpp
//
// [EOF]
//
