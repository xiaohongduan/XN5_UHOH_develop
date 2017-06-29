//      balance.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __BALANCE_H__
#define __BALANCE_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define BALANCE_TYPE				(balance_get_type())
#define BALANCE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			BALANCE_TYPE, balance))
#define BALANCE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			BALANCE_TYPE, balanceClass))
#define IS_BALANCE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			BALANCE_TYPE))
#define IS_BALANCE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			BALANCE_TYPE))

typedef struct _balance			balance;
typedef struct _balanceClass		balanceClass;

struct _balance
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double fCumRain,fPotDay,fPotCumEvap,fRunOffDay,fRunOffCum,fLeaching,fCumLeaching;
	double fWaterStorage; // Water Storage in mm in soil profile
	double fWaterStoragedt;	
	double fBalance,fBalanceA;
	double fCumBalance;
	double waterstorage_init;
	double fWaterInput, fWaterOutput;
	double fActETCum;
	double fProfil, fProfilStart;
	
	// Atmosphere:
	double cum_dust1,cum_dust2,cum_dust3,cum_dust4,cum_dust5;
};


// public class member function:

G_MODULE_EXPORT int balance_load(balance *self);
G_MODULE_EXPORT int balance_run(balance *self);

struct _balanceClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType balance_get_type (void);


G_END_DECLS

#endif /* __BALANCE_H__ */

