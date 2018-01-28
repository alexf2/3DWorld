#if !defined(_SORTE_)
#define _SORTE_

#include <_defs.h>
#include <windows.h>
#include <dos.h>

#undef _EXPORT2
#if defined(_SORTEDLL) || defined(_ALLCTLDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_SORTEDLL) || defined(_ALLDLGDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


typedef int _EXPFUNC2 comparF( const void huge*, const void huge* );
typedef int comparF32();

void _EXPFUNC2 SortSimple( BYTE huge* pArr, DWORD dwSize, int iSizeItem, comparF *pCmp );
void _EXPFUNC2 SortSimple32( BYTE huge* pArr, DWORD dwSize, int iSizeItem, comparF32 *pCmp );
void _EXPFUNC2 SortQuick( BYTE huge* pArr, DWORD dwSize, int iSizeItem, comparF *pCmp );

class _EXPORT2 TFlatSelector
 {
public:
  TFlatSelector( BYTE huge* hptr, DWORD dwSize );
  ~TFlatSelector();

  BYTE _FAR* operator()() const { return (BYTE _FAR*)MK_FP(uiFlatSelector, uiOffs); }
  UINT Selector() const { return uiFlatSelector; }
  UINT Offs() const { return uiOffs; }
  DWORD Limit() const { return dwLimit; }

private:
  UINT uiFlatSelector;

  DWORD dwLimit;
  UINT uiOffs;

  BOOL bFlAlloc;

 };


#endif
