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

#include "dag/details/global_objects.h"
#include "dag/dag_context.h"

//#include <vld.h>

namespace sym_dag
{

//------------------------------------------------------------
//                      dag_initializer
//------------------------------------------------------------
static global_objects* m_globals = nullptr;

// nifty counter
static int g_counter = 0;

dag_initializer::dag_initializer()
{
    if (g_counter == 0)
        m_globals = new global_objects();

    ++g_counter;
};

dag_initializer::~dag_initializer()
{
    --g_counter;

    if (g_counter == 0)   
    {
        delete m_globals;
        m_globals = nullptr;
    };
}

global_objects::global_objects()
{};

global_objects::~global_objects()
{
    close_impl();
};

void global_objects::close_impl()
{
    destroy(m_objects_before);

    // registered_dag_context is also global object
    // and will be destroyed together with dag contexts;
    // we must call close() function, because this is the only
    // way to destroy dag context data properly
    registered_dag_context::get().destroy();

    destroy(m_objects_context);
    destroy(m_objects_after);
    destroy(m_objects_last);
}

void global_objects::destroy(vector_obj& vec)
{
    size_t n   = vec.size();

    for (size_t i = 0; i < n; ++i)
        delete vec[i];

    vec.clear();
};

global_objects& global_objects::get_global()
{    
    return *m_globals;
};

void global_objects::close()
{    
    get_global().close_impl();
};

};
