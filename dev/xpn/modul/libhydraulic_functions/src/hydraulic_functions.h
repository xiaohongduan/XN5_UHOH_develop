//      hydraulic_functions.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HYDRAULIC_FUNCTIONS_H__
#define __HYDRAULIC_FUNCTIONS_H__

#include <expertn_modul_base.h>
#include <shp_look_up_table.h>

G_BEGIN_DECLS

#define HYDRAULIC_FUNCTIONS_TYPE				(hydraulic_functions_get_type())
#define HYDRAULIC_FUNCTIONS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			HYDRAULIC_FUNCTIONS_TYPE, hydraulic_functions))
#define HYDRAULIC_FUNCTIONS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			HYDRAULIC_FUNCTIONS_TYPE, hydraulic_functionsClass))
#define IS_HYDRAULIC_FUNCTIONS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			HYDRAULIC_FUNCTIONS_TYPE))
#define IS_HYDRAULIC_FUNCTIONS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			HYDRAULIC_FUNCTIONS_TYPE))

            
#define WATER_CONTENT(x) (*self->WCont)((double)x,\
        (double)pWL->fContAct,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2,\
        (double)pSW->fTau,\
        (double)pSW->fContSat_c,\
        (double)pSW->fContRes_c,\
        (double)pSW->fCondSat_c,\
        (double)pSW->fCondSat_nc,\
        pSW)

#define MATRIX_POTENTIAL(x) (float) (*self->MPotl)((double)pWL->fMatPotAct,\
        (double)x,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2,\
        (double)pSW->fTau,\
        (double)pSW->fContSat_c,\
        (double)pSW->fContRes_c,\
        (double)pSW->fCondSat_c,\
        (double)pSW->fCondSat_nc,\
        pSW)



typedef struct _hydraulic_functions			hydraulic_functions;
typedef struct _hydraulic_functionsClass		hydraulic_functionsClass;

struct _hydraulic_functions
{
	expertn_modul_base parent;
	/* add your public declarations here */
        int hydraulic_function_choice;
        
        int iCheckPointer;
        
    //SG 20180919
    double *shp_table_mat_pot;
	int		shp_table_mat_pot_len;
    double *shp_table_wat_cont;
	int		shp_table_wat_cont_len;
    double *shp_table_h_cond;
	int		shp_table_h_cond_len;
    double *shp_table_dif_wc;
	int		shp_table_dif_wc_len;
    
   	// organization:
	char *ini_filename;
    
    //Added by Hong on 20181212
	// Hydraulic Functions:
	double (*WCont)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2, 
                    double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
	double (*HCond)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2, 
                    double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
	double (*DWCap)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2, 
                    double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
    double (*MPotl)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2, 
                    double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
	
	//End of Hong
	

};


// public class member function:
G_MODULE_EXPORT int hydraulic_functions_set(hydraulic_functions *self,void *WCont,void *HCond, void *DWCap, void *MPotl);
G_MODULE_EXPORT int hydraulic_functions_set_Hutson_and_Cass(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_van_Genuchten_and_Mualem(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Brooks_and_Corey(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Brutsaert_and_Gardner(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Bimodal_van_Genuchten(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Brunswick_model(hydraulic_functions *self); //SG20200325

//SG20200325
G_MODULE_EXPORT int hydraulic_functions_shp_table_load(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_shp_table_run(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_shp_table_done(hydraulic_functions *self);

G_MODULE_EXPORT int hydraulic_functions_van_Genuchten_and_Mualem_run(hydraulic_functions *self);

struct _hydraulic_functionsClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType hydraulic_functions_get_type		(void);

G_END_DECLS

int shp_table_load_ini(hydraulic_functions *self);
int shp_table_load_config(hydraulic_functions *self);
int shp_table_set_soil(hydraulic_functions *self);

/*int shp_table_load_ini(expertn_standard_ini *self);
int shp_table_load_config(expertn_standard_ini *self);
int shp_table_set_soil(expertn_standard_ini *self);*/


#endif /* __HYDRAULIC_FUNCTIONS_H__ */

