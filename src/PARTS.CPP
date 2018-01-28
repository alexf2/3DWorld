//#pragma option -v-

#include "threshol.hpp"
#include "surface.hpp"
#include "INC\errors.inc"
#include "fheader.hpp"
//#include "scena.hpp"
#include <values.h>


MemBlocks* TFacet::mgr = NULL;
long TFacet::countMemMgr = 0;

void _FAR* TFacet::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TFacet::mgr = new MemBlocks( sizeof(TFacet), 100 );

   return TFacet::mgr->allocate( size );
 }

void TFacet::operator delete( void _FAR* pToDel )
 {   
   TFacet::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TFacet::mgr;
      TFacet::mgr = NULL;
    }
 }


int _export CompareFacetY( const void *p1, const void *p2 )
 {
   RTFacet rF1 = *(PTFacet)(*(PTIndexedItem*)p1),
	   rF2 = *(PTFacet)(*(PTIndexedItem*)p2);

   if( rF1.minY < rF2.minY ) return -1;
   else if( rF1.minY > rF2.minY ) return 1;
   else return 0;

 }

int _export CompareFacetYRev( const void *p1, const void *p2 )
 {
   RTFacet rF1 = *(PTFacet)(*(PTIndexedItem*)p1),
	   rF2 = *(PTFacet)(*(PTIndexedItem*)p2);

   if( rF1.maxY > rF2.maxY ) return -1;
   else if( rF1.maxY < rF2.maxY ) return 1;
   else return 0;

 }

int _export CompareFacetYRevInt( const void *p1, const void *p2 )
 {
   RTFacet rF1 = *(PTFacet)(*(PTIndexedItem*)p1),
	   rF2 = *(PTFacet)(*(PTIndexedItem*)p2);

   if( rF1.iMaxY > rF2.iMaxY ) return -1;
   else if( rF1.iMaxY < rF2.iMaxY ) return 1;
   else return 0;

 }



int TFacet::CompareY( RTFacet rFacForCmp )
 {
   if( minY < rFacForCmp.minY ) return -1;
   else if( minY > rFacForCmp.minY ) return 1;
   else return 0;
 }

TVector TFacet::GetTraceNormal( double x, double y )
 {
   return vecOutNormal;
 }

/*TVector TFacet::GetTraceNormalPhong( double x, double y ) const
 {
   //return vecOutNormal;
   if( listAgesItem.getItemsInContainer() < 2 ) return vecOutNormal;

   DoubleListIterator dblIt( listAgesItem );
   PTAdgeItem pAIt = (PTAdgeItem)&dblIt.current();

   PTAdge pA1 = NULL, pA2 = NULL;
   PTVertex pV11, pV12, pV21, pV22, pV11_, pV12_;
   for( ; int(dblIt); pAIt = (PTAdgeItem)&++dblIt )
    {
      PTAdge pA = pAIt->pAdge;
      
      if( fabs(pA->pV1->y - pA->pV2->y) < 0.01 ) continue;

      if( pA->pV1->y < pA->pV2->y ) { pV11_ = pA->pV1; pV12_ = pA->pV2; }
      else { pV11_ = pA->pV2; pV12_ = pA->pV1; }
      

      if( y >= pV11_->y && y <= pV12_->y )
       {
	 if( !pA1 )
	  {
	    pA1 = pA; pV11 = pV11_; pV12 = pV12_;
	  }
	 else if( !pA2 )
	  {
	    if( y == pV11->y && y == pV12_->y || y == pV12->y && y == pV11_->y ) continue;
	    pA2 = pA; pV21 = pV11_; pV22 = pV12_;
	    break;
          }
       }
    }

   if( !pA1 && !pA2 ) return vecOutNormal;
   if( !pA2 && pA1 && (y == pV11->y || y == pV12->y) )
    {
      if( y == pV11->y ) return pV11->vecMeanPhongNormal;
      else return pV12->vecMeanPhongNormal;
    }

   double
     x1 = (y - pV11->y) / (pV12->y - pV11->y) * (pV12->x - pV11->x) + pV11->x,
     x2 = (y - pV21->y) / (pV22->y - pV21->y) * (pV22->x - pV21->x) + pV21->x;

   if( x1 > x2 )
    {
      double dTmp = x1; x1 = x2; x2 = dTmp;
      PTAdge pATmp = pA1; pA1 = pA2; pA2 = pATmp;
      PTVertex pVTmp = pV11; pV11 = pV21; pV21 = pVTmp;
      pVTmp = pV12; pV12 = pV22; pV22 = pVTmp;
    }

   double dLenZ = (x2 - x1) * 0.001;
   if( fabs(x1 - x) < 0.1 || fabs(x2 - x1) < 0.1 )
    {
      double dU = fabs( (pV11->y - y) / (pV11->y - pV12->y) );
      TVector vecN = pV11->vecMeanPhongNormal * (1 - dU) + pV12->vecMeanPhongNormal * dU;
      return Normalyze( vecN );
    }
   else if( fabs(x2 - x) < 0.1 )
    {
      double dU = fabs( (pV21->y - y) / (pV21->y - pV22->y) );
      TVector vecN = pV21->vecMeanPhongNormal * (1 - dU) + pV22->vecMeanPhongNormal * dU;
      return Normalyze( vecN );
    }

   double dU = fabs( (pV11->y - y) / (pV11->y - pV12->y) );
   double dW = fabs( (pV21->y - y) / (pV21->y - pV22->y) );
   double dT = fabs( (x1 - x) / (x1 - x2) );

   TVector vecQ = pV11->vecMeanPhongNormal * (1 - dU) + pV12->vecMeanPhongNormal * dU;
   TVector vecR = pV21->vecMeanPhongNormal * (1 - dW) + pV22->vecMeanPhongNormal * dW;

   TVector vecN = vecQ * (1 - dT) + vecR * dT;

   return Normalyze( vecN );
 }*/

/*static RTAdgeItem near pascal GetIncidentAdge_( RTIndexedDoubleList rList,
  RTAdge rA, RTVertex rV )
 {
   DoubleListIterator dblIt( rList );
   PTAdgeItem pItem = (PTAdgeItem)&dblIt.current();

   for( ; int(dblIt); pItem = (PTAdgeItem)&++dblIt )
    {
      PTAdge pA = pItem->pAdge;
      if( pA == &rA ) continue;

      if( pA->pV1 == &rV || pA->pV2 == &rV ) return *pItem;
    }

   return (RTAdgeItem)NOOBJECT;
 }*/

static double near pascal MinY__( RTAdge rA )
 {
   return (rA.pV1->y < rA.pV2->y) ? rA.pV1->y:rA.pV2->y;
 }

/*static int near pascal Level( RTAdge rA, PTVertex pV )
 {
   double dMin = MinY__( rA );
   return dMin != pV->y;
 }*/


struct TIntersectDta
 {
   enum TIntersectType { IT_MinnMax, IT_Adge, IT_LocalExtr };

   double x;
   TIntersectType itType;
   PTAdgeItem pAI;
   PTVertex pV;
   PTVertex pV1, pV2;

   TIntersectDta() {};
   TIntersectDta( double x_, TIntersectType itType_, PTAdgeItem pAI_, PTVertex pV_, PTVertex pV1_, PTVertex pV2_ )
    {
      x = x_; itType = itType_; pAI = pAI_; pV = pV_;
      pV1 = pV1_; pV2 = pV2_;
    }
 };
typedef TIntersectDta* PTIntersectDta;
typedef TIntersectDta& RTIntersectDta;

static double near pascal IntersectX( double x1, double y1, double x2, double y2, double y )
 {
   return (y - y1) / (y2 - y1) * (x2 - x1) + x1;
 }

static int CmpIntersect( const void *d1, const void *d2 )
 {
   if( PTIntersectDta(d1)->x < PTIntersectDta(d2)->x ) return -1;
   else if( PTIntersectDta(d1)->x > PTIntersectDta(d2)->x ) return 1;
   else return 0;
 }

static TVector near pascal PhongNormal( PTIntersectDta pI1, PTIntersectDta pI2, double x, double y )
 {
   double dU = fabs( (pI1->pV1->y - y) / (pI1->pV1->y - pI1->pV2->y) );
   double dW = fabs( (pI2->pV1->y - y) / (pI2->pV1->y - pI2->pV2->y) );
   double dT;
   if( fabs(pI1->x - pI2->x) < TFACET_d_HorizontalAdge ) dT = 0;
   else dT = fabs( (pI1->x - x) / (pI1->x - pI2->x) );
		  
   TVector vecQ = pI1->pAI->PhongN( pI1->pV1 ) * (1 - dU) + pI1->pAI->PhongN( pI1->pV2 ) * dU;
   TVector vecR = pI2->pAI->PhongN( pI2->pV1 ) * (1 - dW) + pI2->pAI->PhongN( pI2->pV2 ) * dW;

   TVector vecN = vecQ * (1 - dT) + vecR * dT;

   return Normalyze( vecN );
 }


TVector TFacet::GetTraceNormalPhong( double x, double y ) 
 {   
   if( listAgesItem.getItemsInContainer() < 2 || iSmothGroup < 1 ) return vecOutNormal;

   const int iMaxIntersect = 50;
   TIntersectDta  itIntersect[ iMaxIntersect ];


   int iCountIntersect = 0;
   //listAgesItem.ResetActionFlag();

   DoubleListIterator dbiAg( listAgesItem );
   PTAdgeItem pAgP = (PTAdgeItem)&dbiAg.current();   

   for( ; int(dbiAg) && iCountIntersect < iMaxIntersect; pAgP = (PTAdgeItem)&++dbiAg )
    {
      //if( pAgP->bFlAtion == TRUE ) continue;
      PTAdge pA = pAgP->pAdge;      

      PTVertex pV1 = pA->pV1, pV2 = pA->pV2;
      /*if( fabs(pV1->y - pV2->y) < TFACET_d_HorizontalAdge )
       {
	 pAgP->bFlAtion = TRUE;
	 continue;
       }*/

      if( pV1->y > pV2->y )
       {
	 PTVertex pTmp = pV1; pV1 = pV2; pV2 = pTmp;
       }      

      //pAgP->bFlAtion = TRUE;
      if( y < pV1->y || y > pV2->y ) continue;      

      if( pV1->y == y ) continue;
       /*{
	 //RTAdgeItem  rAInc = GetIncidentAdge_( listAgesItem, *pA, *pV1 );

	 if( rAInc != NOOBJECT )
	  {
	    rAInc.bFlAtion = TRUE;
			
	    if( !Level( *rAInc.pAdge, pV1 ) )
	      itIntersect[ iCountIntersect++ ] = TIntersectDta( pV1->x, TIntersectDta::IT_LocalExtr, pAgP, pV1, pV1, pV2 );
	    else
	      itIntersect[ iCountIntersect++ ] = TIntersectDta( pV1->x, TIntersectDta::IT_MinnMax, pAgP, pV1, pV1, pV2 );
	  }
	 else itIntersect[ iCountIntersect++ ] = TIntersectDta( pV1->x, TIntersectDta::IT_LocalExtr, pAgP, pV1, pV1, pV2 );
       }*/
      else if( pV2->y == y ) itIntersect[ iCountIntersect++ ] = TIntersectDta( pV2->x, TIntersectDta::IT_LocalExtr, pAgP, pV2, pV1, pV2 );
       /*{
	 //RTAdgeItem  rAInc = GetIncidentAdge_( listAgesItem, *pA, *pV2 );

	 /*if( rAInc != NOOBJECT )
	  {
	    rAInc.bFlAtion = TRUE;	

	    if( !Level( *rAInc.pAdge, pV2 ) )
	      itIntersect[ iCountIntersect++ ] = TIntersectDta( pV2->x, TIntersectDta::IT_LocalExtr, pAgP, pV2, pV1, pV2 );
	    else
	      itIntersect[ iCountIntersect++ ] = TIntersectDta( pV2->x, TIntersectDta::IT_MinnMax, pAgP, pV2, pV1, pV2 );
	  }
	 else itIntersect[ iCountIntersect++ ] = TIntersectDta( pV2->x, TIntersectDta::IT_LocalExtr, pAgP, pV2, pV1, pV2 );
       } */
      else
	itIntersect[ iCountIntersect++ ] =
	  TIntersectDta(
	                 IntersectX( pV1->x, pV1->y, pV2->x, pV2->y, y ),
			 TIntersectDta::IT_Adge,
			 pAgP, NULL, pV1, pV2
		       );

    }

   if( !iCountIntersect ) return vecOutNormal;

   if( iCountIntersect == 1 )
    {                    
      double dU = fabs( (itIntersect[0].pV1->y - y) / (itIntersect[0].pV1->y - itIntersect[0].pV2->y) );
      TVector vecN = itIntersect[0].pAI->PhongN(itIntersect[0].pV1) * (1 - dU) +
	itIntersect[0].pAI->PhongN(itIntersect[0].pV2) * dU;
      return Normalyze( vecN );
    }

   qsort( itIntersect, iCountIntersect, sizeof(TIntersectDta), CmpIntersect );

   PTIntersectDta pInt = itIntersect;
   double dDelta = INT_MAX;
   PTIntersectDta pIntNearst = NULL;
   for( int iC = iCountIntersect / 2; iC > 0; iC-- )
    {
      PTIntersectDta pI1 = pInt++;
      PTIntersectDta pI2 = pInt++;

      if( x >= pI1->x && x <= pI2->x ) return PhongNormal( pI1, pI2, x, y );
      else
       {
	 double d =  min( fabs( x - pI1->x ), fabs( x - pI2->x ) );
	 if( dDelta > d )
	  {
	    dDelta = d;
	    pIntNearst = pI1;
          }
       }
    }

   if( pIntNearst ) return PhongNormal( pIntNearst, pIntNearst + 1, x, y );
   return vecOutNormal;
 }

MemBlocks* TVertex::mgr = NULL;
long TVertex::countMemMgr = 0;

void _FAR* TVertex::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TVertex::mgr = new MemBlocks( sizeof(TVertex), 100 );

   return TVertex::mgr->allocate( size );
 }

void TVertex::operator delete( void _FAR* pToDel )
 {   
   TVertex::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TVertex::mgr;
      TVertex::mgr = NULL;
    }
 }

int TVertex::isEqual( const Object& rObj ) const
 {
   return (TVertex huge *)this == (TVertex huge *)&rObj;
 }


int TVertex::Load( HFILE hf )
 {
   T3DPoint pt;

   UINT l;
   if( (l = _lread( hf, &pt, sizeof(T3DPoint) )) == HFILE_ERROR ) return -1;
   if( l != sizeof(T3DPoint) ) return -1;
   if( (l = _lread( hf, &vecMeanPhongNormal, sizeof(TVector) )) == HFILE_ERROR ) return -1;
   if( l != sizeof(TVector) ) return -1;

   x = pt.x; y = pt.y; z = pt.z;

   return 0;
 }

int TVertex::Save( HFILE hf )
 {
   T3DPoint pt = T3DPoint( x, y, z );

   if( _lwrite( hf, &pt, sizeof(T3DPoint) ) == HFILE_ERROR ) return -1;
   if( _lwrite( hf, &vecMeanPhongNormal, sizeof(TVector) ) == HFILE_ERROR ) return -1;

   return 0;
 }

MemBlocks* TAdge::mgr = NULL;
long TAdge::countMemMgr = 0;

void _FAR* TAdge::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TAdge::mgr = new MemBlocks( sizeof(TAdge), 100 );

   return TAdge::mgr->allocate( size );
 }

void TAdge::operator delete( void _FAR* pToDel )
 {   
   TAdge::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TAdge::mgr;
      TAdge::mgr = NULL;
    }
 }



int TAdge::isEqual( const Object& rObj ) const
 {
   return (TAdge huge *)this == (TAdge huge *)&rObj;
 }

int TAdge::Save( HFILE hf )
 {
   int ind1 = pV1->Index(), ind2 = pV2->Index();

   if( _lwrite( hf, &ind1, sizeof(int) ) == HFILE_ERROR ) return -1;
   if( _lwrite( hf, &ind2, sizeof(int) ) == HFILE_ERROR ) return -1;

   return 0;
 }

MemBlocks* TAdgeItem::mgr = NULL;
long TAdgeItem::countMemMgr = 0;

void _FAR* TAdgeItem::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TAdgeItem::mgr = new MemBlocks( sizeof(TAdgeItem), 100 );

   return TAdgeItem::mgr->allocate( size );
 }

void TAdgeItem::operator delete( void _FAR* pToDel )
 {   
   TAdgeItem::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TAdgeItem::mgr;
      TAdgeItem::mgr = NULL;
    }
 }



int TAdgeItem::isEqual( const Object& rObj ) const
 {
   return (TAdgeItem huge *)this == (TAdgeItem huge *)&rObj;
 }

int _export CompareAdgeItemY( const void *p1, const void *p2 )
 {
   RTAdgeItem rA1 = *(PTAdgeItem)(*(PTIndexedItem*)p1),
	      rA2 = *(PTAdgeItem)(*(PTIndexedItem*)p2);

   double dYMy = min( rA1.pAdge->pV1->y, rA1.pAdge->pV2->y ),
	  dYCmp = min( rA2.pAdge->pV1->y, rA2.pAdge->pV2->y );

   if( dYMy < dYCmp ) return -1;
   else if( dYMy > dYCmp ) return 1;
   else return 0;
 }


int _export CompareAdgeItemYRev( const void *p1, const void *p2 )
 {
   RTAdgeItem rA1 = *(PTAdgeItem)(*(PTIndexedItem*)p1),
	      rA2 = *(PTAdgeItem)(*(PTIndexedItem*)p2);

   double dYMy = max( rA1.pAdge->pV1->y, rA1.pAdge->pV2->y ),
	  dYCmp = max( rA2.pAdge->pV1->y, rA2.pAdge->pV2->y );

   if( dYMy > dYCmp ) return -1;
   else if( dYMy < dYCmp ) return 1;
   else return 0;
 }


int _export CompareAdgeItemYRevInt( const void *p1, const void *p2 )
 {
   RTAdgeItem rA1 = *(PTAdgeItem)(*(PTIndexedItem*)p1),
	      rA2 = *(PTAdgeItem)(*(PTIndexedItem*)p2);

   int dYMy = max( int(rA1.pAdge->pV1->y), int(rA1.pAdge->pV2->y) ),
       dYCmp = max( int(rA2.pAdge->pV1->y), int(rA2.pAdge->pV2->y) );

   if( dYMy > dYCmp ) return -1;
   else if( dYMy < dYCmp ) return 1;
   else return 0;
 }


int TAdgeItem::CompareY( RTAdgeItem rAi )
 {
   double dYMy = min( pAdge->pV1->y, pAdge->pV2->y ),
	  dYCmp = min( rAi.pAdge->pV1->y, rAi.pAdge->pV2->y );

   if( dYMy < dYCmp ) return -1;
   else if( dYMy > dYCmp ) return 1;
   else return 0;
 }

int _export CompareAdgeY( const void *p1, const void *p2 )
 {
   RTAdge rA1 = *(PTAdge)(*(PTIndexedItem*)p1),
	  rA2 = *(PTAdge)(*(PTIndexedItem*)p2);

   double dYMy = min( rA1.pV1->y, rA1.pV2->y ),
	  dYCmp = min( rA2.pV1->y, rA2.pV2->y );

   if( dYMy < dYCmp ) return -1;
   else if( dYMy > dYCmp ) return 1;
   else return 0;
 }

int TAdge::CompareY( RTAdge rAdgeCmp )
 {
   double dYMy = min( pV1->y, pV2->y ),
	  dYCmp = min( rAdgeCmp.pV1->y, rAdgeCmp.pV2->y );

   if( dYMy < dYCmp ) return -1;
   else if( dYMy > dYCmp ) return 1;
   else return 0;
 }


MemBlocks* TFacetItem::mgr = NULL;
long TFacetItem::countMemMgr = 0;

void _FAR* TFacetItem::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TFacetItem::mgr = new MemBlocks( sizeof(TFacetItem), 100 );

   return TFacetItem::mgr->allocate( size );
 }

void TFacetItem::operator delete( void _FAR* pToDel )
 {   
   TFacetItem::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TFacetItem::mgr;
      TFacetItem::mgr = NULL;
    }
 }


int TFacetItem::isEqual( const Object& rObj ) const
 {
   return (TFacetItem huge *)this == (TFacetItem huge *)&rObj;
 }



int TFacet::isEqual( const Object& rObj ) const
 {
   return (TFacet huge *)this == (TFacet huge *)&rObj;
 }

TVector TFacet::GetNewelNormal( PTUCS pUCS ) const
 {
   double a = 0, b = 0, c = 0;
   DoubleListIterator dbAg = DoubleListIterator( listAgesItem, FALSE );
   PTAdgeItem  pIt1 = (PTAdgeItem)&dbAg.current();
   for( ; int(dbAg); pIt1 = (PTAdgeItem)&--dbAg )
    {
      PTVertex pVer1, pVer2;

      if( pIt1->clockVise == 1 )
       {
         pVer1 = pIt1->pAdge->pV1; pVer2 = pIt1->pAdge->pV2;
       }
      else
       {
         pVer2 = pIt1->pAdge->pV1; pVer1 = pIt1->pAdge->pV2;
       }

      if( !pUCS )
       {
         a += (pVer1->y - pVer2->y) * (pVer1->z + pVer2->z);
         b += (pVer1->z - pVer2->z) * (pVer1->x + pVer2->x);
	 c += (pVer1->x - pVer2->x) * (pVer1->y + pVer2->y);
       }
      else
       {
	 T3DPoint  tp[2];

	 /*tp[0] = T3DPoint( pVer1->x, pVer1->y, pVer1->z );
	 tp[1] = T3DPoint( pVer2->x, pVer2->y, pVer2->z );*/
	 tp[0] = T3DPoint( *(PT3DPoint)pVer1 );
	 tp[1] = T3DPoint( *(PT3DPoint)pVer2 );

	 pUCS->RecalcUserToBase( tp, 2 );

	 a += (tp[0].y - tp[1].y) * (tp[0].z + tp[1].z);
	 b += (tp[0].z - tp[1].z) * (tp[0].x + tp[1].x);
	 c += (tp[0].x - tp[1].x) * (tp[0].y + tp[1].y);
       }
    }

   return Normalyze( TVector( a, b, c ) );
 }

void TFacet::CalcBounds()
 {
   DoubleListIterator dbIt = DoubleListIterator( listAgesItem, FALSE );
   PTAdgeItem pAi = (PTAdgeItem)&dbIt.current();

   minY = minZ = minX = INT_MAX;
   maxY = maxZ = maxX = INT_MIN;

   for( ; int(dbIt); pAi = (PTAdgeItem)&--dbIt )
    {
      PTVertex pVert = pAi->pAdge->pV1;

      while( 1 )
       {
         minX = (pVert->x < minX) ? pVert->x:minX;
         maxX = (pVert->x > maxX) ? pVert->x:maxX;

         minY = (pVert->y < minY) ? pVert->y:minY;
         maxY = (pVert->y > maxY) ? pVert->y:maxY;

         minZ = (pVert->z < minZ) ? pVert->z:minZ;
	 maxZ = (pVert->z > maxZ) ? pVert->z:maxZ;

	 if( (void huge *)pVert == (void huge *)(pAi->pAdge->pV2) ) break;
	 pVert = pAi->pAdge->pV2;
       }
    }

   iMinX = floor( minX );
   iMaxX = ceil( maxX );
   iMinY = floor( minY );
   iMaxY = ceil( maxY );
   iMinZ = floor( minZ );
   iMaxZ = ceil( maxZ );
 }



T3DPoint TFacet::GetGeomCenter() const
 {
   /*double minY, minZ, minX, maxY, maxZ, maxX;
   minY = minZ = minX = INT_MAX;
   maxY = maxZ = maxX = INT_MIN;


   DoubleListIterator dbAg = DoubleListIterator( listAgesItem, FALSE );
   PTAdgeItem  pIt1 = (PTAdgeItem)&dbAg.current();
   for( ; int(dbAg); pIt1 = (PTAdgeItem)&--dbAg )
    {
      minX = (pIt1->pAdge->pV1->x < minX) ? pIt1->pAdge->pV1->x:minX;
      maxX = (pIt1->pAdge->pV1->x > maxX) ? pIt1->pAdge->pV1->x:maxX;

      minY = (pIt1->pAdge->pV1->y < minY) ? pIt1->pAdge->pV1->y:minY;
      maxY = (pIt1->pAdge->pV1->y > maxY) ? pIt1->pAdge->pV1->y:maxY;

      minZ = (pIt1->pAdge->pV1->z < minZ) ? pIt1->pAdge->pV1->z:minZ;
      maxZ = (pIt1->pAdge->pV1->z > maxZ) ? pIt1->pAdge->pV1->z:maxZ;
    }

   return T3DPoint( minX + fabs(maxX - minX) / 2.0,
		    minY + fabs(maxY - minY) / 2.0,
		    minZ + fabs(maxZ - minZ) / 2.0 );*/
   CalcBounds();
   return  T3DPoint( minX + fabs(maxX - minX) / 2.0,
		     minY + fabs(maxY - minY) / 2.0,
		     minZ + fabs(maxZ - minZ) / 2.0
		   );
 }


long TFacet::GetBytesSize() const
 {
   return (long)listAgesItem.getItemsInContainer() * TAdgeItem::SizeData();
 }

int TFacet::Save( HFILE hf )
 {
   if( _lwrite( hf, &vecOutNormal, sizeof(TVector) ) == HFILE_ERROR ) return -1;
   if( _lwrite( hf, &iSmothGroup, sizeof(int) ) == HFILE_ERROR ) return -1;

   int iNumAdges = listAgesItem.getItemsInContainer();
   if( _lwrite( hf, &iNumAdges, sizeof(int) ) == HFILE_ERROR ) return -1;

   DoubleListIterator dbIt = DoubleListIterator( listAgesItem, FALSE );
   PTAdgeItem pIt = (PTAdgeItem)&dbIt.current();
   for( ; int(dbIt); pIt = (PTAdgeItem)&--dbIt )
    {
      int ind = pIt->pAdge->Index();
      if( _lwrite( hf, &(pIt->clockVise), sizeof(int) ) == HFILE_ERROR ) return -1;
      if( _lwrite( hf, &ind, sizeof(int) ) == HFILE_ERROR ) return -1;
      //if( pIt->pAdge->Save(hf) == -1 ) return -1;
    }

   return 0;
 }
