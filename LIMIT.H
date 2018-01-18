#if !defined(_CHECK_LIMIT_)
#define _CHECK_LIMIT_

extern "C" {
  int far LimitInt( char far * );            //return 0 if Ok
  int far LimitByte( LPSTR );
  int far LimitUInt( char far * );
  int far LimitSByte( char far * );
	   }

#endif
