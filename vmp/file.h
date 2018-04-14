



#ifndef     __FILE_H
#define     __FILE_H

#define  LE16 WORD
#define  LE32 DWORD
typedef long off_t;

class file
{
protected:
	int _fd;
	int _mode;
	off_t _length;

	struct pe_header_t
	{
		// 0x0
		char    _[4];               // pemagic
		LE16    cpu;
		LE16    objects;
		char    __[12];             // timestamp + reserved
		LE16    opthdrsize;
		LE16    flags;
		// optional header
		LE16    coffmagic;          // NEW: Stefan Widmann
		char    ___[2];             // linkerversion
		LE32    codesize;
		// 0x20
		LE32    datasize;
		LE32    bsssize;
		LE32    entry;
		LE32    codebase;
		// 0x30
		LE32    database;
		// nt specific fields
		LE32    imagebase;
		LE32    objectalign;
		LE32    filealign;          // should set to 0x200 ?
		// 0x40
		char    ____[16];           // versions
		// 0x50
		LE32    imagesize;
		LE32    headersize;
		LE32    chksum;             // should set to 0
		LE16    subsystem;
		LE16    dllflags;
		// 0x60
		char    _____[20];          // stack + heap sizes
		// 0x74
		LE32    ddirsentries;       // usually 16

		struct ddirs_t
		{
			LE32    vaddr;
			LE32    size;
		};
		ddirs_t ddirs[16];
	};

	struct pe_section_t
	{
		char name[8];
		LE32 vsize;
		LE32 vaddr;
		LE32 size;
		LE32 rawdataptr;
		char _[12];
		LE32 flags;
	};

public:
	file();
	virtual ~file();

public:
	virtual off_t st_size() const { return _length;}

	virtual void sopen( const char *name);
	virtual void seek( off_t off, int whence);
	virtual int read( void *buf, int len);
	virtual int close();
	virtual int isOpen() const { return _fd >= 0;}
};

class pefile : public file
{
private:
	unsigned pe_offset;
	file *fi;
	pe_header_t ih;
	pe_section_t *isection;
	
	char *buf;	

public:
	pefile( file *f);
	~pefile();

public:
	int readFileHeader();
	void loadSection();
	void loadImports();
	void loadRelocs();
	void loadResources();

	char **getBufferAddr();

	int peFileLoader();
	int getOffset( int index);
	int getDistance( int index);
};



#endif      __FILE_H