#pragma once


#include <zlib/zlib.h>
#ifndef DEF_WBITS
#define DEF_WBITS MAX_WBITS
#endif

namespace zs_detail
{

#pragma pack( push , 1)
struct GZipHeader
{
	GZipHeader();
	unsigned char ID1;	//fixed:0x1F 
	unsigned char ID2;	//fixed:0x8B
	unsigned char CM;	//fixed:8
	unsigned char FLG;	//bit 0 FTEXT - 指示文本数据 
				//bit 1 FHCRC - 指示存在CRC16头校验字段
				//bit 2 FEXTRA - 指示存在可选项字段
				//bit 3 FNAME - 指示存在原文件名字段
				//bit 4 FCOMMENT - 指示存在注释字段
				//bit 5-7 保留
	int MTIME;	//更改时间
	unsigned char XFL;	//1字节。附加的标志。当CM = 8时，XFL = 2 - 最大压缩但最慢的算法；XFL = 4 - 最快但最小压缩的算法
	unsigned char OS;	//1字节。操作系统，确切地说应该是文件系统。有下列定义： 
				//0 - FAT文件系统 (MS-DOS, OS/2, NT/Win32)
				//1 - Amiga
				//2 - VMS/OpenVMS
				//3 - Unix
				//4 - VM/CMS
				//5 - Atari TOS
				//6 - HPFS文件系统 (OS/2, NT)
				//7 - Macintosh
				//8 - Z-System
				//9 - CP/M
				//10 - TOPS-20
				//11 - NTFS文件系统 (NT)
				//12 - QDOS
				//13 - Acorn RISCOS
				//255 - 未知

};

//额外的头字段： 
//(若 FLG.FEXTRA = 1)
//
//+---+---+---+---+===============//================+
//|SI1|SI2|  XLEN |      长度为XLEN字节的可选项     |
//+---+---+---+---+===============//================+

struct GZFExtraItem
{
unsigned char  SI1;
unsigned char  SI2;
unsigned short LEN; 
};

//(若 FLG.FNAME = 1)
//
//+=======================//========================+
//|               原文件名(以NULL结尾)              |
//+=======================//========================+
//(若 FLG.FCOMMENT = 1)
//
//+=======================//========================+
//|   注释文字(只能使用iso-8859-1字符，以NULL结尾)  |
//+=======================//========================+
//(若 FLG.FHCRC = 1)
//
//+---+---+
//| CRC16 |
//+---+---+

struct GZipTailer
{
	int CRC32;
	int ISIZE;
};

#pragma pack( pop )

}



class CZlibStream
{
public:
    CZlibStream( int nCompressLevel = Z_DEFAULT_COMPRESSION ,
                 int nWBits = -MAX_WBITS , int nMemLevel = MAX_MEM_LEVEL, int nStrategyType = Z_DEFAULT_STRATEGY , int nMethod = Z_DEFLATED );
    ~CZlibStream( void );
private:
    CZlibStream( const CZlibStream &other );
    CZlibStream& operator=( const CZlibStream &other );

//GZip 格式压缩
public:
    bool CompressGZip( const char *src , int len , std::string &dstData );
	bool DecompressGZip( const char *src , int len , std::string &dstData );
	bool CompressGZip( const std::string& srcData , std::string &dstData );
	bool DecompressGZip( const std::string& srcData , std::string &dstData );

//原始deflate 格式压缩
public:
    bool Compress( const char *src , int len , std::string &dstData );
	bool Decompress( const char *src , int len , std::string &dstData );
	bool Compress( const std::string& srcData , std::string &dstData );
	bool Decompress( const std::string& srcData , std::string &dstData );

private:
    z_stream m_zInflate;
    z_stream m_zDeflate;

    bool m_bInitInflate;
    bool m_bInitDeflate;


    //#define Z_NO_COMPRESSION         0
    //#define Z_BEST_SPEED             1
    //#define Z_BEST_COMPRESSION       9
    //#define Z_DEFAULT_COMPRESSION  (-1)
    int m_nCompressLevel;


    //#define Z_DEFLATED 8
    int m_nMethod;

    //#define MAX_WBITS 15
    //#define DEF_WBITS MAX_WBITS
    int m_nWBits;

    //#define MAX_MEM_LEVEL
    //#define DEF_MEM_LEVEL 8
    int m_nMemLevel;

    //#define Z_FILTERED            1
    //#define Z_HUFFMAN_ONLY        2
    //#define Z_RLE                 3
    //#define Z_FIXED               4
    //#define Z_DEFAULT_STRATEGY    0
    int m_nStrategyType;

	struct GZFExtraItemExt
	{
		zs_detail::GZFExtraItem header;
		std::string data;
	};
	std::vector< GZFExtraItemExt > m_extraitems;
};


class CGZipFile
{
public:
    CGZipFile( int nCompressLevel = Z_DEFAULT_COMPRESSION , int nStrategyType = Z_DEFAULT_STRATEGY );
    ~CGZipFile();
private:
    CGZipFile( const CGZipFile &other );
    CGZipFile& operator=( const CGZipFile &other );
public:
    bool Open( LPCTSTR lpszFileName , bool bWrite = TRUE );
    bool Close();
    bool Flush( int nFlushType );
    bool IsOpen() const;
    bool IsEOF() const;

    bool WriteBuffer( voidp pBuffer, size_t nBytes );
	bool WriteBuffer( const std::string &srcdata );
	bool CompressFile( LPCTSTR lpszSrcFileName , LPCTSTR lpszDstFileName );

    bool ReadBuffer( voidp &ppBuffer, size_t& nBytes );
	bool ReadBuffer( std::string &srcData );
	bool DecompressFile( LPCTSTR lpszSrcFileName , LPCTSTR lpszDstFileName );

    int ReadBufferSize( voidp pBuffer, size_t nBytes );
private:
	gzFile m_gzipFile;
    unsigned int m_BufferSize;
    int m_nCompressLevel;
    int m_nStrategyType;
	bool m_bWrite;

private:
    void UpdateParams();

};


