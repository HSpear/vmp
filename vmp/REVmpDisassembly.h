



#ifndef    __MY_RE_VMP_DISASSEMBLY_H
#define    __MY_RE_VMP_DISASSEMBLY_H

#include "itab.h"
#include <string>
#include <set>
#include <vector>
using namespace std;

struct __struct_node;
typedef struct __struct_node *_pstruct_node;

typedef struct __operand
{
	char    _lval;
	short   _modrm_mod;//0x24 0x84 0x44 0xC 0xA 0xE (0x?|4)
	short   _modrm_reg;
	char    _modrm_rm;
	char    _sib_base;
	char    _sib_scable;
	int     _rest;
	char    _isaddressing;
	char    _reg_type;
	int     _other;
	int     _rest_size;
	int     _jcc_index;
	struct __operand()
	{
		memset( this, 0, sizeof(*this));
	}
}_operand;

typedef struct __disasm 
{
	_mnemonic_code lock;
	_mnemonic_code rep;
	_mnemonic_code mnemonic;
	char          prefiexs;
	_operand	   operand[3];

	struct __disasm()
	{
		lock = w_none;
		rep =  w_none;
		mnemonic = w_none;
		prefiexs = 0;
	}
}_disasm, *_pdisasm;

typedef struct __result
{
	int pos;
	_mnemonic_code mnemonic;
	int _has_n;

	__result( const int& _p, const _mnemonic_code& _m,const int& _n)
	{
		pos = _p;
		mnemonic = _m;
		_has_n = _n;
	}

	bool operator<( const __result&a) const
	{
		return pos < a.pos;
	}

}_result;

static int _check_is_correct_type1( const int& index);
static void _get_vmp_push_data( int &data ,int _first, int _last);

class Disasm
{
public:
	set<_result> res;
	vector<_pdisasm> pvda;
	string _vmp_encodes;
	vector<string> _dump_encodes;

public:
	int _section_erva;
	int _section_brva;

public:
	Disasm();
	Disasm( int rva,int size);
	~Disasm();

public:
	int _Process_Disp_Imme( _mnemonic_code code, const int& data);
	int _SearchJcc_Tpye( int data);
	int _SearchJcc_RVA( int _first);
	void _REDisassmebly();
	void _PtfASM();
	void _PtfDump();

public:
	void _AnalyzeModrm( string &s, int pi, int oi);

public:
	void _FreeNodes();
	void _FreeResult();
	void _AnalyzeResult();
	void _Get_Opcode_Mnemonic_Result( int size, const _pstruct_node pn);
	void _Get_Opcode_Mnemonic();

	int  _String_Match_Searching( const string& text, const string& pattern, _mnemonic_code _mnemonic, int _n);
	int  _Get_Opcode_Mnemonic_init();

public:
	void _Analyze();
};


#endif     __MY_RE_VMP_DISASSEMBLY_H