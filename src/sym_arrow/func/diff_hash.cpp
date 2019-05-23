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

#include "diff_hash.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace details
{

//--------------------------------------------------------------------
//                  symbol_wrap
//--------------------------------------------------------------------

symbol_wrap::symbol_wrap(const symbol& s)
    :m_symbol(s)
{};

void symbol_wrap::release(stack_type& st)
{
    st.push_back(const_cast<ast::symbol_ptr&>(m_symbol.get_ptr()).release());
}

//--------------------------------------------------------------------
//                  diff_hash_data_base
//--------------------------------------------------------------------
// create uninitialized object
diff_hash_data_base::diff_hash_data_base()
{};

void diff_hash_data_base::release(stack_type& st)
{
    base_type::close(st);
};

diff_hash_data_base::diff_hash_data_base(const symbol& s)
{
    base_type::push_back(symbol_wrap(s));
};

void diff_hash_data_base::add(const symbol& s)
{
    base_type::push_back(symbol_wrap(s));
};

bool diff_hash_data_base::is_empty() const
{
    return base_type::current_size() == 0 && base_type::has_previous() == false;
}

size_t diff_hash_data_base::current_size() const
{
    return base_type::current_size();
}

bool diff_hash_data_base::has_previous() const
{
    return base_type::has_previous();
}

const diff_hash_data_base* diff_hash_data_base::get_previous() const
{
    return base_type::get_previous();
}

const symbol& diff_hash_data_base::elem(size_t pos) const
{
    return base_type::elem(pos).get();
}

//--------------------------------------------------------------------
//                  diff_hash_data
//--------------------------------------------------------------------
diff_hash_data::diff_hash_data()
{};

diff_hash_data::diff_hash_data(const expr& dif)
    :m_diff(dif.get_ptr())
{};

expr diff_hash_data::get_diff() const
{
    return expr(m_diff);
}

void diff_hash_data::release(stack_type& st)
{
    st.push_back(m_diff.release());
}

bool diff_hash_data::is_empty() const
{
    return false;
};

//--------------------------------------------------------------------
//                  diff_hash
//--------------------------------------------------------------------

diff_hash::diff_hash()
{
    using track_func    = ast::expr_base::track_function;
    using track_buf     = expr_cache::track_function;
    using stack_type    = ast::expr_base::stack_type;

    size_t code     = (size_t)ast::track_function_code::diff;
    track_func f    = [=](ast::expr_handle h, stack_type& st) 
                        { this->unregister(h, st); };

    ast::expr_base::add_tracking_function(code, f);

    sym_dag::registered_dag_context::get().register_cache(this);

    track_buf f2    = [=](ast::expr_handle h, stack_type& st) 
                        { this->unregister_cache(h, st); };

    m_cache.set_track_function(f2);
}

diff_hash::~diff_hash()
{
    m_cache.close();
    m_hash_map_root.close();
    m_hash_map_dif.close();
};

void diff_hash::clear()
{
    m_cache.clear();
    m_hash_map_root.clear();
    m_hash_map_dif.clear();
};

diff_hash* g_diff_hash
    = sym_dag::global_objects::make_before<diff_hash>();

diff_hash& diff_hash::get()
{
    return *g_diff_hash;
}

void diff_hash::unregister_cache(ast::expr_handle h, stack_type& st)
{
    return unregister(h, st);
};

void diff_hash::unregister(ast::expr_handle h, stack_type& st)
{
    auto pos = m_hash_map_root.find(h);

    if (pos.empty() == true)
        return;

    const diff_hash_data_base* sym_data = &pos->get_value();

    for (;;)
    {
        size_t n    = sym_data->current_size();

        for (size_t i = 0; i < n; ++i)
        {
            const symbol& sym   = sym_data->elem(i);
            size_t code         = sym.get_symbol_code();

            auto pos_dif        = m_hash_map_dif.find(expr_sym(h, code));
            m_hash_map_dif.remove(pos_dif, st);
        };

        if (sym_data->has_previous() == false)
            break;

        sym_data    = sym_data->get_previous();
    };

    auto pos2 = m_hash_map_root.find(h);
    m_hash_map_root.remove(pos2, st);
};

expr diff_hash::find(ast::expr_handle h, const symbol& s)
{
    bool is_tracked = h->is_tracked();

    if (is_tracked == false)
        return expr();

    size_t sym_code = s.get_symbol_code();
    auto ed         = m_hash_map_dif.find(expr_sym(h, sym_code));

    if (ed.empty() == true)
        return expr();

    const diff_hash_data& diff_data = ed->get_value();

    if (diff_data.is_empty() == true)
        return expr();

    const expr& dif = diff_data.get_diff();

    m_cache.add(ast::expr_ptr::from_this(h));

    return dif;
};

void diff_hash::add(ast::expr_handle h, const symbol& s, const expr& dif)
{   
    assertion(h != nullptr, "error in set_hashed_subexpr_elim");

    // do not assign diff data for unique objects
    if (h->refcount() < min_refcount)
        return;

    auto pos = m_hash_map_root.find(h);

    if (pos.empty() == true)
        m_hash_map_root.insert(h, diff_hash_data_base(s));
    else
        pos->get_value().add(s);

    size_t sym_code = s.get_symbol_code();
    m_hash_map_dif.insert(expr_sym(h, sym_code), diff_hash_data(dif));
    
    h->set_tracked(true);
    m_cache.add(ast::expr_ptr::from_this(h));
};

};};
