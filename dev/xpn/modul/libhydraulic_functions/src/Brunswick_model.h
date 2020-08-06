#ifndef __BRUNSWICK_MODEL_H__
#define __BRUNSWICK_MODELP_H__

#include <expertn_modul_base.h>
#include <xsoil.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BRUNSWICK_HCond()                                            */
/*                                                                   */
/*  Function : Calculation of hydraulic unsaturated conductivity     */
/*             following Weber et al. (2019)     */
/*             HYDRUS 5.0                                            */
/*  Author   : sg, 13.09.2018                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BRUNSWICK_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2, 
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BRUNSWICK_WCont()                                            */
/*                                                                   */
/*  Function : Calculation of volumetric water content               */
/*             following Weber et al. (2019)         */
/*                                                                   */
/*  Author   : sg, 13.09.2018                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
               
double BRUNSWICK_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2, 
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
				

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BRUNSWICK_DWCap()                                          */
/*                                                                 */
/*  Function : Calculation of differential water capacity          */
/*             following Weber et al. (2019)       */
/*  Author   : sg, 13.09.2018                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BRUNSWICK_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2, 
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
				

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BRUNSWICK_MPotl()                                          */
/*  Function : Calculation of matric potential from                */
/*             a given volumetric water content                    */
/*             following Weber et al. (2019)       */
/*                                                                 */
/*  Autor    : sg, 13.09.2018                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BRUNSWICK_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2, 
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);



#endif
