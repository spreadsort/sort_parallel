//----------------------------------------------------------------------------
/// @file parallel_sort.hpp
/// @brief Contains the parallel_sort class, which is part of the
///        block_indirect_sort algorithm
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_BIS_PARALLEL_SORT_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_BIS_PARALLEL_SORT_HPP

#include <boost/sort/parallel/detail/bis/backbone.hpp>

namespace boost      {
namespace sort       {
namespace parallel   {
namespace detail     {
namespace bis        {

using boost::sort::parallel::detail::util::nbits64;
//
///---------------------------------------------------------------------------
/// @struct parallel_sort
/// @brief This class do a parallel sort, using the quicksort filtering,
///        splitting the data until the number of elements is smaller than a
///        predefined value (max_per_thread)
//----------------------------------------------------------------------------
template <uint32_t Block_size, class Iter_t, class Compare>
struct parallel_sort
{
    //-------------------------------------------------------------------------
    //                  D E F I N I T I O N S
    //-------------------------------------------------------------------------
    typedef typename std::iterator_traits<Iter_t>::value_type value_t;
    typedef std::atomic<uint32_t> atomic_t;
    typedef std::function<void(void)> function_t;
    typedef backbone<Block_size, Iter_t, Compare> backbone_t;

    //------------------------------------------------------------------------
    //                V A R I A B L E S
    //------------------------------------------------------------------------
    // reference to a object with all the data to sort
    backbone_t &bk;
    // maximun number of element to sort woth 1 thread
    size_t max_per_thread;
    // atomic counter for to detect the end of the works created inside
    // the object
    atomic_t counter;

    //------------------------------------------------------------------------
    //                F U N C T I O N S
    //------------------------------------------------------------------------
    parallel_sort(backbone_t &bkbn, Iter_t first, Iter_t last);

    void divide_sort(Iter_t first, Iter_t last);
    //
    //------------------------------------------------------------------------
    //  function : function_divide_sort
    /// @brief create a function_t with a call to divide_sort, and inser in
    ///        the stack of the backbone
    //
    /// @param first : iterator to the first element of the range to divide
    /// @param last : iterator to the next element after the last element of
    ///               the range to divide
    /// @param son_counter : atomic variable which is decremented when finish
    ///                      the function. This variable is used for to know
    ///                      when are finished all the function_t created
    ///                      inside an object
    //------------------------------------------------------------------------
    void function_divide_sort(Iter_t first, Iter_t last, atomic_t &son_counter)
    {
        //-------------------------- begin ---------------------------------
        util::atomic_add(son_counter, 1);
        function_t f1 = [this, first, last, &son_counter]() -> void {
            this->divide_sort(first, last);
            util::atomic_sub(son_counter, 1);
        };
        bk.works.emplace_back(f1);
    };
    //
    //--------------------------------------------------------------------------
}; // end struct parallel_sort
//--------------------------------------------------------------------------
//
//############################################################################
//                                                                          ##
//                                                                          ##
//            N O N     I N L I N E      F U N C T I O N S                  ##
//                                                                          ##
//                                                                          ##
//############################################################################
//
//------------------------------------------------------------------------
//  function : parallel_sort
/// @brief constructor of the class
/// @param [in] bkbn : backbone struct with all the information to sort
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
//------------------------------------------------------------------------
template <uint32_t Block_size, class Iter_t, class Compare>
parallel_sort<Block_size, Iter_t, Compare>
    ::parallel_sort(backbone_t &bkbn, Iter_t first, Iter_t last)
    : bk(bkbn), counter(0)
{
    //-------------------------- begin ------------------------------------
    assert((last - first) >= 0);
    size_t nelem = size_t(last - first);

    //------------------- check if sort --------------------------------------
    bool sorted = true;
    for (Iter_t it1 = first, it2 = first + 1;
         it2 != last and (sorted = not bk.cmp(*it2, *it1)); it1 = it2++)
        ;
    if (sorted) return;

    //-------------------max_per_thread ---------------------------
    uint32_t nbits_size = (nbits64(sizeof(value_t))) >> 1;
    if (nbits_size > 5) nbits_size = 5;
    max_per_thread = 1 << (18 - nbits_size);

    //---------------- check if only single thread -----------------------
    if (nelem < (max_per_thread)) {
        intro_sort(first, last, bk.cmp);
        return;
    };

    divide_sort(first, last);
    // wait until all the parts are finished
    bk.exec(counter);
};

//------------------------------------------------------------------------
//  function : divide_sort
/// @brief this function divide the data in two part, for to be sorted in
///        a parallel mode
/// @param first : iterator to the first element to sort
/// @param last : iterator to the next element after the last
//------------------------------------------------------------------------
template <uint32_t Block_size, class Iter_t, class Compare>
void parallel_sort<Block_size, Iter_t, Compare>
    ::divide_sort(Iter_t first, Iter_t last)
{
    //------------------- check if sort -----------------------------------
    bool sorted = true;
    for (Iter_t it1 = first, it2 = first + 1;
         it2 != last and (sorted = not bk.cmp(*it2, *it1)); it1 = it2++)
        ;
    if (sorted) return;

    //---------------- check if finish the subdivision -------------------
    size_t nelem = last - first;
    if (nelem < (max_per_thread)) {
        return intro_sort(first, last, bk.cmp);
    };

    //-------------------- pivoting  ----------------------------------
    pivot9(first, last, bk.cmp);
    const value_t &val = const_cast<value_t &>(*first);
    Iter_t c_first = first + 1, c_last = last - 1;

    while (bk.cmp(*c_first, val)) ++c_first;
    while (bk.cmp(val, *c_last)) --c_last;
    while (not(c_first > c_last)) {
        std::swap(*(c_first++), *(c_last--));
        while (bk.cmp(*c_first, val)) ++c_first;
        while (bk.cmp(val, *c_last)) --c_last;
    }; // End while
    std::swap(*first, *c_last);

    // insert  the work of the second half in the stack of works
    function_divide_sort(c_first, last, counter);

    // The first half is done by the same thread
    divide_sort(first, c_last);
};
//
//****************************************************************************
}; //    End namespace bis
}; //    End namespace detail
}; //    End namespace parallel
}; //    End namespace sort
}; //    End namespace boost
//****************************************************************************
//
#endif
