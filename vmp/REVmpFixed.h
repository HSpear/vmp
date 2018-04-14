


#ifndef    __MY_RE_VMP_FIXED_H
#define    __MY_RE_VMP_FIXED_H

#include "itab.h"


typedef struct _node 
{
	char _var1;
	char _var2;
	char _var3;
	char _var4;

	char _data;
	int  _disp_imm;

	_node& operator=( const _node& n)
	{
		if( &n != this)
		{
			this->_var1 = n._var1;
			this->_var2 = n._var2;
			this->_var3 = n._var3;
			this->_var4 = n._var4;
			this->_data = n._data;
			this->_disp_imm = n._disp_imm;
		}
		return *this;
	}

	bool operator==( const _node&n)
	{
		return &n == this;
	}
}node, *pnode;

typedef struct __struct_node
{
	pnode table;
	int size;
	_mnemonic_code code;
	int n;
}_struct_node, *_pstruct_node;

typedef vector<pnode> *vpnode;
void _DestoryNode(vector<pnode> &s);
void _DestoryNode(vector<vpnode> &s);
void _PtfNode( const vector<vpnode> &s);
void _MakeNode( void **s);

int  _GetNodeIndex( vector<pnode> &nd, void *s);

void _vmp_fixed_searching( vpnode &tmp, int size, int index);
void _vmp_fixed( node *_node, int _size);
int  __vmp_fixed( vector<pnode> &nd, int &_cur_index, char __lval, char *__type);

void _Conver_Code_To_String( string &s, const vector<pnode>& nd);
void _Conver_Code_To_String( string &s, const vector<_struct_data>& vc);

void _PtfSimVc();
#endif     __MY_RE_VMP_FIXED_H