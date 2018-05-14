//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       ???.h (formerly simplx9.h)
//
//  Contents:   "Old" NR-Solver class, use compiler flag ???
//
//
//
//------------------------------------------------------------------------------


#ifndef _simpl9_h
#define _simpl9_h

#include "BasicDefines.h"

//#include "nrutil.h" //die daraus benoetigten Funktionen stehen hier drin
int* ivector(long nl, long nh);
double** LP_matrix(long nrl, long nrh, long ncl, long nch);
double** convert_matrix(double* a, long nrl, long nrh, long ncl, long nch);
void free_vector(int* v, long nl, long nh);
void free_matrix(double** m, long nrl, long nrh, long ncl, long nch);

//Inputparameter:  m, n, m1, m2, m3, *icase, izrov[], iposv[]
//Outputparameter: a, icase, izrov, iposv
//liefert Anzahl der benoetigten Iterationen zurueck
int simplx(double** a, int m, int n, int m1, int m2, int m3, int* icase,
					int* izrov, int* iposv, double prp, int maxiter);
void simp1(double** a, int mm, int* ll, int nll, int iabf, int* kp,
					double* bmax);
void simp2(double** a, int n, int* l2, int nl2, int* ip, int kp, double* q1,
	double prp);
void simp3(double** a, int i1, int k1, int ip, int kp);


#endif

