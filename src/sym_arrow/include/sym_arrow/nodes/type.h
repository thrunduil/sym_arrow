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
#include "dag/dag.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/symbol.h"

#pragma warning(push)
#pragma warning(disable:4251) //needs to have dll-interface to be used by clients

namespace sym_arrow
{

// representation of a type
class SYM_ARROW_EXPORT type
{
    private:
        identifier      m_type_id;
        bool            m_is_const;

    public:
        // create uninitialized type
        type();

        // create type with given name and const flag
        type(const identifier& name, bool is_const);

        // destructor
        ~type();

        // copy and move constructors
        type(const type& other);
        type(type&& other);

        // assignment and move assignment
        type& operator=(const type& other);
        type& operator=(type&& other);

    public:
        // return true if this symbol is not initialized
        bool                is_null() const;

        // return const flag
        bool                is_const() const;

        // get type name
        const identifier&   type_name() const;
};

// representation of a type
class SYM_ARROW_EXPORT formal_arg
{
    private:
        identifier      m_arg_name;
        type            m_type;

    public:
        // create uninitialized object
        formal_arg();

        // create symbol with given elements; 
        formal_arg(const identifier& arg_name, const type& ty);

        // destructor
        ~formal_arg();

        // copy and move constructors
        formal_arg(const formal_arg& other);
        formal_arg(formal_arg&& other);

        // assignment and move assignment
        formal_arg& operator=(const formal_arg& other);
        formal_arg& operator=(formal_arg&& other);

    public:
        // return true if this symbol is not initialized
        bool                is_null() const;

        // return argument name
        const identifier&   arg_name() const;

        // get argument type
        const type&         type_name() const;
};

// return true if types t1 and t2 are the same
bool SYM_ARROW_EXPORT operator==(const type& t1, const type& t2);

// return true if types t1 and t2 are different
bool SYM_ARROW_EXPORT operator!=(const type& t1, const type& t2);

};

#include "sym_arrow/details/type.inl"

#pragma warning(pop)
