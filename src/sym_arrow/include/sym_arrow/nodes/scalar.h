/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017
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

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/value.h"
#include "dag/dag.h"

#pragma warning(push)
#pragma warning(disable:4251) //needs to have dll-interface to be used by clients

namespace sym_arrow
{

// class representing hashed value; i.e. equal values are represented
// by the same pointer
class SYM_ARROW_EXPORT scalar
{
    private:
        using value_type    = ast::scalar_rep;
        using ptr_type      = sym_dag::dag_ptr<value_type, sym_arrow::ast::term_tag>;

    private:
        ptr_type            m_ptr;

    public:
        // create uninitialized object
        scalar();

        // create and object storing a value constructed from given scalar
        explicit scalar(double v);

        // create and object storing a value v
        explicit scalar(const value& v);

        // create an object from internal representation
        scalar(const ptr_type& rep);

        // destructor
        ~scalar();

        // copy and move constructors
        scalar(const scalar& other);
        scalar(scalar&& other);

        // assignment and move assignment
        scalar& operator=(const scalar& other);
        scalar& operator=(scalar&& other);

        // return true if this object is uninitialized
        bool                is_null() const     { return !m_ptr; };

        // get pointer to internal representation
        const ptr_type&     get_ptr() const     { return m_ptr; };

    public:
        // return stored value
        const value&        get_value() const;

    public:
        // return precomputed scalar representing value 0
        static const scalar&    make_zero();

        // return precomputed scalar representing value 1
        static const scalar&    make_one();

        // return precomputed scalar representing value -1
        static const scalar&    make_minus_one();
};

// comparison function based on addresses of pointers;
// these functions are equivalent to comparison operators
// defined for class expr
bool    operator<(const scalar& s1, const scalar& s2);
bool    operator>(const scalar& s1, const scalar& s2);
bool    operator<=(const scalar& s1, const scalar& s2);
bool    operator>=(const scalar& s1, const scalar& s2);
bool    operator==(const scalar& s1, const scalar& s2);
bool    operator!=(const scalar& s1, const scalar& s2);

};

#pragma warning(pop)
