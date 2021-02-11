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

#include "sym_arrow/ast/builder/vlist.h"
#include "sym_arrow/ast/builder/build_item.h"

namespace sym_arrow { namespace ast
{

// container for integer and real items stored in mult_build
// Value_t - type of values assigned to terms
// Derived - type of derived class
template<class Value_t, class Derived>
class vlist_mult : public vlist_base<build_item<Value_t>,Derived>
{
    public:
        // type of stored items
        using item_type     = build_item<Value_t>;

    private:
        // raise all stored items to given power
        Value_t              m_pow;        

    public:
        // create vector of items with zero elements; set pow
        // scalar to supplied value
        explicit vlist_mult(const Value_t& pow);

        // get power scalar
        const Value_t&      get_pow() const;

        // return true if this object stores only one regular item
        // and power value assigned to this item is one
        bool                is_simple_mult() const;

    protected:
        // copy vector from exisiting temporary list
        vlist_mult(vlist_mult&& other);

        vlist_mult(const vlist_mult& other) = delete;
        vlist_mult& operator=(const vlist_mult& other) = delete;

        // destructor
        ~vlist_mult();

        // raise all stored elements to the power v
        void                make_pow(const Value_t& v);

        // set power scalar 
        void                set_pow(const Value_t& v);

        void                init_with_default_values(const Value_t& v);
};

// return true is a value is equal to 1
inline bool     is_value_one(int val)           { return val == 1; };
inline bool     is_value_one(const value& val)  { return val.is_one(); };

// return true is a value is equal to 0
inline bool     is_value_zero(int val)          { return val == 0; };
inline bool     is_value_zero(const value& val) { return val.is_zero(); };

class rlist_mult;

// container for integer items stored in mult_build
class ilist_mult : public vlist_mult<int, ilist_mult>
{
    public:
        // type of base class
        using vlist_mult_type       = vlist_mult<int, ilist_mult>;

        // this container stores integer items
        static const bool is_ilist  = true;

    public:
        // create vector of items with zero elements; set pow
        // scalar to supplied value
        explicit ilist_mult(int pow);

        // raise all stored elements to the power v; store results
        // in a container rl for real items
        void                make_pow(const value& p, rlist_mult& rl) const;

        // raise all stored elements to the power v
        void                make_pow(int p);

    private:
        // copy vector from exisiting temporary list
        ilist_mult(ilist_mult&& other);

        ilist_mult(const ilist_mult&) = delete;
        ilist_mult& operator=(const ilist_mult&) = delete;

        friend vlist_base;

        // set power scalar to 1
        void                init_with_default_values();
};

// container for real items stored in mult_build
class rlist_mult : public vlist_mult<value, rlist_mult>
{
    public:
        // type of base class
        using vlist_mult_type       = vlist_mult<value, rlist_mult>;

        // this container stores real items
        static const bool is_ilist  = false;

    public:
        // create vector of items with zero elements; set pow
        // scalar to supplied value
        explicit rlist_mult(const value& pow);

    private:
        rlist_mult(const rlist_mult&) = delete;
        rlist_mult& operator=(const rlist_mult&) = delete;

        // copy vector from exisiting temporary list
        rlist_mult(rlist_mult&& other);        

        friend vlist_base;

    public:
        // raise all stored elements to the power p
        void                make_pow(const value& p);

        // raise all stored elements to the power p
        void                make_pow(int p);        

        // set power scalar to 1
        void                init_with_default_values();
};

};};
