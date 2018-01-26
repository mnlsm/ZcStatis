#include "StdAfx.h"
#include "ZlibStream.h"
#include <atlsync.h>



namespace zs_detail
{
static const unsigned char GZH_FLG_FTEXT = 0x01;
static const unsigned char GZH_FLG_FHCRC = 0x02;
static const unsigned char GZH_FLG_FEXTRA = 0x04;
static const unsigned char GZH_FLG_FNAME = 0x08;
static const unsigned char GZH_FLG_FCOMMENT = 0x10;

static const unsigned char GZH_OS_FAT = 0;				//0 - FAT文件系统 (MS-DOS, OS/2, NT/Win32)
static const unsigned char GZH_OS_AMIGA = 1;				//1 - Amiga
static const unsigned char GZH_OS_VMS = 2;				//2 - VMS/OpenVMS
static const unsigned char GZH_OS_UNIX = 3;				//3 - Unix
static const unsigned char GZH_OS_CMS = 4;				//4 - VM/CMS
static const unsigned char GZH_OS_ATARITOS = 5;				//5 - Atari TOS
static const unsigned char GZH_OS_HPFS = 6;				//6 - HPFS文件系统 (OS/2, NT)
static const unsigned char GZH_OS_MACINTOSH = 7;				//7 - Macintosh
static const unsigned char GZH_OS_ZSYSTEM = 8;				//8 - Z-System
static const unsigned char GZH_OS_CP = 9;				//9 - CP/M
static const unsigned char GZH_OS_TOPS20 = 10;			//10 - TOPS-20
static const unsigned char GZH_OS_NTFS = 11;				//11 - NTFS文件系统 (NT)
static const unsigned char GZH_OS_QDOS = 12;				//12 - QDOS
static const unsigned char GZH_OS_ACORN = 13;			//13 - Acorn RISCOS
static const unsigned char GZH_OS_UNKNOWN = 255;			//255 - 未知

GZipHeader::GZipHeader()
{
	ID1 = 0x1F;
	ID2 = 0x8B;
	CM = 8;
	FLG = 0;
	MTIME = (int)time( NULL );
	XFL = 4;
	OS = 0;
}
}

static ATL::CMutex g_CompressMutex( NULL , FALSE , NULL );

CZlibStream::CZlibStream( int nCompressLevel,
                          int nWBits, int nMemLevel, int nStrategyType , int nMethod )
        : m_nMethod( nMethod ),
        m_nCompressLevel( nCompressLevel ),
        m_nWBits( nWBits ),
        m_nMemLevel( nMemLevel ),
        m_nStrategyType( nStrategyType ),
        m_bInitInflate( false ),
        m_bInitDeflate( false )
{
    ::ZeroMemory( &m_zInflate , sizeof( m_zInflate ) );
    ::ZeroMemory( &m_zDeflate , sizeof( m_zDeflate ) );
}

CZlibStream::~CZlibStream( void )
{
    inflateEnd( &m_zInflate );
    deflateEnd( &m_zDeflate );
}

bool CZlibStream::CompressGZip( const char *src , int len , std::string &dstData )
{
	dstData.clear();
	zs_detail::GZipHeader gzh;
	dstData.append( (char*)&gzh , sizeof(gzh) );
	bool bOK = Compress( src , len , dstData );
	if( bOK )
	{
		zs_detail::GZipTailer gzt;
		gzt.ISIZE = len ;
		gzt.CRC32 = 0;
		dstData.append( (char*)&gzt , sizeof(gzt) );
	}
	return bOK;
}

bool CZlibStream::DecompressGZip( const char *src , int len , std::string &dstData )
{
	if( len < sizeof( zs_detail::GZipHeader ) + sizeof( zs_detail::GZipTailer ) )
	{
		return false;
	}
	dstData.clear();
	zs_detail::GZipHeader gzh;
	memcpy( &gzh , src , sizeof( gzh ) );
	src += sizeof( gzh );
	len -= sizeof( gzh );
	if( ( gzh.FLG & zs_detail::GZH_FLG_FEXTRA ) == 1 )
	{
		char extra_len_byte[2] = {'\0'};
		memcpy( &extra_len_byte , src , 2 );
		const char *extra_item_begin = src + 2;
		unsigned short extra_len = *( (unsigned short*)extra_len_byte ) ;
		while( extra_len >= sizeof( zs_detail::GZFExtraItem ) )
		{
			CZlibStream::GZFExtraItemExt extra_item;
			memcpy( &extra_item.header , extra_item_begin , sizeof( extra_item.header ) );
			extra_item.data.assign( extra_item_begin + sizeof( extra_item.header ), extra_item.header.LEN - sizeof( extra_item.header ) );
			m_extraitems.push_back( extra_item );
			extra_len -= extra_item.header.LEN;
			extra_item_begin += extra_item.header.LEN;
		}
		src += extra_len ;
		len -= extra_len ;
	}

	if( ( gzh.FLG &  zs_detail::GZH_FLG_FNAME ) == 1 )
	{
		int fnamelen = strlen( src ) + 1;
		src += fnamelen;
		len -= fnamelen;
	}

	if( ( gzh.FLG &  zs_detail::GZH_FLG_FCOMMENT ) == 1 )
	{
		int fcommentlen = strlen( src ) + 1;
		src += fcommentlen;
		len -= fcommentlen;
	}

	if( ( gzh.FLG &  zs_detail::GZH_FLG_FHCRC ) == 1 )
	{
		short crc  = 0;
		memcpy( &crc , src , sizeof( crc ) );
		src += sizeof( crc );
		len -= sizeof( crc );
	}
	len -= sizeof( zs_detail::GZipTailer );
	return Decompress( src , len , dstData );
}

bool CZlibStream::CompressGZip( const std::string& srcData , std::string &dstData )
{
	return CompressGZip( srcData.data() , srcData.size() , dstData );
}

bool CZlibStream::DecompressGZip( const std::string& srcData , std::string &dstData )
{
	return DecompressGZip( srcData.data() , srcData.size() , dstData );
}

bool CZlibStream::Compress( const char *src , int len , std::string &dstData )
{
    //dstData.clear();
    if ( len <= 0 )
    {
        return true;
    }

    if ( !m_bInitDeflate )
    {
        m_zDeflate.zalloc = Z_NULL;
        m_zDeflate.zfree = Z_NULL;
        m_zDeflate.opaque = Z_NULL;
        int ret = deflateInit2( &m_zDeflate, m_nCompressLevel, m_nMethod, m_nWBits, m_nMemLevel, m_nStrategyType );
        if ( ret != Z_OK )
        {
            ATLTRACE( _T( "\r\ndeflateInit call failed! ret = %d \r\n" ) , ret  );
            return false;
        }
        m_bInitDeflate = true;
    }

    ATL::CMutexLock( g_CompressMutex , TRUE );
    int CHUNK = len + ( len / 100 ) + 13;
	std::string out;
	out.resize( CHUNK , '\0' );
	char *in = const_cast<char*>( src );
    m_zDeflate.avail_in = len;
    m_zDeflate.next_in = ( Bytef* )in;
    do
    {
        m_zDeflate.avail_out = CHUNK;
		m_zDeflate.next_out = ( Bytef* )out.data();
        int ret = deflate( &m_zDeflate, Z_SYNC_FLUSH );
        if ( ret != Z_OK )
        {
            ATLTRACE( _T( "\r\ndeflate call failed! ret = %d \r\n" ) , ret  );
            return false;
        }
		dstData.append( out.data(), CHUNK - m_zDeflate.avail_out );
    }while ( m_zDeflate.avail_out == 0 );
    return true;
}

bool CZlibStream::Decompress( const char *src , int len , std::string &dstData )
{
	//dstData.clear();
    if ( len <= 0 )
    {
        return false;
    }
    if ( !m_bInitInflate )
    {
        m_zInflate.zalloc = Z_NULL;
        m_zInflate.zfree = Z_NULL;
        m_zInflate.opaque = Z_NULL;
        m_zInflate.avail_in = 0;
        m_zInflate.next_in = Z_NULL;
        int ret = inflateInit2( &m_zInflate , m_nWBits );
        if ( ret != Z_OK )
        {
            ATLTRACE( _T( "\r\ninflateInit2 call failed! ret = %d \r\n" ) , ret  );
            return false;
        }
        m_zInflate.avail_in = 0;
        m_zInflate.next_in = Z_NULL;
        m_bInitInflate = true;
    }
    int CHUNK = 1024 * 16;
	std::string out;
	out.resize( CHUNK , '\0' );
	char *in = const_cast<char*>( src );
    m_zInflate.avail_in = len;
    m_zInflate.next_in = ( Bytef* )in;
    do
    {
        m_zInflate.avail_out = CHUNK;
		m_zInflate.next_out = ( Bytef* )out.data();
        int ret = inflate( &m_zInflate, Z_SYNC_FLUSH );
        if ( ret != Z_OK )
        {
			dstData.append( out.data(), CHUNK - m_zInflate.avail_out );
			if( Z_STREAM_END == ret ) 
				return true;
            ATLTRACE( _T( "\r\ninflate call failed! ret = %d \r\n" ) , ret  );
			return false;
        }
		dstData.append( out.data(), CHUNK - m_zInflate.avail_out );
    }while ( m_zInflate.avail_out == 0 );
	return true;
}


bool CZlibStream::Compress( const std::string& srcData , std::string &dstData )
{
	return Compress( srcData.data() , (int)srcData.size() , dstData );
}

bool CZlibStream::Decompress( const std::string& srcData , std::string &dstData )
{
	return Decompress( srcData.data() , (int)srcData.size() , dstData );
}




CGZipFile::CGZipFile( int nCompressLevel , int nStrategyType )
        : m_nCompressLevel( nCompressLevel ),
        m_nStrategyType( nStrategyType ),
        m_gzipFile( NULL ),
        m_BufferSize( 0 ),
        m_bWrite( TRUE )
{

}

CGZipFile::~CGZipFile()
{
    Close();
}

bool CGZipFile::Open( LPCTSTR lpszFileName , bool bWrite  )
{
    if ( IsOpen() )
    {
        return false;
    }

    m_bWrite = bWrite;
    if ( bWrite )
    {
        m_gzipFile = gzopen( CT2A( lpszFileName ), "wb" );
        UpdateParams();
    }
    else
    {
        ATL::CAtlFile atlFile;
        HRESULT hr = atlFile.Create( lpszFileName , GENERIC_READ , FILE_SHARE_READ , OPEN_EXISTING );
        if ( hr != S_OK )
        {
            return false;
        }
        ULONGLONG uFileSize = 0;
        hr = atlFile.GetSize( uFileSize );
        if ( hr != S_OK )
        {
            return false;
        }
        atlFile.Close();
		m_BufferSize = ( unsigned int )uFileSize;
        m_gzipFile = gzopen( CT2A( lpszFileName ), "rb" );
    }

    return m_gzipFile != NULL ;

}

bool CGZipFile::Close()
{
    if ( !IsOpen() )
    {
        return false;
    }
    gzclose( m_gzipFile );
    m_gzipFile = NULL;
	return true;
}

bool CGZipFile::Flush( int nFlushType )
{
    if ( !IsOpen() || !m_bWrite )
        return false;
    return gzflush( m_gzipFile, nFlushType ) == Z_OK;
}

bool CGZipFile::WriteBuffer( voidp pBuffer, size_t nBytes )
{
    if ( !IsOpen() || !m_bWrite )
    {
        return false;
    }
    int written = gzwrite( m_gzipFile, pBuffer, nBytes );
    return written == ( int )( nBytes );
}

bool CGZipFile::WriteBuffer( const std::string &srcdata )
{
	return WriteBuffer( (voidp)srcdata.data() , srcdata.size() );
}

bool CGZipFile::ReadBuffer( voidp &ppBuffer, size_t& nBytes )
{
    nBytes = 0;
    if ( !IsOpen() || m_bWrite )
    {
        return false;
    }
    if ( m_BufferSize == 0 )
    {
        return false;
    }

	std::list< std::pair < char*, size_t > > lBuffers;
    char* pBuffer = NULL;
    int read = 1 ;
    while ( read > 0 )
    {
        pBuffer = new char[m_BufferSize];
        read = gzread( m_gzipFile, pBuffer, m_BufferSize );
        if ( read > 0 )
        {
			lBuffers.push_back( std::pair<char*, size_t>( pBuffer, read ) );
            nBytes += read;
        }
        else
		{
			delete pBuffer;
		}
    }

    if ( read == -1 )
    {
        while ( !lBuffers.empty() )
        {
            delete[] lBuffers.front().first;
            lBuffers.pop_front();
        }
        return false;
    }

    // allocating memory and writing buffer
	if( nBytes == 0 ) return false;
	ppBuffer = new char[nBytes];
	if( ppBuffer == NULL ) return false;
    size_t offset = 0;
    while ( !lBuffers.empty() )
    {
        pBuffer = lBuffers.front().first;
        read = lBuffers.front().second;
        memcpy( ( char* )ppBuffer + offset, pBuffer, read );
        offset += read;
        delete[] pBuffer;
        lBuffers.pop_front();
    }
    return nBytes != 0;

}

bool CGZipFile::ReadBuffer( std::string &srcData )
{
	voidp  ppBuffer = NULL; 
	size_t nBytes = 0;
	if( ReadBuffer( ppBuffer , nBytes ) )
	{
		srcData.assign( ( char * )ppBuffer , nBytes );
		delete []( char * )ppBuffer;
		return true;
	}
	return false;
}

int CGZipFile::ReadBufferSize( voidp pBuffer, size_t nBytes )
{
	if( !IsOpen() || m_bWrite )
		return false;
	return gzread( m_gzipFile, pBuffer, nBytes );
}

void CGZipFile::UpdateParams()
{
	if( !IsOpen() || !m_bWrite )
		return;
	gzsetparams( m_gzipFile, m_nCompressLevel, m_nStrategyType );
}

bool CGZipFile::IsOpen() const
{
    return m_gzipFile != NULL;
}

bool CGZipFile::IsEOF() const
{
    if ( !IsOpen() )
    {
        return true;
    }
    return gzeof( m_gzipFile ) == 1;
}

bool CGZipFile::CompressFile( LPCTSTR lpszSrcFileName , LPCTSTR lpszDstFileName )
{
	if( !Open( lpszDstFileName , TRUE ) )
	{
		DeleteFile( lpszDstFileName );
		return FALSE;
	}
	CAtlFile atlFileSrc;
	HRESULT hr = atlFileSrc.Create( lpszSrcFileName , GENERIC_READ , FILE_SHARE_READ , OPEN_EXISTING );
	if( hr != S_OK ) return false;
	DWORD dwRead = 0;
	char szTmpBuf[4096] = {'\0'};
	do
	{
		hr = atlFileSrc.Read( szTmpBuf , 4096 , dwRead );
		if( hr == S_OK )
		{
			if( dwRead > 0 )
			{
				if( !WriteBuffer( szTmpBuf , dwRead ) )
				{
					Close();
					DeleteFile( lpszDstFileName );
					return FALSE;
				}
			}
			else if( dwRead == 0 ) //end of the file
			{
				break;			
			}
		}
	}while( hr == S_OK && dwRead != 0 );	
	return TRUE;
}

bool CGZipFile::DecompressFile( LPCTSTR lpszSrcFileName , LPCTSTR lpszDstFileName )
{
	if( !Open( lpszSrcFileName , FALSE ) )
	{
		return FALSE;
	}
	CAtlFile atlFileDst;
	HRESULT hr = atlFileDst.Create( lpszDstFileName , GENERIC_WRITE , 0 , CREATE_ALWAYS );
	if( hr != S_OK ) return false;
	int nRead = 0;
	char szTmpBuf[4096] = {'\0'};
	do
	{
		nRead = ReadBufferSize( szTmpBuf , 4096 );
		if( nRead > 0 )
		{
			DWORD dwWrite = 0;
			char *pos = szTmpBuf;
			do
			{
				if( atlFileDst.Write( pos , (DWORD)nRead , &dwWrite ) != S_OK )
				{
					Close();
					atlFileDst.Close();
					DeleteFile( lpszDstFileName );
					return FALSE;
				}
				nRead -= dwWrite;
				pos += dwWrite;

			}while( nRead > 0 );
		}
		else if( nRead < 0 ) //error
		{
			Close();
			atlFileDst.Close();
			DeleteFile( lpszDstFileName );
			return FALSE;
		}
	}while( nRead > 0 );	
	return TRUE;
}















