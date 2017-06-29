#include "libtreemix.h"
#include <math.h>

int libtreemix_CanopyFormation(libtreemix *self)
{
	/* Description */
	/*
	 * Computes the distribution of increment to either height growth, 
	 * or diameter growth, or both, depending on crown competition and current HD ratio.
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTCARBON		pPlC = xpn->pPl->pPltCarbon;
	//PPLTNITROGEN 	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	//PWEATHER 	pWe  = xpn->pCl->pWeather;
	
	/* Variables */
	int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double IncrSt;	// stem increment [tC/tree*yr]
	double a;		// RedFac (for PGM module);
		
	/*Functions*/
	extern int libtreemix_set_tree_geometry(libtreemix *self, int i);	//StartValues.c
	//extern int libtreemix_set_tree_morphology(libtreemix *self, int i);
	int libtreemix_geometric_relations(libtreemix *self, int i);
	
	/****************************************************************************************************************/
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		libtreemix_set_tree_geometry(self, i);
		libtreemix_geometric_relations(self, i);
		
		// eq. 102
		a = (M_PI/4.0)*self->plant[i].CWdDens*self->plant[i].Form;											
		
		// eq. 103, CWdMass is not being used, since the woodmass is calculated
		// IncrSt must be equal to the stem increment computed from the total wood increment after accounting for stem number
		// , deadwood losses, underground wood fraction, and branch fraction
		// [tC/tree*yr]
		IncrSt = ((self->plant[i].Increment - self->plant[i].DeadWdLoss * self->plant[i].CWdWeight/1000.0)
				/  self->plant[i].density) * (1.0-self->plant[i].UgWdFr)*(1.0-self->plant[i].BrWdFr);									
		self->plant[i].IncrSt = IncrSt;
	
		if(self->conf.Tree_Geometry == 1)
		{
			// no crowding, H/D is 
			if((self->silv[i].Crowding == 0.0)&&(self->plant[i].HD <= self->plant[i].HDmin))
			{
				self->plant[i].IncrD = IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2.0));		// eq. 105
				self->plant[i].IncrH = self->plant[i].HD*self->plant[i].IncrD;							// eq. 104
				self->plant[i].CanopyFormation = 1.0;
			}
			else if((self->silv[i].Crowding == 0.0)&&(self->plant[i].HD > self->plant[i].HDmin))
			{
				self->plant[i].IncrD = IncrSt/(2.0*a*self->plant[i].D*self->plant[i].H);				// eq. 107
				self->plant[i].IncrH = 0.0;																// eq. 106
				self->plant[i].CanopyFormation = 2.0;
			}
			else if((self->silv[i].Crowding == 1.0)&&(self->plant[i].HD < self->plant[i].HDmax))
			{
				self->plant[i].IncrD = 0.0;																// eq. 108
				self->plant[i].IncrH = IncrSt/(a*pow(self->plant[i].D, 2.0));							// eq. 109
				self->plant[i].CanopyFormation = 3.0;
			}
			else if((self->silv[i].Crowding == 1.0)&&(self->plant[i].HD >= self->plant[i].HDmax))
			{
				self->plant[i].IncrD = IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2.0));		// eq. 105
				self->plant[i].IncrH = self->plant[i].HD*self->plant[i].IncrD;							// eq. 104
				self->plant[i].CanopyFormation = 4.0;
			}
		}
		else if(self->conf.Tree_Geometry == 2)
		{
			//Here the new formulation of Height and Diameter Increment is implemented 
			//according to Jansen et al. (1995, Forschungszentrum Waldökosysteme)
			//p. A-22
			
			self->plant[i].IncrD = IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2));
			self->plant[i].IncrH = self->plant[i].HD*self->plant[i].IncrD;
			
			if(self->silv[i].Crowding == 0.0)
			{				
				self->plant[i].Incr_fh = pow(((self->plant[i].HDmax - self->plant[i].HD)/(self->plant[i].HDmax - self->plant[i].HDmin)), self->plant[i].Incr_p);
				self->plant[i].Incr_Red = (3.0 - self->plant[i].Incr_fh)/2.0;
				self->plant[i].CanopyFormation = 5.0;				
			}
			else if(self->silv[i].Crowding == 1.0)
			{
				self->plant[i].Incr_Red = pow(((self->plant[i].HD - self->plant[i].HDmin)/(self->plant[i].HDmax - self->plant[i].HDmin)), self->plant[i].Incr_p);
				self->plant[i].Incr_fh = 3.0-(2.0*self->plant[i].Incr_Red);
				self->plant[i].CanopyFormation = 6.0;
			}
			
			//Resulting Increments:
			self->plant[i].IncrD = self->plant[i].Incr_Red * (IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2.0)));
			self->plant[i].IncrH = self->plant[i].Incr_fh * self->plant[i].HD*self->plant[i].IncrD;
			//self->plant[i].IncrH = (3.0/self->plant[i].Incr_Red - 2.0) * self->plant[i].HD*self->plant[i].IncrD;
	
		}
		
		
		if(NewDay(xpn->pTi))
		{
			self->plant[i].IncrD_Day = 0.0;
			self->plant[i].IncrH_Day = 0.0;
		}
		self->plant[i].IncrD_Day += self->plant[i].IncrD/365.25*dt;
		self->plant[i].IncrH_Day += self->plant[i].IncrH/365.25*dt;
		
		self->plant[i].IncrD_Sum += self->plant[i].IncrD/365.25*dt;
		self->plant[i].IncrH_Sum += self->plant[i].IncrH/365.25*dt;
		
	}
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
};


int libtreemix_geometric_relations(libtreemix *self, int i)
{
	double V;	// stem volume per tree [m3/tree]
	double G;	// carbon mass per tree stem [tC/tree]
		
	/* note: the expon. factor 'brexp' (NOW: BrWdFrCoeff) in the orig. TREEDYN source code is = 100.0, why? It is 10.0 in BOSSEL!!!! */
	self->plant[i].BrWdFr = self->plant[i].FiBrWdFr + (1.0-self->plant[i].FiBrWdFr)*exp(-self->plant[i].BrWdFrCoeff*self->plant[i].D/self->plant[i].Dmax); //eq. 20, pt.1
	
	/* determining specific wood masses by morphologic relations and fractions */
	V = (M_PI/4.0)*self->plant[i].Form * pow(self->plant[i].D, 2)*self->plant[i].H;	// eq. 15, [m3], Stem Volume of an individual tree
	G = V*(self->plant[i].CWdDens * 1000.0);										// eq. 16, [kgC/tree], (Carbon) mass of an individual stem
	
	/* whole stand [kgC/ha] */
	self->plant[i].CStMass = G*self->plant[i].density;	// Stem carbon mass, [kgC/ha]
	
	self->plant[i].CTrMass = self->plant[i].CWdWeight/1000.0/self->plant[i].density; // [tC/tree], wood mass per tree incl. branches, coarse roots
	
	// calculating ratio of respiring to total wood
		self->plant[i].b = 1.0;		// eq. 26		
		if(self->plant[i].D > (2.0*self->plant[i].RespLay))
		{
			double Hc;
			double Hi;
			double Vc;
			double Vo;

			Hc = (12.0/(PI*self->plant[i].CWdDens))*(self->plant[i].CTrMass/pow(self->plant[i].D, 2.0));// eq. 22
			Hi = (self->plant[i].D-(2.0*self->plant[i].RespLay))*(Hc/self->plant[i].D);					// eq. 23

			Vc = (PI/12.0)*pow(self->plant[i].D, 2.0)*Hc;												// eq. 24
			Vo = (PI/12.0)*(pow(self->plant[i].D, 2.0)*Hc - pow((self->plant[i].D-(2.0*self->plant[i].RespLay)), 2.0)*Hi);	// eq. 25

			self->plant[i].b = Vo/Vc;																	// eq. 26
		}
		self->plant[i].CWdResp = (self->plant[i].b*self->plant[i].CWdWeight)/1000.0;						// resp. wood mass, [tC/ha]	
	
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
}