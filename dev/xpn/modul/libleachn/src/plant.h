//      leachn_plant.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __LEACHN_PLANT_H__
#define __LEACHN_PLANT_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define LEACHN_PLANT_TYPE				(leachn_plant_get_type())
#define LEACHN_PLANT(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			LEACHN_PLANT_TYPE, leachn_plant))
#define LEACHN_PLANT_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			LEACHN_PLANT_TYPE, leachn_plantClass))
#define IS_LEACHN_PLANT(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			LEACHN_PLANT_TYPE))
#define IS_LEACHN_PLANT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			LEACHN_PLANT_TYPE))

typedef struct _leachn_plant			leachn_plant;
typedef struct _leachn_plantClass		leachn_plantClass;

struct _leachn_plant
{
	expertn_modul_base parent;
	/* add your public declarations here */	
	int __ERROR;
	int   iIsConstRootDens;
	double  fRootPot;			              				/*   [mm]   */
	int nroot;
	int use_dist_func; // 0 == noah; 1 == cos
    char *plant_measurement_filename;
	int plant_iAlreadyInitialised;
	int IsConstRootDens;
	int iCropCover;
	double afConstRootDens[MAXSCHICHT];
};



#define GROWING_TIME ((pPM != NULL) && (pPM->pNext != NULL)\
                     && ((xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY)>=0)) && ((xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY)<0))\
                     &&  (pPM->pNext->fDevStage > pPM->fDevStage))

// public class member function:
G_MODULE_EXPORT int leachn_plant_load(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_act_transpiration(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_act_biomass_growth(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_RootMaizeLeachEP(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_RootMaizeLeach(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_LeafMaizeLeach(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_NUptake(leachn_plant *self);
G_MODULE_EXPORT int leachn_plant_done(leachn_plant *self);

int load_plant_measurement_data(leachn_plant *self,char *filename);
int leachn_plant_InitiateLeachModel(leachn_plant *self);


struct _leachn_plantClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType leachn_plant_get_type (void);

G_END_DECLS

#endif /* __LEACHN_PLANT_H__ */
