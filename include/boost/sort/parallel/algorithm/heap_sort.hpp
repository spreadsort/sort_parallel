//----------------------------------------------------------------------------
/// @file heap_sort.hpp
/// @brief Insertion Sort algorithm
///
/// @author Copyright (c) 2010 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_ALGORITHM_HEAP_SORT_HPP
#define __BOOST_SORT_PARALLEL_ALGORITHM_HEAP_SORT_HPP

#include <boost/sort/parallel/util/definition.hpp>
#include <boost/sort/parallel/util/config.hpp>
#include <boost/sort/parallel/util/util_iterator.hpp>
#include <boost/sort/parallel/algorithm/token.hpp>
#include <boost/sort/parallel/algorithm/indirect.hpp>
#include <stdexcept>
#include <cstdint>
#include <cassert>
#include <utility> // for std::swap

namespace boost
{
namespace sort
{
namespace parallel
{
namespace algorithm
{
using namespace boost::sort::parallel::util ;
//
//------------------------------------------------------------------------------
//  function : sort
/// @brief Sort and signal the changes of three values
/// @param [in] R0 : first value to compare
/// @param [in] R1 : second value to compare
/// @param [in] R2 : third value to compare
/// @param [out] B0 : if true indicates R0 had been changed
/// @param [out] B1 : if true indicates R1 had been changed
/// @param [out] B2 : if true indicates R2 had been changed
/// @return if true , some value had changed
/// @remarks
//-----------------------------------------------------------------------------
template <class value_t>
inline bool sort3 (value_t &R0, value_t &R1, value_t &R2,
                   bool    &B0 ,bool    &B1, bool    &B2  )
{   //----------------------------- begin-------------------------------
    B0 = B1 = B2 = false ;
    int value = 0 ;
    if ( R0 < R1) value +=4 ;
    if ( R1 < R2) value += 2;
    if ( R0 < R2) value += 1 ;

    switch ( value )
    {   case 0: break ;
        case 2: std::swap ( R1 , R2);
                B1 = B2 = true ;
                break ;
        case 3: if ( not ( R0 >  R1))
                {   std::swap ( R0 , R2);
                    B0 = B2 = true ;
                }
                else
                {   auto Aux = std::move ( R2 );
                    R2 = std::move(R1 );
                    R1 = std::move ( R0 );
                    R0 = std::move ( Aux );
                    B0 = B1 = B2 = true ;
                };
                break;
        case 4: std::swap ( R0 , R1);
                B0 = B1 = true ;
                break ;

        case 5: if ( R1 > R2)
                {   auto Aux = std::move ( R0);
                    R0 = std::move(R1);
                    R1 = std::move( R2);
                    R2 = std::move ( Aux);
                    B0 = B1 = B2 = true ;
                }
                else
                {   std::swap ( R0 , R2);
                    B0 = B2 = true ;
                };
                break ;

        case 7: std::swap ( R0 , R2);
                B0 = B2 = true ;
                break ;
        default: abort();

    }

    return ( B0 or B1 or B2);
};
//
//-----------------------------------------------------------------------------
//  function : make_heap
/// @brief Make the heap for to extract the sorted elements
/// @param [in] first : iterator to the first element of the range
/// @param [in] N : number of lements of the range
/// @param [in] comp : object for to compare two elements
/// @remarks This algorithm is O(NLogN)
//-----------------------------------------------------------------------------
template < class iter_t,
           class Compare = std::less < typename iter_value<iter_t>::type >
         >
void make_heap ( iter_t first , size_t N , Compare comp = Compare())
{   //-------------------- begin -----------------------------
    size_t pos_father , pos_son ;
    iter_t iter_father = first , iter_son = first ;
    bool SW = false ;

    for ( size_t i = 1 ; i < N ; ++i)
    {   pos_father = i ;
        iter_father = first + i ;
        SW = false ;
        do
        {   iter_son = iter_father ;
            pos_son = pos_father ;
            pos_father =  ( pos_son - 1 )>>1 ;
            iter_father = first + pos_father;
            if ( (SW = comp(*iter_father, *iter_son)) )
                std::swap (*iter_father , *iter_son);
        } while ( SW and pos_father != 0  );
    };
};
//
//-----------------------------------------------------------------------------
//  function : heap_sort
/// @brief : Heap sort algorithm
/// @param [in] first: iterator to the first element of the range
/// @param [in] last : iterator to the next element of the last in the range
/// @param [in] comp : object for to do the comparison between the elements
/// @remarks This algorithm is O(NLogN)
//-----------------------------------------------------------------------------
template < class iter_t,
           class Compare =std::less <typename iter_value<iter_t>::type>
         >
void heap_sort ( iter_t first , iter_t last , Compare comp= Compare()  )
{   //--------------------------- begin -----------------------------
    assert ( (last - first) >= 0 );
    size_t N = last - first ;
    if ( N < 2 ) return ;
    //-----------------------------------------------------------------------
    // Creating the initial heap
    //-----------------------------------------------------------------------
    make_heap ( first , N ,comp);

    //-----------------------------------------------------------------------
    //  Sort the heap
    //-----------------------------------------------------------------------
    size_t pos_father , pos_son ;
    iter_t iter_father = first , iter_son = first ;

    bool SW = false ;
    for ( size_t i = 1 ; i < N ; ++i)
    {   std::swap ( *first , * (first + (N-i)));
        pos_father =0 ;
        pos_son = 1 ;
        iter_father = first ;
        SW = true ;
        while (SW and pos_son  < (N-i) )
        {   // if the father have two sons must select the bigger
            iter_son = first + pos_son ;
            if ( ( pos_son + 1 ) < (N-i) and comp(*iter_son,*(iter_son +1)) )
            {   ++pos_son ;
                ++iter_son ;
            };
            if ( (SW = comp(*iter_father, *iter_son)) )
                std::swap ( *iter_father , *iter_son);
            pos_father = pos_son ;
            iter_father = iter_son ;
            pos_son = ( pos_father <<1) +1 ;
        };
    };
};
//############################################################################
//                                                                          ##
//                I N D I R E C T     F U N C T I O N S                     ##
//                                                                          ##
//############################################################################

//-----------------------------------------------------------------------------
//  function : indirect_heap_sort
/// @brief : Heap sort algorithm. Create an index of iterators. Sort the index,
///          an at end sort the elements according this index sorted
/// @param [in] first: iterator to the first element of the range
/// @param [in] last : iterator to the next element of the last in the range
/// @param [in] comp : object for to do the comparison between the elements
/// @remarks This algorithm is O(NLogN)
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>
         >
void indirect_heap_sort ( iter_t first, iter_t last ,
                                    compare comp = compare() )
{   //------------------------------- begin--------------------------
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    heap_sort  ( VP.begin() , VP.end(), compare_ptr(comp) );
    sort_index ( first , VP) ;
};
//
//****************************************************************************
};//    End namespace algorithm
};//    End namespace namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
