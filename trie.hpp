/*
 * STL-LIKE C++ trie tree library, Version 1.0.0,
 * Copyright (C) 2014, Ren Bin (ayrb13@gmail.com)
 * 
 * This library is free software. Permission to use, copy, modify,
 * and/or distribute this software for any purpose with or without fee
 * is hereby granted, provided that the above copyright notice and
 * this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * a.k.a. as Open BSD license
 * (http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/share/misc/license.template)
 *
 * You can get latest version of this library from github
 * (https://github.com/ayrb13/trie)
 */

#ifndef _TRIE_H_
#define _TRIE_H_

#include <memory>

namespace trie
{
	template<typename _kty
		,typename _ty>
	class trie_node;

	template<typename _kty
		,typename _ty>
	class trie_iterator;

	template<typename _kty>
	struct trie_default_wildcard
	{
		bool operator()(const _kty& key)
		{
			return true;
		}
	};

	template<typename _kty
		,typename _ty
		,typename _single_wildcard = trie_default_wildcard<_kty>
		,typename _multi_wildcard = trie_default_wildcard<_kty>
		,typename _alloc_node = std::allocator<trie_node<_kty, _ty> >
		,typename _alloc_value = std::allocator<_ty> >
	class trie_map;

	template<typename _kty
		,typename _ty>
	class trie_node
	{
	public:
		template<typename _kty_c
			,typename _ty_c
			,typename _single_wildcard_c
			,typename _multi_wildcard_c
			,typename _alloc_node_c
			,typename _alloc_value_c>
		friend class trie_map;

		template<typename _kty_c
			,typename _ty_c>
		friend class trie_iterator;

		typedef trie_node<_kty, _ty> my_type;
	public:
		_ty* get_p_value()
		{
			return *_value;
		}
	private:
		trie_node()
			:_key(_kty())
			,_parent(NULL)
			,_child(NULL)
			,_brother(NULL)
			,_last_brother(NULL)
			,_value(NULL)
			,_child_size(0)
		{
		}
		trie_node(const _kty& nodekey)
			:_key(nodekey)
			,_parent(NULL)
			,_child(NULL)
			,_brother(NULL)
			,_last_brother(NULL)
			,_value(NULL)
			,_child_size(0)
		{
		}
		my_type* get_child_by_key(const _kty& childname)
		{
			my_type* cur_child = _child;
			while(cur_child)
			{
				if(cur_child->_key == childname)
					return cur_child;
				else
					cur_child = cur_child->_brother;
			}
			return cur_child;
		}
		my_type* insert_child(my_type* pchild)
		{
			if(_child == NULL)
			{
				_child = pchild;
				pchild->_parent = this;
			}
			else
			{
				my_type* cur = _child;
				while(cur->_brother)
				{
					if(cur->_child_size != 0)
						cur = cur->_brother;
					else break;
				}
				pchild->_brother = cur->_brother;
				pchild->_last_brother = cur;
				cur->_brother = pchild;
				if(pchild->_brother)
					pchild->_brother->_last_brother = pchild;
				pchild->_parent = this;
			}
			_child_size++;
			while(_last_brother)
			{
				if(_last_brother->_child_size < _child_size)
				{
					_last_brother->_brother = _brother;
					if(_brother)
						_brother->_last_brother = _last_brother;
					if(_last_brother->_last_brother)
						_last_brother->_last_brother->_brother = this;
					_brother = _last_brother;
					_last_brother = _brother->_last_brother;
					_brother->_last_brother = this;
					if(!_last_brother)
					{
						_parent->_child = this;
					}
				}
				else
				{
					break;
				}
			}
			return pchild;
		}
		void remove_this()
		{
			_parent->_child_size--;
			if(_last_brother == NULL)
			{
				_parent->_child = _brother;
			}
			else
			{
				_last_brother->_brother = _brother;
			}
			if(_brother)
			{
				_brother->_last_brother = _last_brother;
			}

			while(_parent->_brother)
			{
				if(_parent->_brother->_child_size > _parent->_child_size)
				{
					_parent->_brother->_last_brother = _parent->_last_brother;
					if(_parent->_last_brother)
						_parent->_last_brother->_brother = _parent->_brother;
					else
					{
						_parent->_parent->_child = _parent->_brother;
					}

					if(_parent->_brother->_brother)
						_parent->_brother->_brother->_last_brother = _parent;
					_parent->_last_brother = _parent->_brother;
					_parent->_brother = _parent->_last_brother->_brother;
					_parent->_last_brother->_brother = _parent;
				}
				else
				{
					break;
				}
			}
		}
	private:
		const _kty _key;
		my_type* _parent;
		my_type* _child;
		my_type* _brother;
		my_type* _last_brother;
		_ty* _value;
		unsigned long _child_size;
	};

	template<typename _kty
		,typename _ty>
	class trie_iterator
	{
	public:
		template<typename _kty_c
			,typename _ty_c
			,typename _single_wildcard_c
			,typename _multi_wildcard_c
			,typename _alloc_node_c
			,typename _alloc_value_c>
			friend class trie_map;
		typedef trie_iterator<_kty, _ty> my_type;
		typedef trie_node<_kty, _ty> node_type;
	public:
		trie_iterator()
			:_p_node(NULL)
		{
		}
		trie_iterator(const my_type& other)
			:_p_node(other._p_node)
		{
		}
	public:
		template<typename reiter>
		reiter get_paths(reiter rbegin) const
		{
			node_type* cur = _p_node;
			while(cur->_parent)
			{
				*rbegin = cur->_key;
				rbegin++;
				cur = cur->_parent;
			}
			return rbegin;
		}
		_ty& get_value()
		{
			return *_p_node->_value;
		}
	public:
		my_type& operator=(const my_type& other)
		{
			_p_node = other._p_node;
			return *this;
		}

		bool operator==(const my_type& other) const
		{
			return _p_node == other._p_node;
		}
		bool operator!=(const my_type& other) const
		{
			return _p_node != other._p_node;
		}
		bool operator<(const my_type& other) const
		{
			return _p_node < other._p_node;
		}
		my_type& operator++()
		{
			while(_p_node)
			{
				if(_p_node->_child)
				{
					_p_node = _p_node->_child;
					if(_p_node->_value)
						return *this;
				}
				else if(_p_node->_brother)
				{
					_p_node = _p_node->_brother;
					if(_p_node->_value)
						return *this;
				}
				else
				{
					while(true)
					{
						if(_p_node->_brother)
						{
							_p_node = _p_node->_brother;
							if(_p_node->_value)
								return *this;
							else break;
						}
						else if(_p_node->_parent == NULL)
						{
							return *this;
						}
						else
						{
							_p_node = _p_node->_parent;
						}
					}
				}
			}
			return *this;
		}
	private:
		trie_iterator(node_type* p_node)
			:_p_node(p_node)
		{
		}
	private:
		node_type* _p_node;
	};

	template<typename _kty
		,typename _ty>
	class const_trie_iterator
	{
	public:
		template<typename _kty_c
			,typename _ty_c
			,typename _single_wildcard_c
			,typename _multi_wildcard_c
			,typename _alloc_node_c
			,typename _alloc_value_c>
			friend class trie_map;
		typedef const_trie_iterator<_kty, _ty> my_type;
		typedef trie_node<_kty, _ty> node_type;
	public:
		const_trie_iterator()
			:_p_node(NULL)
		{
		}
		const_trie_iterator(const my_type& other)
			:_p_node(other._p_node)
		{
		}
	public:
		template<typename reiter>
		void get_paths(reiter begin) const
		{
			if(_p_node)
			{
				node_type* cur = _p_node;
				while(cur->_parent)
				{
					*begin = cur->_key;
					++begin;
				}
			}
		}
		const _ty& get_value() const
		{
			return *_p_node->_value;
		}
	public:
		my_type& operator=(const my_type& other)
		{
			_p_node = other._p_node;
			return *this;
		}
		bool operator==(const my_type& other) const
		{
			return _p_node == other._p_node;
		}
		bool operator!=(const my_type& other) const
		{
			return _p_node != other._p_node;
		}
		bool operator<(const my_type& other) const
		{
			return _p_node < other._p_node;
		}
		my_type& operator++()
		{
			while(_p_node)
			{
				if(_p_node->_child)
				{
					_p_node = _p_node->_child;
					if(_p_node->_value)
						return *this;
				}
				else if(_p_node->_brother)
				{
					_p_node = _p_node->_brother;
					if(_p_node->_value)
						return *this;
				}
				else
				{
					while(true)
					{
						if(_p_node->_brother)
						{
							_p_node = _p_node->_brother;
							if(_p_node->_value)
								return *this;
							else break;
						}
						else if(_p_node->_parent == NULL)
						{
							return *this;
						}
						else
						{
							_p_node = _p_node->_parent;
						}
					}
				}
			}
			return *this;
		}
	private:
		const_trie_iterator(node_type* p_node)
			:_p_node(p_node)
		{

		}
	private:
		const node_type* _p_node;
	};

	template<typename _kty
		,typename _ty
		,typename _single_wildcard
		,typename _multi_wildcard
		,typename _alloc_node
		,typename _alloc_value>
	class trie_map
	{
	public:
		typedef trie_node<_kty,_ty> node_type;
		typedef trie_iterator<_kty,_ty> iterator;
		typedef const_trie_iterator<_kty,_ty> const_iterator;
		typedef unsigned long size_type;
	public:
		trie_map()
		{
			_size = 0;
			_base_node = _create_p_node();
		}
		~trie_map()
		{
			_destroy_p_node_all(_base_node);
		}
	public:
		iterator end()
		{
			return iterator(_base_node);
		}
		iterator begin()
		{
			return ++end();
		}
	public:
		template<typename keyiter>
		std::pair<iterator,bool> insert(keyiter begin, keyiter end, const _ty& value)
		{
			bool ret;
			node_type *tmp,*cur = _base_node;
			keyiter last = end;
			--last;
			for(; begin != end; ++begin)
			{
				tmp = cur->get_child_by_key(*begin);
				if(tmp)
				{
					cur = tmp;
				}
				else
				{
					cur = cur->insert_child(_create_p_node(*begin));
				}
			}
			if(cur->_value)
			{
				*(cur->_value) = value;
				ret = false;
			}
			else
			{
				cur->_value = _create_p_value(value);
				ret = true;
			}
			return std::make_pair(iterator(cur),ret);
		}
		template<typename keyiter, typename OutIter>
		void finds(keyiter begin, keyiter end, OutIter Dest) const
		{
			_finds_leafs_by_path(_base_node,begin,end,Dest,const_iterator());
		}
		template<typename keyiter, typename OutIter>
		void finds(keyiter begin, keyiter end, OutIter Dest)
		{
			_finds_leafs_by_path(_base_node,begin,end,Dest,iterator());
		}
		template<typename keyiter>
		const_iterator find(keyiter begin, keyiter end) const
		{
			node_type* cur = _base_node;
			for(; begin != end && cur != NULL; ++begin)
			{
				cur = cur->get_child_by_key(*begin);
			}
			if(!cur) cur = _base_node;
			else if(!cur->_value) cur = _base_node;
			return const_iterator(cur);
		}
		template<typename keyiter>
		iterator find(keyiter begin, keyiter end)
		{
			node_type* cur = _base_node;
			for(; begin != end && cur != NULL; ++begin)
			{
				cur = cur->get_child_by_key(*begin);
			}
			if(!cur) cur = _base_node;
			else if(!cur->_value) cur = _base_node;
			return iterator(cur);
		}
		iterator erase(iterator iter)
		{
			if(iter._p_node->_child)
			{
				_destroy_p_value(iter._p_node->_value);
				iter._p_node->_value = NULL;
				return ++iter;
			}
			else
			{
				while(iter._p_node->_parent)
				{
					iter._p_node->remove_this();
					node_type* parent = iter._p_node->_parent;
					if(!parent->_child)
					{
						_destroy_p_node_only(iter._p_node);
						iter._p_node = parent;
					}
					else
					{
						iterator ret = iter;
						++ret;
						_destroy_p_node_only(iter._p_node);
						return ret;
					}
				}
				return iter;
			}
		}
		template<typename keyiter>
		iterator erase(keyiter begin, keyiter end)
		{
			iterator iter = find(begin, end);
			if(iter == end())
				return iter;
			if(iter._p_node->_child)
			{
				_destroy_p_value(iter._p_node->_value);
				iter._p_node->_value = NULL;
				return ++iter;
			}
			else
			{
				while(iter._p_node->_parent)
				{
					iter._p_node->remove_this();
					node_type* parent = iter._p_node->_parent;
					if(!parent->_child)
					{
						_destroy_p_node_only(iter._p_node);
						iter._p_node = parent;
					}
					else
					{
						iterator ret = iter;
						++ret;
						_destroy_p_node_only(iter._p_node);
						return ret;
					}
				}
				return iter;
			}
		}
		void clear()
		{
			if(_base_node->_child)
			{
				_destroy_p_node_all(_base_node->_child);
				_base_node->_child = NULL;
			}
		}
		size_type size()
		{
			return _size;
		}
	private:
		template<typename Iter,typename OutIter,typename MapIter>
		void _finds_leafs_by_path(node_type* pnode, Iter begin, Iter end, OutIter Dest, MapIter)
		{
			if(begin == end)
			{
				if(pnode->_value)
				{
					*Dest = MapIter(pnode);
					++Dest;
				}
			}
			else
			{
				if(_s_wildcard(*begin))
				{
					Iter next = begin;
					++next;
					for(node_type* cur = pnode->_child;cur != NULL;cur = cur->_brother)
						_finds_leafs_by_path(cur,next,end,Dest,MapIter());
				}
				else if(_m_wildcard(*begin))
				{
					Iter next = begin;
					++next;
					while(true)
					{
						if(next != end)
						{
							if(_m_wildcard(*next))
							{
								begin = next;
								++next;
								continue;
							}
							else
							{
								_finds_leafs_by_path(pnode,next,end,Dest,MapIter());
								for(node_type* cur = pnode->_child;cur != NULL;cur = cur->_brother)
									_finds_leafs_by_path(cur,begin,end,Dest,MapIter());
							}
						}
						else
						{
							_get_all_leafs(pnode,Dest,MapIter());
						}
						break;
					}
				}
				else
				{
					node_type* cur = pnode->get_child_by_key(*begin);
					if(cur)
						_finds_leafs_by_path(cur,++begin,end,Dest,MapIter());
				}
			}
		}
		template<typename OutIter,typename MapIter>
		void _get_all_leafs(node_type* pnode, OutIter Dest, MapIter)
		{
			if(pnode->_value)
			{
				*Dest = MapIter(pnode);
				++Dest;
			}
			for(node_type* cur = pnode->_child;cur != NULL;cur = cur->_brother)
				_get_all_leafs(cur,Dest,MapIter());

		}
	private:
		node_type* _create_p_node()
		{
			node_type* pnode = _node_alloc.allocate(1);
			new (pnode) node_type();
			return pnode;
		}
		node_type* _create_p_node(const _kty& key)
		{
			node_type* pnode = _node_alloc.allocate(1);
			new (pnode) node_type(key);
			return pnode;
		}
		_ty* _create_p_value(const _ty& value)
		{
			_ty* pvalue = _value_alloc.allocate(1);
			new (pvalue) _ty(value);
			_size++;
			return pvalue;
		}
	private:
		void _destroy_p_node_all(node_type* pnode)
		{
			if(pnode->_child)
				_destroy_p_node_all(pnode->_child);
			if(pnode->_brother)
				_destroy_p_node_all(pnode->_brother);
			_destroy_p_node_only(pnode);
		}
		void _destroy_p_node_only(node_type* pnode)
		{
			if(pnode->_value)
			{
				_destroy_p_value(pnode->_value);
			}
			_node_alloc.destroy(pnode);
			_node_alloc.deallocate(pnode,1);
		}
		void _destroy_p_value(_ty* pvalue)
		{
			_value_alloc.destroy(pvalue);
			_value_alloc.deallocate(pvalue,1);
			_size--;
		}
	private:
		node_type* _base_node;
		_alloc_node _node_alloc;
		_alloc_value _value_alloc;
		_single_wildcard _s_wildcard;
		_multi_wildcard _m_wildcard;
		size_type _size;
	};
}
#endif//_TRIE_H_