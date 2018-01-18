#include "sorte.hpp"
#include "s_asm.h"

//#pragma  inline

#pragma option -G

static  comparF    *Fcmp;
static  comparF32  *Fcmp32;
static  unsigned    qWidth;
static void  near pascal  qSortHelp ( char huge *pivotP, DWORD nElem );


static  void  near pascal  Exchange( void  huge* leftP, void huge* rightP )
{

  int  i;
  char  c;

  for (i = 0; i < qWidth; i++ )
   {
     c = *((char huge *) rightP);
     *((char huge *) rightP )++ = *((char huge *) leftP);
     *((char huge *) leftP )++ = c;
   }

/*
asm     push    DS

asm     cld
asm     mov     cx, qWidth
asm     les     di, rightP
asm     lds     si, leftP

 // Assert: qWidth is never zero, see test at entry to qsort().

asm     shr     cx, 1                   // test for an odd number of bytes 
asm     jnc     xch_wordLoop

asm     mov     al, es:[di]
asm     movsb                           // swap bytes, advancing pointers 
asm     mov     [si-1], al
asm     jz      xch_end                 // if CX was originally 1       

  //Swapping words is almost twice as fast even if running on an 8088 or if
  //the data is odd-aligned, since the CPU needs to fetch only half the
  //number of instructions per byte.

xch_wordLoop:
asm     mov     ax, es:[di]
asm     movsw                           // swap words, advancing pointers 
asm     mov     [si-2], ax
asm     loop    xch_wordLoop

xch_end:
asm     pop     DS

    return;*/
}


TFlatSelector::TFlatSelector( BYTE huge* hptr, DWORD dwSize ):
  bFlAlloc( FALSE )
 {
   uiOffs = FP_OFF( (BYTE far*)hptr );
   uiFlatSelector = AllocSelector( FP_SEG((BYTE far*)hptr) );
   SetSelectorLimit( uiFlatSelector, (dwLimit = dwSize + uiOffs) );
   //SetSelectorLimit( uiFlatSelector, 1000 );

   bFlAlloc = TRUE;
 }

TFlatSelector::~TFlatSelector()
 {
   if( bFlAlloc == TRUE ) { bFlAlloc = FALSE; FreeSelector(uiFlatSelector); }
 }

void SortSimple( BYTE huge* pArr, DWORD dwSize, int iSizeItem, comparF *pCmp )
 {
   if( !dwSize || !iSizeItem ) return;

   Fcmp = pCmp;
   qWidth = iSizeItem;

   BOOL bFlSorted = FALSE;
   for( LONG i = 0; i < dwSize - 1 && bFlSorted == FALSE; i++ )   
    {
      bFlSorted = TRUE;
      BYTE  huge *p1 = pArr, huge *p2 = pArr + iSizeItem;

      for( LONG j = 0; j < dwSize - 1 - i; j++, p1+=iSizeItem, p2+=iSizeItem )
	if( Fcmp(p1, p2) > 0 )
         {
	   Exchange( p1, p2 );
	   bFlSorted = FALSE;
	 }
    }
 }

void SortSimple32( BYTE huge* pArr, DWORD dwSize, int iSizeItem, comparF32 *pCmp )
 {
   TFlatSelector fsSelector( pArr, dwSize * (DWORD)iSizeItem );

   Fcmp32 = pCmp;
   qWidth = iSizeItem;

   SortSimple32_( fsSelector(), fsSelector.Offs(), pCmp, iSizeItem, dwSize );
 }

void SortQuick( BYTE huge* pArr, DWORD dwSize, int iSizeItem, comparF *pCmp )
 {
   if( !(qWidth = iSizeItem) ) return;
   Fcmp = pCmp;

   qSortHelp( pArr, dwSize );
 }

#define _LT_(x,y)       (x < y)

static void  near pascal  qSortHelp ( char huge *pivotP, DWORD nElem )
 {
    char    huge *leftP, huge *rightP;
    char    huge *pivotEnd, huge *pivotTemp, huge *leftTemp;

    unsigned  lNum;
    int       retval;


tailRecursion:
    if (nElem <= 2)
        {
        if (nElem == 2)
            {
            if (Fcmp (pivotP, rightP = qWidth + pivotP) > 0)
                Exchange (pivotP, rightP);
            }
        return;
        }

    rightP = (nElem - 1) * qWidth + pivotP;
    leftP  = (nElem >> 1) * qWidth + pivotP;

/*  sort the pivot, left, and right elements for "median of 3" */

    if (Fcmp (leftP, rightP) > 0)
        Exchange (leftP, rightP);
    if (Fcmp (leftP, pivotP) > 0)
        Exchange (leftP, pivotP);
    else if (Fcmp (pivotP, rightP) > 0)
        Exchange (pivotP, rightP);

    if (nElem == 3)
        {
        Exchange (pivotP, leftP);
        return;
        }

/*  now for the classic Hoare algorithm */

    leftP = pivotEnd = pivotP + qWidth;

    do
        {
        while ((retval = Fcmp(leftP, pivotP)) <= 0)
            {
            if (retval == 0)
                {
                Exchange(leftP, pivotEnd);
                pivotEnd += qWidth;
                }
            if (_LT_ (leftP, rightP))
                leftP += qWidth;
            else
                goto qBreak;
            }

        while (_LT_(leftP, rightP))
            {
            if ((retval = Fcmp(pivotP, rightP)) < 0)
                rightP -= qWidth;
            else
                {
                Exchange (leftP, rightP);
                if (retval != 0)
                    {
                    leftP += qWidth;
                    rightP -= qWidth;
                    }
                break;
                }
            }
        }   while (_LT_(leftP, rightP));

qBreak:

    if (Fcmp(leftP, pivotP) <= 0)
        leftP = leftP + qWidth;

    leftTemp = leftP - qWidth;

    pivotTemp = pivotP;

    while ((pivotTemp < pivotEnd) && (leftTemp >= pivotEnd))
        {
        Exchange(pivotTemp, leftTemp);
        pivotTemp += qWidth;
        leftTemp -= qWidth;
        }

    lNum = (leftP - pivotEnd) / qWidth;
    nElem = ((nElem * qWidth + pivotP) - leftP)/qWidth;

    // Sort smaller partition first to reduce stack usage
    if (nElem < lNum)
        {
        qSortHelp (leftP, nElem);
        nElem = lNum;
        }
    else
        {
        qSortHelp (pivotP, lNum);
        pivotP = leftP;
        }

    goto tailRecursion;
 }

