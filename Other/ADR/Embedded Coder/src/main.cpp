#include <iostream>
#include "GPSaidedINS.h"
#include "GPSaidedINS_terminate.h"
#include "GPSaidedINS_types.h"
#include "init_filter.h"
#include "init_navigation_state.h"
#include "rt_nonfinite.h"
int main()
{ 
	struct4_T s;
	struct0_T in;
	double  u[1200] = {};
	double x_h[10] = {};
	double P[15 * 15] = {};
	double Q1[6 * 6] = {};
	double Q2[6 * 6] = {};
	double R[3 * 3] = {};
	double H[45] = {};
	double delta_u[6]= {};
	double t_prev = {};
	struct5_T out;
	init_navigation_state(u, &s, x_h);
	init_filter(&s, P, Q1, Q2, R, H);
	GPSaidedINS(&in, &s, x_h, delta_u, P, Q1, Q2, t_prev,&out);
	return 0;
}