/*
 * testOdb.h
 *
 *  Created on: Dec 10, 2015
 *      Author: troost
 */

#ifndef TESTODB_H_
#define TESTODB_H_
#include "MatrixDouble.h"

#ifdef ODBSTORE
	#include <odb/core.hxx>
#endif


#pragma db object
struct testWrap {
		#pragma db id
		int id;

		MatrixDouble matrix;

		testWrap () {
			id = 0;

		}
		testWrap(int i_) {
			id = i_;
			matrix.allocate(4,4);
			matrix.setAllValues(2.0);
			matrix.setValue(1,1,1);
		}
		~testWrap () {
			matrix.deallocate();
		}
};
#endif /* TESTODB_H_ */
