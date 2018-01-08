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

#include "sym_arrow/nodes/expr.h"

namespace sym_arrow { namespace details
{

template<class Initializer_impl>
class diff_context_static_initializer_impl;

template<class Initializer_impl>
class function_evaler_static_initializer_impl;

}}

namespace sym_arrow
{

// helper class, that adds new definitions to global diff_context;
// new definitions can also de added directly, but in this case it cannot be
// done during static object initialization
class SYM_ARROW_EXPORT diff_context_initializer
{
    public:
        virtual ~diff_context_initializer(){};

        // implementation of initialization logic
        // this function must be defined in derived classes 
        virtual void initialize(diff_context& dc) = 0;

    private:
        static void register_initializer(diff_context_initializer* init);

        template<class Derived>
        friend class details::diff_context_static_initializer_impl;
};

// helper class, that adds new definitions to global function_evaler;
// new definitions can also de added directly, but in this case it cannot be
// done during static object initialization
class SYM_ARROW_EXPORT function_evaler_initializer
{
    public:
        virtual ~function_evaler_initializer(){};

        // implementation of initialization logic
        // this function must be defined in derived classes 
        virtual void initialize(function_evaler& dc) = 0;

    private:
        static void register_initializer(function_evaler_initializer* init);

        template<class Derived>
        friend class details::function_evaler_static_initializer_impl;
};

// when this class is instantiated, then new global diff_context
// initializer is called;
// Derived class must implement diff_context_initializer::initialize function
template<class Derived>
class diff_context_static_initializer : public diff_context_initializer 
{
    private:
        using impl_type     = details::diff_context_static_initializer_impl<Derived>;

    private:
        static impl_type* m_global_instance; 

    public:
        virtual ~diff_context_static_initializer() override;

    private:
        //force instantiation of m_global_instance
        virtual void use_global(impl_type*)
        {};
};

// when this class is instantiated, then new global function_evaler
// initializer is called;
// Derived class must implement function_evaler_initializer::initialize function
template<class Derived>
class function_evaler_static_initializer : public function_evaler_initializer 
{
    private:
        using impl_type     = details::function_evaler_static_initializer_impl<Derived>;

    private:
        static impl_type* m_global_instance; 

    public:
        virtual ~function_evaler_static_initializer() override;

    private:
        //force instantiation of m_global_instance
        virtual void use_global(impl_type*)
        {};
};

};

#include "sym_arrow/details/global_context_initializers.inl"