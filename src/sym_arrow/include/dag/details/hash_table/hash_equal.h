/* 
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

namespace sym_dag { namespace details
{

template<class elem_type>
struct obj_hasher
{
    template<class info>
    size_t operator()(const info& pi) const;

    size_t operator()(const elem_type* ptr1) const;
};

template<class elem_type>
struct obj_equaler
{
    template<class info>
    bool operator()(const elem_type* ptr1, size_t hash_value,const info& pi) const;
    bool operator()(const elem_type* ptr1, size_t hash_value,const elem_type* ptr2) const;
};

};};