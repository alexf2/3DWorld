#if !defined(_SORT_ASM_)
#define _SORT_ASM_

extern "C" {
  void far SortSimple32_( char far* pArrFlat, unsigned int uiOffs,
    int (far* pCmp)(), int iSizeItem, unsigned long dwSize );

  int Cmp32( void );
	   }

#endif
