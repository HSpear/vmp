


#include "ProcessEsiEncode.h"
#include "revmpfixed.h"
#include "type.h"

#include <string>
using namespace std;

#define  _POSSIBLE_TURE   0x1
#define  _POSSIBLE_FALSE  0x0

/************************************************************************/
/* sub_cmp_sbb                                                          */
/************************************************************************/

int _node_size = 0;

//_var1


/************************************************************************/
/* vmp_fixed_need_func                                                  */
/************************************************************************/

int _check_is_lods_pop( node *s)
{
	return s->_var1 == 2
		&& s->_var2 == 2
		&& ( !s->_var3 || s->_data != 4);
}

int _check_is_lods_push_context( node *s, int lval, int reg)
{
	return s->_var1 == 1
		&& s->_var2 == 2
		&& lval == s->_var3
		&& reg == s->_data;
}

int _check_is_lods_push( node *s, char lval)
{
	return s->_var1 == 1
		&& s->_var2 < 3
		&& lval == s->_var3;
}

int _check_is_lodsw_push( node *s, char lval)
{
	return s->_var1 == 1
		&& s->_var2 == 1
		&& lval == s->_var3;
}

int _check_is_pop_ax( node *s)
{
	return s->_var1 == 2 && s->_var2 == 1;
}

int _check_is_push_sp_esp( node *s)
{
	return s->_var2 == 2
		&& s->_var3 < 3
		&& s->_data == 4;
}

vector<pnode> nd;
vector<vpnode> arr_nd;
char _possible[] = {
	_POSSIBLE_FALSE,_POSSIBLE_FALSE,
	_POSSIBLE_FALSE,_POSSIBLE_TURE,
	_POSSIBLE_TURE,_POSSIBLE_FALSE,
	_POSSIBLE_TURE,_POSSIBLE_TURE,
};
const int _possible_size = sizeof(_possible) / 2;
/************************************************************************/
/* other                                                                */
/************************************************************************/

int  _GetNodeIndex( vector<pnode> &nd, void *s)
{
	vector<pnode>::iterator it = find( nd.begin(), nd.end(), (pnode)s);
	return   it != nd.end() ? it - nd.begin() : -1;
}

void _Conver_Code_To_String( string &s, const vector<pnode>& nd)
{
	s.clear();
	int size = nd.size();
//	printf("\n");
	for( int i = 0; i < size; i++)
	{
//		printf("%x %x %x %x\n", nd[i]->_var1, nd[i]->_var2, nd[i]->_var3, nd[i]->_var4);
		s += nd[i]->_var1;
		s += nd[i]->_var2;
		s += nd[i]->_var3;
		s += nd[i]->_var4;
	}
}

void _Conver_Code_To_String( string &s, const vector<_struct_data>& vc)
{
	s.clear();
	int size = vc.size();
//	printf("\n");
	for( int i = 0; i < size; i++)
	{
//		printf("%x %x %x %x\n", vc[i]._var[0], vc[i]._var[1], vc[i]._var[2], vc[i]._var[3]);
		for( int j = 0; j < _var_size; j++)
		{
			s += vc[i]._var[j];
		}
	}
}
/************************************************************************/
/* xxx                                                                  */
/************************************************************************/

void _MakeNode( void **s, int size)
{
	nd.clear();
	for( int i = 0; i < size; i++)
	{
		pnode pntmp = new node( *((pnode)(s) + i));
		nd.push_back( pntmp);
	}
}

int __vmp_fixed( vector<pnode> &nd,int &_cur_index, char __lval, char *__type)//, int _lval)
{
	int nRet = 0;
	int __ = 0;
	int _judge = 0;
	if( _cur_index >= 2)
	{
		int cur_index = _cur_index;
		node *cur = nd[cur_index];
		char _var1 = cur->_var1;
		char _var3 = cur->_var3;
		char _reg = cur->_data;
		if( _var1 != 4 && (unsigned char)_var1 != 0xf6 || _var3 != __lval)
		{
			if( (unsigned char)(_var1 - 0x1c) >= 2)
			{
				if( _check_is_lods_pop(cur) && _node_size - 1 > _cur_index)
				{
					node *next = nd[_cur_index+1];
					if( _check_is_lods_push_context( 
						next,
						_var3,
						_reg))
					{

					}
				}
				else if( _check_is_pop_ax( cur))
				{
					//预留
				}
			}
			else if( _var3 == __lval)
			{
				node *_prev_prev = nd[_cur_index - 2];
				node *_prev = nd[_cur_index - 1];
				if( _check_is_lodsw_push( _prev_prev, 1) && _check_is_lods_push( _prev, __lval))
				{
					_cur_index -= 2;
					//不确定
				}
			}
		}
		else
		{
			node *_prev_prev = nd[_cur_index - 2];
			node *_prev = nd[_cur_index - 1];
			if( _check_is_lods_push( _prev_prev, __lval) && _check_is_lods_push( _prev, __lval))
			{
				_cur_index -= 2;
				//两种可能
				if( __type[0] == _POSSIBLE_FALSE)
				{
					nRet = !_check_is_push_sp_esp( _prev_prev) && !_check_is_push_sp_esp( _prev);
				}
				else
				{
					if( _check_is_push_sp_esp( _prev_prev))
					{
						if( _prev->_var2 == 1)
						{
							_prev->_disp_imm += 4;
						}
					}
					else if( _check_is_push_sp_esp( _prev))
					{
						if( _prev_prev->_var2 == 1)
						{
							_prev_prev->_disp_imm -= 4;
						}
					}
					else
					{
						nRet = 1;
					}
					if( _prev->_var2 != 2
						|| _prev_prev->_var2 != 2
						|| _prev_prev->_data != _prev->_data)
					{
						node tmp = *_prev;
						*_prev = *_prev_prev;
						*_prev_prev = tmp;
					}
					else
					{
						_prev->_data = 4;
						_prev->_var3 = 2;
						//生成新的结构
						pnode tmp = new node;
						tmp->_var1 = t_push;
						tmp->_var2 = 0x3;
						tmp->_var3 = __lval;
						tmp->_var4 = 0x3;
						tmp->_data =  t_reg_none;
						nd.insert( nd.begin() + cur_index,1,tmp);
					}
				}
			}
			else
			{
				if( _check_is_lods_push( _prev, __lval))
				{
					--_cur_index;
					if( _check_is_push_sp_esp( _prev))
					{
						return nRet;
					}
					int _ = _cur_index - 1;
					if( !__vmp_fixed( nd, _, __lval, __type))
					{
						return nRet;
					}
					_cur_index = _;
					__ = _;
					nRet = 1;
				}
				else
				{
					int _ = _cur_index - 1;
					if( __vmp_fixed( nd, _, __lval, __type))
					{
						_cur_index = _;
						node *tmp1 = nd[_ - 1];
						_prev = nd[_];
						if( _check_is_lods_push( tmp1, __lval))
						{
							_cur_index--;
							if( _check_is_push_sp_esp( tmp1))
							{
								return nRet;
							}
							__ = _cur_index;
							nRet = 1;
						}
						else
						{
							_ = _cur_index - 1;
							if( __vmp_fixed( nd, _, __lval, __type))
							{
								_cur_index = _;
								__ = _;
								nRet = 1;
							}
						}
					}
				}
				if( nRet && __type[1] == _POSSIBLE_FALSE)
				{
					//两种可能
					int _index1 = _GetNodeIndex( nd, _prev);
					int _index2 = _GetNodeIndex( nd, cur);
					_judge = __ - _index1 == _index1 - _index2;
					if ( _judge &&  _index2 - 1 >= _index1)
					{
						int _size = _index2 - _index1;
						int _first = _index1;
						while(1)
						{
							int _ret = 0;//找到不同的
							if( !_ret)
								break;
							_first++;
						}
						_judge = 0;
					}
					if( _judge)
					{
						nd[_index1]->_var1 = 1;
						nd[_index1]->_var2 = 2;
						nd[_index1]->_data = 4;
						nd[_index1]->_var3 = 2;
						nd[_index1+1]->_var1 = 1;
						nd[_index1+1]->_var2 = 3;
						nd[_index1+1]->_var4 = 3;
						nd[_index1+1]->_var3 = __lval;
						if( _index1 + 2 <= _index2 - 1)
						{
							int _index3 = _index1 + 2 - _index2;//为负数
							int _index4 = _index2 - 1;
							for( ; _index3;)
							{

							}
						}
					}
					else
					{
						if( _index2 - 1 >= __)
						{
							for( int _dis = _index2 - __,_first = __; _dis; _dis--, _first++)
							{
								// other |= 4
							}
						}
						if( _index2 - 1 >= _index1)
						{
							for( int _dis = _index2 - _index1, _first = _index1; _dis; _dis--, _first++)
							{
								//插入 然后 删除
								int _i = _first + __ - _index1;
								pnode tmp = nd[_first];
								nd.insert(  nd.begin() + _i, 1, tmp);
								nd.erase( nd.begin() + _first + 1);
							}
						}
					}
				}

			}
		}
	}
	return nRet;
}

/************************************************************************/
/* call                                                                 */
/************************************************************************/

void _DestoryNode(vector<pnode> &s)
{
	s.clear();
}

void _DestoryNode(vector<vpnode> &s)
{
	int size = s.size();
	for( int i = 0; i < size; i++)
	{
		int nd_size = s[i]->size();
		vpnode tmp = s[i];
		for( int j = 0; j < nd_size; j++)
		{
			if( (*tmp)[j] != NULL)
			{
				delete (*tmp)[j];
				(*tmp)[j] = NULL;
			}
		}
	}
	s.clear();
}

void _PtfNode( const vector<vpnode> &s)
{
	int size = s.size();
	for( int i = 0; i < size; i++)
	{
		int nd_size = arr_nd[i]->size();

//		printf("\n");
		for( int j = 0; j < nd_size; j++)
		{
			vpnode tmp = arr_nd[i];
			vector<pnode> tmp1 = *tmp;
//			printf("%x %x %x %x\n", tmp1[j]->_var1, tmp1[j]->_var2, tmp1[j]->_var3, tmp1[j]->_var4);
		}
	}
//	printf("\n\n");
}

void _vmp_fixed_searching( vpnode &tmp, int size, int index)
{
	for( int j = size - 1; j >= 2; j--)
	{
		__vmp_fixed( *tmp, j, (*tmp)[j]->_var3, _possible + index * 2);
	}
}

void _vmp_fixed( node *_node, int _size)
{
	//maybe
	_node_size = _size;
	arr_nd.clear();
	for( int i = 0; i < _possible_size; i++)
	{
		vpnode tmp = new vector<pnode>;
		_MakeNode( (void**)_node, _size);
		tmp->assign( nd.begin(), nd.end());
		_vmp_fixed_searching( tmp, _size, i);
		arr_nd.push_back( tmp);
	}
}