



#include "StdAfx.h"

int _entry = 0;
int _size = 0;
int _rva = 0;
char **_buf = NULL;

void _SetNeedData(pefile* pf)
{
	_size = pf->getDistance(0);
	_rva = pf->getOffset(0);
	_buf = pf->getBufferAddr();	
}

void _Analyze( char *argv[])
{
	file f;
	f.sopen( argv[1]);

	pefile fi(&f);
	fi.peFileLoader();

	_SetNeedData( &fi);

	if( !_entry || !_buf)
		return;

	ProcessEsiEncode _esi( _entry);
	_esi._GetVmpEncdoes( _buf);

//	_PtfSimVc();

	Disasm da( _rva, _size);	
	da._Analyze();
	da._PtfDump();
}


int main( int argc, char *argv[])
{
	if( !argv[1])
		return 0;
	
//	scanf("%d", &_entry);
	_entry = 0x00401050;
	_Analyze( argv);
	
	return 0;
}

