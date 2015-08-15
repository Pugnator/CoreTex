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

	char *
	strstr(const char *in, const char *str)
	{
	    char c;
	    size_t len;

	    c = *str++;
	    if (!c)
	        return (char *) in;	// Trivial empty string case

	    len = strlen(str);
	    do {
	        char sc;

	        do {
	            sc = *in++;
	            if (!sc)
	                return (char *) 0;
	        } while (sc != c);
	    } while (strncmp(in, str, len) != 0);

	    return (char *) (in - 1);
	}
	
	int
	strncmp ( const char* s1, const char* s2, size_t n )
	{
		for ( ; n > 0; s1++, s2++, --n )
			if ( *s1 != *s2 )
				return ( ( * ( unsigned char* ) s1 < * ( unsigned char* ) s2 ) ? -1 : +1 );
			else if ( *s1 == '\0' )
				return 0;
		return 0;
	}
	
	void*
	memset ( void* dst, int c, size_t n )
	{
		if ( n )
		{
			char* d = ( char* ) dst;
			
			do
			{
				*d++ = c;
			}
			while ( --n );
		}
		return dst;
	}
	
	char*
	strcpy ( char* s1, const char* s2 )
	{
		char* s = s1;
		while ( ( *s++ = *s2++ ) != 0 )
			;
		return ( s1 );
	}
	
	
	char*
	strtok ( char* s, const char* delim )
	{
		char* spanp;
		int c, sc;
		char* tok;
		static char* last;
		
		
		if ( s == NULL && ( s = last ) == NULL )
			return ( NULL );
			
		/*
		 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
		 */
cont:
		c = *s++;
		for ( spanp = ( char* ) delim; ( sc = *spanp++ ) != 0; )
		{
			if ( c == sc )
				goto cont;
		}
		
		if ( c == 0 )       /* no non-delimiter characters */
		{
			last = NULL;
			return ( NULL );
		}
		tok = s - 1;
		
		/*
		 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
		 * Note that delim must have one NUL; we stop if we see that, too.
		 */
		for ( ;; )
		{
			c = *s++;
			spanp = ( char* ) delim;
			do
			{
				if ( ( sc = *spanp++ ) == c )
				{
					if ( c == 0 )
						s = NULL;
					else
						s[-1] = 0;
					last = s;
					return ( tok );
				}
			}
			while ( sc != 0 );
		}
		/* NOTREACHED */
	}
	
}

void* sbrk ( intptr_t increment );

int isdigit ( int c )
{
	return ( ( c>='0' ) && ( c<='9' ) );
}

double atof ( char* s )
{
	double a = 0.0;
	int e = 0;
	int c;
	while ( ( c = *s++ ) != '\0' && isdigit ( c ) )
	{
		a = a*10.0 + ( c - '0' );
	}
	if ( c == '.' )
	{
		while ( ( c = *s++ ) != '\0' && isdigit ( c ) )
		{
			a = a*10.0 + ( c - '0' );
			e = e-1;
		}
	}
	if ( c == 'e' || c == 'E' )
	{
		int sign = 1;
		int i = 0;
		c = *s++;
		if ( c == '+' )
			c = *s++;
		else if ( c == '-' )
		{
			c = *s++;
			sign = -1;
		}
		while ( isdigit ( c ) )
		{
			i = i*10 + ( c - '0' );
			c = *s++;
		}
		e += i*sign;
	}
	while ( e > 0 )
	{
		a *= 10.0;
		e--;
	}
	while ( e < 0 )
	{
		a *= 0.1;
		e++;
	}
	return a;
}
