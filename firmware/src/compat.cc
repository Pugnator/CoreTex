#include <global.hpp>

extern "C"
{

	void __dso_handle ( void )
	{
	
	}
	
	int _getpid ( void )
	{
		return 1;
	}
	
	void _kill ( int pid )
	{
	}
	
	char* _sbrk ( int delta )
	{
		return NULL;
	}
	
	int _exit ( void )
	{
		return 1;
	}
	
	int _close ( void )
	{
		return 1;
	}
	
	int _lseek ( int file, int ptr, int dir )
	{
		return 0;
	}
	
	int _read ( int file, char* ptr, int len )
	{
		//TODO
		return len;
	}
	
	int _write ( int file, char* ptr, int len )
	{
		return len;
	}
	
	int _isatty ( int file )
	{
		return 0;
	}
	
	int _fstat ( int file, struct stat* st )
	{
		return 0;
	}
	
	int open ( const char* name, int flags, int mode )
	{
		return -1;
	}
	
}
