/*
 * interp1D.hpp
 *
 *  Created on: Apr 14, 2020
 *      Author: forma
 */

#ifndef EXAMPLES_SRC_INTERP1D_INTERP1D_HPP_
#define EXAMPLES_SRC_INTERP1D_INTERP1D_HPP_
#include <iterator>
#include <limits>
#include <exception>
#include <algorithm>
#include <functional>
#include<array>
#include<vector>
#include <type_traits>
namespace apsc{
  /*! A general piecewise-linear interpolator
   *
   * This function is the building block for rather general piecewise-liner interpolation
   * It requires to have a range defined by two bidirectional iterators in input. The iterators
   * iterates over a container of a generic type from which we can extract a Key and a Value.
   * The range must be strictly ordered with respect to the key (i.e. without repetition of elements with equivalent keys).
   * The ordering relation must be specified as argument, together as the functors that extract the key and the value
   * from the container, respectively.  It is also assumed that usual arithmetic operations may be performed on keys and values.
   *
   * If the value of the key where to interpolate falls between the interval defined by the keys in the range, piecewise-linear
   * interpolation is performed. Otherwise, extrapolation is performed using the last or first two elements of the container, depending on the case.
   *
   * The procedure followed is a binary search (bisection) on the keys.
   *
   * Complexity: if the iterator is a forward iterator the complexity is log2 N, N being  the size of the range.
   * otherwise complexity is basically linear, because we need to advance iterators and this is linear on non forward iterators
   *
   * @note We recall that the range is defined by [begin, end[
   *
   * @tparam RAIterator A bi-directional iterator
   * @tparam Key The type of the Key
   * @tparam ExtractKey The type of the functor that extracts the key form a dereferenced iterator
   * @tparam ExtractValue The type of functor that extracts the value form a dereferenced iterator
   * @tparam CompareKey Type of comparison operator between keys
   * @param begin Start of the range
   * @param end   End of the range
   * @param keyVal The value of the key to interpolate
   * @param extractKey The actual functor for extraction of key
   * @param extractValue The actual functor for extraction of values
   * @param comp The comparison operator for keys (defaulted to std::less<Key>()
   * @return the value found in correspondence of keyVal
   */
  template <typename RAIterator, typename Key, typename ExtractKey, typename ExtractValue, typename CompareKey=std::less<Key>>
  auto interp1D(RAIterator const & begin, RAIterator const & end,  Key const & keyVal,
		ExtractKey const & extractKey, ExtractValue const & extractValue, CompareKey const & comp=std::less<Key>())
  {
    // I avoid users using wrong iterators
    // I nice use of iterator_traits and iterator_tags
    using category = typename std::iterator_traits<RAIterator>::iterator_category;
    static_assert(std::is_same_v<category, std::bidirectional_iterator_tag> ||
		  std::is_same_v<category, std::random_access_iterator_tag>, "Iterators must be (at least) bidirectional");
    // I need at least two point for interpolating anything. This checks also that end is after begin!
    if (std::distance(begin,end)<1) throw std::runtime_error("Interp1D: I need at least 2 points to interpolate!");

    RAIterator a{begin};
    RAIterator b{end};
    // bisection
    for(auto dis=std::distance(a,b); dis>1;)
      {
	RAIterator c = std::next(a,dis/2); // midpoint
	if( comp(keyVal,extractKey(*c)) ) // keyVal on the left of c
	  b=c;
	else  // keyVal on the right of c or on c
	  a=c;
	dis=std::distance(a,b); // recompute distance
      }
    //! interval found
    b = std::next(a,1); // get other end of interval
    if (b==end) // complex situation I need to go back of 1
      {
	b=a;
	std::advance(a,-1); // here I need bi-directionality!
      }
    auto valueLeft=extractValue(*a);
    Key keyLeft=extractKey(*a);
    auto valueRight=extractValue(*b);
    Key keyRight=extractKey(*b);
    auto len= keyRight - keyLeft;
    // I assume no nodes are repeated
    auto coeffRight = (keyVal-keyLeft)/len;
    auto coeffLeft  = 1.0 - coeffRight;
    return valueLeft*coeffLeft + valueRight*coeffRight;
  }

  //
  /*! Implementation for vector<T> where T behaves like an array with 2 components (key value).
   *
   * @tparam CompOper Comparison operator
   * @tparam T the type stores in the array. it should behave as an array of 2 components. The first is the key, the second is the value
   * @param v a vector
   * @param keyVal the point to be interpolates
   * @param comp Comparison operator, defaults to less<T>
   * @return
   */
  template <typename A, typename CompOper=std::less<double>>
  inline double interp1D(std::vector<A> const & v, double const & keyVal,CompOper const & comp=std::less<double>())
  {

    return interp1D(
	v.cbegin(),
	v.cend(),
	keyVal,
	[](A const & x){return x[0];},
	[](A const & x){return x[1];},
	comp
    );

  }
}




#endif /* EXAMPLES_SRC_INTERP1D_INTERP1D_HPP_ */