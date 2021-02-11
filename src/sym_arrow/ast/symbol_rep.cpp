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

#include "symbol_rep.h"
#include "sym_arrow/ast/ast.h"
#include "term_context_data.h"
#include "symbol_context_data.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                          identifier_rep
//----------------------------------------------------------------------
identifier_rep::identifier_rep(const identifier_info& info)
    :base_type(this)
    , m_code(context_type::get().get_context_data().get_fresh_ident_code())
    , m_scope(scope_ptr::from_this(info.m_scope))
{
    size_t N            = info.m_size;
    size_t hash         = eval_hash(info);
    m_name              = details::string_data(info.m_name, N, hash);

    if (N == 0)
        throw std::runtime_error("invalid symbol name");

    if (info.m_special == false && info.m_name[0] == '$')
        throw std::runtime_error("invalid symbol name");
    if (info.m_special == true && info.m_name[0] != '$')
        throw std::runtime_error("invalid symbol name");

    context_type::get().get_context_data().register_ident(this);
};

identifier_ptr identifier_rep::make_special(size_t code, const scope_rep* scope)
{
    std::ostringstream os;
    os << "$" << code;

    std::string name    = os.str();
    identifier_info info(name.c_str(), name.size(), scope);
    info.set_special();

    return identifier_rep::make(info);
}

identifier_ptr identifier_rep::make_special(const std::string& name, const scope_rep* scope)
{
    size_t N    = name.size();

    if (N < 2)
        throw std::runtime_error("invalid symbol name");

    if (name[0] != '$')
        throw std::runtime_error("invalid symbol name");
    if (isalpha(name[1]) == false)
        throw std::runtime_error("invalid symbol name");

    identifier_info info(name.c_str(), name.size(), scope);
    info.set_special();

    return identifier_rep::make(info);
}

bool identifier_rep::is_qualified() const
{
    //TODO:
    if (m_scope.get() == nullptr)
        return false;
    else
        return true;
}

identifier_rep::~identifier_rep()
{
    m_name.destroy();
    context_type::get().get_context_data().unregister_ident(this);
};

size_t identifier_rep::eval_hash(const identifier_info& info)
{
    size_t seed = (size_t)info.m_scope;

    for (size_t i = 0; i < info.m_size; ++i)
        boost::hash_combine(seed, info.m_name[i]);

    return seed;
};

bool identifier_rep::equal(const identifier_info& info) const
{
    if (info.m_size != this->get_name_size())
        return false;

    if (info.m_scope != this->m_scope.get())
        return false;

    int cmp = strcmp(this->get_name(), info.m_name);

    if (cmp != 0)
        return false;

    return true;
};

//----------------------------------------------------------------------
//                          symbol_rep
//----------------------------------------------------------------------

symbol_rep::symbol_rep(const symbol_info& pi)
    :base_type(this), m_hash(pi.m_hash)
    , m_size(pi.m_size), m_expr(nullptr), m_name(identifier_ptr::from_this(pi.m_name))
    , m_type(*pi.m_type)
{
    if (m_type.is_null() == true)
        m_type      = sym_dag::dag_context<unique_nodes_tag>::get().get_context_data().default_type();

    add_symbol(pi.m_name->get_identifier_code());

    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    m_expr              = (expr_ptr*)c.malloc(m_size *sizeof(expr_ptr));

    for (size_t i = 0; i < m_size; ++i)
    {
        expr_handle h   = pi.m_args[i].get_ptr().get();
        new(m_expr+i) expr_ptr(h, sym_dag::copy_t());

        add_symbols(h);
    }
};

symbol_rep::~symbol_rep()
{
    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    for (size_t i = 0; i < m_size; ++i)
        m_expr[i].~expr_ptr();

    c.free(m_expr, m_size * sizeof(expr_ptr));
};

bool symbol_rep::equal(const symbol_info& pi) const
{
    size_t elem_size = size();

    if (elem_size != pi.m_size)
        return false;

    if (this->m_name.get() != pi.m_name)
        return false;

    for (size_t i = 0; i < elem_size; ++i)
    {
        if (pi.m_args[i].get_ptr().get() != this->arg(i))
            return false;
    };

    return true;
};

size_t symbol_rep::eval_hash(const symbol_info& pi)
{
    if (pi.m_hash != 0)
        return pi.m_hash;

    size_t seed = (size_t)pi.m_name; 

    for (size_t i = 0; i < pi.m_size; ++i)
        boost::hash_combine(seed,pi.m_args[i].get_ptr().get());

    pi.m_hash = seed;
    return seed;
};

void symbol_rep::release(stack_type& st)
{
    for (size_t i = 0; i < m_size; ++i)
        st.push_back(m_expr[i].release());
};

};};

/*
TODO
symbol scope::operator[](const symbol& s) const
{
    if (s.get_scope().is_global_scope() == false)
    {
        std::ostringstream msg;
        msg << "unable to create " << this->m_ptr->get_name() << "::";
        s.get_handle()->do_disp(msg, disp_context(true));

        throw std::runtime_error(msg.str());
    };

    if (s.get_handle()->is_unique_symbol() == true)
    {
        std::ostringstream msg;
        msg << "unable to qualify unique symbol";

        throw std::runtime_error(msg.str());
    }

    bool real                   = s.get_handle()->is_real();
    details::context_impl& c    = lib::global_context().get_impl();
    return symbol(c.get_symbol(s.get_handle(), m_ptr.get(), real));
};

*/