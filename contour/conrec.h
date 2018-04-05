/**
 * @file   conrec.h
 * @author Jens Munk Hansen <jmh@debian9laptop.parknet.dk>
 * @date   Thu Mar 29 22:00:22 2018
 *
 * @brief
 *
 *
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void Contour(double **d,int ilb,int iub,int jlb,int jub,
             double *x,double *y,int nc,double *z,
             void (*ConrecLine)(double,double,double,double,int));

#ifdef __cplusplus
}
#endif
