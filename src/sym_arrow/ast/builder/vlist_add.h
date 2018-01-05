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

#include "sym_arrow/ast/builder/vlist.h"
#include "sym_arrow/ast/builder/build_item.h"

namespace sym_arrow { namespace ast
{

// container for items stored in add_build
class vlist_add : public vlist_base<build_item<value>, vlist_add>
{
    private:
        // additive term
        value                   m_add;

        // scalar multiplying all stored elements
        value                   m_scal;

    public:
        // create vector of items with zero elements
        vlist_add();

        // create vector of items with zero elements; set add and scal
        // scalars to supplied values
        vlist_add(const value& add, const value& scal);

        // destructor
        ~vlist_add();

    private:
        vlist_add(vlist_add&& other);

        vlist_add(const vlist_add& other) = delete;
        vlist_add& operator=(const vlist_add& other) = delete;

        friend vlist_base;

    public:
        // multiply all stored elements by scal
        void                    make_scal(const value& scal);

        // increase add scalar by 'add'
        void                    make_add(const value& add);

        // get add scalar
        const value&            get_add() const;

        // get scaling
        const value&            get_scal() const;

        // return true if this object stores only one regular item
        // and add scalar is zero
        bool                    is_simple_add() const;

    private:
        void                    init_with_default_values();
};

};};

#include "sym_arrow/ast/builder/vlist_add.inl"
