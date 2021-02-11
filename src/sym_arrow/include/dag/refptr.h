/* 
 *  This file is a part of sym_arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#pragma once

#include "dag/config.h"

namespace sym_dag
{

// make copy flag
struct copy_t{};

// class storing pointers of type with allocation/deallocation
// management according to reference counting; memory management
// and reference updated are governed by the Traits class,
// which must implement functions:
//     template<class T>
//     struct traits_impl // (some implementation of Traits)
//     {
//         // called, when refptr is destroyed
//         static void check_free(T* val);
//         // called by a copy constructor of refptr
//         static void copy(T* val)
//         // called by an assign operator of refptr
//         static void assign(T* to, T* from) { copy(from); check_free(to); };
//     };
template<class T, class Traits>
class refptr
{
    private:
        template<class S, class SS>
        friend class refptr;

    public:
        // class governing memory management
        using type_traits   = Traits;

        // type of stored value
        using value_type    = T;

    private:
        T*                  m_value;

    public:
        // make refptr storing null pointer
        refptr();

        // make refptr storing value p
        static refptr       make(T* p);

        // make refptr storing value p; call Traits::copy
        refptr(T* p, copy_t);

        // copy constructor; calls Traits::copy
        refptr(refptr const& r);

        // move constructor
        refptr(refptr && r);

        // destructor; calls Traits::check_free
        ~refptr();        

        // copy costructor from refptr of other type
        // calls Traits::copy
        template<class Y>   refptr(refptr<Y,type_traits> const& r);

        // move costructor from refptr of other type
        template<class Y>   refptr(refptr<Y,type_traits> && r);

        // swap content of two refptr objects
        void                swap(refptr& other);

        // assignment; calls Traits::assign
        refptr&             operator=(refptr const & r);

        // move assignment
        refptr&             operator=(refptr && r);

        // assignment from refptr value of other type; 
        // calls Traits::assign
        template<class Y> 
        refptr&             operator=(refptr<Y,type_traits> const & r);

        // move assignment from refptr value of other type
        template<class Y> 
        refptr&             operator=(refptr<Y,type_traits> && r);

        // access to stored value
        T*                  operator->() const      { return m_value; };

        // access to stored value
        T*                  get() const             { return m_value; };

        // return stored value; value stored in this object is
        // set to nullptr; no functions from Traits class are called
        T*                  release();

        // release pointer and push stored pointer on the stack
        template<class Stack_type>
        void                release(Stack_type& st);

        // cast operator to boolean value
        explicit            operator bool() const   { return m_value? true : false; };

        // boolean negation operator
        bool                operator!() const       { return !m_value; };

        // create additional refptr object from a value p
        template<class Y> 
        static refptr       from_this(Y* p);

    private:
        explicit            refptr(T* p);

        template<class Y, class TT> refptr(refptr<Y,TT> const& r) = delete;
        template<class Y, class TT> refptr(refptr<Y,TT> && r) = delete;

        template<class Y, class TT> 
        refptr&             operator=(refptr<Y,TT> const & r) = delete;

        template<class Y, class TT> 
        refptr&             operator=(refptr<Y,TT> && r) = delete;
};

// test equality of stored values
template<class T1, class T2, class U1, class U2> 
inline bool operator==(refptr<T1,T2> const & a, refptr<U1,U2> const & b)
{
    return a.get() == b.get();
}

// test inequality of stored values
template<class T1, class T2, class U1, class U2>
inline bool operator!=(refptr<T1,T2> const & a, refptr<U1,U2> const & b)
{
    return a.get() != b.get();
}

// call operator < for values stored in refptr objects
template<class T1, class T2, class U1, class U2>
inline bool operator<(refptr<T1,T2> const & a, refptr<U1,U2> const & b)
{
    return a.get() < b.get();
}

// call operator <= for values stored in refptr objects
template<class T1, class T2, class U1, class U2>
inline bool operator<=(refptr<T1,T2> const & a, refptr<U1,U2> const & b)
{
    return a.get() <= b.get();
}

// call operator > for values stored in refptr objects
template<class T1, class T2, class U1, class U2>
inline bool operator>(refptr<T1,T2> const & a, refptr<U1,U2> const & b)
{
    return a.get() > b.get();
}

// call operator >= for values stored in refptr objects
template<class T1, class T2, class U1, class U2>
inline bool operator>=(refptr<T1,T2> const & a, refptr<U1,U2> const & b)
{
    return a.get() < b.get();
}

};

#include "dag/details/refptr.inl"
