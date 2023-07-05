//
// File: xgeqp3.cpp
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

// Include Files
#include "xgeqp3.h"
#include "rt_nonfinite.h"
#include "xnrm2.h"
#include <cmath>
#include <cstring>
#include <string.h>

// Function Declarations
static double rt_hypotd_snf(double u0, double u1);

// Function Definitions
//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_hypotd_snf(double u0, double u1)
{
  double a;
  double y;
  a = std::abs(u0);
  y = std::abs(u1);
  if (a < y) {
    a /= y;
    y *= std::sqrt(a * a + 1.0);
  } else if (a > y) {
    y /= a;
    y = a * std::sqrt(y * y + 1.0);
  } else if (!std::isnan(y)) {
    y = a * 1.4142135623730951;
  }
  return y;
}

//
// Arguments    : double A_data[]
//                const int A_size[2]
//                double tau_data[]
//                int *tau_size
//                int jpvt_data[]
//                int jpvt_size[2]
// Return Type  : void
//
namespace coder {
namespace internal {
namespace lapack {
void xgeqp3(double A_data[], const int A_size[2], double tau_data[],
            int *tau_size, int jpvt_data[], int jpvt_size[2])
{
  double vn1_data[6];
  double vn2_data[6];
  double work_data[6];
  int ix;
  int m;
  int n;
  m = A_size[0];
  n = A_size[1];
  ix = A_size[0];
  *tau_size = A_size[1];
  if (ix <= *tau_size) {
    *tau_size = ix;
  }
  if (0 <= *tau_size - 1) {
    std::memset(&tau_data[0], 0, *tau_size * sizeof(double));
  }
  if ((A_size[0] == 0) || (A_size[1] == 0) || (*tau_size < 1)) {
    jpvt_size[0] = 1;
    jpvt_size[1] = A_size[1];
    ix = A_size[1];
    if (0 <= ix - 1) {
      std::memset(&jpvt_data[0], 0, ix * sizeof(int));
    }
    for (int j{0}; j < n; j++) {
      jpvt_data[j] = j + 1;
    }
  } else {
    double d;
    int j;
    int lastc;
    int ma;
    jpvt_size[0] = 1;
    jpvt_size[1] = A_size[1];
    ix = A_size[1];
    if (0 <= ix - 1) {
      std::memset(&jpvt_data[0], 0, ix * sizeof(int));
    }
    for (lastc = 0; lastc < n; lastc++) {
      jpvt_data[lastc] = lastc + 1;
    }
    ma = A_size[0];
    ix = A_size[1];
    if (0 <= ix - 1) {
      std::memset(&work_data[0], 0, ix * sizeof(double));
    }
    ix = A_size[1];
    if (0 <= ix - 1) {
      std::memset(&vn1_data[0], 0, ix * sizeof(double));
    }
    ix = A_size[1];
    if (0 <= ix - 1) {
      std::memset(&vn2_data[0], 0, ix * sizeof(double));
    }
    for (j = 0; j < n; j++) {
      d = blas::xnrm2(m, A_data, j * ma + 1);
      vn1_data[j] = d;
      vn2_data[j] = d;
    }
    for (int i{0}; i < *tau_size; i++) {
      double atmp;
      double s;
      double smax;
      int b_i;
      int ii;
      int ip1;
      int knt;
      int mmi;
      int nmi;
      int pvt;
      ip1 = i + 2;
      j = i * ma;
      ii = j + i;
      nmi = n - i;
      mmi = m - i;
      if (nmi < 1) {
        ix = -1;
      } else {
        ix = 0;
        if (nmi > 1) {
          smax = std::abs(vn1_data[i]);
          for (lastc = 2; lastc <= nmi; lastc++) {
            s = std::abs(vn1_data[(i + lastc) - 1]);
            if (s > smax) {
              ix = lastc - 1;
              smax = s;
            }
          }
        }
      }
      pvt = i + ix;
      if (pvt != i) {
        ix = pvt * ma;
        for (lastc = 0; lastc < m; lastc++) {
          knt = ix + lastc;
          smax = A_data[knt];
          b_i = j + lastc;
          A_data[knt] = A_data[b_i];
          A_data[b_i] = smax;
        }
        ix = static_cast<signed char>(jpvt_data[pvt]);
        jpvt_data[pvt] = static_cast<signed char>(jpvt_data[i]);
        jpvt_data[i] = ix;
        vn1_data[pvt] = vn1_data[i];
        vn2_data[pvt] = vn2_data[i];
      }
      if (i + 1 < m) {
        atmp = A_data[ii];
        ix = ii + 2;
        tau_data[i] = 0.0;
        if (mmi > 0) {
          smax = blas::xnrm2(mmi - 1, A_data, ii + 2);
          if (smax != 0.0) {
            s = rt_hypotd_snf(A_data[ii], smax);
            if (A_data[ii] >= 0.0) {
              s = -s;
            }
            if (std::abs(s) < 1.0020841800044864E-292) {
              knt = -1;
              b_i = ii + mmi;
              do {
                knt++;
                for (lastc = ix; lastc <= b_i; lastc++) {
                  A_data[lastc - 1] *= 9.9792015476736E+291;
                }
                s *= 9.9792015476736E+291;
                atmp *= 9.9792015476736E+291;
              } while (!(std::abs(s) >= 1.0020841800044864E-292));
              s = rt_hypotd_snf(atmp, blas::xnrm2(mmi - 1, A_data, ii + 2));
              if (atmp >= 0.0) {
                s = -s;
              }
              tau_data[i] = (s - atmp) / s;
              smax = 1.0 / (atmp - s);
              for (lastc = ix; lastc <= b_i; lastc++) {
                A_data[lastc - 1] *= smax;
              }
              for (lastc = 0; lastc <= knt; lastc++) {
                s *= 1.0020841800044864E-292;
              }
              atmp = s;
            } else {
              tau_data[i] = (s - A_data[ii]) / s;
              smax = 1.0 / (A_data[ii] - s);
              b_i = ii + mmi;
              for (lastc = ix; lastc <= b_i; lastc++) {
                A_data[lastc - 1] *= smax;
              }
              atmp = s;
            }
          }
        }
        A_data[ii] = atmp;
      } else {
        tau_data[i] = 0.0;
      }
      if (i + 1 < n) {
        int jA;
        int lastv;
        atmp = A_data[ii];
        A_data[ii] = 1.0;
        jA = (ii + ma) + 1;
        if (tau_data[i] != 0.0) {
          boolean_T exitg2;
          lastv = mmi - 1;
          ix = (ii + mmi) - 1;
          while ((lastv + 1 > 0) && (A_data[ix] == 0.0)) {
            lastv--;
            ix--;
          }
          lastc = nmi - 2;
          exitg2 = false;
          while ((!exitg2) && (lastc + 1 > 0)) {
            int exitg1;
            ix = jA + lastc * ma;
            j = ix;
            do {
              exitg1 = 0;
              if (j <= ix + lastv) {
                if (A_data[j - 1] != 0.0) {
                  exitg1 = 1;
                } else {
                  j++;
                }
              } else {
                lastc--;
                exitg1 = 2;
              }
            } while (exitg1 == 0);
            if (exitg1 == 1) {
              exitg2 = true;
            }
          }
        } else {
          lastv = -1;
          lastc = -1;
        }
        if (lastv + 1 > 0) {
          if (lastc + 1 != 0) {
            if (0 <= lastc) {
              std::memset(&work_data[0], 0, (lastc + 1) * sizeof(double));
            }
            knt = 0;
            b_i = jA + ma * lastc;
            for (pvt = jA; ma < 0 ? pvt >= b_i : pvt <= b_i; pvt += ma) {
              smax = 0.0;
              ix = pvt + lastv;
              for (j = pvt; j <= ix; j++) {
                smax += A_data[j - 1] * A_data[(ii + j) - pvt];
              }
              work_data[knt] += smax;
              knt++;
            }
          }
          if (!(-tau_data[i] == 0.0)) {
            for (j = 0; j <= lastc; j++) {
              d = work_data[j];
              if (d != 0.0) {
                smax = d * -tau_data[i];
                b_i = lastv + jA;
                for (knt = jA; knt <= b_i; knt++) {
                  A_data[knt - 1] += A_data[(ii + knt) - jA] * smax;
                }
              }
              jA += ma;
            }
          }
        }
        A_data[ii] = atmp;
      }
      for (j = ip1; j <= n; j++) {
        ix = i + (j - 1) * ma;
        d = vn1_data[j - 1];
        if (d != 0.0) {
          smax = std::abs(A_data[ix]) / d;
          smax = 1.0 - smax * smax;
          if (smax < 0.0) {
            smax = 0.0;
          }
          s = d / vn2_data[j - 1];
          s = smax * (s * s);
          if (s <= 1.4901161193847656E-8) {
            if (i + 1 < m) {
              d = blas::xnrm2(mmi - 1, A_data, ix + 2);
              vn1_data[j - 1] = d;
              vn2_data[j - 1] = d;
            } else {
              vn1_data[j - 1] = 0.0;
              vn2_data[j - 1] = 0.0;
            }
          } else {
            vn1_data[j - 1] = d * std::sqrt(smax);
          }
        }
      }
    }
  }
}

} // namespace lapack
} // namespace internal
} // namespace coder

//
// File trailer for xgeqp3.cpp
//
// [EOF]
//
