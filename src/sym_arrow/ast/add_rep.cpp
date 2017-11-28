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

#include "sym_arrow/ast/add_rep.inl"
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/details/scalar.inl"
#include "sym_arrow/utils/stack_array.h"

namespace sym_arrow { namespace ast
{

namespace sd = sym_arrow :: details;

add_rep::~add_rep()
{
    for (size_t i = 0; i < m_size + 1; ++i)
        m_data[i].~value_expr();

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    c.free(m_data, (1+m_size)*sizeof(value_expr));
};

void add_rep::release(stack_type& st)
{
    for (size_t i = 0; i < m_size + 1; ++i)
        st.push_back(m_data[i].m_expr.release());
};

void add_rep::remove_add(const add_rep* h, value& add, expr& res)
{
    add = h->V0();

    if (h->V0().is_zero() == true && h->has_log() == false)
    {        
        res = expr(h);
        return;
    };

    if (h->size() == 1 && h->has_log() == false && h->V(0).is_one() == true)
    {
        res = expr(h->E(0));
        return;
    };

    bool has_log    = h->has_log();
    size_t n        = h->size();

    add_rep_info<value_expr> ai(value::make_zero(), n, h->m_data + 1, 
                                has_log ? h->m_data : nullptr);

    expr_ptr res_ptr    = add_rep::make(ai);
    res                 = expr(res_ptr);
    return;
};

value add_rep::add_scalar_normalize(const add_rep* h, value& add, expr& res)
{
    value new_add       = h->V0() + add;

    if (h->size() == 1 && h->has_log() == false && new_add.is_zero() == true)
    {
        res = expr(h->E(0));
        return h->V(0);
    };

    value scal          = h->has_log() ? value::make_one()
                            : cannonize::get_normalize_scaling(new_add, h->size(), h->VE());

    // check if resulting add_rep is normalized; maximum value of scalars must be 
    // one and first nonzero scalar must be positive
    bool is_norm    = scal.is_one();

    if (is_norm)
    {
        bool has_log    = h->has_log();
        size_t n        = h->size();

        add_rep_info<value_expr> ai(new_add, n, h->m_data + 1, 
                                    has_log ? h->m_data : nullptr);

        expr_ptr res_ptr    = add_rep::make(ai);
        res                 = expr(res_ptr);
        return value::make_one();
    };

    //expression must be normalized

    //destructor is trivial
    using item_handle       = build_item<value>::handle_type;
    static const size_t ih_buffer_size = 20;

    size_t n                = h->size();

    sd::stack_array<sd::pod_type<item_handle>, ih_buffer_size> ih_array(n);
    item_handle* ih = (item_handle*)ih_array.get();
    
    for (size_t i = 0; i < n; ++i)
        new(ih + i) item_handle(h->V(i) / scal, h->E(i));

    add_rep_info<item_handle> ai(new_add / scal, n, ih, nullptr);

     expr_ptr res_ptr    = add_rep::make(ai);
     res                 = expr(res_ptr);
     return scal;
};

void add_rep::remove_add_log(const add_rep* h, expr& res)
{
    if (h->V0().is_zero() == true && h->has_log() == false)
    {
        res = expr(h);
        return;
    };

    if (h->size() == 0)
    {
        res = scalar_rep::make_zero();
        return;
    }

    if (h->size() == 1 && h->V(0).is_one() == true)
    {
        res = expr(h->E(0));
        return;
    };

    size_t n    = h->size();

    add_rep_info<value_expr> ai(value::make_zero(), n, h->m_data+1, nullptr);

    expr_ptr res_ptr    = add_rep::make(ai);
    res                 = expr(res_ptr);
    return;
};

};};
