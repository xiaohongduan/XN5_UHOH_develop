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
#endif /* MULTIPERIODMIPVARIABLECOEFFICIENT_H_ */
