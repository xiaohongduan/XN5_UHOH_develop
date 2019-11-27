/*
 * MultiperiodMipVariableCoefficient.h
 *
 *  Created on: May 20, 2015
 *      Author: troost
 */

#ifndef MULTIPERIODMIPVARIABLECOEFFICIENT_H_
#define MULTIPERIODMIPVARIABLECOEFFICIENT_H_

#include <vector>
//#include "boost/bind.hpp"
//#include "boost/function.hpp"
/*
struct MultiPeriodMipGetFunction
{
		virtual double retrieveValue() = 0;
		virtual ~MultiPeriodMipGetFunction();
};
*/
class mpPrbl;

//typedef  boost::function<double(mpPrbl mip, double input_val_, double* varsRHS, double* zRow)> MultiperiodMipUpdateFunction;

class MultiperiodMipVariableValue
{
protected:
		//attributes
		int col;
		int row;
		double multiplier;
		double summand;
		//MultiPeriodMipGetFunction* func;
public:
		std::vector<int> arguments;

		//constructor
		MultiperiodMipVariableValue(int col_, int row_ , double summand_, double multiplier_, std::vector<int> arguments_)
		: col(col_), row(row_), multiplier(multiplier_), summand(summand_), arguments(arguments_)  {}
		virtual ~MultiperiodMipVariableValue(){}
		//
		double finalValue(double input_) { return summand + multiplier * input_;}
//		MultiperiodMipUpdateFunction createSpecificMipUpdateFunction();
		//MultiperiodMipUpdateFunction createGenericMipUpdateFunction();
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_,double* varsRHS, double* zrow) = 0;
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_) = 0;
		virtual void updateGenericMatrix(mpPrbl* mip, double val_) = 0;
		static MultiperiodMipVariableValue* makeMipUpdater(int col_, int row_ , double summand_, double multiplier_, std::vector<int> arguments_);
		static MultiperiodMipVariableValue* makeMipUpdaterFirstMover(int col_, int row_ , double summand_, double multiplier_, std::vector<int> arguments_, mpPrbl* mip);
};

class MultiperiodMipVariableMatrixCoefficient : public MultiperiodMipVariableValue
{
	public:
		MultiperiodMipVariableMatrixCoefficient(int col_, int row_ , double summand_, double multiplier_, std::vector<int> arguments_)
	: MultiperiodMipVariableValue(col_, row_ , summand_, multiplier_, arguments_)	{	}
		virtual void updateSpecificMatrix(mpPrbl* mip, double val_, double* varsRHS, double* zrow);
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_);
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableRhsValue : public MultiperiodMipVariableValue
{
	public:
		MultiperiodMipVariableRhsValue( int row_ , double summand_, double multiplier_, std::vector<int> arguments_)
		:  MultiperiodMipVariableValue(-1, row_ , summand_, multiplier_, arguments_){}
		virtual void updateSpecificMatrix(mpPrbl* mip, double val_, double* varsRHS, double* zrow);
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_);
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableObjCoeff : public MultiperiodMipVariableValue
{
	public:
		MultiperiodMipVariableObjCoeff(int col_, double summand_, double multiplier_, std::vector<int> arguments_)
		: MultiperiodMipVariableValue(col_, -1 , summand_, multiplier_, arguments_) {}
		virtual void updateSpecificMatrix(mpPrbl* mip, double val_, double* varsRHS, double* zrow);
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_);
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableColUbound : public MultiperiodMipVariableValue
{
	public:
		MultiperiodMipVariableColUbound(int col_, double summand_, double multiplier_, std::vector<int> arguments_)
		: MultiperiodMipVariableValue(col_, -3 , summand_, multiplier_, arguments_) {}
		virtual void updateSpecificMatrix(mpPrbl* mip, double val_, double* varsRHS, double* zrow);
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_);
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableColLbound : public MultiperiodMipVariableValue
{
	public:
		MultiperiodMipVariableColLbound(int col_, double summand_, double multiplier_, std::vector<int> arguments_)
		: MultiperiodMipVariableValue(col_, -2 , summand_, multiplier_, arguments_) {}
		virtual void updateSpecificMatrix(mpPrbl* mip, double val_, double* varsRHS, double* zrow);
		virtual void updateSpecificMatrix(mpPrbl* mip,  double val_);
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
//191001 Note: The following first mover classes have become necessary
//as we now allow adding up of inserted values. This is why most methods are changed from set.. to addTo..
//For the specific matrix this works without problems, because the specific matrix is reinitialized
//from the generic matrix for each agent in each period. So all past additions are erased
// The generic matrix however is kept around. In order not to accumulate changes over
// the years, the generic matrix needs to be reset to initial state before any additions are made.
// This is achieved by letting the first inserter that writes into a cell of the generic matrix
// set, while later inserters writing into the same place add.
// The order of writing is fixed and first movers are identified upon input file reading.
// To maintain any value that was originally in the generic matrix, before any inserters
// this value is added to the summand of the first mover at input file processing.

class MultiperiodMipVariableMatrixCoefficientFirstMover : public MultiperiodMipVariableMatrixCoefficient
{
	public:
		MultiperiodMipVariableMatrixCoefficientFirstMover( int col_, int  row_,double summand_, double multiplier_, std::vector<int> arguments_)
	      :  MultiperiodMipVariableMatrixCoefficient( col_, row_, summand_, multiplier_, arguments_) {}
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableRhsValueFirstMover : public MultiperiodMipVariableRhsValue
{
	public:
		MultiperiodMipVariableRhsValueFirstMover( int row_ , double summand_, double multiplier_, std::vector<int> arguments_)
		:  MultiperiodMipVariableRhsValue( row_ , summand_, multiplier_, arguments_){}
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableObjCoeffFirstMover : public MultiperiodMipVariableObjCoeff
{
	public:
		MultiperiodMipVariableObjCoeffFirstMover(int col_, double summand_, double multiplier_, std::vector<int> arguments_)
		: MultiperiodMipVariableObjCoeff(col_, summand_, multiplier_, arguments_) {}

		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableColUboundFirstMover : public MultiperiodMipVariableColUbound
{
	public:
		MultiperiodMipVariableColUboundFirstMover(int col_, double summand_, double multiplier_, std::vector<int> arguments_)
		: MultiperiodMipVariableColUbound(col_ , summand_, multiplier_, arguments_) {}

		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};
class MultiperiodMipVariableColLboundFirstMover : public MultiperiodMipVariableColLbound
{
	public:
		MultiperiodMipVariableColLboundFirstMover(int col_, double summand_, double multiplier_, std::vector<int> arguments_)
		: MultiperiodMipVariableColLbound(col_ , summand_, multiplier_, arguments_) {}
		virtual void updateGenericMatrix(mpPrbl* mip, double val_);
};



#endif /* MULTIPERIODMIPVARIABLECOEFFICIENT_H_ */
