//
// File: mrdivide_helper.cpp
//
// MATLAB Coder version            : 5.3
// C/C++ source code generated on  : 28-Feb-2023 04:54:20
//

// Include Files
#include "mrdivide_helper.h"
#include "GPSaidedINS_types.h"
#include "rt_nonfinite.h"
#include "xgeqp3.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string.h>

// Function Definitions
//
// Arguments    : double K_data[]
//                int K_size[2]
//                const double C_data[]
//                const int C_size[2]
//                double a
//                const signed char b[9]
//                const struct4_T *settings
//                double b_a
//                const signed char b_b[4]
//                const signed char B_tmp_data[]
//                const int *B_tmp_size
// Return Type  : void
//
void binary_expand_op(double K_data[], int K_size[2], const double C_data[],
                      const int C_size[2], double a, const signed char b[9],
                      const struct4_T *settings, double b_a,
                      const signed char b_b[4], const signed char B_tmp_data[],
                      const int *B_tmp_size)
{
  double b_C_data[36];
  double c_a[36];
  int b_C_size[2];
  int a_tmp;
  int aux_0_1;
  int aux_1_1;
  int i;
  int loop_ub;
  int stride_0_1;
  int stride_1_0;
  int stride_1_1;
  for (i = 0; i < 3; i++) {
    c_a[6 * i] = a * static_cast<double>(b[3 * i]);
    a_tmp = 6 * (i + 3);
    c_a[a_tmp] = 0.0;
    c_a[6 * i + 1] = a * static_cast<double>(b[3 * i + 1]);
    c_a[a_tmp + 1] = 0.0;
    c_a[6 * i + 2] = a * static_cast<double>(b[3 * i + 2]);
    c_a[a_tmp + 2] = 0.0;
    c_a[6 * i + 3] = 0.0;
  }
  c_a[21] = settings->sigma_speed * settings->sigma_speed;
  c_a[27] = 0.0;
  c_a[33] = 0.0;
  for (i = 0; i < 4; i++) {
    c_a[6 * i + 4] = 0.0;
    c_a[6 * i + 5] = 0.0;
  }
  c_a[28] = b_a * static_cast<double>(b_b[0]);
  c_a[29] = b_a * static_cast<double>(b_b[1]);
  c_a[34] = b_a * static_cast<double>(b_b[2]);
  c_a[35] = b_a * static_cast<double>(b_b[3]);
  if (*B_tmp_size == 1) {
    b_C_size[0] = C_size[0];
    b_C_size[1] = C_size[1];
  } else {
    b_C_size[0] = *B_tmp_size;
    b_C_size[1] = *B_tmp_size;
  }
  a_tmp = (C_size[0] != 1);
  stride_0_1 = (C_size[1] != 1);
  stride_1_0 = (*B_tmp_size != 1);
  stride_1_1 = (*B_tmp_size != 1);
  aux_0_1 = 0;
  aux_1_1 = 0;
  if (*B_tmp_size == 1) {
    loop_ub = C_size[1];
  } else {
    loop_ub = *B_tmp_size;
  }
  for (i = 0; i < loop_ub; i++) {
    int b_loop_ub;
    if (*B_tmp_size == 1) {
      b_loop_ub = C_size[0];
    } else {
      b_loop_ub = *B_tmp_size;
    }
    for (int i1{0}; i1 < b_loop_ub; i1++) {
      b_C_data[i1 + b_C_size[0] * i] =
          C_data[i1 * a_tmp + C_size[0] * aux_0_1] +
          c_a[(B_tmp_data[i1 * stride_1_0] + 6 * (B_tmp_data[aux_1_1] - 1)) -
              1];
    }
    aux_1_1 += stride_1_1;
    aux_0_1 += stride_0_1;
  }
  coder::internal::mrdiv(K_data, K_size, b_C_data, b_C_size);
}

//
// Arguments    : double A_data[]
//                int A_size[2]
//                const double B_data[]
//                const int B_size[2]
// Return Type  : void
//
namespace coder {
namespace internal {
void mrdiv(double A_data[], int A_size[2], const double B_data[],
           const int B_size[2])
{
  double Y_data[90];
  double b_B_data[90];
  double b_A_data[36];
  double tau_data[6];
  int jpvt_data[6];
  int b_A_size[2];
  int jpvt_size[2];
  int minmn;
  if ((A_size[1] == 0) || ((B_size[0] == 0) || (B_size[1] == 0))) {
    int maxmn;
    A_size[0] = 15;
    A_size[1] = static_cast<signed char>(B_size[0]);
    maxmn = static_cast<signed char>(B_size[0]);
    for (int i{0}; i < maxmn; i++) {
      std::memset(&A_data[i * 15], 0, 15U * sizeof(double));
    }
  } else if (B_size[0] == B_size[1]) {
    double tol;
    int b_i;
    int i;
    int i1;
    int j;
    int k;
    int ldap1;
    int m;
    int maxmn;
    int mn;
    int n;
    int u1;
    n = B_size[1];
    maxmn = B_size[0] * B_size[1];
    if (0 <= maxmn - 1) {
      std::copy(&B_data[0], &B_data[maxmn], &b_A_data[0]);
    }
    minmn = B_size[1];
    jpvt_data[0] = 1;
    maxmn = 1;
    for (k = 2; k <= minmn; k++) {
      maxmn++;
      jpvt_data[k - 1] = maxmn;
    }
    ldap1 = B_size[1];
    minmn = B_size[1] - 1;
    u1 = B_size[1];
    if (minmn <= u1) {
      u1 = minmn;
    }
    for (j = 0; j < u1; j++) {
      int jj;
      int jp1j;
      int rankA;
      rankA = n - j;
      mn = j * (n + 1);
      jj = j * (ldap1 + 1);
      jp1j = mn + 2;
      if (rankA < 1) {
        minmn = -1;
      } else {
        minmn = 0;
        if (rankA > 1) {
          tol = std::abs(b_A_data[jj]);
          for (k = 2; k <= rankA; k++) {
            double wj;
            wj = std::abs(b_A_data[(mn + k) - 1]);
            if (wj > tol) {
              minmn = k - 1;
              tol = wj;
            }
          }
        }
      }
      if (b_A_data[jj + minmn] != 0.0) {
        if (minmn != 0) {
          maxmn = j + minmn;
          jpvt_data[j] = maxmn + 1;
          for (k = 0; k < n; k++) {
            minmn = k * n;
            m = j + minmn;
            tol = b_A_data[m];
            minmn += maxmn;
            b_A_data[m] = b_A_data[minmn];
            b_A_data[minmn] = tol;
          }
        }
        i = jj + rankA;
        for (b_i = jp1j; b_i <= i; b_i++) {
          b_A_data[b_i - 1] /= b_A_data[jj];
        }
      }
      minmn = mn + n;
      maxmn = jj + ldap1;
      for (m = 0; m <= rankA - 2; m++) {
        tol = b_A_data[minmn + m * n];
        if (tol != 0.0) {
          i = maxmn + 2;
          i1 = rankA + maxmn;
          for (mn = i; mn <= i1; mn++) {
            b_A_data[mn - 1] += b_A_data[((jj + mn) - maxmn) - 1] * -tol;
          }
        }
        maxmn += n;
      }
    }
    if (A_size[1] != 0) {
      for (j = 0; j < n; j++) {
        minmn = 15 * j - 1;
        maxmn = n * j;
        for (k = 0; k < j; k++) {
          m = 15 * k;
          i = k + maxmn;
          if (b_A_data[i] != 0.0) {
            for (b_i = 0; b_i < 15; b_i++) {
              i1 = (b_i + minmn) + 1;
              A_data[i1] -= b_A_data[i] * A_data[b_i + m];
            }
          }
        }
        tol = 1.0 / b_A_data[j + maxmn];
        for (b_i = 0; b_i < 15; b_i++) {
          i = (b_i + minmn) + 1;
          A_data[i] *= tol;
        }
      }
    }
    if (A_size[1] != 0) {
      for (j = n; j >= 1; j--) {
        minmn = 15 * (j - 1) - 1;
        maxmn = n * (j - 1) - 1;
        i = j + 1;
        for (k = i; k <= n; k++) {
          m = 15 * (k - 1);
          i1 = k + maxmn;
          if (b_A_data[i1] != 0.0) {
            for (b_i = 0; b_i < 15; b_i++) {
              mn = (b_i + minmn) + 1;
              A_data[mn] -= b_A_data[i1] * A_data[b_i + m];
            }
          }
        }
      }
    }
    i = B_size[1] - 1;
    for (j = i; j >= 1; j--) {
      i1 = jpvt_data[j - 1];
      if (i1 != j) {
        for (b_i = 0; b_i < 15; b_i++) {
          minmn = b_i + 15 * (j - 1);
          tol = A_data[minmn];
          mn = b_i + 15 * (i1 - 1);
          A_data[minmn] = A_data[mn];
          A_data[mn] = tol;
        }
      }
    }
  } else {
    double tol;
    int b_i;
    int i;
    int i1;
    int j;
    int jj;
    int jp1j;
    int k;
    int m;
    int maxmn;
    int mn;
    int rankA;
    jj = A_size[1];
    maxmn = A_size[1];
    for (i = 0; i < 15; i++) {
      for (i1 = 0; i1 < maxmn; i1++) {
        b_B_data[i1 + jj * i] = A_data[i + 15 * i1];
      }
    }
    b_A_size[0] = B_size[1];
    b_A_size[1] = B_size[0];
    maxmn = B_size[0];
    for (i = 0; i < maxmn; i++) {
      minmn = B_size[1];
      for (i1 = 0; i1 < minmn; i1++) {
        b_A_data[i1 + b_A_size[0] * i] = B_data[i + B_size[0] * i1];
      }
    }
    lapack::xgeqp3(b_A_data, b_A_size, tau_data, &minmn, jpvt_data, jpvt_size);
    rankA = 0;
    if (b_A_size[0] < b_A_size[1]) {
      minmn = b_A_size[0];
      maxmn = b_A_size[1];
    } else {
      minmn = b_A_size[1];
      maxmn = b_A_size[0];
    }
    if (minmn > 0) {
      tol = 2.2204460492503131E-15 * static_cast<double>(maxmn) *
            std::abs(b_A_data[0]);
      while ((rankA < minmn) &&
             (!(std::abs(b_A_data[rankA + b_A_size[0] * rankA]) <= tol))) {
        rankA++;
      }
    }
    jp1j = static_cast<signed char>(b_A_size[1]);
    maxmn = static_cast<signed char>(b_A_size[1]) * 15;
    if (0 <= maxmn - 1) {
      std::memset(&Y_data[0], 0, maxmn * sizeof(double));
    }
    m = b_A_size[0];
    minmn = b_A_size[0];
    mn = b_A_size[1];
    if (minmn <= mn) {
      mn = minmn;
    }
    for (j = 0; j < mn; j++) {
      if (tau_data[j] != 0.0) {
        i = j + 2;
        for (k = 0; k < 15; k++) {
          double wj;
          maxmn = jj * k;
          minmn = j + maxmn;
          tol = b_B_data[minmn];
          wj = tol;
          for (b_i = i; b_i <= m; b_i++) {
            wj += b_A_data[(b_i + b_A_size[0] * j) - 1] *
                  b_B_data[(b_i + maxmn) - 1];
          }
          wj *= tau_data[j];
          if (wj != 0.0) {
            b_B_data[minmn] = tol - wj;
            for (b_i = i; b_i <= m; b_i++) {
              minmn = (b_i + maxmn) - 1;
              b_B_data[minmn] -= b_A_data[(b_i + b_A_size[0] * j) - 1] * wj;
            }
          }
        }
      }
    }
    for (k = 0; k < 15; k++) {
      for (b_i = 0; b_i < rankA; b_i++) {
        Y_data[(jpvt_data[b_i] + jp1j * k) - 1] = b_B_data[b_i + jj * k];
      }
      for (j = rankA; j >= 1; j--) {
        minmn = jp1j * k;
        maxmn = (jpvt_data[j - 1] + minmn) - 1;
        m = b_A_size[0] * (j - 1);
        Y_data[maxmn] /= b_A_data[(j + m) - 1];
        for (b_i = 0; b_i <= j - 2; b_i++) {
          maxmn = (jpvt_data[b_i] + minmn) - 1;
          Y_data[maxmn] -=
              Y_data[(jpvt_data[j - 1] + jp1j * k) - 1] * b_A_data[b_i + m];
        }
      }
    }
    A_size[0] = 15;
    A_size[1] = static_cast<signed char>(b_A_size[1]);
    for (i = 0; i < jp1j; i++) {
      for (i1 = 0; i1 < 15; i1++) {
        A_data[i1 + 15 * i] = Y_data[i + jp1j * i1];
      }
    }
  }
}

} // namespace internal
} // namespace coder

//
// File trailer for mrdivide_helper.cpp
//
// [EOF]
//
