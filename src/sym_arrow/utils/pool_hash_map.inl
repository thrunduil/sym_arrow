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

#include "sym_arrow/utils/pool_hash_map.h"

namespace sym_arrow { namespace utils { namespace details
{

template<class Key, class Value, class Hash_equal>
inline hash_map_data<Key, Value, Hash_equal>::hash_map_data
                            (const Key& key, const Value& v)
    :m_key(key), m_value(v)
{};

template<class Key, class Value, class Hash_equal>
inline hash_map_data<Key, Value, Hash_equal>::hash_map_data
                            (const Key& key, Value&& v)
    :m_key(key), m_value(std::move(v))
{};

template<class Key, class Value, class Hash_equal>
inline size_t hash_map_data<Key, Value, Hash_equal>::hash_value() const
{
    return Hash_equal::hash_value(m_key);
}

template<class Key, class Value, class Hash_equal>
inline bool hash_map_data<Key, Value, Hash_equal>::equal(const Key& other) const
{
    return Hash_equal::equal(m_key, other);
}

template<class Key, class Value, class Hash_equal>
inline size_t hash_map_data<Key, Value, Hash_equal>::eval_hash(const Key& k)
{
    return Hash_equal::hash_value(k);
}

template<class Key, class Value, class Hash_equal>
inline const Value& hash_map_data<Key, Value, Hash_equal>::get_value() const
{
    return m_value;
}

template<class Key, class Value, class Hash_equal>
inline Value& hash_map_data<Key, Value, Hash_equal>::get_value()
{
    return m_value;
}

template<class Key, class Value, class Hash_equal>
template<class Stack>
inline void hash_map_data<Key, Value, Hash_equal>::release(Stack& st)
{
    m_value.release(st);
}

}}}

namespace sym_arrow { namespace utils
{

template<class Key, class Value, class Hash_equal>
inline pool_hash_map<Key, Value, Hash_equal>::pool_hash_map()
{};

template<class Key, class Value, class Hash_equal>
pool_hash_map<Key, Value, Hash_equal>::~pool_hash_map()
{
    clear();
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::clear()
{
    m_table.clear();
};

template<class Key, class Value, class Hash_equal>
template<class Stack>
void pool_hash_map<Key, Value, Hash_equal>::clear(Stack& st)
{
    m_table.clear(st);
};

template<class Key, class Value, class Hash_equal>
inline size_t pool_hash_map<Key, Value, Hash_equal>::size() const
{
    return m_table.size();
}

template<class Key, class Value, class Hash_equal>
inline typename pool_hash_map<Key, Value, Hash_equal>::handle_type 
pool_hash_map<Key, Value, Hash_equal>::find(const key_type& key)
{
    return m_table.find(key);
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::insert(handle_type h, 
                           const key_type& k, const value_type& v)
{
    return m_table.set(h, impl_type(k, v));
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::insert(handle_type h, 
                           const key_type& k, value_type&& v)
{
    return m_table.set(h, impl_type(k, std::move(v)));
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::remove(handle_type h)
{
    return m_table.remove(h);
};

template<class Key, class Value, class Hash_equal>
template<class Stack_type>
void pool_hash_map<Key, Value, Hash_equal>::remove(handle_type h, Stack_type& st)
{
    return m_table.remove(h, st);
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::insert(const key_type& k, const value_type& v)
{
    auto h = m_table.find(k);
    m_table.set(h, impl_type(k, v));
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::insert(const key_type& k, value_type&& v)
{
    auto h = m_table.find(k);
    m_table.set(h, impl_type(k, std::move(v)));
};

template<class Key, class Value, class Hash_equal>
void pool_hash_map<Key, Value, Hash_equal>::close()
{
    m_table.close();
};

};};
