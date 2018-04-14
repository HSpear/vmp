




#ifndef   __VMP_TOSTRUCT_ESI_H
#define   __VMP_TOSTRUCT_ESI_H

#include <vector>
#include <stack>
#include <algorithm>
#include "type.h"
using namespace std;


typedef struct _struct_data
{
	char _var[_var_size];

	int _dis_imm;

	int  _lval;
	int  _data;
	int  _size;
	int  _reg;
	int  _prefix;
	int _other;
	int _change;
	int _add;

	int _rva;

	_struct_data()
	{
		memset( _var, -1, sizeof(_var));
		_change = -1;
		_lval = -1;
		_data = -1;
		_add = -1;
		_size = -1;
		_reg = -1;
		_other = 0;
		_dis_imm = 0;
		_rva = 0;
	}
}*_pstruct_data;

class ProcessEsiEncode
{

public:
	int _vm_pos_dispathcer;
	int _vm_entry;
	int _section_offset;

public:
	ProcessEsiEncode();
	ProcessEsiEncode( int _entry);
	~ProcessEsiEncode();

	void _GetVmpEncdoes( char **_buf);
};

#endif   __VMP_TOSTRUCT_ESI_H
