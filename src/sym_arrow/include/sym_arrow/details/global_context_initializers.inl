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

#include "sym_arrow/functions/global_context_initializers.h"

namespace sym_arrow { namespace details
{

template<class Initializer_impl>
class diff_context_static_initializer_impl
{
    private:
        Initializer_impl    m_instance;

    public:
        diff_context_static_initializer_impl()
        {
            diff_context_initializer::register_initializer(&m_instance);
        }
};

template<class Initializer_impl>
class function_evaler_static_initializer_impl
{
    private:
        Initializer_impl    m_instance;

    public:
        function_evaler_static_initializer_impl()
        {
            function_evaler_initializer::register_initializer(&m_instance);
        }
};

}}

namespace sym_arrow
{

template<class Derived>
diff_context_static_initializer<Derived>::~diff_context_static_initializer()
{
    use_global(m_global_instance);
};

template<class Derived>
typename diff_context_static_initializer<Derived>::impl_type*
diff_context_static_initializer<Derived>::m_global_instance
    = sym_dag::global_objects::make_before<impl_type>();

template<class Derived>
function_evaler_static_initializer<Derived>::~function_evaler_static_initializer()
{
    use_global(m_global_instance);
};

template<class Derived>
typename function_evaler_static_initializer<Derived>::impl_type*
function_evaler_static_initializer<Derived>::m_global_instance
    = sym_dag::global_objects::make_before<impl_type>();

};
