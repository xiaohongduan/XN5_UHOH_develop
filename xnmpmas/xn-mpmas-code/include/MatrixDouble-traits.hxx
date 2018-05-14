
#ifndef ODB_MATRIXDOUBLE_TRAITS_HXX
#define ODB_MATRIXDOUBLE_TRAITS_HXX

#include <odb/pre.hxx>

#include "MatrixDouble.h"

#include <odb/container-traits.hxx>

namespace odb
{
  template<>
  class access::container_traits<MatrixDouble>
  {
  public:
    static const container_kind kind = ck_ordered;
    static const bool smart = false;

    typedef MatrixDouble container_type;

    typedef double value_type;
    typedef typename vector<value_type>::size_type index_type;

    typedef ordered_functions<index_type, value_type> functions;

  public:
    static void
    persist (const container_type& c, const functions& f)
    {

	vector<value_type> vec;
	vec.reserve(c.rows() * c.cols() + 2);
	vec.push_back(c.rows());
	vec.push_back(c.cols());
	for (int cRow = 0; cRow < c.rows(); ++cRow)
	{  for (int cCol = 0; cCol < c.cols(); ++cCol)
	   {	vec.push_back(c.getValue(cRow,cCol));
	   }
	}
        for (index_type i (0), n (vec.size ()); i < n; ++i)
      	  	  f.insert (i, vec[i]);
    }

    static void
    load (container_type& c, bool more, const functions& f)
    {
      c.deallocate();
      vector<value_type> vec;

      while (more)
      {
        index_type dummy;
        vec.push_back (value_type ());
		  more = f.select (dummy, vec.back ());
		}
		int rL = (int) (vec[0] + 0.00001);
		int cL = (int) (vec[1] + 0.00001);
		if (rL == 0 && cL == 0)
		 c.setEmpty();
		else
		{
			c.allocate(rL, cL);
			for (int cRow = 0; cRow < c.rows(); ++cRow)
			{  for (int cCol = 0; cCol < c.cols(); ++cCol)
				{	c.setValue(cRow,cCol, vec[2+ cRow * cL +cCol]   );
				}
			}
		}
    }

    static void
    update (const container_type& c, const functions& f)
    {
   	 	 f.delete_ ();

			vector<value_type> vec;
			vec.reserve(c.rows() * c.cols() + 2);
			vec.push_back(c.rows());
			vec.push_back(c.cols());
			for (int cRow = 0; cRow < c.rows(); ++cRow)
			{  for (int cCol = 0; cCol < c.cols(); ++cCol)
				{	vec.push_back(c.getValue(cRow,cCol));
				}
			}
        for (index_type i (0), n (vec.size ()); i < n; ++i)
      	  f.insert (i, vec[i]);
    }

    static void
    erase (const functions& f)
    {
      f.delete_ ();
    }
  };
}

#include <odb/post.hxx>

#endif // ODB_STD_VECTOR_TRAITS_HXX
