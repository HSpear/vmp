



#include "REVmpDisassembly.h"
#include "ProcessEsiEncode.h"
#include "REVmpFixed.h"
#include "type.h"
#include "itab.h"
#include "extern.h"

extern vector<_struct_data> sim_vc;
extern vector<vpnode> arr_nd;
extern vector<pnode> nd;


#define  _BASEADDR               0x400000
#define _MAX_SIZE                0x10
#define _NONE_NONE               0x0       
#define _JCC_HAS_N               0x1
#define _REModRM( a, b, c)       ( ( a << 6 ) + ( b << 3) + c)
#define _PADDING_ASM( a, b)      ( a += b)
#define _ITAB_IS_NONE( itab)     ( itab != w_none)
#define _CLAC_SUB( a, b)         ( a - b)

/************************************************************************/
/* ptfsimvc                                                             */
/************************************************************************/

void _PtfSimVc()
{

	printf("\n");
	int _vc_size = sim_vc.size();
	for( int i = 0; i < _vc_size; i++)
	{
		printf("%x %x %x %x", sim_vc[i]._var[0], sim_vc[i]._var[1], sim_vc[i]._var[2], sim_vc[i]._var[3]);
		if( sim_vc[i]._reg != -1)
		{
			printf(" SecondVar_Trans_ModRM_rm_Index:%d", sim_vc[i]._reg);
		}
		else if( sim_vc[i]._size > 1)
		{
			printf(" data:%x", sim_vc[i]._data);
		}
		else if( sim_vc[i]._add == 1)
		{
			printf(" tree");
		}
		printf("\n");
	}
}

/************************************************************************/
/* cmp                                                                  */
/************************************************************************/

node _push_mem_byte[] = {
	t_push, t_mem_type, t_byte, t_mem_es, t_init_disp_imm,
	t_push, t_mem_type, t_byte, t_mem_cs, t_init_disp_imm,
	t_push, t_mem_type, t_byte, t_mem_ss, t_init_disp_imm,
	t_push, t_mem_type, t_byte, t_mem_ds, t_init_disp_imm,
	t_push, t_mem_type, t_byte, t_mem_fs, t_init_disp_imm,
	t_push, t_mem_type, t_byte, t_mem_gs, t_init_disp_imm
};

node _push_mem_word[] = {
	t_push, t_mem_type, t_word, t_mem_es, t_init_disp_imm,
	t_push, t_mem_type, t_word, t_mem_cs, t_init_disp_imm,
	t_push, t_mem_type, t_word, t_mem_ss, t_init_disp_imm,
	t_push, t_mem_type, t_word, t_mem_ds, t_init_disp_imm,
	t_push, t_mem_type, t_word, t_mem_fs, t_init_disp_imm,
	t_push, t_mem_type, t_word, t_mem_gs, t_init_disp_imm
};

node _push_mem_dword[] = {
	t_push, t_mem_type, t_dword, t_mem_es, t_init_disp_imm,
	t_push, t_mem_type, t_dword, t_mem_cs, t_init_disp_imm,
	t_push, t_mem_type, t_dword, t_mem_ss, t_init_disp_imm,
	t_push, t_mem_type, t_dword, t_mem_ds, t_init_disp_imm,
	t_push, t_mem_type, t_dword, t_mem_fs, t_init_disp_imm,
	t_push, t_mem_type, t_dword, t_mem_gs, t_init_disp_imm
};

node _push_prefix[] = {
	t_push, t_prefix_type, t_word, t_prefix_es, t_reg_none, t_init_disp_imm,
	t_push, t_prefix_type, t_word, t_prefix_ss, t_reg_none, t_init_disp_imm,
	t_push, t_prefix_type, t_word, t_prefix_ds, t_reg_none, t_init_disp_imm,
	t_push, t_prefix_type, t_word, t_prefix_fs, t_reg_none, t_init_disp_imm,
	t_push, t_prefix_type, t_word, t_prefix_gs, t_reg_none, t_init_disp_imm
};

node _push_cr[] = {
	t_push, t_cr_type, t_dword, t_cr0, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr1, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr2, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr3, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr4, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr5, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr6, t_reg_none, t_init_disp_imm,
	t_push, t_cr_type, t_dword, t_cr7, t_reg_none, t_init_disp_imm,
};

node _push_dr[] = {
	t_push, t_dr_type, t_dword, t_dr0, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr1, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr2, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr3, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr4, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr5, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr6, t_reg_none, t_init_disp_imm,
	t_push, t_dr_type, t_dword, t_dr7, t_reg_none, t_init_disp_imm,
};

node _pop_mem_byte[] = {
	t_pop, t_mem_type, t_byte, t_mem_es, t_init_disp_imm,
	t_pop, t_mem_type, t_byte, t_mem_cs, t_init_disp_imm,
	t_pop, t_mem_type, t_byte, t_mem_ss, t_init_disp_imm,
	t_pop, t_mem_type, t_byte, t_mem_ds, t_init_disp_imm,
	t_pop, t_mem_type, t_byte, t_mem_fs, t_init_disp_imm,
	t_pop, t_mem_type, t_byte, t_mem_gs, t_init_disp_imm
};

node _pop_mem_word[] = {
	t_pop, t_mem_type, t_word, t_mem_es, t_init_disp_imm,
	t_pop, t_mem_type, t_word, t_mem_cs, t_init_disp_imm,
	t_pop, t_mem_type, t_word, t_mem_ss, t_init_disp_imm,
	t_pop, t_mem_type, t_word, t_mem_ds, t_init_disp_imm,
	t_pop, t_mem_type, t_word, t_mem_fs, t_init_disp_imm,
	t_pop, t_mem_type, t_word, t_mem_gs, t_init_disp_imm
};

node _pop_mem_dword[] = {
	t_pop, t_mem_type, t_dword, t_mem_es, t_init_disp_imm,
	t_pop, t_mem_type, t_dword, t_mem_cs, t_init_disp_imm,
	t_pop, t_mem_type, t_dword, t_mem_ss, t_init_disp_imm,
	t_pop, t_mem_type, t_dword, t_mem_ds, t_init_disp_imm,
	t_pop, t_mem_type, t_dword, t_mem_fs, t_init_disp_imm,
	t_pop, t_mem_type, t_dword, t_mem_gs, t_init_disp_imm
};

node _pop_prefix[] = {
	t_pop, t_prefix_type, t_word, t_prefix_es, t_reg_none, t_init_disp_imm,
	t_pop, t_prefix_type, t_word, t_prefix_ss, t_reg_none, t_init_disp_imm,
	t_pop, t_prefix_type, t_word, t_prefix_ds, t_reg_none, t_init_disp_imm,
	t_pop, t_prefix_type, t_word, t_prefix_fs, t_reg_none, t_init_disp_imm,
	t_pop, t_prefix_type, t_word, t_prefix_gs, t_reg_none, t_init_disp_imm
};

node _pop_cr[] = {
	t_pop, t_cr_type, t_dword, t_cr0, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr1, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr2, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr3, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr4, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr5, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr6, t_reg_none, t_init_disp_imm,
	t_pop, t_cr_type, t_dword, t_cr7, t_reg_none, t_init_disp_imm,
};

node _pop_dr[] = {
	t_pop, t_dr_type, t_dword, t_dr0, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr1, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr2, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr3, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr4, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr5, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr6, t_reg_none, t_init_disp_imm,
	t_pop, t_dr_type, t_dword, t_dr7, t_reg_none, t_init_disp_imm,
};

node _add[] = {
	t_add, t_none, t_byte, t_none, t_reg_none, t_init_disp_imm,
	t_add, t_none, t_byte, t_other, t_reg_none, t_init_disp_imm,
	t_add, t_none, t_word, t_none, t_reg_none, t_init_disp_imm,
	t_add, t_none, t_word, t_other, t_reg_none, t_init_disp_imm,
	t_add, t_none, t_dword, t_none, t_reg_none, t_init_disp_imm,
	t_add, t_none, t_dword, t_other, t_reg_none, t_init_disp_imm,
};

node _shl[] = {
	t_shl, t_none, t_byte, t_none, t_reg_none, t_init_disp_imm,
	t_shl, t_none, t_byte, t_other, t_reg_none, t_init_disp_imm,
	t_shl, t_none, t_word, t_none, t_reg_none, t_init_disp_imm,
	t_shl, t_none, t_word, t_other, t_reg_none, t_init_disp_imm,
	t_shl, t_none, t_dword, t_none, t_reg_none, t_init_disp_imm,
	t_shl, t_none, t_dword, t_other, t_reg_none, t_init_disp_imm,
	t_shl, t_none, t_dword+1, t_other, t_reg_none, t_init_disp_imm,
};


node _shr[] = {
	t_shr, t_none, t_byte, t_none, t_reg_none, t_init_disp_imm,
	t_shr, t_none, t_byte, t_other, t_reg_none, t_init_disp_imm,
	t_shr, t_none, t_word, t_none, t_reg_none, t_init_disp_imm,
	t_shr, t_none, t_word, t_other, t_reg_none, t_init_disp_imm,
	t_shr, t_none, t_dword, t_none, t_reg_none, t_init_disp_imm,
	t_shr, t_none, t_dword, t_other, t_reg_none, t_init_disp_imm,
	t_shr, t_none, t_dword+1, t_other, t_reg_none, t_init_disp_imm,
};

node _nand[] = {
	t_nand, t_none, t_byte, t_none, t_reg_none, t_init_disp_imm,
	t_nand, t_none, t_byte, t_other, t_reg_none, t_init_disp_imm,
	t_nand, t_none, t_word, t_none, t_reg_none, t_init_disp_imm,
	t_nand, t_none, t_word, t_other, t_reg_none, t_init_disp_imm,
	t_nand, t_none, t_dword, t_none, t_reg_none, t_init_disp_imm,
	t_nand, t_none, t_dword, t_other, t_reg_none, t_init_disp_imm,
};

node _may_node[] = {
/*1*/	t_pop,  t_prefix_type, t_word,   t_none,  t_unkown,   t_init_disp_imm,
/*2*/	t_pop,  t_other,       t_word,   t_none,  t_reg_none, t_init_disp_imm,
/*3*/	t_push, t_other,       t_word,   t_none,  t_reg_none, t_init_disp_imm,
/*4*/	t_push, t_prefix_type, t_word,   t_none,  t_reg_none, t_init_disp_imm,
/*5*/	t_pop,  t_dr_type,     t_dword,  t_none,  t_unkown,   t_init_disp_imm,
/*6*/	t_push, t_dr_type,     t_dword,  t_none,  t_unkown,   t_init_disp_imm,
/*7*/	t_pop,  t_cr_type,     t_dword,  t_none,  t_unkown,   t_init_disp_imm,
/*8*/	t_push, t_cr_type,     t_dword,  t_none,  t_unkown,   t_init_disp_imm,
/*9*/	t_push, t_other,       t_unkown, t_none,  t_unkown,   t_init_disp_imm,
/*10*/	t_push, t_other,       t_word,   t_none,  t_reg_none, t_init_disp_imm,
/*11*/	t_push, t_other,       t_word,   t_none,  t_reg_none, t_init_disp_imm,
/*12*/	t_push, t_other1,      t_unkown, t_none,  t_unkown,   t_init_disp_imm,
/*13*/	t_pop,  t_other1,      t_unkown, t_none,  t_unkown,   t_init_disp_imm,
/*14*/	t_add,  t_none,        t_dword,  t_none,  t_reg_none, t_init_disp_imm,
/*15*/  t_add,  t_none,        t_unkown, t_none,  t_reg_none, t_init_disp_imm,
/*16*/  t_push, t_other1,      t_word,   t_none,  t_reg_1,    t_init_disp_imm
};

node _sub_4857DC[] = {
	0x1,0x2,0x1,0x0,0x8,0x0,
	0x1,0x2,0x1,0x0,0x8,0x0,
	0xf6,0x0,0x1,0x0,0x0,0x0,
	0x1,0x1,0x1,0x0,0x0,0x0,
	0xf6,0x0,0x1,0x0,0x0,0x0,
};

node _sub_485884[] = {
	_sub_4857DC[0],
	_sub_4857DC[1],
	_sub_4857DC[2],
	_sub_4857DC[3],
	_sub_4857DC[4],
	0x4,0x0,0x1,0x0,0x0,0x0,
	0x2,0x2,0x1,0x0,0x8,0x0
};

node _node_cmp_has_mod[] = {
	0x1,0x1,0x2,0x0,0x0,0x0,
	0x1,0x1,0x2,0x0,0x0,0x0,
	0x1,0x2,0x2,0x0,0xff,0x0,
	0x4,0x0,0x2,0x0,0x0,0x0,
	0x1,0x3,0x2,0x3,0x0,0x0,
	0x4,0x0,0x2,0x0,0x0,0x0,
	_sub_485884[0],
	_sub_485884[1],
	_sub_485884[2],
	_sub_485884[3],
	_sub_485884[4],
	_sub_485884[5],
	_sub_485884[6]
};

node _node_pop_context[] = {
	t_pop,0x2,0x2,0x0,0xa,0x0,
	t_pop,0x2,0x2,0x0,0x7,0x0,
	t_pop,0x2,0x2,0x0,0x6,0x0,
	t_pop,0x2,0x2,0x0,0x5,0x0,
	t_pop,0x2,0x2,0x0,0xff,0x0,
	t_pop,0x2,0x2,0x0,0x3,0x0,
	t_pop,0x2,0x2,0x0,0x2,0x0,
	t_pop,0x2,0x2,0x0,0x1,0x0,
	t_pop,0x2,0x2,0x0,0x0,0x0,
	t_pop,0x2,0x2,0x0,0x8,0x0,
	t_pop,0x1,0x2,0x0,0xff,0x0
};

node _node_retf[] = {
	t_retf,0x0,0x2,0x0,0x0,0x0
};

node _node_retn[] = {
	t_retn,0x0,0x2,0x0,0x0,0x0
};

node _node_push_context[] ={
	t_push,0x2,0x2,0x0,0x8,0x0,
	t_push,0x2,0x2,0x0,0x0,0x0,
	t_push,0x2,0x2,0x0,0x1,0x0,
	t_push,0x2,0x2,0x0,0x2,0x0,
	t_push,0x2,0x2,0x0,0x3,0x0,
	t_push,0x2,0x2,0x0,0xff,0x0,
	t_push,0x2,0x2,0x0,0x5,0x0,
	t_push,0x2,0x2,0x0,0x6,0x0,
	t_push,0x2,0x2,0x0,0x7,0x0,
	t_push,0x2,0x2,0x0,0xa,0x0,
	_node_retn[0]//maybe retf/retn
};

node _node_j_shl[] = {
	0x1,0x1,0x0,0x0,0xff,0x0,
	0x1,0x2,0x1,0x0,0x9,0x0,
	0x1c,0x0,0x1,0x0,0x0,0x0,
	0x2,0x2,0x1,0x0,0x9,0x0
};

node _node_j_shr[] = {
	0x1,0x1,0x1,0x0,0xff,0x0,
	0x1,0x2,0x1,0x0,0x9,0x0,
	0x1d,0x0,0x1,0x0,0x0,0x0,
	0x2,0x2,0x1,0x0,0x0,0x0
};

node _node_jcc_has_n[] = {
	0x1,0x2,0x1,0x0,0x8,0x0,
	0x1,0x1,0x1,0x0,0x0,0x0,
	0xf6,0x0,0x1,0x0,0x0,0x0,
	0x2,0x2,0x1,0x0,0x9,0x0,
	_node_j_shr[0],
	_node_j_shr[1],
	_node_j_shr[2],
	_node_j_shr[3]
};

node _node_LABEL_19[] = {
	0x1,0x2,0x2,0x0,0x4,0x0,
	0x1,0x1,0x1,0x0,0x0,0x0,
	0x1,0x2,0x1,0x0,0x9,0x0,
	0x4,0x0,0x2,0x0,0x0,0x0,
	0x1,0x3,0x2,0x3,0x0,0x0,
	0x1,0x2,0x2,0x0,0x4,0x0,
	0x1,0x1,0x2,0x0,0x8,0x0,
	0x4,0x0,0x2,0x0,0x0,0x0,
	0x2,0x3,0x2,0x3,0x0,0x0,
	0x2,0x1,0x2,0x0,0x0,0x0,
	0x1,0x1,0x2,0x0,0x0,0x0
};

node _node_jcc_loope_loopne_loop_jcxz[] = {
	0x1,0x1,0x2,0x0,0xff,0x0,
	0x1,0x2,0x2,0x0,0x0,0x0,
	0x4,0x0,0x2,0x0,0x0,0x0,
	0x1,0x1,0x2,0x0,0xff,0x0,
	0x1,0x2,0x2,0x0,0x0,0x0,
	0x4,0x0,0x2,0x0,0x0,0x0,
};
 
_struct_node _table[] = {
	{ _node_pop_context, sizeof(_node_pop_context) / sizeof(node), w_popfd_popad, _NONE_NONE}, 
	{ _node_push_context, sizeof(_node_push_context) / sizeof(node), w_pushad_pushfd, _NONE_NONE},
	{ _node_cmp_has_mod, sizeof(_node_cmp_has_mod) / sizeof(node), w_cmp, _NONE_NONE},
	{ _node_jcc_has_n, sizeof(_node_jcc_has_n) / sizeof(node), w_j, _JCC_HAS_N}
};

Disasm::Disasm()
{

}

Disasm::Disasm( int rva,int size)
{
	_section_brva = rva + _BASEADDR;
	_section_erva = _section_brva + size;
}

Disasm::~Disasm()
{
	_FreeNodes();
}

int Disasm::_String_Match_Searching( const string& text, const string& pattern, _mnemonic_code _mnemonic, int _n)
{
	int t_size = text.size();
	int p_size = pattern.size();
	int i = 0, j = 0;
	for( i = 0, j = 0; i < t_size; i++)
	{
		if( (text[i] == pattern[j] || pattern[j] == -1) && j < p_size)
		{
			j++;
		}
		else
		{
			if( j)
			{
				j = 0;
				i--;
			}
		}
		if( j == p_size)
		{
			j = 0;
			_result tmp((i - p_size + 1) / _var_size, _mnemonic, _n);
			res.insert( tmp);
		}
	}
	return - 1;
}

void Disasm::_FreeResult()
{
	_FreeSet(res);
}

void Disasm::_FreeNodes()
{
	_DestoryNode( arr_nd);
	_DestoryNode( nd);
	_FreeResult();
}

int Disasm::_Get_Opcode_Mnemonic_init()
{
	int _table_size = sizeof(_table) / sizeof(_struct_node);
	_FreeResult();
	_Conver_Code_To_String( _vmp_encodes, sim_vc);
	return _table_size;
}

void Disasm::_Get_Opcode_Mnemonic_Result( int size, const _pstruct_node pn)//_mnemonic_code code, int n)
{
	string _may_code;
	for( int j = 0; j < size; j++)
	{
		_Conver_Code_To_String( _may_code, *(arr_nd[j]));
		_String_Match_Searching( _vmp_encodes, _may_code, pn->code, pn->n);
	}
}

void Disasm::_Get_Opcode_Mnemonic()
{
	int _table_size = _Get_Opcode_Mnemonic_init();
	for( int i = 0; i < _table_size; i++)
	{	
		_vmp_fixed( _table[i].table, _table[i].size);
		_Get_Opcode_Mnemonic_Result(arr_nd.size(), &_table[i]);// _table[i].code, _table[i].n);
	}
}

int Disasm::_Process_Disp_Imme( _mnemonic_code code, const int& data)
{
	int _resthex = 0;
	if( code != w_cmp && code != w_sbb)
	{
		_resthex = ~data;
	}
	else
	{
		_resthex = -data;
	}
	return _resthex;
}



void Disasm::_AnalyzeModrm( string &s,int pi, int oi)
{
	char res;
	char _first_var = pvda[pi]->operand[oi]._modrm_mod;
	char _modrm_mod = 0;
	switch( _first_var)
	{
	case 0xe:
		{
			//ModRM.mod 可能是 01 10
			if( pvda[pi]->operand[oi]._rest_size == 1)
			{
				_modrm_mod = 1;
			}
			else
			{
				_modrm_mod = 2;
			}
			break;
		}
	case 0xc:
		{
			break;
		}
	case 0xa:
		{
			break;
		}
	}
	if( _first_var & 8)
	{
		if( _ITAB_IS_NONE(pvda[pi]->lock)
			|| _ITAB_IS_NONE(pvda[pi]->prefiexs)
			|| _ITAB_IS_NONE(pvda[pi]->rep))
		{

		}
		else
		{
			if( pvda[pi]->operand[oi]._modrm_rm == 4)
			{

			}
			else
			{
				//可确定ModRM.r/m
				res = _REModRM( _modrm_mod, pvda[pi]->operand[oi]._modrm_reg, pvda[pi]->operand[oi]._modrm_rm);
				_PADDING_ASM( s, res);
			}
		}
		if( pvda[pi]->operand[oi]._rest)//disp
		{
			switch(pvda[pi]->operand[oi]._rest_size)
			{
			case 1:
				{
					s += (char)pvda[pi]->operand[oi]._rest;
					break;
				}
			case 2:
				{
					s += (short)pvda[pi]->operand[oi]._rest;
					break;
				}
			case 4:
				{
					s += (int)pvda[pi]->operand[oi]._rest;
					break;
				}
			}
		}
	}
	else
	{
		res = _REModRM( _modrm_mod, pvda[pi]->operand[oi]._modrm_reg, pvda[pi]->operand[oi]._modrm_rm);
		_PADDING_ASM( s, res);
	}
}

char data[8] = {0};
void Disasm::_PtfDump()
{
	int desize = _dump_encodes.size();
	for( int i = 0; i < desize; i++)
	{
		int dssize = _dump_encodes[i].size();
		for( int j = 0; j < dssize; j++)
		{
			printf("%02X", (unsigned char)_dump_encodes[i][j]);
		}
		printf("\n");
	}
}

static const int  _jcc_index[] = {
	1,2,3,4,7,5,6,8
};
static const char _jcc[] = {
		0x00,   0x00,
/*1*/	0x70,	0x71,	
/*2*/	0x72,	0x73,	
/*3*/	0x74,	0x75,	
/*4*/	0x76,	0x77,	
/*7*/	0x78,	0x79,	
/*5*/	0x7A,	0x7B,	
/*6*/	0x7C,	0x7D,
/*8*/	0x7E,	0x7F
};

static int _check_is_correct_type1( const int& index)
{
	return ( sim_vc[index]._var[0] == t_push
		&& sim_vc[index]._var[1] == 0x1
		&& sim_vc[index]._var[2] == t_word);
}

static void _get_vmp_push_data( int &data ,int _first, int _last)
{
	for( int i = _first; i < _last; i++)
	{
		if( _check_is_correct_type1(i) != 0)
		{
			data = sim_vc[i]._data;
			break;
		}
	}
}

static const short _eflags[] = {
	0x40,0x800,0x1,0x40,0x41,0x4,0x880,0x80,0x8C0
};

int Disasm::_SearchJcc_Tpye( int data)
{
	int ret = -1;
	int size = sizeof(_eflags) / sizeof(short);
	for( int j = 1; j < size; j++)
	{
		if( data == ~_eflags[j])
		{
			ret = j;
			break;
		}
	}
	return ret;
}

static int _check_is_vaild_rva( int f, int l, int c)
{
	return c >= f && c < l;
}

int Disasm::_SearchJcc_RVA( int _first)
{
	int _rva[2] = {0};
	int _count = 0;
	int _last = sim_vc.size();


	for( int i = _first; i <= _last; i++)
	{
		int data = sim_vc[i]._data;
		if( _check_is_vaild_rva( _section_brva, _section_erva, data))
		{
			_rva[_count++] = data;
		}
		if( _count == 2)
		{
			break;
		}
	}

	return  (_count == 2) ?
		(_rva[0] < _rva[1] ? _CLAC_SUB( _rva[1], _rva[0]) : _CLAC_SUB( _rva[0], _rva[1]))
		: -1;
}

void Disasm::_REDisassmebly()
{
	string _dump;
	int size = pvda.size();
	for( int i = 0; i < size; i++)
	{
		_mnemonic_code code = pvda[i]->mnemonic;
		if( code > 0)
		{
			switch( code)
			{
			case w_cmp:
				{
					//需要解决类型 0x80 0x81 0x82 0x83
					//0x81 data >= 0x100
					if( pvda[i]->operand[1]._rest >= 0x100)
					{
						_PADDING_ASM( _dump, 0x81);
					}
					else//需要确定一些东西
					{
					}
					//可确定ModRM.reg 为 111
					pvda[i]->operand[0]._modrm_reg = 7;
					_AnalyzeModrm( _dump, i, 0);
					memcpy( data, (void*)(&(pvda[i]->operand[1]._rest)), 8);
					_PADDING_ASM( _dump, data);
					break;
				}
			case w_j:
				{
					//0x70 ~ 0x7F
					_PADDING_ASM( _dump, _jcc[ pvda[i]->operand[0]._jcc_index] );
					memcpy( data, (void*)(&(pvda[i]->operand[0]._rest)), 8);
					_PADDING_ASM( _dump, data);
					break;
				}
			}
			_dump_encodes.push_back(_dump);
			_dump.clear();
		}
	}
}

void Disasm::_AnalyzeResult()
{
	for( set<_result>::iterator it = res.begin(); it != res.end(); it++)
	{
		_mnemonic_code code = it->mnemonic;
		_pdisasm _pdtmp = NULL;
		int pos = it->pos;
		int prevpos = 0;
		int nextpos = 0;
		int data = 0;
		if( code > 0)
		{
			_pdtmp = new _disasm;

			switch( code)
			{
			case w_cmp:
				{
					//2种
					//has mod
					if( sim_vc[pos+3]._var[0] == 0x4)
					{
						_pdtmp->mnemonic = code;
					
						_pdtmp->operand[1]._modrm_mod = 2;
						_pdtmp->operand[1]._rest = _Process_Disp_Imme( code, sim_vc[pos]._data);
						_pdtmp->operand[1]._lval = sim_vc[pos]._var[2];
						_pdtmp->operand[1]._rest_size = sim_vc[pos]._size - 1;

						_pdtmp->operand[0]._modrm_mod = 0xe;//不一定
						_pdtmp->operand[0]._rest = sim_vc[pos+1]._data;
						_pdtmp->operand[0]._modrm_rm = sim_vc[pos+2]._reg;
						_pdtmp->operand[0]._lval = sim_vc[pos+1]._var[2];//不确定 2 或者 _lval
						_pdtmp->operand[0]._rest_size = sim_vc[pos+1]._size - 1;
					}
					//not has mod
					break;
				}
			case w_j:
				{
					_pdtmp->mnemonic = code;

					//0x70/0x71 0x72/0x73 0x74/0x75 0x76/0x77 0x78/0x79 0x7A/0x7B 0x7C/0x7D 0x7E/0X7F 
					//需要通过 0x40 0x800 0x1 0x40 0x41 0x880 0x80 0x8c0 规则来确定
					//只存在operand[0]
					_get_vmp_push_data( data, pos - 1, pos + 2);
					_pdtmp->operand[0]._jcc_index = _SearchJcc_Tpye( data) * 2 + it->_has_n;
					//然后通过判断是否有
					_pdtmp->operand[0]._lval = 2;
					_pdtmp->operand[0]._modrm_mod = 2;
					int _dis = _SearchJcc_RVA( pos);
					if( _dis != -1)
					{
						_pdtmp->operand[0]._rest = _dis;
					}
					break;
				}
			}

			pvda.push_back(_pdtmp);
		}
	}
}

void Disasm::_PtfASM()
{
// 	string str;
// 	int size = pvda.size();
// 	int index = 0;
// 	char log[4] = {0};
// 	for( int i = 0; i < size; i++)
// 	{
// 		_mnemonic_code code = w_none;
// 		code = pvda[i]->lock;
// 		if( _ITAB_IS_NONE(code))
// 		{
// 			_padding_asm( str, _mnemonics_str[code]);
// 		}
// 		code = pvda[i]->rep;
// 		if( _ITAB_IS_NONE(code))
// 		{
// 			_padding_asm( str, _mnemonics_str[code]);
// 		}
// 		code = pvda[i]->mnemonic;
// 		_padding_asm( str, _mnemonics_str[code]);
// 		//判断是否带n
// 		if( (unsigned __int8)(code - 0x10) < 0x77)
// 		{
// 			str += _tpye[pvda[i]->operand1._lval];
// 		}
// 		else if( code == w_jczx)
// 		{
// 			if( pvda[i]->operand2._lval == 2)
// 			{
// 				str += "jecxz";
// 			}
// 			else if( pvda[i]->operand2._lval == 3)
// 			{
// 				str += "jrcxz";
// 			}
// 		}
// 		if( code == w_db)
// 		{
// 
// 		}
// 		else if( (unsigned __int8)(code + 12) >= 2
// 				|| pvda[i]->operand1._modrm_mod != 2
// 				|| pvda[i]->operand2._modrm_rm != 2)
// 		{
// 			for( index = 0; index < 3; index++)
// 			{
// 				
// 			}
// 		}
// 	}
}


void Disasm::_Analyze()
{
	_Get_Opcode_Mnemonic();
	_AnalyzeResult();
	_REDisassmebly();
}