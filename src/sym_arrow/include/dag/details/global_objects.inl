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

#include "dag/details/global_objects.h"

namespace sym_dag { namespace details
{

template<class Type>
inline global_object_type<Type>::~global_object_type()
{
    global_objects::delete_item(m_ptr);
};

}};

namespace sym_dag
{

template<class Type, class ... Args>
Type* global_objects::make_before(Args&&  ... args)  
{ 
    Type* obj       = new Type(std::forward<Args>(args)...);
    item_type* h    = new details::global_object_type<Type>(obj);

    get_global().m_objects_before.push_back(h);

    return obj; 
};

template<class Type, class ... Args>
Type* global_objects::make_context(Args&&  ... args)  
{ 
    Type* obj       = new Type(std::forward<Args>(args)...);
    item_type* h    = new details::global_object_type<Type>(obj);

    get_global().m_objects_context.push_back(h);

    return obj; 
};

template<class Type, class ... Args>
Type* global_objects::make_after(Args&&  ... args)  
{ 
    Type* obj       = new Type(std::forward<Args>(args)...);
    item_type* h    = new details::global_object_type<Type>(obj);

    get_global().m_objects_after.push_back(h);

    return obj; 
};

template<class Type, class ... Args>
Type* global_objects::make_last(Args&&  ... args)  
{ 
    Type* obj       = new Type(std::forward<Args>(args)...);
    item_type* h    = new details::global_object_type<Type>(obj);

    get_global().m_objects_last.push_back(h);

    return obj; 
};

template<class Type>
void global_objects::delete_item(Type* ptr)
{
    delete ptr;
}

};
