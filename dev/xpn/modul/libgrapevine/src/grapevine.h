//      grapevine.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __GRAPEVINE_H__
#define __GRAPEVINE_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define GRAPEVINE_TYPE				(grapevine_get_type())
#define GRAPEVINE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			GRAPEVINE_TYPE, grapevine))
#define GRAPEVINE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			GRAPEVINE_TYPE, grapevineClass))
#define IS_GRAPEVINE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			GRAPEVINE_TYPE))
#define IS_GRAPEVINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			GRAPEVINE_TYPE))

typedef struct _grapevine			grapevine;
typedef struct _grapevineClass		grapevineClass;

struct _grapevine
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
	
	double fQ10c;
	
	// Harvest Dates can be same day in each year or (later) specified for each year
	double iDOYDormCalcBegin, iDOYDormCalcLatest, iDOYHarvest, iDOYDormBreakLatest;
	double fTempSumDormancy, fTempSumVeraison;
	int bDormancyThisYear;
	
	int __LOAD_DONE;
	int __DONE_DONE;
};


// public class member function:
G_MODULE_EXPORT int grapevine_load(grapevine *self);
G_MODULE_EXPORT int grapevine_done(grapevine *self);
G_MODULE_EXPORT int grapevine_phenological_development(grapevine *self);
G_MODULE_EXPORT int grapevine_biomass_growth(grapevine *self);
G_MODULE_EXPORT int grapevine_canopy_formation(grapevine *self);
G_MODULE_EXPORT int grapevine_nitrogen_demand(grapevine *self);
G_MODULE_EXPORT int grapevine_nitrogen_uptake(grapevine *self);
G_MODULE_EXPORT int grapevine_potential_transpiration(grapevine *self);
G_MODULE_EXPORT int grapevine_actual_transpiration(grapevine *self);
G_MODULE_EXPORT int grapevine_photosynthesis(grapevine *self);
G_MODULE_EXPORT int grapevine_root_length_growth(grapevine *self);
G_MODULE_EXPORT int grapevine_crop_senescence(grapevine *self);


struct _grapevineClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType grapevine_get_type (void);

G_END_DECLS

#endif /* __GRAPEVINE_H__ */

