



#include "StdAfx.h"
#include "file.h"
#include "REVmpDisassembly.h"

/************************************************************************/
//FILE
/************************************************************************/

file::file():
	_fd(-1), _mode(0), _length(0)
{

}

file::~file()
{
//	close();
}

void file::sopen( const char *name)
{
	close();
	struct _stat st;
	_mode = 0600;
	_fd = _sopen( name, O_RDONLY | O_BINARY, SH_DENYWR, _mode);
	_fstat( _fd, &st);
	_length = st.st_size;
}

void file::seek( off_t off, int whence)
{
	if( whence == SEEK_END)
	{
		off += _length;
		whence = 0;
	}
	lseek( _fd, off, whence);
}

int file::read( void *buf, int len)
{
	return ::read( _fd, buf, len);
}

int file::close()
{
	int ok = 1;
	if( isOpen() && _fd != STDIN_FILENO && _fd != STDOUT_FILENO && _fd != STDERR_FILENO)
	{
		if( ::close( _fd) == -1)
		{
			ok = 0;
		}
	}
	_fd = -1;
	_mode = 0;
	_length = 0;
	return ok;
}


/************************************************************************/
//PEFILE
/************************************************************************/

pefile::pefile( file *f):
	fi(f)
{

}

pefile::~pefile()
{
	fi->close();
	delete[] isection;
}

int pefile::readFileHeader()
{
	struct exe_header_t
	{
		WORD mz;
		WORD m512;
		WORD p512;
		char _[18];
		WORD relocoffs;
		char __[34];
		DWORD nexepos;
	};

	exe_header_t h;
	int ic = 0;
	pe_offset = 0;

	for( ic = 0; ic < 20; ic++)
	{
		fi->seek( pe_offset, SEEK_SET);
		fi->read( &h, sizeof(h));

		if( h.mz == 'M' + 'Z' * 256)
		{
			if( h.relocoffs >= 0x40)
			{
				pe_offset += h.nexepos;
			}
			else
			{
				pe_offset += h.p512 * 512 + h.m512 - h.m512 ? 512 : 0;
			}
		}
		else if( h.mz == 'P' + 'E' * 256)
		{
			break;
		}
		else
			 return 0;
	}

	if( ic == 20)
		return 0;
	fi->seek( pe_offset, SEEK_SET);
	fi->read( &ih, sizeof(ih));
	fi->seek( 0x200, SEEK_SET);
	fi->read( &h, 6);
	return 1;
}

void pefile::loadSection()
{
	//区段数目
	const unsigned objs = ih.objects;
	isection = new pe_section_t[objs];
	fi->seek( pe_offset + sizeof(ih), SEEK_SET);
	fi->read( isection, sizeof( pe_section_t) * objs);

	unsigned ic = 0,jc;
	for( ic = 0; ic < objs; ic++)
	{
		fi->seek( isection[ic].rawdataptr, SEEK_SET);
		jc = isection[ic].size;
		fi->read( buf + isection[ic].vaddr, jc);
	}
}

void pefile::loadImports()
{
	struct import_desc
	{
		LE32 oft;
		char _[8];
		LE32 dllname;
		LE32 iat;
	};

	unsigned dllnum = 0;
	DWORD vaddr = ih.ddirs[PEDIR_IMPORT].vaddr;

	import_desc *im = ( import_desc*)( buf + vaddr);
	import_desc * const im_save = im;

	for( ; vaddr && im->dllname; dllnum++, im++);
	im = im_save;

	LE32 *lookupt = (LE32*)( buf + ( im->oft ? im->oft : im->iat));

	unsigned ic = 0,jc = 0;
	for( ic = 0, jc = 0; ic < dllnum; ic++)
	{
		HINSTANCE hInstance = LoadLibrary( (LPCTSTR)(buf + im->dllname));

		if( hInstance == NULL)
		{
			//异常
			return;
		}

		for( LE32 *tarr = lookupt; *tarr; tarr++, jc++)//遍历
		{
			FARPROC fpFun;
			if( *tarr & 0x80000000)//序号导入,由于最高位是1和0所以可以这样判断
			{
				fpFun = GetProcAddress( hInstance, (LPCSTR)( buf + (*tarr & 0xffff)));
			}
			else//名字导入
			{
				fpFun = GetProcAddress( hInstance, (LPCSTR)( buf + *tarr + 2));//前两位有ord,需要+2
			}

			*(LE32*)( buf + im->iat + jc * 4) = (LE32)fpFun;
		}
		FreeLibrary(hInstance);
	}
}

void pefile::loadRelocs()
{
//	buf + ih.ddirs[].vaddr

	if( *buf == ih.imagebase || !ih.ddirs[PEDIR_RELOC].size)
	{
		return;
	}

	struct reloc
	{
		LE32 pagestart;
		LE32 size;
	};

	DWORD vaddr = ih.ddirs[PEDIR_RELOC].vaddr;
	void *p = ( void*)( buf + vaddr);

	reloc *rel = (reloc *)p;
	LE16 *rel1 = (LE16*)( (char*)p + sizeof(reloc));

	unsigned _pos, _type;
	unsigned _count = 0;
	unsigned _off = 0;


	for ( ; rel->pagestart; )
	{
		_count = (rel->size - 8)>>1;
		_off += rel->size;

		//重定位
		for( int _index = 0; _index < _count; _index++)
		{
			_pos = (*rel1 & 0xfff);
			_type = *rel1++ >> 12;
			if( _type == 3)
			{
				*(LE32*)( buf + _pos + rel->pagestart) += (int)buf - ih.imagebase;
			}
		}

		rel = (reloc *)( (int)p + _off);
		rel1 = (LE16*)( (char*)p + sizeof(reloc) + _off);
	}
}

void pefile::loadResources()
{
// 	const unsigned vaddr = ih.ddirs[PEDIR_RESOURCE].vaddr;
// 	if( vaddr == 0)
// 		return;
// 
// 	struct _rnode
// 	{
// 		unsigned          id;
// 		unsigned char     *iname;
// 		_rnode            *parent;
// 	};
// 
// 	struct resource
// 	{
// 		const unsigned char *start;
// 		unsigned char       *newstart;
// 		_rnode              *root;
// 	};
}


int pefile::peFileLoader()
{
	readFileHeader();
// 	buf = new char[ih.imagesize];
// 	if( !buf)
// 		return -1;
	LPVOID  addr = VirtualAlloc( NULL,
							   ih.imagesize,
							   MEM_RESERVE | MEM_COMMIT,
							   PAGE_EXECUTE_READWRITE);
	memset( addr, 0, ih.imagesize);
	buf = (char*)addr;

	//加载区段
	loadSection();
	//导入表
	loadImports();
	//重定位
	loadRelocs();
	//资源表
//	LoadResources();


//	_vmp_fixed();

	VirtualFree( addr, ih.imagesize, MEM_RESERVE | MEM_COMMIT);
	return 0;
}

/************************************************************************/
/* other                                                                */
/************************************************************************/

int pefile::getDistance( int index)
{
	return isection[index].size;
}

int pefile::getOffset( int index)
{
	return isection[index].vaddr;
}

char ** pefile::getBufferAddr()
{
	return &buf;
}