#ifndef __SHP_LOOKUP_TABLE_H__
#define __SHP_LOOKUP_TABLE_H__

#include <expertn_modul_base.h>
#include <xsoil.h>

G_BEGIN_DECLS

/*#define SHP_TABLE_TYPE				shp_table_get_type())
#define SHP_TABLE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			SHP_TABLE_TYPE, shp_table))
#define SHP_TABLE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			SHP_TABLE_TYPE, shp_tableClass))
#define IS_SHP_TABLE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			SHP_TABLE_TYPE))
#define IS_SHP_TABLE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			SHP_TABLE_TYPE))*/


/*//SG20200325
G_MODULE_EXPORT int hydraulic_functions_shp_table_load(shp_table *self);
G_MODULE_EXPORT int hydraulic_functions_shp_table_run(shp_table *self);
G_MODULE_EXPORT int hydraulic_functions_shp_table_done(shp_table *self);*/

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
	
  	double SimTime; // ?
	char *ini_filename; // ?
	
	int __ERROR; //added by Hong on 20180124


#define GET_INI_INT_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_integer_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\

#define GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_double_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\

#define GET_INI_DOUBLE_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(double)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = std_value;\
					}\
			} else\
			{\
				GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key);\
			}\
	}\
	
#define CHECK_LEN(var1,var2)\
	{\
		if (var1!=var2)\
			{\
				gchar *S;\
				S = g_strdup_printf  ("%s is not %s. Check your configuration in file: %s!",#var1,#var2,filename);\
				PRINT_ERROR(S);\
				g_free(S);\
				return -1;\
			}\
	}\
	

double SHP_AFGENERATOR(double fInput, RESPONSE* pResp);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_HCond()                                                                     */
/*                                                                                                                       */
/*  Function : Read tabular values of soil hydraulic conductivity           */
/*                     versus soil matric potential                                                  */
/*                                                                                                                       */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double SHP_LT_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,    
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_WCont()                                                                     */
/*                                                                                                                       */
/*  Function : Read tabular values of volumetric soil waer content      */
/*                     versus soil matric potential                                                  */
/*                                                                                                                       */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++*/
               
double SHP_LT_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,    
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
				

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_DWCap()                                                                    */
/*                                                                                                                       */
/*  Function : Read tabular values of differential water capacity           */
/*                     versus soil matric potential                                                  */
/*                                                                                                                       */ 
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double SHP_LT_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,    
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);
				


/*++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_MPotl()                                                                    */
/*                                                                                                                    */
/*  Function : Read tabular values of soil matric potential                   */
/*                                                                                                                   */ 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double SHP_LT_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,    
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW);


//SG 20180919
int array_length(int var1,int var2);

#endif

