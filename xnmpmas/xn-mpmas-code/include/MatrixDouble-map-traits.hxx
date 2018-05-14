#ifndef ODB_MATRIXDOUBLE_TRAITS_HXX
#define ODB_MATRIXDOUBLE_TRAITS_HXX

#include <odb/pre.hxx>

#include "MatrixDouble.h"
#include <map>
#include <vector>
#include <utility> // std::move std::pair
#include <algorithm>

#include <odb/container-traits.hxx>
#include <odb/details/config.hxx>   // ODB_CXX11

namespace odb
{
  template <>
  class access::container_traits<MatrixDouble>
  {
  public:
    static const container_kind kind = ck_map;
    static const bool smart = false;

    typedef MatrixDouble container_type;

    typedef int_pair key_type;  //make sure to include a std:pair int int with #pragma db value in MatrixDouble.h
    typedef double value_type;
    typedef typename std::pair<key_type,value_type> pair_type;

    typedef map_functions<key_type, value_type> functions;

  public:
    static void
    persist (const container_type& c, const functions& f)
    {
//      for (typename container_type::const_iterator i (c.begin ()),
//             e (c.end ()); i != e; ++i)
   	 for (int r = 0; r < c.rows(); ++r )
   	 {
   		 	 for (int s = 0; s < c.cols(); ++s )
   		 	 {
   		 		 key_type mKey (r,s);
   		 	    value_type val = c.getValue(r,s);
   		 		 f.insert (mKey, val);
   		 	 }
   	 }
    }

    static void
    load (container_type& c, bool more, const functions& f)
    {
      c.deallocate ();

      //first load everything into an auxiliary vector,
      //since total size of array will not be known before all elements have been retrieved
      std::vector<pair_type> vec;

      while (more)
      {
        key_type k;
        value_type v;
        more = f.select (k, v);
        vec.push_back (pair_type (k, v));
      }
      //sort vector so that element with highest indices is last
      std::sort(vec.begin(), vec.end());

      //read maximum indices for both dimensions from last element of sorted vector
      //and allocate matrix accordingly
      int maxR = vec.back().first.first;
      int maxC = vec.back().first.second;
      c.allocate(maxR + 1, maxC + 1);

      //copy values from vector to matrix
      for (vector<pair_type>::iterator it = vec.begin(); it != vec.end(); it++)
      {
      	c.setValue(it->first.first, it->first.second, it->second);
      }
    }

    static void
    update (const container_type& c, const functions& f)
    {
      f.delete_ ();
		 for (int r = 0; r < c.rows(); ++r )
		 {
				 for (int s = 0; s < c.cols(); ++s )
				 {
					 key_type mKey (r,s);
					 value_type val = c.getValue(r,s);
					 f.insert (mKey, val);
				 }
		 }

    }

    static void
    erase (const functions& f)
    {
      f.delete_ ();
    }
  };


#include <odb/post.hxx>

#endif // ODB_STD_MAP_TRAITS_HXX
