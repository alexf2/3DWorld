//#pragma option -v-

#include "autorc.hpp"
#include "ind_dbl.hpp"
#include "indicat.hpp"

#include <search.h>
#include <mem.h>


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif


int TIndexedItem::isEqual( const Object& rObj ) const
 {
   return ((TIndexedItem huge *)this == (TIndexedItem huge *)(&rObj));
 };

static int ItemFind( const Object& rObj, Pvoid pPar )
 {
   return (((RTIndexedItem)rObj).Index() == (int)pPar );
 }

TIndexedDoubleList::~TIndexedDoubleList()
 {
   DestroyChache();
 }

static void GetMaxIndexInstance( Object _FAR &rObj, void _FAR *pInd )
 {
   if( RTIndexedItem(rObj).Index() > *(int *)pInd )
     *(int *)pInd = RTIndexedItem(rObj).Index();
 }

int TIndexedDoubleList::GetMaxIndex() const
 {
   int iMaxInd = 0;
   forEach( GetMaxIndexInstance, Pvoid(&iMaxInd) );
   return iMaxInd; 
 }

void TIndexedDoubleList::CreateChache( WORD wSize )//0 - auto detect
 {
   //if( pChacheArray ) delete pChacheArray;
   //wSizeChache = 0;
   DestroyChache();

   if( !wSize ) wSize = GetMaxIndex();
   if( !wSize ) return;

   if( (pChacheArray  = new PTIndexedItem[ wSize ]) )
    {
      _fsetmem( pChacheArray, wSize * sizeof(PTIndexedItem), 0 );
      //for( WORD i = 0; i < wSize; i++ ) pChacheArray[i] = NULL;
      wSizeChache = wSize;

      DoubleListIterator dbIt = DoubleListIterator( *this );
      PTIndexedItem pIt = (PTIndexedItem)&dbIt.current();
      for( ; int(dbIt); pIt = (PTIndexedItem)&++dbIt )
	if( pIt->Index() > 0 && pIt->Index() <= wSize ) pChacheArray[ pIt->Index() - 1 ] = pIt;

      bFlChacheState = TRUE;
    }   
 }

void TIndexedDoubleList::DestroyChache()
 {
   if( pChacheArray ) delete []pChacheArray;
   pChacheArray = NULL;
   wSizeChache = 0;
 }

PTIndexedItem TIndexedDoubleList::operator[]( int ind )
 {
   if( pChacheArray && bFlChacheState == TRUE && ind <= wSizeChache && ind > 0 )
     if( pChacheArray[ind - 1] ) return pChacheArray[ind - 1];

   RTIndexedItem  rItem = (RTIndexedItem)firstThat( ItemFind, (Pvoid)ind );
   if( rItem == NOOBJECT ) return NULL;

   return &rItem;
 }

void ResetFlag( Object _FAR &rObj, void _FAR * )
 {
   ((RTIndexedItem)rObj).bFlAtion = FALSE;
 }

void TIndexedDoubleList::ResetActionFlag()
 {
   forEach( ResetFlag, 0 );
 }

void TIndexedDoubleList::SetIterator( int index, DoubleListIterator& rIter )
 {
   rIter = DoubleListIterator( *this, FALSE );   
   PTIndexedItem  pItem = (PTIndexedItem)&rIter.current();

   for( ; int(rIter); pItem = (PTIndexedItem)&--rIter )
     if( pItem->Index() == index ) return;
 }

int TIndexedDoubleList::GetFreeIndex() const
 {
   DoubleListIterator dbIter = DoubleListIterator( *this );

   BOOL bFlFound = FALSE;
   int index = 1;
   while( bFlFound == FALSE )
    {
      bFlFound = TRUE;

      dbIter.restart();
      PTIndexedItem  pItem = (PTIndexedItem)&dbIter.current();
      for( ; int(dbIter); pItem = (PTIndexedItem)&++dbIter )
	if( pItem->Index() == index )
	 {
	   bFlFound = FALSE; index++;
           break;
         }
    }

   return index;
 }


void TIndexedDoubleList::add( Object _FAR &toAdd )
 {
   DoubleList::add( toAdd );
   PutToChache( RTIndexedItem(toAdd) );
 }

void TIndexedDoubleList::addAtHead( Object _FAR &toAdd )
 {
   DoubleList::addAtHead( toAdd );
   PutToChache( RTIndexedItem(toAdd) );
 }

void TIndexedDoubleList::addAtTail( Object _FAR &toAdd )
 {
   DoubleList::addAtTail( toAdd );
   PutToChache( RTIndexedItem(toAdd) );
 }

void TIndexedDoubleList::detach( Object _FAR &toDetach, DeleteType dt )
 {                   //Container
   int ind = RTIndexedItem(toDetach).Index();
   DoubleList::detach( toDetach, dt );
   RemoveFromChache( ind );
 }

void TIndexedDoubleList::detachFromHead( Object _FAR &toDetach, DeleteType dt )
 {
   int ind = RTIndexedItem(toDetach).Index();
   DoubleList::detachFromHead( toDetach, dt );
   RemoveFromChache( ind );
 }

void TIndexedDoubleList::detachFromTail( Object _FAR &toDetach, DeleteType dt )
 {
   int ind = RTIndexedItem(toDetach).Index();
   DoubleList::detachFromTail( toDetach, dt );
   RemoveFromChache( ind );
 }

void TIndexedDoubleList::flush( DeleteType dt )
 {
   DoubleList::flush( dt );
   //for( WORD i = 0; i < wSizeChache; i++ ) pChacheArray[i] = NULL;
   DestroyChache();
 }

void TIndexedDoubleList::destroyFromHead( Object _FAR &toDestroy )
 {
   int ind = RTIndexedItem(toDestroy).Index();
   DoubleList::destroyFromHead( toDestroy );
   RemoveFromChache( ind );
 }

void TIndexedDoubleList::destroyFromTail( Object _FAR &toDestroy )
 {
   int ind = RTIndexedItem(toDestroy).Index();
   DoubleList::destroyFromTail( toDestroy );
   RemoveFromChache( ind );
 }

void TIndexedDoubleList::PutToChache( RTIndexedItem toAdd )
 {
   if( pChacheArray && toAdd.Index() <= wSizeChache && toAdd.Index() > 0 )
     pChacheArray[toAdd.Index() - 1] = &toAdd;
 }

void TIndexedDoubleList::RemoveFromChache( int ind )
 {
   if( pChacheArray && ind <= wSizeChache && ind > 0 )
     pChacheArray[ ind - 1 ] = NULL;
 }

struct TGetMaxDta
 {
   PTCompareFunc pFC;
   PTIndexedItem pMaxItem;

   TGetMaxDta( PTCompareFunc pFC_, PTIndexedItem pIt )
    {
      pFC = pFC_; pMaxItem = pIt;
    }
 };
typedef TGetMaxDta* PTGetMaxDta;
//_PTRDEF(TGetMaxDta)

static void GetMaximumInstance( Object _FAR &rObj, void _FAR *pD )
 {
   if( (((RTIndexedItem)rObj).*(PTGetMaxDta(pD)->pFC))(*(PTGetMaxDta(pD)->pMaxItem)) > 0 )
     PTGetMaxDta(pD)->pMaxItem = &((RTIndexedItem)rObj);
 }


RTIndexedItem TIndexedDoubleList::GetMaximum( PTCompareFunc  pCmpF )
 {
   if( peekAtHead() == NOOBJECT ) return (RTIndexedItem)*ZERO;

   TGetMaxDta gmdDta( pCmpF, (PTIndexedItem)&peekAtHead() );
   forEach( GetMaximumInstance, Pvoid(&gmdDta) );

   return *gmdDta.pMaxItem;
 }

/* void TIndexedDoubleList::Sort( PTCompareFunc pCmpF, PTIndicator pInd )
 {
   TIndexedDoubleList idlTmp;
   DWORD dwWork = 0;


   PTIndexedItem pMax = NULL;
   while( *(pMax = &GetMaximum(pCmpF)) != NOOBJECT )
    {
      detach( *pMax, TShouldDelete::NoDelete );
      idlTmp.addAtHead( *pMax );
      if( pInd ) pInd->WorkMessage( ++dwWork );
    }

   DoubleListIterator dbIt = DoubleListIterator( idlTmp, TRUE );
   PTIndexedItem pIt = (PTIndexedItem)&dbIt.current();
   for( ; int(dbIt); pIt = (PTIndexedItem)&++dbIt )
    {
      //idlTmp.detach( *pIt, TShouldDelete::NoDelete );
      addAtTail( *pIt );
      if( pInd ) pInd->WorkMessage( ++dwWork );
    }


   idlTmp.flush( TShouldDelete::NoDelete );
 }*/

 void TIndexedDoubleList::Sort( int _export (*pCmpF)(const void *, const void *), PTIndicator pInd )
  {
    if( getItemsInContainer() < 2 ) return;

    int iNumbItems;
    TAutoMemoryRc<PTIndexedItem*> mrcPtr = new PTIndexedItem[ (iNumbItems = getItemsInContainer()) ];
    if( !mrcPtr ) return;

    DoubleListIterator dblIt( *this );
    PTIndexedItem pItem = (PTIndexedItem)&dblIt.current();
    
    for( PTIndexedItem* pDst = mrcPtr(); int(dblIt); pItem = (PTIndexedItem)&++dblIt, pDst++ )
      *pDst = pItem;

    flush( TShouldDelete::NoDelete );

    qsort( mrcPtr(), iNumbItems, sizeof(PTIndexedItem), pCmpF );

    int iCount = iNumbItems;
    for( PTIndexedItem* pSrc = mrcPtr(); iCount > 0; iCount--, pSrc++ )
      addAtTail( **pSrc );

    if( pInd ) pInd->WorkIs100();
  }
