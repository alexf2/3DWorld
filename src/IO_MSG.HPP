#if !defined(_ERROR_MESSAGE_)
#define _ERROR_MESSAGE_

#include "INC\io_em.inc"
#include <owl.h>

#undef _EXPORT2
#if defined(_IOMSGDLL) || defined(_ALLLIBDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_IOMSGDLL) || defined(_ALLLIBDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


int _EXPFUNC2 MakeMsg( HINSTANCE hInst, HWND hwnd, UINT idStrTemplate, UINT idTitle, UINT fuStyle, LPSTR lpName = NULL );
int _EXPFUNC2 MakeMsgErr( HINSTANCE hInst, HWND hwnd, UINT idStrTemplate, UINT idTitle, UINT fuStyle, LPSTR lpName = NULL );

#endif
