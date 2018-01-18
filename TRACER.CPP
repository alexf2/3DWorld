//#pragma option -v-

#include "threshol.hpp"
#include "tracer.hpp"
#include "scena.hpp"
#include "log.hpp"

#include <stdlib.h>
#include <search.h>
#include <bwcc.h>
#include <memmgr.h>
#include <applicat.h>
#include <stdio.h>
#include "INC\classtyp.inc"

#pragma option -G

extern HINSTANCE hInstRC;

static int TestVisibleRayTraceInt( RTFacet rF )
 {
   return !(rF.vecOutNormal.z < TRACER_d_ZOutNormalVisibleInt);
 }

static int TestVisibleRayTraceReal( RTFacet rF )
 {
   return !(rF.vecOutNormal.z < TRACER_d_ZOutNormalVisibleReal);
 }

static int TestVisibleStrScan( RTFacet rF )
 {
   return !(rF.vecOutNormal.z > TRACER_d_ZOutNormalVisibleStrScan);
 }

static int TestHoriz( RTAdge rA )
 {
   return fabs(rA.pV1->y - rA.pV2->y) < TFACET_d_HorizontalAdge;
 }

static int TestHorizInt( RTAdge rA )
 {
   return int(rA.pV1->y) == int(rA.pV2->y);
 }


typedef int (*PTTestVisibleFunc)( RTFacet );
typedef int (*PTTestHorizFunc)( RTAdge );
struct TRemoveDta
 {
   PTTestVisibleFunc pF;
   PTTestHorizFunc pAF;
   RTIndexedDoubleList rListFac;
   PTIndexedDoubleList pListAdgItem;

   TRemoveDta( RTIndexedDoubleList rLi, PTTestVisibleFunc pF_, PTTestHorizFunc pAF_ ):
     rListFac( rLi )
    {
      pF = pF_; pAF = pAF_;
    }
 };
typedef TRemoveDta* PTRemoveDta;
typedef TRemoveDta& RTRemoveDta;


static void RemoveHorizAdgesInst( Object _FAR& rObj, void _FAR* pDta )
 {   
   if( (PTRemoveDta(pDta)->pAF)( *RTAdgeItem(rObj).pAdge ) == TRUE )
     PTRemoveDta(pDta)->pListAdgItem->detach( rObj, TShouldDelete::Delete );
 }

static void RemoveBackFacesInstance2( Object _FAR& rObj, void _FAR* pDta )
 {
   PTRemoveDta(pDta)->pListAdgItem = &RTFacet(rObj).listAgesItem;
						    
   if( (PTRemoveDta(pDta)->pF)( RTFacet(rObj) ) == FALSE )
     PTRemoveDta(pDta)->rListFac.detach( rObj, TShouldDelete::Delete );
   else RTFacet(rObj).listAgesItem.forEach( RemoveHorizAdgesInst, pDta );
 }

static void RemoveBackFacesInstance( Object _FAR& rObj, void _FAR* pDta )
 {
   TRemoveDta dta( RTSurface(rObj).listFacets, PTRemoveDta(pDta)->pF, PTRemoveDta(pDta)->pAF );

   if( RTSurface(rObj).IsTransparency() == FALSE )
     RTSurface(rObj).listFacets.forEach( RemoveBackFacesInstance2, Pvoid(&dta) );
 }

static void near pascal RemoveBackFacesAndHorizAdgesItems( RTIndexedDoubleList rListSurf, PTTestVisibleFunc pFTst, PTTestHorizFunc pAF_ )
 {
   TRemoveDta dta( RTIndexedDoubleList(Object::ZERO), pFTst, pAF_ );
   rListSurf.forEach( RemoveBackFacesInstance, Pvoid(&dta) );
 }



static void RemoveHorizAdgesItemsInst3( Object _FAR& rObj, void _FAR* pDta )
 {   
   if( (PTRemoveDta(pDta)->pAF)( *RTAdgeItem(rObj).pAdge ) == TRUE )
     PTRemoveDta(pDta)->pListAdgItem->detach( rObj, TShouldDelete::Delete );
 }

static void RemoveHorizAdgesItemsInst2( Object _FAR& rObj, void _FAR* pDta )
 {
   PTRemoveDta(pDta)->pListAdgItem = &RTFacet(rObj).listAgesItem;
						    
   RTFacet(rObj).listAgesItem.forEach( RemoveHorizAdgesItemsInst3, pDta );
 }

static void RemoveHorizAdgesItemsInst( Object _FAR& rObj, void _FAR* pDta )
 {
   TRemoveDta dta( RTSurface(rObj).listFacets, NULL, PTRemoveDta(pDta)->pAF );

   RTSurface(rObj).listFacets.forEach( RemoveHorizAdgesItemsInst2, Pvoid(&dta) );
 }

static void near pascal RemoveHorizAdgesItems( RTIndexedDoubleList rListSurf, PTTestHorizFunc pAF_ )
 {
   TRemoveDta dta( RTIndexedDoubleList(Object::ZERO), NULL, pAF_ );
   rListSurf.forEach( RemoveHorizAdgesItemsInst, Pvoid(&dta) );
 }




void near pascal TIntersectBuf::GrowBuf()
 {
   int iNewSize = iSize + iDelta;
   PTTracData pTmpInitBuf = new TTracData[ iNewSize ];
   if( !pTmpInitBuf )
    {
      intBuf.FreeRc(); iSize = 0; return;
    }

   PTTracData* pTmpInitBufP = new PTTracData[ iNewSize ];
   if( !pTmpInitBufP )
    {
      delete pTmpInitBuf; iSize = 0;
      intBufP.FreeRc(); return;
    }

   if( iCurSize > 0 )
    {
      _fmemcpy( pTmpInitBuf, intBuf(), sizeof(TTracData) * iCurSize );
      _fmemcpy( pTmpInitBufP, intBufP(), sizeof(PTTracData) * iCurSize );
    }

   intBuf = pTmpInitBuf;
   intBufP = pTmpInitBufP;
   iSize = iNewSize;
 }

TIntersectBuf::TIntersectBuf( int iInitSize, int iDelta_ )
 {
   iSize = iInitSize;
   iDelta = iDelta_;
   iCurSize = 0; iIter = 0; bFlDir = TRUE;

   intBuf  = new TTracData[ iSize ];
   intBufP = new PTTracData[ iSize ];

   if( !intBuf || !intBufP ) iSize = 0;
 }

int near pascal TIntersectBuf::AddIntersect( PTSurface  psfSurf, PTFacet pFac, double z )
 {
   if( !(iCurSize < iSize) )
    {
      GrowBuf();
      if( Status() == FALSE ) return FALSE;
    }

   intBuf()[ iCurSize++ ] = TTracData( psfSurf, pFac, z );

   return TRUE;
 }

static int CompareIntBuf( const void *p1, const void *p2 )
 {
   PTTracData* pt1 = (PTTracData*)p1;
   PTTracData* pt2 = (PTTracData*)p2;

   if( (*pt1)->z < (*pt2)->z ) return -1;
   else if( (*pt1)->z > (*pt2)->z ) return 1;
   else return 0;
 }

static int CompareIntBufRev( const void *p1, const void *p2 )
 {
   PTTracData* pt1 = (PTTracData*)p1;
   PTTracData* pt2 = (PTTracData*)p2;

   if( (*pt1)->z < (*pt2)->z ) return 1;
   else if( (*pt1)->z > (*pt2)->z ) return -1;
   else return 0;
 }


void near pascal TIntersectBuf::Normalyze()
 {
   if( iCurSize < 1 ) return;

   for( int i = 0; i < iCurSize; i++ )
     intBufP()[ i ] = intBuf() + i;


   if( bFlDir == TRUE )
     qsort( intBufP(), iCurSize, sizeof(PTTracData*), CompareIntBuf );
   else
     qsort( intBufP(), iCurSize, sizeof(PTTracData*), CompareIntBufRev );
 }

PTTracData near pascal TIntersectBuf::Get( int ind ) const
 {
   if( ind < iCurSize ) return intBufP()[ ind ];
   else return NULL;
 }

int near pascal TIntersectBuf::BottomIndex() const
 {
   for( int i = 0; i < iCurSize && intBufP()[ i ]->psfSurf->IsTransparency() == TRUE; i++ );

   return i;
 }

PTTracData near pascal TIntersectBuf::Next()
 {
   if( iIter + 1 < iCurSize ) return intBufP()[ ++iIter ];
   else return NULL;
 }

PTTracData near pascal TIntersectBuf::Prev()
 {
   if( iIter > 0 ) return intBufP()[ --iIter ];
   else return NULL;
 }




int near pascal TScena::FindIntersectReal( double& dZ, double clm,
					   double str, PTFacet pFacet,
					   DoubleList& rdblRemove,
					   BOOL bFlRemove
                                         )
 {

INTERSECT_RESTART1:
   int intersectCount = 0;
   DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem );
   PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();

   for( ; int(dbAdges); pAge = (PTAdgeItem)&++dbAdges )
    {
      PTAdge pA = pAge->pAdge;

      if( str > pA->pV1->y ) break;

      if( str < pA->pV2->y )
       {
	 if( bFlRemove == FALSE ) continue;

	 pFacet->listAgesItem.detach( *pAge, TShouldDelete::NoDelete );
	 rdblRemove.add( *pAge );
	 goto INTERSECT_RESTART1;
       }

      float x1 = pA->pV1->x, x2 = pA->pV2->x,
	    y1 = pA->pV1->y, y2 = pA->pV2->y;

      if( str == y1 )
       {
	 if( x1 >= clm ) intersectCount++;
	 continue;	 
       }
      else if( str == y2 ) continue;
      else
       {
	 if( pAge->dStrScan != str )
	  {
	    pAge->dStrScan = str;
	    double t = (str - y1) / (y2 - y1);
	    pAge->dTestParam = x1 + t * (x2 - x1);
	    pAge->bTestParam = t > 0 && t < 1;
	  }

	 if( pAge->bTestParam && pAge->dTestParam >= clm ) intersectCount++;
       }
    }

   if( !(intersectCount & 1) ) return -1;

   dZ = -( (pFacet->d + pFacet->vecOutNormal.x * clm + pFacet->vecOutNormal.y * str ) /
	   pFacet->vecOutNormal.z );

   return 0;
    

   
   /*int intersectCount = 0;

   DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem, TRUE );
   PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();


   for( ; int(dbAdges); pAge = (PTAdgeItem)&++dbAdges )
    {
      PTAdge pA = pAge->pAdge;
      double x1 = pA->pV1->x, x2 = pA->pV2->x,
	     y1 = pA->pV1->y, y2 = pA->pV2->y;

      if( y1 == y2 ) continue;

      if( (str < y1 && str < y2) ||
	  (str > y1 && str > y2)
	) continue;

      if( (clm <= x1 && str == y1 && y1 > y2) ||
	  (clm <= x2 && str == y2 && y2 > y1)
	) continue;

      double xt = x1 + (y1 - str) * ( (x2 - x1) / (y1 - y2) );


      if( clm > xt ) continue;

      intersectCount++;
    }    


   if( !(intersectCount % 2) ) return -1;

   dZ = -( (pFacet->d + pFacet->vecOutNormal.x * clm + pFacet->vecOutNormal.y * str ) /
	   pFacet->vecOutNormal.z );

   return 0;*/

 }


int near pascal TScena::FindIntersect( double& dZ, double clm_,
				       double str_, PTFacet pFacet,
				       DoubleList& rdblRemove,
				       BOOL bFlRemove
				     )
 {
   int clm = clm_, str = str_;

INTERSECT_RESTART1:
   int intersectCount = 0;
   DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem );
   PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();

   for( ; int(dbAdges); pAge = (PTAdgeItem)&++dbAdges )
    {
      PTAdge pA = pAge->pAdge;     

      if( str > int(pA->pV1->y) ) break;

      if( str < int(pA->pV2->y) )
       {
	 if( bFlRemove == FALSE ) continue;

	 pFacet->listAgesItem.detach( *pAge, TShouldDelete::NoDelete );
	 rdblRemove.add( *pAge );
	 goto INTERSECT_RESTART1;
       }

      int x1 = pA->pV1->x, x2 = pA->pV2->x,
	  y1 = pA->pV1->y, y2 = pA->pV2->y;

      if( str == y1 )
       {
	 if( x1 >= clm ) intersectCount++;
	 continue;
       }
      else if( str == y2 ) continue;
      else
       {
	 if( int(pAge->dStrScan) != str )
	  {
	    pAge->dStrScan = str;
	    double t = float(str - y1) / float(y2 - y1);
	    pAge->dTestParam = float(x1) + t * float(x2 - x1);
	    pAge->bTestParam = t > 0.0 && t < 1.0;
	  }

	 if( pAge->bTestParam && pAge->dTestParam >= clm ) intersectCount++;
       }
    }

   if( !(intersectCount & 1) ) return -1;

   dZ = -( (pFacet->d + pFacet->vecOutNormal.x * float(clm) + pFacet->vecOutNormal.y * float(str) ) /
	   pFacet->vecOutNormal.z );

   return 0;


   /*int intersectCount = 0;

   DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem, TRUE );
   PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();
   //PTAdgeItem pA1 = pAge;

   for( ; int(dbAdges); pAge = (PTAdgeItem)&++dbAdges )
    {
      PTAdge pA = pAge->pAdge;
      int x1 = pA->pV1->x, x2 = pA->pV2->x,
          y1 = pA->pV1->y, y2 = pA->pV2->y;

      if( y1 == y2 ) continue;

      if( (str < y1 && str < y2) ||
	  (str > y1 && str > y2)
	) continue;

      if( (clm <= x1 && str == y1 && y1 > y2) ||
	  (clm <= x2 && str == y2 && y2 > y1)
	) continue;

      double xt = double(x1) + double(y1 - str) * ( double(x2 - x1) / double(y1 - y2) );

      if( double(clm) > xt ) continue;

      intersectCount++;
    }    


   if( !(intersectCount % 2) ) return -1;



   dZ = -( (pFacet->d + pFacet->vecOutNormal.x * double(clm) + pFacet->vecOutNormal.y * double(str) ) /
	   pFacet->vecOutNormal.z );

   return 0;*/

   /*int intersectCount = 0;
   BOOL bFlAdge = FALSE;


   DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem, FALSE );
   PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();
   double d = -pFacet->vecOutNormal.x * pAge->pAdge->pV1->x -
	       pFacet->vecOutNormal.y * pAge->pAdge->pV1->y -
	       pFacet->vecOutNormal.z * pAge->pAdge->pV1->z;

   for( ; int(dbAdges); pAge = (PTAdgeItem)&--dbAdges )
    {
      PTAdge pA = pAge->pAdge;
      double xIntersect;

      if( str == pA->pV1->y )
       {
	 if( str == pA->pV2->y )
	  {
	    if( clm < pA->pV1->x && clm < pA->pV2->x ) continue;
	    else if( clm > pA->pV1->x && clm > pA->pV2->x ) continue;
	    else
	     {
	       bFlAdge = TRUE; break;
             }
	  }
	 else if( clm == pA->pV1->x )
	  {
	    bFlAdge = TRUE; break;
	  }
	 else if( clm < pA->pV1->x )
	  {
	    if(  pA->pV2->y > pA->pV1->y )
	     {
	       intersectCount++; continue;
	     }
	    else continue;
	  }
         else continue;
       }
      else if( str == pA->pV2->y )
       {
	 if( clm == pA->pV2->x )
	  {
	    bFlAdge = TRUE; break;
	  }
	 else if( clm < pA->pV2->x )
	  {
	    if( pA->pV1->y > pA->pV2->y )
	     {
	       intersectCount++; continue;
	     }
	    else continue;
	  }
         else continue;
       }
      else if( str < pA->pV1->y && str < pA->pV2->y ) continue;
      else if( str > pA->pV1->y && str > pA->pV2->y ) continue;
      else
       {
         
	 xIntersect = pA->pV1->x + (pA->pV1->y - double(str)) *
	   ( (pA->pV2->x - pA->pV1->x)/(pA->pV1->y - pA->pV2->y) );   
      

	 if( xIntersect == clm )
	  {
	    bFlAdge = TRUE; break;
	  }
	 else if( xIntersect < clm ) continue;	
	 else
	  {
	    intersectCount++; continue;
          }

       }

    }       

   if( !(intersectCount % 2) && bFlAdge == FALSE ) return -1;

   if( fabs(pFacet->vecOutNormal.z) < 0.005  )  return -1;
   dZ = -( (d + pFacet->vecOutNormal.x * double(clm) + pFacet->vecOutNormal.y * double(str) ) /
	   pFacet->vecOutNormal.z );
     

   return 0;*/
 }

#pragma option -G.


RECT TScena::GetFormatImage( double& dScale, int& iSx, int& iSy ) const
 {
   RECT r; _fsetmem( &r, sizeof(RECT), 0 );

   switch( ifFormat )
    {
      case IF_Windowed:
	GetClientRect( pWin->HWindow, &r );
	while( (r.right % 4) )  r.right++;
	break;

      case IF_FullScreen:        
	r.right = GetSystemMetrics( SM_CXSCREEN );
	r.bottom = GetSystemMetrics( SM_CYSCREEN );
	while( r.right % 4 ) r.right++;
	break;

      case IF_640x480:
	r.right = 640;
	r.bottom = 480;
	break;

      case IF_800x600:
	r.right = 800;
	r.bottom = 600;
	break;

      case IF_1024x768:
	r.right = 1024;
	r.bottom = 768;
	break;
    }

   if( ifFormat != IF_Windowed )
    {
      RECT rW; GetClientRect( pWin->HWindow, &rW );
      if( rW.right && rW.bottom )
       {
	 double dH = double(r.right) / double(rW.right),
		dV = double(r.bottom) / double(rW.bottom);
	 dScale = min( dH, dV );

	 rW.right = double(rW.right) * dScale;
	 rW.bottom = double(rW.bottom) * dScale;
	 iSx = double(r.right - rW.right) / 2.0;
	 iSy = double(r.bottom - rW.bottom) / 2.0;	 
       }
      else
       {
         dScale = 1;
         iSx = iSy = 0;
       }
    }
   else
    {
      dScale = 1;
      iSx = iSy = 0;
    }   

   return r;
 }

static void near pascal ScaleLights( RTIndexedDoubleList listSurf, double dSc )
 {
   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {   
      if( pSerf->isA() == TYP_TOMNI )
       {
	 PTOmni pOmni = (PTOmni)(pSerf->TopPtr());
	 pOmni->Mult( pOmni->GetMult() * dSc);
       }
    }

 }

void TScena::MakeCompatibleFormatImage( double dScale, double iSx, double iSy )
 {
   if( dScale == 1 && !iSx && !iSy ) return;

   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {

      DoubleListIterator dbV = DoubleListIterator( pSerf->listVertex );
      PTVertex  pVert = (PTVertex)&dbV.current();

      for( ; int(dbV); pVert = (PTVertex)&++dbV )
       {
	 pVert->x *= dScale; pVert->y *= dScale; pVert->z *= dScale;
	 pVert->x += iSx; pVert->y -= iSy;
       }

      if( pSerf->isA() == TYP_TOMNI )
       {
	 PTOmni pOmni = (PTOmni)(pSerf->TopPtr());
	 pOmni->Mult( pOmni->GetMult() * dScale );
       }
    } 

 }

void TScena::SpecialPointLPtoDP( double& x, double& y, int w, int h, double dSc ) const
 {
   double dShx, dShy;
   pUcs->GetShift( dShx, dShy );
   x = x + double(w)/2.0 + dShx * dSc;
   y = -y + double(h)/2.0 + dShy * dSc;
 }

void TScena::SpecialPointDPtoLP( double& x, double& y, int w, int h, double dSc ) const
 {
   double dShx, dShy;
   pUcs->GetShift( dShx, dShy );
   x = x - double(w)/2.0 - dShx * dSc;
   y = double(h)/2.0 + dShy * dSc - y;
 }

void TScena::SpecialBounds( double& x, double& y, RECT r ) const
 {
   if( x < 0 ) x = 0;
   if( y < 0 ) y = 0;
   if( x > double(r.right - 1) ) x = r.right - 1;
   if( y > double(r.bottom - 1) ) y = r.bottom - 1;
 }

#pragma option -G
PTDIB TScena::Render( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog )
 {
   if( polyMethod == PM_StringScan )
     return RenderStrScan( pInd, rpPctTbl, rTLog );
   

   PTUCS pucsKey = pUcs;
   HDC hdcKey = hdc;
   PTWindow pwinKey = pWin;

   double dSc;
   int iSx, iSy;
   RECT r = GetFormatImage( dSc, iSx, iSy );

   TTracer  tr = TTracer( r, hdc, rgbAdges,
     (bFlMarkIntersect == TRUE) ? rgbIntersect:rgbAdges );

   if( tr.Status )
    {
      BWCCMessageBox( pWin->HWindow, "Not enought memory for create bitmap",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
      
      return NULL;
    }


   pInd->AddStr( "Transforming objects" );
   pInd->Show();

   /*pUcs = pucsKey;
   hdc = hdcKey;
   pWin = pwinKey;*/


   rTLog.StartCopy();

   PTScena  pScenaRender = new TScena( *this, pInd, TRUE, FALSE );
   //pInd->Init( pScenaRender->GetBytesSize() );
   if( !pScenaRender )
    {
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == -1 )
     {
       delete pScenaRender;
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == 1 )
     {
       delete pScenaRender;
       

       return NULL;
     }
   rTLog.EndCopy();

   rTLog.StartTransform();

   TIndexedDoubleList dblLights, dblCameras;
   pScenaRender->Separate( dblLights, dblCameras );
   dblLights.flush( TShouldDelete::Delete );
   dblCameras.flush( TShouldDelete::Delete );

   pScenaRender->pUcs = pucsKey;
   pScenaRender->hdc = hdcKey;
   pScenaRender->pWin = pwinKey;

   pScenaRender->RecalcUserToBase();
   double dSx, dSy;
   pScenaRender->pUcs->GetShift( dSx, dSy );
   //pScenaRender->MakeCompatibleFormatImage( dSc, iSx, iSy );
   pScenaRender->MakeCompatibleFormatImage( dSc, 0, 0 );
     
   //pScenaRender->RecalcLPtoDP();

   //pScenaRender->MakeCompatibleFormatImage( dSc, iSx, iSy );

   pInd->UpdateStr( 2, "Calc normals" );
   pInd->Init( pScenaRender->listSurf.getItemsInContainer() );

   DoubleListIterator  dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();
   for( LONG lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )  
     {       
       pSerf->CalcNormals();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  //GetSwap( pInd );
	  
	  delete pScenaRender;
	  //pInd->Close();
          return  NULL;
        }
     }

   pInd->WorkIs100();
   


   pInd->UpdateStr( 2, "Calc bounds" );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );

   dbIter = DoubleListIterator( pScenaRender->listSurf );
   pSerf = (PTSurface)&dbIter.current();
   for( lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     {
       pSerf->CalcBounds();      
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}

       pSerf->CalcBoundsAllFacets();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
	  return  NULL;
	}
     }

   //pScenaRender->TestToFileBounds( "g:\\sort1.txt" );

   pInd->WorkIs100();

   rTLog.EndTransform();
   rTLog.StartOptimize();

   pInd->UpdateStr( 2, "Optimize body data..." );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );
   //pScenaRender->listSurf.Sort( (PTCompareFunc)&TSurface::CompareY, pInd );
   pScenaRender->listSurf.Sort( CompareSurfaceYRevInt, pInd );
   //pScenaRender->SortFacets( pInd );
   pScenaRender->SortFacets( CompareFacetYRevInt, pInd );
   pScenaRender->SortAdges( CompareAdgeItemYRevInt, pInd );

   if( pScenaRender->ListAdgeNormalyzeRevInt( pInd ) == FALSE )
    {
      delete pScenaRender;
      return  NULL;
    }

   pScenaRender->CalcBounds();

   rTLog.EndOptimize();

   double dXStaBmp, dYStaBmp, dXEndBmp, dYEndBmp;
   dXStaBmp = pScenaRender->dMinX;
   dYStaBmp = pScenaRender->dMaxY;
   pScenaRender->SpecialPointLPtoDP( dXStaBmp, dYStaBmp, r.right, r.bottom, dSc );		
   dXStaBmp--; dYStaBmp--;
   pScenaRender->SpecialBounds( dXStaBmp, dYStaBmp, r );
   dXStaBmp = floor( dXStaBmp );
   dYStaBmp = ceil( dYStaBmp );

   dXEndBmp = pScenaRender->dMaxX;
   dYEndBmp = pScenaRender->dMinY;
   pScenaRender->SpecialPointLPtoDP( dXEndBmp, dYEndBmp, r.right, r.bottom, dSc );
   dXEndBmp++; dYEndBmp++;
   pScenaRender->SpecialBounds( dXEndBmp, dYEndBmp, r );
   dXEndBmp = ceil( dXEndBmp );
   dYEndBmp = floor( dYEndBmp );

   double dXSta = dXStaBmp, dYSta = dYStaBmp,
	  dXEnd = dXEndBmp, dYEnd = dYEndBmp;	   
   pScenaRender->SpecialPointDPtoLP( dXSta, dYSta, r.right, r.bottom, dSc );
   pScenaRender->SpecialPointDPtoLP( dXEnd, dYEnd, r.right, r.bottom, dSc );

   RECT rTrBounds;
   rTrBounds.left = dXStaBmp; rTrBounds.right = dXEndBmp;
   rTrBounds.top = dYStaBmp; rTrBounds.bottom = dYEndBmp;

   tr.Bounds( rTrBounds );

   rTLog.StartInitBkGnd();

   pInd->UpdateStr( 2, "Init backgraund..." );
   tr.Start( mrcBackGraund(), pInd );

   rTLog.EndInitBkGnd();

   pInd->UpdateStr( 2, "Render..." );
   //pInd->Init( LONG(r.bottom) * LONG(r.right) );
   pInd->Init(
		 labs( (LONG)rTrBounds.bottom - (LONG)rTrBounds.top + 1L ) *
		 labs( (LONG)rTrBounds.right - (LONG)rTrBounds.left + 1L )
	     );



   PTSurface pNearstSurf = NULL;
   PTFacet pNearstFac = NULL;
   double z = INT_MIN;
   LONG  lWorking = 1;

   rTLog.StartPrepareRender();

   RemoveBackFacesAndHorizAdgesItems( pScenaRender->listSurf, TestVisibleRayTraceInt, TestHorizInt );
   DoubleList dblRemove;

   pScenaRender->InitStrScanParamOfAdgeItems( dYSta + 100 );

   rTLog.EndPrepareRender();
   rTLog.StartRender();

   //BOOL bFlTraced = FALSE;
   int iEY = dYEndBmp, iEX = dXEndBmp;
   if( dYStaBmp != iEY && dXStaBmp != iEX )
    {
      for( int str = dYStaBmp, y_ = dYSta; str <= iEY; str++, y_-- )
       {

	 for( int clm = dXStaBmp, x_ = dXSta; clm <= iEX; clm++, lWorking++, x_++ )
	  {
	    //bFlTraced = TRUE;

	    pNearstSurf = NULL;
	    pNearstFac = NULL;
	    z = INT_MIN;

RENDER_RESTART1:
	    dbIter = DoubleListIterator( pScenaRender->listSurf );
	    PTSurface  pSerf = (PTSurface)&dbIter.current();
	    for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
	     {	  
	       if( y_ > pSerf->iMaxY ) break;
	       
	       if( y_ < pSerf->iMinY )
                {
		  pScenaRender->listSurf.detach( *pSerf, TShouldDelete::NoDelete );
	          dblRemove.add( *pSerf );
		  goto RENDER_RESTART1;
                }

	       if( x_ < pSerf->iMinX || x_ > pSerf->iMaxX ) continue;

RENDER_RESTART2:
	       DoubleListIterator dbFacet = DoubleListIterator( pSerf->listFacets );
	       PTFacet  pFacet = (PTFacet)&dbFacet.current();
	       for( ; int(dbFacet); pFacet = (PTFacet)&++dbFacet )
		{		  
		  //if( pFacet->vecOutNormal.z < TRACER_d_VectorZOutNormalVisibleInt ) continue;

		  if( y_ > pFacet->iMaxY ) break;
		  
		  if( y_ < pFacet->iMinY )
		   {
		     pSerf->listFacets.detach( *pFacet, TShouldDelete::NoDelete );
		     dblRemove.add( *pFacet );
		     goto RENDER_RESTART2;
                   }

		  if( x_ < pFacet->iMinX || x_ > pFacet->iMaxX ) continue;

		  double dZ = 0;
		  int res = FindIntersect( dZ, x_, y_, pFacet, dblRemove, TRUE );
		  if( res != -1 && dZ > z )
		   {
		     z = dZ; pNearstSurf = pSerf; pNearstFac = pFacet;
		   }
		}
	     }
	   tr.Pixel( clm, str, pNearstSurf, pNearstFac, z );

	   if( pInd->WorkMessage( lWorking ) == TRUE )
	    {
	      //GetSwap( pInd );

	      delete pScenaRender;
	      //pInd->Close();
	      return NULL;
	    }
	  }

       }//for
      tr.End();      

    }


   rTLog.EndRender();

   dblRemove.flush( TShouldDelete::Delete );
   pInd->WorkIs100();

   TNoisyDta ndDta;   pScenaRender->GetNoisy( ndDta );
   delete pScenaRender;

   //PTColorTable pctColorTbl = pScenaRender->CreateColorTbl();

	       /*TTracerPhoto  trPhoto = TTracerPhoto( r.right, r.bottom );//(((((((((((((((
	       trPhoto.IntBuf( tr.Buffer() );
	       trPhoto.ConvertToRGB( tr.Buffer(), bFlStrictGamma );
	       trPhoto.Free();*/

   if( !iPaletteSize )
    {
      rpPctTbl = NULL;

      return new TDIB( tr.Buffer(), r.right, r.bottom, NULL, 0, TRUE, pApplicat );
      //return tr.CreateGetBitmap( NULL, 0 );
    }


   int iPS = iPaletteSize ? ((iPaletteSize == -16) ? 16:iPaletteSize):236;
   TConvertAlho alho = (iPaletteSize == -16) ? CA_System:caAlhoPalette;

   rTLog.StartCalcPalette();

   if( iPS )
    {     
      pInd->SetTitle( "Convert image to indexed" );
      rpPctTbl = TrueColorTo256( tr.Buffer(), tr.SizeBuf(), iPS,
	iPaletteSize ? FALSE:TRUE, alho, r.right, r.bottom,
	&ndDta,
	pInd );
    }
   else rpPctTbl = NULL;

   rTLog.EndCalcPalette();

   PTDIB pRetDIB = new TDIB( tr.Buffer(), r.right, r.bottom,
     iPaletteSize ? rpPctTbl:NULL,
     (iPaletteSize == -16) ? 16:iPaletteSize,
     TRUE, pApplicat );
   //HGLOBAL hRetDIB = tr.CreateGetBitmap( iPaletteSize ? rpPctTbl:NULL,
     //(iPaletteSize == -16) ? 16:iPaletteSize );
   if( rpPctTbl && iPaletteSize )
    {
      delete []rpPctTbl; rpPctTbl = NULL;
    }

   return pRetDIB;
 }

static double near pascal MinY__( RTAdge rA )
 {
   return (rA.pV1->y < rA.pV2->y) ? rA.pV1->y:rA.pV2->y;
 }

static double near pascal MaxY__( RTAdge rA )
 {
   return (rA.pV1->y > rA.pV2->y) ? rA.pV1->y:rA.pV2->y;
 }

static double near pascal IntersectX( double x1, double y1, double x2, double y2, double y )
 {
   return (y - y1) / (y2 - y1) * (x2 - x1) + x1;
 }

static int near pascal Level( RTAdge rA, PTVertex pV )
 {
   double dMin = MinY__( rA );
   return dMin != pV->y;
 }

static int near pascal MinY__Int( RTAdge rA )
 {
   return (int(rA.pV1->y) < int(rA.pV2->y)) ? rA.pV1->y:rA.pV2->y;
 }


static int near pascal LevelInt( RTAdge rA, PTVertex pV )
 {
   int dMin = MinY__Int( rA );
   return dMin != int(pV->y);
 }


static int CmpIntersectInt( const void *d1, const void *d2 )
 {
   if( *(int*)d1 < *(int*)d2 ) return -1;
   else if( *(int*)d1 > *(int*)d2 ) return 1;
   else return 0;
 }


static int CmpIntersect( const void *d1, const void *d2 )
 {
   if( *(double*)d1 < *(double*)d2 ) return -1;
   else if( *(double*)d1 > *(double*)d2 ) return 1;   
   else return 0;
 }

static RTAdgeItem near pascal GetIncidentAdge( RTIndexedDoubleList rList,
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
 }



/*PTDIB TScena::RenderStrScan( PTIndicator pInd, PTColorTable& rpPctTbl )
 {
   PTUCS pucsKey = pUcs;
   HDC hdcKey = hdc;
   PTWindow pwinKey = pWin;   


   double dSc;
   int iSx, iSy;
   RECT r = GetFormatImage( dSc, iSx, iSy );

   TTracer  tr = TTracer( r, hdc, rgbAdges,
     (bFlMarkIntersect == TRUE) ? rgbIntersect:rgbAdges );

   if( tr.Status )
    {
      BWCCMessageBox( pWin->HWindow, "Not enought memory for create bitmap",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
      
      return NULL;
    }


   pInd->AddStr( "Transforming objects" );
   pInd->Show();


   PTScena  pScenaRender = new TScena( *this, pInd, TRUE, FALSE );
   //pInd->Init( pScenaRender->GetBytesSize() );
   if( !pScenaRender )
    {
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == -1 )
     {
       delete pScenaRender;
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == 1 )
     {
       delete pScenaRender;
       

       return NULL;
     }

   pScenaRender->pUcs = pucsKey;
   pScenaRender->hdc = hdcKey;
   pScenaRender->pWin = pwinKey;

   pScenaRender->RecalcUserToBase();

   TIndexedDoubleList dblLights, dblCameras;
   pScenaRender->Separate( dblLights, dblCameras );
   //dblLights.flush( TShouldDelete::Delete );
   //dblCameras.flush( TShouldDelete::Delete );
     

   pInd->UpdateStr( 2, "Calc normals" );
   pInd->Init( pScenaRender->listSurf.getItemsInContainer() );

   DoubleListIterator  dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();
   for( LONG lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )  
     {       
       pSerf->CalcNormals();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  //GetSwap( pInd );
	  
	  delete pScenaRender;
	  //pInd->Close();
          return  NULL;
        }
     }

   pInd->WorkIs100();

   pInd->UpdateStr( 2, "Calc bounds" );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );

   dbIter = DoubleListIterator( pScenaRender->listSurf );
   pSerf = (PTSurface)&dbIter.current();
   for( lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     {
       pSerf->CalcBounds();      
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}

       pSerf->CalcBoundsAllFacets();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}
     }

   //pScenaRender->TestToFileBounds( "g:\\sort1.txt" );

   pInd->WorkIs100();

   pInd->UpdateStr( 2, "Optimize body data..." );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );
   pScenaRender->listSurf.Sort( (PTCompareFunc)&TSurface::CompareY, pInd );
   pScenaRender->SortSurfFacAdg( pInd );

   pScenaRender->CalcBounds();

   //pScenaRender->TestToFileBounds( "g:\\sort2.txt" );


   double dXStaBmp, dYStaBmp, dXEndBmp, dYEndBmp;
   dXStaBmp = (pScenaRender->dMinX - 1) * dSc;
   dYStaBmp = (pScenaRender->dMaxY + 1) * dSc;
   pScenaRender->SpecialPointLPtoDP( dXStaBmp, dYStaBmp, r.right, r.bottom, dSc );		
   dXStaBmp--; dYStaBmp--;
   pScenaRender->SpecialBounds( dXStaBmp, dYStaBmp, r );
   dXStaBmp = floor( dXStaBmp );
   dYStaBmp = ceil( dYStaBmp );

   dXEndBmp = (pScenaRender->dMaxX + 1)* dSc;
   dYEndBmp = (pScenaRender->dMinY - 1) * dSc;
   pScenaRender->SpecialPointLPtoDP( dXEndBmp, dYEndBmp, r.right, r.bottom, dSc );
   dXEndBmp++; dYEndBmp++;
   pScenaRender->SpecialBounds( dXEndBmp, dYEndBmp, r );
   dXEndBmp = ceil( dXEndBmp );
   dYEndBmp = floor( dYEndBmp );

   double dXSta = dXStaBmp, dYSta = dYStaBmp,
	  dXEnd = dXEndBmp, dYEnd = dYEndBmp;	   
   pScenaRender->SpecialPointDPtoLP( dXSta, dYSta, r.right, r.bottom, dSc );
   pScenaRender->SpecialPointDPtoLP( dXEnd, dYEnd, r.right, r.bottom, dSc );
   dXSta /= dSc; dYSta /= dSc;
   dXEnd /= dSc; dYEnd /= dSc;

   double dPix = (fabs(dXEnd - dXSta) + 1) / (fabs(dXEndBmp - dXStaBmp) + 1);
   double dHalfPix = dPix /  2;
   //double x_ = dXSta + dHalfPix, y_ = dYSta - dHalfPix;
   double y_ = dYEnd + dHalfPix;
      

   RECT rTrBounds;
   rTrBounds.left = dXStaBmp; rTrBounds.right = dXEndBmp;
   rTrBounds.top = dYStaBmp; rTrBounds.bottom = dYEndBmp;

   tr.Bounds( rTrBounds );

   pInd->UpdateStr( 2, "Init backgraund..." );
   tr.Start( mrcBackGraund(), pInd );

   pInd->UpdateStr( 2, "Render..." );
   pInd->Init( labs((LONG)rTrBounds.bottom - (LONG)rTrBounds.top + 1L) );

   		
   TAutoMemoryRc<double*> mrcIntersect = new double[ 2 * pScenaRender->GetMaxAdges() ];
   int iCountIntersect;
   LONG  lWorking = 1;

   DoubleList dblRemovedParts;
   //DoubleList dblActSurf, dblActFac, dblAc

   int iEY = dYEndBmp, iEX = dXEndBmp;
   if( dYStaBmp != iEY && dXStaBmp != iEX )
    {
      for( int str = iEY; str >= dYStaBmp; str--, y_ += dPix )
       {
	 BOOL bFlStrEmpty = TRUE;
	 

	 DoubleListIterator dbiSurf( pScenaRender->listSurf );
	 PTSurface pSf = (PTSurface)&dbiSurf.current();
	 for( ; int(dbiSurf); pSf = (PTSurface)&++dbiSurf )
	  {
	    if( pSf->minY > y_  ) break;

	    if( pSf->maxY < y_  )
	     {
	       pScenaRender->listSurf.detach( *pSf, TShouldDelete::NoDelete );
	       dblRemovedParts.add( *pSf );
	       dbiSurf = DoubleListIterator( pScenaRender->listSurf );
	     }
          }

	 dbiSurf = DoubleListIterator( pScenaRender->listSurf );
	 pSf = (PTSurface)&dbiSurf.current();
	 for( ; int(dbiSurf); pSf = (PTSurface)&++dbiSurf )
	  {
	    if( pSf->minY > y_  ) break;
 
	    bFlStrEmpty = FALSE;

	    DoubleListIterator dbiFac( pSf->listFacets );
	    PTFacet pF = (PTFacet)&dbiFac.current();
	    for( ; int(dbiFac); pF = (PTFacet)&++dbiFac )
	     {
	       if( pF->minY > y_  ) break;

	       if( pF->maxY < y_  )
		{
		  pSf->listFacets.detach( *pF, TShouldDelete::NoDelete );
		  dblRemovedParts.add( *pF );
		  dbiFac = DoubleListIterator( pSf->listFacets );
		}
             }


	    dbiFac = DoubleListIterator( pSf->listFacets );
	    pF = (PTFacet)&dbiFac.current();
	    for( ; int(dbiFac); pF = (PTFacet)&++dbiFac )
	     {
	       if( pF->minY > y_  ) break;
	       
	       if( pF->vecOutNormal.z < 0.01 ) continue;

	       DoubleListIterator dbiAdgeItem( pF->listAgesItem );
	       PTAdgeItem pAitem = (PTAdgeItem)&dbiAdgeItem.current();
	       for( ; int(dbiAdgeItem); pAitem = (PTAdgeItem)&++dbiAdgeItem )
		{
		  RTAdge rA = *pAitem->pAdge;

		  double dMinYVal = MinY__(*pAitem->pAdge);
		  if( dMinYVal > y_ && fabs(dMinYVal - y_) > dPix ) break;

		  double dMaxYVal = MaxY__(rA);
		  if( dMaxYVal < y_ && fabs(dMaxYVal - y_) > dPix )
		   {
		     
		   } 
		}
                     

	       iCountIntersect = 0;
	       pF->listAgesItem.ResetActionFlag();
	       dbiAdgeItem = DoubleListIterator( pF->listAgesItem );
	       pAitem = (PTAdgeItem)&dbiAdgeItem.current();
	       for( ; int(dbiAdgeItem); pAitem = (PTAdgeItem)&++dbiAdgeItem )
		{
		  double dMinYVal = MinY__(*pAitem->pAdge);
		  if( dMinYVal > y_ && fabs(dMinYVal - y_) > dPix ) continue;
		  double dMaxYVal_ = MaxY__(*pAitem->pAdge);
		  if( dMaxYVal_ < y_ && fabs(dMaxYVal_ - y_) > dPix ) continue;

		  RTAdge rA = *pAitem->pAdge;
		  if( pAitem->bFlAtion == TRUE ) continue;
		  pAitem->bFlAtion = TRUE;

		  if( fabs(rA.pV1->y - rA.pV2->y) < 0.01 ) continue;
		  if( fabs(rA.pV1->y - y_) <= dPix || fabs(rA.pV2->y - y_) <= dPix )
		  //if( rA.pV1->y == y_  || rA.pV2->y == y_ )
		   {
		     //RTAdgeItem rItem = (RTAdgeItem)++dbiAdgeItem;
		     //if( rItem == NOOBJECT ) continue;
		     //RTAdge rA2 = *rItem.pAdge;		     

		     PTVertex pV1, pV2;
		     if( fabs(rA.pV1->y - y_) < dPix ) pV1 = rA.pV1;
		     //if( rA.pV1->y == y_ ) pV1 = rA.pV1;
		     else pV1 = rA.pV2;

		     RTAdgeItem rA2Item = GetIncidentAdge( pF->listAgesItem, rA, *pV1 );
		     if( rA2Item == NOOBJECT ) continue;
		     RTAdge rA2 = *rA2Item.pAdge;
		     if( rA2Item.bFlAtion == TRUE ) continue;
		     rA2Item.bFlAtion = TRUE;

		     if( pV1 == rA2.pV1 ) pV2 = rA2.pV1;
		     else pV2 = rA2.pV2;

		     int iLevel1 = Level( rA, pV1 ),
			 iLevel2 = Level( rA2, pV2 );

		     
		     if( iLevel1 == iLevel2 )
		      {
			mrcIntersect()[ iCountIntersect++ ] = pV1->x;
			mrcIntersect()[ iCountIntersect++ ] = pV1->x;
		      }
		     else
		       mrcIntersect()[ iCountIntersect++ ] = pV1->x;
		   }
		  else		   
		    mrcIntersect()[ iCountIntersect++ ] =
		      IntersectX( rA.pV1->x, rA.pV1->y, rA.pV2->x, rA.pV2->y, y_ );
		}

	       qsort( mrcIntersect(), iCountIntersect, sizeof(double), CmpIntersect );

	       double* pInt = mrcIntersect();
	       for( int iC = iCountIntersect / 2; iC > 0; iC-- )
		{
		  double x1 = *pInt++;
		  double x2 = *pInt++;

		  if( x1 > dXEnd || x2 < dXSta ) continue;
		  if( x1 < dXSta ) x1 = dXSta;
		  if( x2 > dXEnd ) x2 = dXEnd;
		  

		  double x1T = x1 * dSc, x2T = x2 * dSc;
		  double dTmpY = y_ * dSc;
		  pScenaRender->SpecialPointLPtoDP( x1T, dTmpY, r.right, r.bottom, dSc );
		  dTmpY = y_ * dSc;
		  pScenaRender->SpecialPointLPtoDP( x2T, dTmpY, r.right, r.bottom, dSc );
		  //x1T /= dSc;
		  //x2T /= dSc;

		  //int iXSta = floor( x1T ), iXEnd = ceil( x2T );
		  int iXSta = x1T, iXEnd = x2T;
		  //if( fabs(x1- x2) < dPix || iXSta == iXEnd )
		  if( x1 == x2  || iXSta == iXEnd )
		    tr.PixelSmart( iXSta, pSf, pF,
		         -( (pF->d + pF->vecOutNormal.x * x1 + pF->vecOutNormal.y * y_ ) /
		         pF->vecOutNormal.z )
				    );
		  else
                   {
		     //double dStep = fabs(iXSta - iXEnd + 1) / fabs(x1 - x2);
		     for( ; iXSta <= iXEnd; iXSta++, x1 += dPix )		   
		       tr.PixelSmart( iXSta, pSf, pF,
		         -( (pF->d + pF->vecOutNormal.x * x1 + pF->vecOutNormal.y * y_ ) /
		         pF->vecOutNormal.z )
				    );
		   }
		}
             }
	  }

	 if( bFlStrEmpty == FALSE ) tr.NextStrBack( str, TRUE );

	 if( pInd->WorkMessage( lWorking++ ) == TRUE )
	  {
	    delete pScenaRender;
            return NULL;
	  }
       }

      tr.EndBack();
      //tr.NextStrBack( dYStaBmp + 1, TRUE );
    }


   pInd->WorkIs100();

   delete pScenaRender;

   //PTColorTable pctColorTbl = pScenaRender->CreateColorTbl();

   if( !iPaletteSize )
    {
      rpPctTbl = NULL;

      return new TDIB( tr.Buffer(), r.right, r.bottom, NULL, 0, TRUE, pApplicat );
      //return tr.CreateGetBitmap( NULL, 0 );
    }


   int iPS = iPaletteSize ? ((iPaletteSize == -16) ? 16:iPaletteSize):236;
   TConvertAlho alho = (iPaletteSize == -16) ? CA_System:caAlhoPalette;

   if( iPS )
    {
      pInd->SetTitle( "Convert image to indexed" );
      rpPctTbl = TrueColorTo256( tr.Buffer(), tr.SizeBuf(), iPS,
	iPaletteSize ? FALSE:TRUE, alho, r.right, r.bottom,
	pInd );
    }
   else rpPctTbl = NULL;

   PTDIB pRetDIB = new TDIB( tr.Buffer(), r.right, r.bottom,
     iPaletteSize ? rpPctTbl:NULL,
     (iPaletteSize == -16) ? 16:iPaletteSize,
     TRUE, pApplicat );
   //HGLOBAL hRetDIB = tr.CreateGetBitmap( iPaletteSize ? rpPctTbl:NULL,
     //(iPaletteSize == -16) ? 16:iPaletteSize );
   if( rpPctTbl && iPaletteSize )
    {
      delete rpPctTbl; rpPctTbl = NULL;
    }

   return pRetDIB;

 }*/

_CLASSDEF(TAdgePoly)
_CLASSDEF(TPolygonSS)

class TAdgePoly: public TIndexedItem
 {

public:
  PTVertex pV1, pV2;
  PTAdge  pSelf;
  PTAdge  pIncedentV1, pIncedentV2;

  double dDx;
  int    x1, y1;

  TAdgePoly( PTVertex pV1_, PTVertex pV2_, PTAdge  pSelf_ ):
    TIndexedItem( 0 )
   {
     pSelf = pSelf_;
     pV1 = pV1_; pV2 = pV2_;
     pIncedentV1 = pIncedentV2 = NULL;

     x1 = pV1_->x; y1 = pV1_->y;
     dDx = fabs(pV1->x - pV2->x) / fabs(pV1->y - pV2->y);
     if( int(pV2->x) < int(pV1->x) ) dDx = -dDx;
   }


  int IntersectX( int str )
   {
     return int( (double)x1 + double(str - y1) * dDx );
   }

  virtual classType isA() const { return 1; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& rObj ) const
   {
     return &(RTAdgePoly)rObj == this;
   }
  virtual char *nameOf() const { return ""; }
  virtual void printOn( ostream& ) const {};

  void _FAR* operator new( size_t size );  
  void operator delete( void _FAR* pToDel );

private:
  static MemBlocks *mgr;
  static long count;

 };

MemBlocks* TAdgePoly::mgr = NULL;
long TAdgePoly::count = 0;

void _FAR* TAdgePoly::operator new( size_t size )
 {
   if( count++ == 0 )
     TAdgePoly::mgr = new MemBlocks( sizeof(TAdgePoly), 100 );

   return TAdgePoly::mgr->allocate( size );
 }

void TAdgePoly::operator delete( void _FAR* pToDel )
 {   
   TAdgePoly::mgr->free( pToDel );

   if( --count == 0 )
    {
      delete TAdgePoly::mgr;
      TAdgePoly::mgr = NULL;
    }
 }


class TPolygonSS: public TIndexedItem
 {

friend int ComparePolygonSSY( const void* p1, const void* p2 );
friend PTDIB TScena::RenderStrScan( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog );
friend PTDIB TScena::RenderStrScanPhoto( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog );
friend BOOL near pascal CreateStrScanData( TIndexedDoubleList& rdblPolygon, RTScena rSc, BOOL bFlBackFace, double dZZero, PTIndicator pInd );

public:

  TPolygonSS( int iMinX_, int iMaxX_, int iMinY_, int iMaxY_, PTSurface pSf_, PTFacet pF_ ):
    TIndexedItem( 0 )
   {
     pSf = pSf_;
     pF = pF_;
     iMinX = iMinX_; iMaxX = iMaxX_;
     iMinY = iMinY_; iMaxY = iMaxY_;
   }

  virtual classType isA() const { return 1; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& rObj ) const
   {
     return &(RTPolygonSS)rObj == this;
   }
  virtual char *nameOf() const { return ""; }
  virtual void printOn( ostream& ) const {};

  void _FAR* operator new( size_t size );
  void operator delete( void _FAR* pToDel );

  void ResetActionFlag();

  int CmpY( RTIndexedItem );


private:
  int iMinX, iMaxX, iMinY, iMaxY;
  DoubleList dblAdges;

  PTSurface pSf;
  PTFacet pF;
  
  static MemBlocks *mgr;
  static long count;

 };

int _export ComparePolygonSSY( const void* p1, const void* p2 )
 {
   RTPolygonSS rP1 = *(PTPolygonSS)(*(PTIndexedItem*)p1),
	       rP2 = *(PTPolygonSS)(*(PTIndexedItem*)p2);

   if( rP1.iMinY < rP2.iMinY ) return -1;
   else if( rP1.iMinY > rP2.iMinY ) return 1;
   else return 0;
 }

int TPolygonSS::CmpY( RTIndexedItem rObj )
 {
   if( iMinY < RTPolygonSS(rObj).iMinY ) return -1;
   else if( iMinY > RTPolygonSS(rObj).iMinY ) return 1;
   else return 0;
 }

static void ResetActionFlagInst( Object _FAR& rObj, void _FAR* )
 {
   RTAdgePoly(rObj).pSelf->bFlAtion = FALSE;
 }                         

void TPolygonSS::ResetActionFlag()
 {
   dblAdges.forEach( ResetActionFlagInst, 0 );
 }


MemBlocks* TPolygonSS::mgr = NULL;
long TPolygonSS::count = 0;

void _FAR* TPolygonSS::operator new( size_t size )
 {
   if( count++ == 0 )
     TPolygonSS::mgr = new MemBlocks( sizeof(TPolygonSS), 100 );

   return TPolygonSS::mgr->allocate( size );
 }

void TPolygonSS::operator delete( void _FAR* pToDel )
 {
   TPolygonSS::mgr->free( pToDel );

   if( --count == 0 )
    {
      delete TPolygonSS::mgr;
      TPolygonSS::mgr = NULL;
    }
 }



static BOOL near pascal CreateStrScanData( TIndexedDoubleList& rdblPolygon, RTScena  rSc, BOOL bFlBackFace, double dZZero, PTIndicator pInd )
 {
   DoubleListIterator dbiSurf( rSc.listSurf );
   PTSurface pSf = (PTSurface)&dbiSurf.current();

   for( ; int(dbiSurf); pSf = (PTSurface)&++dbiSurf )
    {

      DoubleListIterator dbiFac( pSf->listFacets );
      PTFacet pF = (PTFacet)&dbiFac.current();
	    
      for( ; int(dbiFac); pF = (PTFacet)&++dbiFac )
       {
	 if( bFlBackFace == FALSE && pF->vecOutNormal.z > dZZero ) continue;

	 PTPolygonSS pPol = new TPolygonSS( pF->minX, pF->maxX, pF->minY, pF->maxY, pSf, pF );
	 if( !pPol )
	  {
	    BWCCMessageBox( NULL, "Not enought memory for create string-scan data",
              "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL
			  );
	    return FALSE;
	  }

	 rdblPolygon.addAtTail( *pPol );

	 DoubleListIterator dbiAdgeItem( pF->listAgesItem );
	 PTAdgeItem pAitem = (PTAdgeItem)&dbiAdgeItem.current();
	 for( ; int(dbiAdgeItem); pAitem = (PTAdgeItem)&++dbiAdgeItem )
	  {	    
	    PTAdgePoly pAP;
	    BOOL bTst;

	    if( int(pAitem->pAdge->pV1->y) == int(pAitem->pAdge->pV2->y) ) continue;

	    if( (bTst = int(pAitem->pAdge->pV1->y) < int(pAitem->pAdge->pV2->y)) )
	      pAP = new TAdgePoly( pAitem->pAdge->pV1, pAitem->pAdge->pV2, pAitem->pAdge );
	    else
	      pAP = new TAdgePoly( pAitem->pAdge->pV2, pAitem->pAdge->pV1, pAitem->pAdge );
	    if( !pAP )
	     {
	       BWCCMessageBox( NULL, "Not enought memory for create string-scan data",
                "Error",
	        MB_ICONSTOP | MB_OK | MB_APPLMODAL
			     );
	       return FALSE;
	     }

	    pPol->dblAdges.addAtTail( *pAP );

	    RTAdgeItem rIncident = GetIncidentAdge( pF->listAgesItem, *pAitem->pAdge,
	      (bTst == TRUE) ? *pAitem->pAdge->pV2:*pAitem->pAdge->pV1 );
	    if( rIncident == NOOBJECT ) continue;

	    if( int(rIncident.pAdge->pV1->y) != int(rIncident.pAdge->pV2->y) )
	      pAP->pIncedentV2 = rIncident.pAdge;

	    RTAdgeItem rIncident2 = GetIncidentAdge( pF->listAgesItem, *pAitem->pAdge,
	      (bTst == TRUE) ? *pAitem->pAdge->pV1:*pAitem->pAdge->pV2 );
	    if( rIncident2 == NOOBJECT ) continue;

	    if( int(rIncident2.pAdge->pV1->y) != int(rIncident2.pAdge->pV2->y) )
	      pAP->pIncedentV1 = rIncident2.pAdge;
	  }
       }

    }

    if( pInd )
     {
       pInd->UpdateStr( 2, "Sorting string scan data..." );
       pInd->Init( 2 * rdblPolygon.getItemsInContainer() );
     }
   //rdblPolygon.Sort( (PTCompareFunc)&TPolygonSS::CmpY, pInd );
   rdblPolygon.Sort( ComparePolygonSSY, pInd );

   return TRUE;
 }

PTDIB TScena::RenderStrScan( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog )
 {
   PTUCS pucsKey = pUcs;
   HDC hdcKey = hdc;
   PTWindow pwinKey = pWin;   


   double dSc;
   int iSx, iSy;
   RECT r = GetFormatImage( dSc, iSx, iSy );

   TTracer  tr = TTracer( r, hdc, rgbAdges,
     (bFlMarkIntersect == TRUE) ? rgbIntersect:rgbAdges );

   if( tr.Status )
    {
      BWCCMessageBox( pWin->HWindow, "Not enought memory for create bitmap",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
      
      return NULL;
    }


   pInd->AddStr( "Transforming objects" );
   pInd->Show();

   rTLog.StartCopy();

   PTScena  pScenaRender = new TScena( *this, pInd, TRUE, FALSE );
   //pInd->Init( pScenaRender->GetBytesSize() );
   if( !pScenaRender )
    {
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == -1 )
     {
       delete pScenaRender;
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == 1 )
     {
       delete pScenaRender;
       

       return NULL;
     }

   rTLog.EndCopy();
   rTLog.StartTransform();

   pScenaRender->pUcs = pucsKey;
   pScenaRender->hdc = hdcKey;
   pScenaRender->pWin = pwinKey;

   pScenaRender->RecalcUserToBase();   
   pScenaRender->RecalcLPtoDP();
   pScenaRender->MakeCompatibleFormatImage( dSc, iSx, -iSy );

   TIndexedDoubleList dblLights, dblCameras;
   pScenaRender->Separate( dblLights, dblCameras );
   dblLights.flush( TShouldDelete::Delete );
   dblCameras.flush( TShouldDelete::Delete );
     

   pInd->UpdateStr( 2, "Calc normals" );
   pInd->Init( pScenaRender->listSurf.getItemsInContainer() );

   DoubleListIterator  dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();
   for( LONG lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )  
     {       
       pSerf->CalcNormals();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  //GetSwap( pInd );
	  
	  delete pScenaRender;
	  //pInd->Close();
          return  NULL;
        }
     }

   pInd->WorkIs100();
   

   pInd->UpdateStr( 2, "Calc bounds" );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );

   dbIter = DoubleListIterator( pScenaRender->listSurf );
   pSerf = (PTSurface)&dbIter.current();
   for( lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     {
       pSerf->CalcBounds();      
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}

       pSerf->CalcBoundsAllFacets();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}
     }

   //pScenaRender->TestToFileBounds( "g:\\sort1.txt" );

   rTLog.EndTransform();
   rTLog.StartOptimize();

   pInd->WorkIs100();

   pInd->UpdateStr( 2, "Optimize body data..." );
   //pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );
   //pScenaRender->listSurf.Sort( (PTCompareFunc)&TSurface::CompareY, pInd );
   pScenaRender->SortSurfFacAdg( pInd );

   pScenaRender->CalcBounds();

   rTLog.EndOptimize();

   //pScenaRender->TestToFileBounds( "g:\\sort2.txt" );

   RECT rB;
   rB.left = rB.top = 0; rB.right = r.right - 1; rB.bottom = r.bottom - 1;
   tr.Bounds( rB );

   rTLog.StartInitBkGnd();

   pInd->UpdateStr( 2, "Init backgraund..." );
   tr.Start( mrcBackGraund(), pInd );

   rTLog.EndInitBkGnd();
   		
   TAutoMemoryRc<int*> mrcIntersect = new int[ 2 * pScenaRender->GetMaxAdges() ];
   int iCountIntersect;
   LONG  lWorking = 1;

   TIndexedDoubleList rdblPolygon;

   rTLog.StartPrepareRender();

   if( !CreateStrScanData(rdblPolygon, *pScenaRender, FALSE, TRACER_d_ZOutNormalVisibleStrScan, pInd) )
    {
      delete pScenaRender;
      return  NULL;
    }

   rTLog.EndPrepareRender();
   rTLog.StartRender();

   pInd->UpdateStr( 2, "Render..." );
   pInd->Init( labs((LONG)r.bottom - (LONG)r.top) );

   //RemoveBackFaces( pScenaRender->listSurf, TestVisibleStrScan );

   DoubleList dblFlush;

   for( int str = 0; str < r.bottom; str++ )
    {      

LBL_RESTART1:
      DoubleListIterator dbiP( rdblPolygon );
      PTPolygonSS pP = (PTPolygonSS)&dbiP.current();
      
      for( ; int(dbiP); pP = (PTPolygonSS)&++dbiP )
       {
         if( pP->iMinY > str ) break;

	 if( pP->iMaxY < str /*|| pP->pF->vecOutNormal.z > -0.01*/ )
	  {
	    rdblPolygon.detach( *pP, TShouldDelete::NoDelete );
	    dblFlush.add( *pP );
	    //dbiP = DoubleListIterator( rdblPolygon );
	    goto LBL_RESTART1;
	  }
       }
      //dblFlush.flush( TShouldDelete::Delete );

      dbiP = DoubleListIterator( rdblPolygon );
      pP = (PTPolygonSS)&dbiP.current();
      for( ; int(dbiP); pP = (PTPolygonSS)&++dbiP )
       {
	 if( pP->iMinY > str ) break;

	 /*if( pP->pF->vecOutNormal.z > -0.01 ) continue;*/

LBL_RESTART2:
	 DoubleListIterator dbiAg( pP->dblAdges );
	 PTAdgePoly pAgP = (PTAdgePoly)&dbiAg.current();
      
	 for( ; int(dbiAg); pAgP = (PTAdgePoly)&++dbiAg )
	  {
	    if( int(pAgP->pV1->y) > str ) break;

	    if( int(pAgP->pV2->y) < str )
	     {
	       pP->dblAdges.detach( *pAgP, TShouldDelete::NoDelete );
	       dblFlush.add( *pAgP );
	       //dbiAg = DoubleListIterator( pP->dblAdges );
	       goto LBL_RESTART2;
	     }
	  }
	 //dblFlush.flush( TShouldDelete::Delete );

	 pP->ResetActionFlag();
	 //pP->StartScanY( str );

	 iCountIntersect = 0;
	 dbiAg = DoubleListIterator( pP->dblAdges );
	 pAgP = (PTAdgePoly)&dbiAg.current();      
	 for( ; int(dbiAg); pAgP = (PTAdgePoly)&++dbiAg )
	  {
	    if( pAgP->pV1->y > str ) break;

	    //if( pAgP->pSelf->bFlAtion == TRUE ) { (*pAgP)++; continue; }
	    if( pAgP->pSelf->bFlAtion == TRUE ) continue; 
	    if( int(pAgP->pV2->y) < str ) continue;
	    pAgP->pSelf->bFlAtion = TRUE;

	    if( int(pAgP->pV1->y) == str )
	     {
	       //pAgP->StartScan();
	       if( pAgP->pIncedentV1 )  //*************
		{
		  pAgP->pIncedentV1->bFlAtion = TRUE;		  

		  if( LevelInt( *pAgP->pIncedentV1, pAgP->pV1 ) )                   
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
		  else
		   {
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
		   }
		}
	       else mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
             }
	    else if( int(pAgP->pV2->y) == str )
	     {
	       if( pAgP->pIncedentV2 )
		{
		  pAgP->pIncedentV2->bFlAtion = TRUE;

		  if( !LevelInt( *pAgP->pIncedentV2, pAgP->pV2 ) )
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
		  else
		   {
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
		   }
		}
	       else mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
	     }
            else
	      mrcIntersect()[ iCountIntersect++ ] = pAgP->IntersectX( str );

	    //(*pAgP)++;
	  }

	 qsort( mrcIntersect(), iCountIntersect, sizeof(int), CmpIntersectInt );

	 int* pInt = mrcIntersect();
	 for( int iC = iCountIntersect / 2; iC > 0; iC-- )
	  {
	    int x1 = *pInt++;
	    int x2 = *pInt++;

	    if( x1 > r.right - 1 || x2 < 0 ) continue;
	    if( x1 < 0 ) x1 = 0;
	    if( x2 > r.right - 1 ) x2 = r.right - 1;
		  
	    if( x1 == x2 ) 
	      tr.PixelSmart( x1, pP->pSf, pP->pF,
		-( (pP->pF->d + pP->pF->vecOutNormal.x * double(x1) + pP->pF->vecOutNormal.y * double(str) ) /
		pP->pF->vecOutNormal.z )
		           );
	    else         	
	      for( ; x1 <= x2; x1++ )
		 tr.PixelSmart( x1, pP->pSf, pP->pF,
		   -( (pP->pF->d + pP->pF->vecOutNormal.x * double(x1) + pP->pF->vecOutNormal.y * double(str) ) /
		   pP->pF->vecOutNormal.z )
			      );
	  }
       }//*********

      tr.NextStr( str, TRUE );      

      if( pInd->WorkMessage( lWorking++ ) == TRUE )
       {
         delete pScenaRender;
         return NULL;
       }
    }

   tr.End();

   pInd->WorkIs100();

   rTLog.EndRender();

   dblFlush.flush( TShouldDelete::Delete );

   TNoisyDta ndDta;   pScenaRender->GetNoisy( ndDta );
   delete pScenaRender;
   rdblPolygon.flush( TShouldDelete::Delete );

   //PTColorTable pctColorTbl = pScenaRender->CreateColorTbl();

   if( !iPaletteSize )
    {
      rpPctTbl = NULL;

      return new TDIB( tr.Buffer(), r.right, r.bottom, NULL, 0, TRUE, pApplicat );
      //return tr.CreateGetBitmap( NULL, 0 );
    }


   int iPS = iPaletteSize ? ((iPaletteSize == -16) ? 16:iPaletteSize):236;
   TConvertAlho alho = (iPaletteSize == -16) ? CA_System:caAlhoPalette;

   rTLog.StartCalcPalette();

   if( iPS )
    {
      pInd->SetTitle( "Convert image to indexed" );
      rpPctTbl = TrueColorTo256( tr.Buffer(), tr.SizeBuf(), iPS,
	iPaletteSize ? FALSE:TRUE, alho, r.right, r.bottom,
        &ndDta,
	pInd );
    }
   else rpPctTbl = NULL;

   rTLog.EndCalcPalette();

   PTDIB pRetDIB = new TDIB( tr.Buffer(), r.right, r.bottom,
     iPaletteSize ? rpPctTbl:NULL,
     (iPaletteSize == -16) ? 16:iPaletteSize,
     TRUE, pApplicat );
   //HGLOBAL hRetDIB = tr.CreateGetBitmap( iPaletteSize ? rpPctTbl:NULL,
     //(iPaletteSize == -16) ? 16:iPaletteSize );
   if( rpPctTbl && iPaletteSize )
    {
      delete []rpPctTbl; rpPctTbl = NULL;
    }

   return pRetDIB;

 }

PTDIB TScena::RenderStrScanPhoto( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog )
 {
   PTUCS pucsKey = pUcs;
   HDC hdcKey = hdc;
   PTWindow pwinKey = pWin;   


   double dSc;
   int iSx, iSy;
   RECT r = GetFormatImage( dSc, iSx, iSy );

   TTracer  tr = TTracer( r, hdc, rgbAdges,
     (bFlMarkIntersect == TRUE) ? rgbIntersect:rgbAdges );
   TTracerPhoto  trPhoto = TTracerPhoto( r.right, r.bottom );

   if( tr.Status || !trPhoto.Status() )
    {
      BWCCMessageBox( pWin->HWindow, "Not enought memory for create bitmap",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
      
      return NULL;
    }


   pInd->AddStr( "Transforming objects" );
   pInd->Show();


   TimeLog tlLog2;
   rTLog.StartCopy();

   PTScena  pScenaRender = new TScena( *this, pInd, TRUE, FALSE );
   pScenaRender->bFlLeftUCS = TRUE;
   //pInd->Init( pScenaRender->GetBytesSize() );
   if( !pScenaRender )
    {
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == -1 )
     {
       delete pScenaRender;
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == 1 )
     {
       delete pScenaRender;
       

       return NULL;
     }

   rTLog.EndCopy();
   rTLog.StartTransform();

   pScenaRender->pUcs = pucsKey;
   pScenaRender->hdc = hdcKey;
   pScenaRender->pWin = pwinKey;

   pScenaRender->RecalcUserToBase();   
   pScenaRender->RecalcLPtoDP();
   pScenaRender->MakeCompatibleFormatImage( dSc, iSx, -iSy );
   ScaleLights( pScenaRender->listSurf, pScenaRender->pUcs->GetScale() );


   pInd->UpdateStr( 2, "Calc normals" );
   pInd->Init( pScenaRender->listSurf.getItemsInContainer() );

   DoubleListIterator  dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();
   for( LONG lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )  
     {       
       pSerf->CalcNormals();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  //GetSwap( pInd );
	  
	  delete pScenaRender;
	  //pInd->Close();
          return  NULL;
        }
     }

   pInd->WorkIs100();

   

   pInd->UpdateStr( 2, "Calc bounds" );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );

   dbIter = DoubleListIterator( pScenaRender->listSurf );
   pSerf = (PTSurface)&dbIter.current();
   for( lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     {
       pSerf->CalcBounds();      
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}

       pSerf->CalcBoundsAllFacets();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
	  return  NULL;
	}
     }

   //pScenaRender->TestToFileBounds( "g:\\sort1.txt" );

   pInd->WorkIs100();

   rTLog.EndTransform();
   rTLog.StartOptimize();

   TIndexedDoubleList dblLights, dblCameras;
   pScenaRender->Separate( dblLights, dblCameras );
   //dblLights.flush( TShouldDelete::Delete );
   //dblCameras.flush( TShouldDelete::Delete );

   pInd->UpdateStr( 2, "Optimize body data..." );
   //pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );
   //pScenaRender->listSurf.Sort( (PTCompareFunc)&TSurface::CompareY, pInd );
   pScenaRender->SortSurfFacAdg( pInd );

   pScenaRender->CalcBounds();

   rTLog.EndOptimize();
   rTLog.StartPrepareRender();

   if( pScenaRender->RenderPrepareBodys( pInd ) == FALSE )
    {
      delete pScenaRender;
      return  NULL;
    }

   rTLog.EndPrepareRender();
   rTLog.StartCopyShadows();


   

   TAutoMemoryRc<PTScena> mrcShade;

   if( bFlShadows == TRUE )
    {
      pInd->UpdateStr( 2, "Transforming objects" );
      pInd->Init( pScenaRender->GetBytesSize() );
      mrcShade = new TScena( *pScenaRender, pInd, TRUE, FALSE );
      if( !mrcShade )
       {
         delete pScenaRender;
         return  NULL;
       }

      RemoveHorizAdgesItems( mrcShade()->listSurf, TestHorizInt );

      pInd->UpdateStr( 2, "Optimize shadows data..." );

      mrcShade()->listSurf.Sort( CompareSurfaceYRev, pInd );
      mrcShade()->SortFacets( CompareFacetYRev, pInd );
      mrcShade()->SortAdges( CompareAdgeItemYRev, pInd );
      if(     
	  mrcShade()->ListAdgeNormalyzeRev( pInd ) == FALSE
        )
       {
         delete pScenaRender;
         return  NULL;
       }

      mrcShade()->bFlLeftUCS = TRUE;
      mrcShade()->InitStrScanParamOfAdgeItems( INT_MAX );
      mrcShade()->pFIntersect = &TScena::FindIntersectReal;

      mrcShade()->CalcAllTraceObol( pInd );
    }

   rTLog.EndCopyShadows();

   tlLog2.Reset();
   tlLog2.StartPrepareRender();

   RemoveBackFacesAndHorizAdgesItems( pScenaRender->listSurf, TestVisibleStrScan, TestHorizInt );

   tlLog2.EndPrepareRender();
   rTLog += tlLog2;

   //pScenaRender->TestToFileBounds( "g:\\sort2.txt" );

   RECT rB;
   rB.left = rB.top = 0; rB.right = r.right - 1; rB.bottom = r.bottom - 1;
   tr.Bounds( rB );

   rTLog.StartInitBkGnd();

   pInd->UpdateStr( 2, "Init backgraund..." );
   tr.Start( mrcBackGraund(), pInd );

   rTLog.EndInitBkGnd();

   tlLog2.Reset();
   tlLog2.StartPrepareRender();

   trPhoto.IntBuf( tr.Buffer() );


   TAutoMemoryRc<int*> mrcIntersect = new int[ 2 * pScenaRender->GetMaxAdges() ];
   int iCountIntersect;
   LONG  lWorking = 1;

   TIndexedDoubleList rdblPolygon;

   if( !CreateStrScanData(rdblPolygon, *pScenaRender, FALSE, TRACER_d_ZOutNormalVisibleStrScan, pInd) )
    {
      delete pScenaRender;
      return  NULL;
    }

   tlLog2.EndPrepareRender();
   rTLog += tlLog2;


   rTLog.StartRender();

   pInd->UpdateStr( 2, "Render..." );
   pInd->Init( labs((LONG)r.bottom - (LONG)r.top) );

   DoubleList dblFlush;

   //RemoveBackFaces( pScenaRender->listSurf, TestVisibleStrScan );

   for( int str = 0; str < r.bottom; str++ )
    {
LBL_RESTART3:
      DoubleListIterator dbiP( rdblPolygon );
      PTPolygonSS pP = (PTPolygonSS)&dbiP.current();

      
      for( ; int(dbiP); pP = (PTPolygonSS)&++dbiP )
       {
         if( pP->iMinY > str ) break;

	 if( pP->iMaxY < str /*|| pP->pF->vecOutNormal.z > -0.01*/ )
	  {
	    rdblPolygon.detach( *pP, TShouldDelete::NoDelete );
	    dblFlush.add( *pP );
	    //dbiP = DoubleListIterator( rdblPolygon );
	    goto LBL_RESTART3;
	  }
       }
      //dblFlush.flush( TShouldDelete::Delete );

      dbiP = DoubleListIterator( rdblPolygon );
      pP = (PTPolygonSS)&dbiP.current();
      for( ; int(dbiP); pP = (PTPolygonSS)&++dbiP )
       {
	 if( pP->iMinY > str ) break;

	 /*if( pP->pF->vecOutNormal.z > -0.01 ) continue;*/

LBL_RESTART4:
	 DoubleListIterator dbiAg( pP->dblAdges );
	 PTAdgePoly pAgP = (PTAdgePoly)&dbiAg.current();
      
	 for( ; int(dbiAg); pAgP = (PTAdgePoly)&++dbiAg )
	  {
	    if( int(pAgP->pV1->y) > str ) break;

	    if( int(pAgP->pV2->y) < str )
	     {
	       pP->dblAdges.detach( *pAgP, TShouldDelete::NoDelete );
	       dblFlush.add( *pAgP );
	       //dbiAg = DoubleListIterator( pP->dblAdges );
	       goto LBL_RESTART4;
	     }
	  }
	 //dblFlush.flush( TShouldDelete::Delete );

	 pP->ResetActionFlag();
	 //pP->StartScanY( str );

	 iCountIntersect = 0;
	 dbiAg = DoubleListIterator( pP->dblAdges );
	 pAgP = (PTAdgePoly)&dbiAg.current();      
	 for( ; int(dbiAg); pAgP = (PTAdgePoly)&++dbiAg )
	  {
	    if( pAgP->pV1->y > str ) break;

	    //if( pAgP->pSelf->bFlAtion == TRUE ) { (*pAgP)++; continue; }
	    if( pAgP->pSelf->bFlAtion == TRUE ) continue; 
	    if( int(pAgP->pV2->y) < str ) continue;
	    pAgP->pSelf->bFlAtion = TRUE;

	    if( int(pAgP->pV1->y) == str )
	     {
	       //pAgP->StartScan();
	       if( pAgP->pIncedentV1 )  //*************
		{
		  pAgP->pIncedentV1->bFlAtion = TRUE;		  

		  if( LevelInt( *pAgP->pIncedentV1, pAgP->pV1 ) )                   
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
		  else
		   {
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
		   }
		}
	       else mrcIntersect()[ iCountIntersect++ ] = pAgP->pV1->x;
             }
	    else if( int(pAgP->pV2->y) == str )
	     {
	       if( pAgP->pIncedentV2 )
		{
		  pAgP->pIncedentV2->bFlAtion = TRUE;

		  if( !LevelInt( *pAgP->pIncedentV2, pAgP->pV2 ) )
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
		  else
		   {
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
		     mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
		   }
		}
	       else mrcIntersect()[ iCountIntersect++ ] = pAgP->pV2->x;
	     }
            else
	      mrcIntersect()[ iCountIntersect++ ] = pAgP->IntersectX( str );

	    //(*pAgP)++;
	  }

	 qsort( mrcIntersect(), iCountIntersect, sizeof(int), CmpIntersectInt );

	 int* pInt = mrcIntersect();
	 for( int iC = iCountIntersect / 2; iC > 0; iC-- )
	  {
	    int x1 = *pInt++;
	    int x2 = *pInt++;

	    if( x1 > r.right - 1 || x2 < 0 ) continue;
	    if( x1 < 0 ) x1 = 0;
	    if( x2 > r.right - 1 ) x2 = r.right - 1;
		  
	    if( x1 == x2 ) 
	      /*tr.PixelSmart( x1, pP->pSf, pP->pF,
		-( (pP->pF->d + pP->pF->vecOutNormal.x * double(x1) + pP->pF->vecOutNormal.y * double(str) ) /
		pP->pF->vecOutNormal.z ) );*/
	      trPhoto.PixelSmart( x1, x1, str,
		-( (pP->pF->d + pP->pF->vecOutNormal.x * double(x1) + pP->pF->vecOutNormal.y * double(str) ) /
		pP->pF->vecOutNormal.z ),
		pP->pSf, pP->pF
				);
		           
	    else         	
	      for( ; x1 <= x2; x1++ )
		 /*tr.PixelSmart( x1, pP->pSf, pP->pF,
		   -( (pP->pF->d + pP->pF->vecOutNormal.x * double(x1) + pP->pF->vecOutNormal.y * double(str) ) /
		   pP->pF->vecOutNormal.z )
			      );*/
	        trPhoto.PixelSmart( x1, x1, str,
		  -( (pP->pF->d + pP->pF->vecOutNormal.x * double(x1) + pP->pF->vecOutNormal.y * double(str) ) /
		  pP->pF->vecOutNormal.z ),
		  pP->pSf, pP->pF
				  );
	  }

       }//*******************************


      //tr.NextStr( str, TRUE );
      trPhoto.StartShading();
      PTStrTracData pDStr = trPhoto.Current();
      for( ; int(trPhoto); pDStr = trPhoto++ )
       {
	 if( pDStr->psfSurf && pDStr->pFac )
          {
	    //TTracData dtaRay( pNearstSurf, pNearstFac, z );
	     double dZNorm;
	     TRGBd rgbPix = pScenaRender->Shadow( dblLights, *pDStr, pDStr->x, pDStr->y, mrcShade(), dZNorm );
	     trPhoto.PixelAbs( pDStr->x, pDStr->y, rgbPix.dR, rgbPix.dG, rgbPix.dB );
	  }
       }

      trPhoto.InitStr();

      if( pInd->WorkMessage( lWorking++ ) == TRUE )
       {
         delete pScenaRender;
         return NULL;
       }
    }

   mrcShade.FreeRc();
   dblFlush.flush( TShouldDelete::Delete );
   //tr.End();

   rTLog.EndRender();
   pInd->WorkIs100();

   trPhoto.ConvertToRGB( tr.Buffer(), bFlStrictGamma );
   trPhoto.Free();


   TNoisyDta ndDta;   pScenaRender->GetNoisy( ndDta );
   delete pScenaRender;
   rdblPolygon.flush( TShouldDelete::Delete );

   //PTColorTable pctColorTbl = pScenaRender->CreateColorTbl();

   if( !iPaletteSize )
    {
      rpPctTbl = NULL;

      return new TDIB( tr.Buffer(), r.right, r.bottom, NULL, 0, TRUE, pApplicat );
      //return tr.CreateGetBitmap( NULL, 0 );
    }


   int iPS = iPaletteSize ? ((iPaletteSize == -16) ? 16:iPaletteSize):236;
   TConvertAlho alho = (iPaletteSize == -16) ? CA_System:caAlhoPalette;

   rTLog.StartCalcPalette();

   if( iPS )
    {
      pInd->SetTitle( "Convert image to indexed" );
      rpPctTbl = TrueColorTo256( tr.Buffer(), tr.SizeBuf(), iPS,
	iPaletteSize ? FALSE:TRUE, alho, r.right, r.bottom,
        &ndDta,
	pInd );
    }
   else rpPctTbl = NULL;

   rTLog.EndCalcPalette();

   PTDIB pRetDIB = new TDIB( tr.Buffer(), r.right, r.bottom,
     iPaletteSize ? rpPctTbl:NULL,
     (iPaletteSize == -16) ? 16:iPaletteSize,
     TRUE, pApplicat );
   //HGLOBAL hRetDIB = tr.CreateGetBitmap( iPaletteSize ? rpPctTbl:NULL,
     //(iPaletteSize == -16) ? 16:iPaletteSize );
   if( rpPctTbl && iPaletteSize )
    {
      delete []rpPctTbl; rpPctTbl = NULL;
    }

   return pRetDIB;

 }


/*
struct TTracData
 {
   PTSurface  psfSurf;
   PTFacet    pFac;
   double     z;

   TTracData() {};
   TTracData( PTSurface  psfSurf_, PTFacet pFac_, double z_ )
    {
      psfSurf = psfSurf_;
      pFac = pFac_;
      z = z_;
    }
 };
*/


BOOL near pascal TScena::Shade( RTVector vecOmni, RTVector vecPoint, RTTracData rDta, RTVector vecRay )
 {
   //if( !pScenaShadow ) return FALSE;

   TVector vecL = vecOmni - vecPoint;
   vecL = Normalyze( vecL );

   BOOL bXTst = fabs(vecPoint.x - vecOmni.x) < 0.01,
	bYTst = fabs(vecPoint.y - vecOmni.y) < 0.01;
	
   double dL, dR;
   if( !bXTst ) { dL = min(vecPoint.x, vecOmni.x); dR = max(vecPoint.x, vecOmni.x); }
   else if( !bYTst ) { dL = min(vecPoint.y, vecOmni.y); dR = max(vecPoint.y, vecOmni.y); }
   else { dL = min(vecPoint.z, vecOmni.z); dR = max(vecPoint.z, vecOmni.z); }

   DoubleList rdblRemove;
   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      float b = vecL.x * (vecPoint.x - pSerf->vecCenter.x) +
		vecL.y * (vecPoint.y - pSerf->vecCenter.y) +
		vecL.z * (vecPoint.z - pSerf->vecCenter.z);
      float c = (vecPoint.x - pSerf->vecCenter.x) * (vecPoint.x - pSerf->vecCenter.x) +
	(vecPoint.y - pSerf->vecCenter.y) * (vecPoint.y - pSerf->vecCenter.y) +
	(vecPoint.z - pSerf->vecCenter.z) * (vecPoint.z - pSerf->vecCenter.z) -
	pSerf->dRadSphricObol * pSerf->dRadSphricObol;

      float dDiscrem = b * b - c;
      if( dDiscrem < 0 ) continue;
      float dDiscremSQ = sqrt( dDiscrem );

      float t = -b + dDiscremSQ;
      if( t <= 0 && -b - dDiscremSQ <= 0 ) continue;

      /*
      float xPlus = pSerf->maxX, yPlus = pSerf->maxY, zPlus = pSerf->maxZ,
	    xMinus = pSerf->minX, yMinus = pSerf->minY, zMinus = pSerf->minZ;

      if( fabs(vecL.x) <= 0.001 && (vecPoint.x < xMinus || vecPoint.x > xPlus) ) continue;
      if( fabs(vecL.x) > 0.001 )
       {

	 float fT1x = (xMinus - vecPoint.x) / vecL.x,
	       fT2x = (xPlus - vecPoint.x) / vecL.x;
	 float fTNear, fTFar;
	 if( fT1x < fT2x ) { fTNear = fT1x; fTFar = fT2x; }
	 else { fTNear = fT2x; fTFar = fT1x; }

	 if( fabs(vecL.y) > 0.001 )
	  {
	    float fT1y = (yMinus - vecPoint.y) / vecL.y,
		  fT2y = (yPlus - vecPoint.y) / vecL.y;
	    if( fT1y > fTNear ) { fTNear = fT1y; }
	    if( fT2y < fTFar ) { fTFar = fT2y; }

	    if( fTNear > fTFar || fTFar < 0 ) continue;

	    if( fabs(vecL.z) > 0.001 )
	     {
	       float fT1z = (zMinus - vecPoint.z) / vecL.z,
		     fT2z = (zPlus - vecPoint.z) / vecL.z;
	       if( fT1z > fTNear ) { fTNear = fT1z; }
	       if( fT2z < fTFar ) { fTFar = fT2z; }

	       if( !(0 < fTFar || (0 < fTNear && fTNear < fTFar)) ) continue;
	     }
          }
       }*/


      DoubleListIterator dbFacet = DoubleListIterator( pSerf->listFacets );
      PTFacet  pFacet = (PTFacet)&dbFacet.current();
      

      for( ; int(dbFacet); pFacet = (PTFacet)&++dbFacet )
       {
	 if( rDta.pFac == pFacet->pSPParent ) continue;
       	 
	 double dSclA = pFacet->vecOutNormal & vecL;
	 if( fabs(dSclA) <= TRACER_d_ShadingSclrZero ) continue;
         
	 double dSclB = -((pFacet->vecOutNormal & vecPoint) + pFacet->d);

	 double dT = dSclB / dSclA;

	 if( dT < 0 ) continue;

	 TVector vecIntersect = vecPoint + vecL * dT;

	 if( !bXTst )
	  {
	    if( vecIntersect.x <= dL || vecIntersect.x >= dR ) continue;
          }
	 else if( !bYTst )	  
	  {
	    if( vecIntersect.y <= dL || vecIntersect.y >= dR ) continue;
	  }
	 else
	  {
	    if( vecIntersect.z <= dL || vecIntersect.z >= dR ) continue;
          }

	 double z;

	 if( !(this->*pFIntersect)( z, vecIntersect.x, vecIntersect.y,
	     pFacet, rdblRemove, FALSE )
	   )
	  {
	    if( pSerf->IsTransparency() == FALSE ) return TRUE;


	    vecRay -= vecRay * (1 - pSerf->medium.GdKt());
	    if( vecRay.x < 0 ) vecRay.x = 0;
	    if( vecRay.y < 0 ) vecRay.y = 0;
	    if( vecRay.z < 0 ) vecRay.z = 0;
            break;
          }
       }

    }

   return FALSE;
 }

TRGBd near pascal TScena::Shadow( TIndexedDoubleList& rdblLights, RTTracData rDta, double x, double y, PTScena pScenaShadow, double& rZNorm )
 {
   double dR = rDta.psfSurf->dIAmbientR,
	  dG = rDta.psfSurf->dIAmbientG,
	  dB = rDta.psfSurf->dIAmbientB;

   if( !rdblLights.getItemsInContainer() ) return TRGBd( dR, dG, dB );

   TVector vecNorm = ((rDta.pFac)->*(rDta.psfSurf->pGetTraceNormalFunc))( x, y );
   if( bFlLeftUCS == TRUE ) vecNorm = -vecNorm;
   rZNorm = vecNorm.z;

   TVector vecS( 0, 0, 1 );
   double dK = 0.001;

   double dKd = rDta.psfSurf->medium.GdKd(),
	  dKs = rDta.psfSurf->medium.GdKs(),
	  dN  = rDta.psfSurf->medium.GPhong();
   //RGBQUAD rgb = rDta.psfSurf->medium.GetRGBColor();
   TRGBd   rgbD( rDta.psfSurf->medium.GetRGBColor() );

   DoubleListIterator dblIt( rdblLights );
   PTSurface pSTmp = (PTSurface)&dblIt.current();
   for( ; int(dblIt); pSTmp = (PTSurface)&++dblIt )
    {
      PTOmni pOmni = (PTOmni)(pSTmp->TopPtr());

      if( pOmni->IsOn() == FALSE ) continue;

      T3DPoint  rptOmni = pOmni->Place();
      TVector vecRVOmni( rptOmni.x, rptOmni.y, rptOmni.z ),
	      vecRVPoint( x, y, rDta.z );
      //TVector vecL( rptOmni.x - x, rptOmni.y - y, rptOmni.z - rDta.z );
      TVector vecL = vecRVOmni - vecRVPoint;

      double d = (!vecL) / pOmni->GetMult();
      vecL = Normalyze( vecL );

      TVector vecV = -vecL;
      TVector vecR( vecV - vecNorm * 2 * (vecV & vecNorm) );
      vecR = Normalyze( vecR );
				     //if( (vecR & vecS) < 0 ) MessageBeep(-1);
      double dSclNL = vecNorm & vecL;
      if( dSclNL <= TRACER_d_ShadingSclrZero ) continue;

      TVector vecLight( pOmni->Rd(), pOmni->Gd(), pOmni->Bd() );
      if( bFlShadows == TRUE && pOmni->Shadow() == TRUE &&
	  pScenaShadow->Shade( vecRVOmni, vecRVPoint, rDta, vecLight ) == TRUE
	) continue;

      double dZerc = dKs * pow(vecR & vecS, dN);
      if( (vecR & vecS) <= TRACER_d_ShadingSclrZero ) dZerc = 0;

      double dM1 = dKd * dSclNL;
      double dMultR = rgbD.dR * dM1 + dZerc,
	     dMultG = rgbD.dG * dM1 + dZerc,
	     dMultB = rgbD.dB * dM1 + dZerc;

	      //double dRKey = dR, dGKey = dG, dBKey = dB;

      float dDst = d + dK;
      dR += (vecLight.x / dDst * dMultR);
      dG += (vecLight.y / dDst * dMultG);
      dB += (vecLight.z / dDst * dMultB);

      /*if( (double(pOmni->R()) / 255.0 / (d + dK) * dMultR) < 0 ||
	  (double(pOmni->G()) / 255.0 / (d + dK) * dMultG) < 0 ||
	  (double(pOmni->B()) / 255.0 / (d + dK) * dMultB) < 0 )
       {
	 BWCCMessageBox( pWin->HWindow, "Colour is negative",
           "Error",
	  MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       }

      if( dRKey > dR || dGKey > dG || dBKey > dB )
       {
	 BWCCMessageBox( pWin->HWindow, "Low lights",
           "Error",
	  MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       }*/

    }

   return TRGBd( dR, dG, dB );
 }

static void near pascal TraceAll( PTSurface& pNearstSurf, PTFacet& pNearstFac,
			          double& z,
			          PTScena pScenaRender,
			          double x_, double y_,
			          DoubleList& rdblRemove,
				  BOOL bFlRemove,
				  RTIntersectBuf rintBuf
				)
 {
   pNearstSurf = NULL;
   pNearstFac = NULL;
   z = INT_MIN;

   rintBuf.Reset();

//TRACE_RESTART1:
   DoubleListIterator dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      if( y_ > pSerf->maxY ) break;        

      if( y_ < pSerf->minY )
       {
         continue;
	 /*if( bFlRemove == FALSE ) continue;

	 pScenaRender->listSurf.detach( *pSerf, TShouldDelete::NoDelete );
	 rdblRemove.add( *pSerf );
	 goto TRACE_RESTART1;*/
       }      

      if( x_ < pSerf->minX || x_ > pSerf->maxX ) continue;

//TRACE_RESTART2:
      DoubleListIterator dbFacet = DoubleListIterator( pSerf->listFacets );
      PTFacet  pFacet = (PTFacet)&dbFacet.current();

      for( ; int(dbFacet); pFacet = (PTFacet)&++dbFacet )
       { 
	 //if( pFacet->vecOutNormal.z < TRACER_d_VectorZOutNormalVisibleReal ) continue;

	 if( y_ > pFacet->maxY ) break;	 
	   
	 if( y_ < pFacet->minY && bFlRemove == TRUE )
	  {
	    continue;
	    /*if( bFlRemove == FALSE ) continue;

	    pSerf->listFacets.detach( *pFacet, TShouldDelete::NoDelete );
	    rdblRemove.add( *pFacet );
	    goto TRACE_RESTART2;*/
	  }

	 if( x_ < pFacet->minX || x_ > pFacet->maxX ) continue;

	 double dZ = 0;

	 int res = pScenaRender->FindIntersectReal( dZ, x_, y_, pFacet, rdblRemove, bFlRemove );
	 if( res != -1 && dZ > z )
	  {
	    z = dZ; pNearstSurf = pSerf; pNearstFac = pFacet;	    
	  }
	 if( res != -1 ) rintBuf.AddIntersect( pSerf, pFacet, dZ );
       }
    }

   rintBuf.Normalyze();
 }


static void near pascal Trace( PTSurface& pNearstSurf, PTFacet& pNearstFac,
			       double& z,
			       PTScena pScenaRender,
			       double x_, double y_,
			       DoubleList& rdblRemove,
			       BOOL bFlRemove
			     )
 {
   pNearstSurf = NULL;
   pNearstFac = NULL;
   z = INT_MIN;

TRACE_RESTART1:
   DoubleListIterator dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      if( y_ > pSerf->maxY ) break;        

      if( y_ < pSerf->minY )
       {
	 if( bFlRemove == FALSE ) continue;

	 pScenaRender->listSurf.detach( *pSerf, TShouldDelete::NoDelete );
	 rdblRemove.add( *pSerf );
	 goto TRACE_RESTART1;
       }      

      if( x_ < pSerf->minX || x_ > pSerf->maxX ) continue;

TRACE_RESTART2:
      DoubleListIterator dbFacet = DoubleListIterator( pSerf->listFacets );
      PTFacet  pFacet = (PTFacet)&dbFacet.current();

      for( ; int(dbFacet); pFacet = (PTFacet)&++dbFacet )
       { 
	 //if( pFacet->vecOutNormal.z < TRACER_d_VectorZOutNormalVisibleReal ) continue;

	 if( y_ > pFacet->maxY ) break;	 
	   
	 if( y_ < pFacet->minY && bFlRemove == TRUE )
	  {
	    if( bFlRemove == FALSE ) continue;

	    pSerf->listFacets.detach( *pFacet, TShouldDelete::NoDelete );
	    rdblRemove.add( *pFacet );
	    goto TRACE_RESTART2;
	  }

	 if( x_ < pFacet->minX || x_ > pFacet->maxX ) continue;

	 double dZ = 0;

	 int res = pScenaRender->FindIntersectReal( dZ, x_, y_, pFacet, rdblRemove, bFlRemove );
	 if( res != -1 && dZ > z )
	  {
	    z = dZ; pNearstSurf = pSerf; pNearstFac = pFacet;
	  }
       }
    }
 }

inline double Rnd()
 {
   return double(rand()) / double(RAND_MAX);
 }

/*
static TVector near pascal TraceWarnok(
		     RTIndexedDoubleList dblLights,
		     PTScena pScena, double x_, double y_, double dCellSize,
		     PTRGBKey prgbkeyStrPrev, int iKeyStrInd, BOOL bFlFirstStr,
		     BOOL& bFlObject,
		     TTracerPhoto& tr,
		     int clm, int str, int iLevel
		   )
 {
   TVector vecRayCorners[ 4 ];
   double dCell4 = dCellSize / 2.0;
   double dCell4Half = dCell4 / 2.0;
   BOOL bKeyObjFlags[ 4 ];
   double dCornersXY[ 4 ][ 2 ] = //center points x, y
    {
      { x_ - dCell4, y_ + dCell4 }, //left top
      { x_ + dCell4, y_ + dCell4 }, //right top
      { x_ - dCell4, y_ - dCell4 }, //left bottom
      { x_ + dCell4, y_ - dCell4 }  //right bottom
    };
   double dCentersXY[ 4 ][ 2 ] =
    {
      { x_ - dCell4Half, y_ + dCell4Half }, //left top
      { x_ + dCell4Half, y_ + dCell4Half }, //right top
      { x_ - dCell4Half, y_ - dCell4Half }, //left bottom
      { x_ + dCell4Half, y_ - dCell4Half }  //right bottom
    };

   bFlObject = FALSE;

   for( int i = 0; i < 4; i++ )
    {
      if( iLevel == 1 ) 
       {
	 if( i == 0 && iKeyStrInd != 0 ) //left top
          {
	    bKeyObjFlags[ i ] = prgbkeyStrPrev[ iKeyStrInd - 1 ].bFlO2;
	    vecRayCorners[i] = prgbkeyStrPrev[ iKeyStrInd - 1 ].vRightTop;
	    continue;
          }

         if( i == 2 && bFlFirstStr == FALSE ) //left bottom
          {
	    bKeyObjFlags[ i ] = prgbkeyStrPrev[ iKeyStrInd ].bFlO1;
	    vecRayCorners[i] = prgbkeyStrPrev[ iKeyStrInd ].vLeftTop;
	    continue;
          }

	 if( i == 3 && bFlFirstStr == FALSE ) //right bottom
          {
	    bKeyObjFlags[ i ] = prgbkeyStrPrev[ iKeyStrInd ].bFlO2;
	    vecRayCorners[i] = prgbkeyStrPrev[ iKeyStrInd ].vRightTop;
	    continue;
	  }
       }

      PTSurface pNearstSurf = NULL;
      PTFacet pNearstFac = NULL;
      double z;
      Trace( pNearstSurf, pNearstFac, z, pScena, dCornersXY[i][0], dCornersXY[i][1] );
      if( pNearstSurf )
       {
	 bKeyObjFlags[ i ] = TRUE;
	 bFlObject = TRUE;		  

	 TTracData dtaRay( pNearstSurf, pNearstFac, z );
	 TRGBd rgbPix = pScena->Shadow( dblLights, dtaRay, dCornersXY[i][0], dCornersXY[i][1] );
	 vecRayCorners[i] = TVector( rgbPix.dR, rgbPix.dG, rgbPix.dB );
       }
      else
       {
	 bKeyObjFlags[ i ] = FALSE;
	 vecRayCorners[i] = tr.GetVectorBkGnd( clm, str );
       }
    }

   if( iLevel == 1 )
     prgbkeyStrPrev[ iKeyStrInd ] = TRGBKey( vecRayCorners[0], bKeyObjFlags[0],
     vecRayCorners[1], bKeyObjFlags[1] );


   if( bFlObject == FALSE ) return tr.GetVectorBkGnd( clm, str );

   double dMaxDeltaRGB = 0;
   double dThsresolRGB = 3.0 * (double(pScena->iMaxCornerDelta) / 255.0);
   for( i = 0; i < 3; i++ )
     for( int j = 1; j < 4; j++ )
      {
	double d = fabs( vecRayCorners[i].x - vecRayCorners[j].x ) +
		   fabs( vecRayCorners[i].y - vecRayCorners[j].y ) +
		   fabs( vecRayCorners[i].z - vecRayCorners[j].z );

	if( d > dThsresolRGB )
	 {
	   if( iLevel >= pScena->iMaxLevel ) break;

	   BOOL bFlObjectLevel = FALSE;
	   for( int k = 0; k < 4; k++ )	    
	     vecRayCorners[k] = TraceWarnok(
		dblLights, pScena,
		dCentersXY[k][0], dCentersXY[k][1], dCell4,
		prgbkeyStrPrev, iKeyStrInd, bFlFirstStr,
		bFlObjectLevel,
		tr,
		clm, str, iLevel + 1 );	    

	   if( bFlObjectLevel == TRUE ) bFlObject = TRUE;
           break;
         }
      }

   return ((vecRayCorners[0] + vecRayCorners[1] + vecRayCorners[2] + vecRayCorners[3]) / 4.0);
 }
*/



PTDIB TScena::RenderPhoto( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog )
 {
   if( polyMethod == PM_StringScan )
     return RenderStrScanPhoto( pInd, rpPctTbl, rTLog );

   if( tmTraceModel == TM_GlobalHolls ) return NULL;

   //TM_Local, TM_GlobalHolls };

   PTUCS pucsKey = pUcs;
   HDC hdcKey = hdc;
   PTWindow pwinKey = pWin;

   double dSc;
   int iSx, iSy;
   RECT r = GetFormatImage( dSc, iSx, iSy );

   TTracer  tr = TTracer( r, hdc, rgbAdges,
     (bFlMarkIntersect == TRUE) ? rgbIntersect:rgbAdges );
   TTracerPhoto  trPhoto = TTracerPhoto( r.right, r.bottom );

   if( !trPhoto.Status() || tr.Status )
    {
      BWCCMessageBox( pWin->HWindow, "Not enought memory for create bitmap",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
      
      return NULL;
    }  


   pInd->AddStr( "Transforming objects" );
   pInd->Show();

   /*pUcs = pucsKey;
   hdc = hdcKey;
   pWin = pwinKey;*/

   TimeLog tlLog2;
   rTLog.StartCopy();

   PTScena  pScenaRender = new TScena( *this, pInd, TRUE, FALSE );
   pScenaRender->bFlLeftUCS = FALSE;
   //pInd->Init( pScenaRender->GetBytesSize() );
   if( !pScenaRender )
    {
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == -1 )
     {
       delete pScenaRender;
       BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
         "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
       
       return NULL;
     }

    if( pScenaRender->Status == 1 )
     {
       delete pScenaRender;
       

       return NULL;
     }

   rTLog.EndCopy();
   rTLog.StartTransform();

   pScenaRender->pUcs = pucsKey;
   pScenaRender->hdc = hdcKey;
   pScenaRender->pWin = pwinKey;

   pScenaRender->RecalcUserToBase();
   ScaleLights( pScenaRender->listSurf, pScenaRender->pUcs->GetScale() );
   

   pInd->UpdateStr( 2, "Calc normals" );
   pInd->Init( pScenaRender->listSurf.getItemsInContainer() );

   if( pScenaRender->ListAdgeNormalyzeRev( pInd ) == FALSE )
    {
      delete pScenaRender;
      return  NULL;
    }   

   DoubleListIterator  dbIter = DoubleListIterator( pScenaRender->listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();
   for( LONG lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )  
     {       
       pSerf->CalcNormals();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  //GetSwap( pInd );
	  
	  delete pScenaRender;
	  //pInd->Close();
          return  NULL;
        }
     }

   pInd->WorkIs100();

   

   pInd->UpdateStr( 2, "Calc bounds" );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );

   dbIter = DoubleListIterator( pScenaRender->listSurf );
   pSerf = (PTSurface)&dbIter.current();
   for( lWork = 0 ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     {
       pSerf->CalcBounds();      
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}

       pSerf->CalcBoundsAllFacets();
       if( pInd->WorkMessage( ++lWork ) == TRUE )
	{
	  delete pScenaRender;
          return  NULL;
	}
     }

   //pScenaRender->TestToFileBounds( "g:\\sort1.txt" );

   rTLog.EndTransform();   

   pInd->WorkIs100();

   rTLog.StartOptimize();

   TIndexedDoubleList dblLights, dblCameras;
   pScenaRender->Separate( dblLights, dblCameras );

   pInd->UpdateStr( 2, "Optimize body data..." );
   pInd->Init( 2 * pScenaRender->listSurf.getItemsInContainer() );
   //pScenaRender->listSurf.Sort( (PTCompareFunc)&TSurface::CompareY, pInd );
   pScenaRender->listSurf.Sort( CompareSurfaceYRev, pInd );

   pScenaRender->SortFacets( CompareFacetYRev, pInd );
   pScenaRender->SortAdges( CompareAdgeItemYRev, pInd );


   pScenaRender->CalcBounds();

   rTLog.EndOptimize();
   rTLog.StartPrepareRender();
   
   if( pScenaRender->RenderPrepareBodys( pInd ) == FALSE )
    {
      delete pScenaRender;
      return  NULL;
    }

   rTLog.EndPrepareRender();
   

   /*DoubleListIterator dblIt__(pScenaRender->listSurf);
   PTSurface pSf__ = (PTSurface)&dblIt__.current();
   for( ; int(dblIt__); pSf__ = (PTSurface)&++dblIt__ )
    {
      int a = 0;
    }*/

   //pScenaRender->TestToFileBounds( "g:\\sort2.txt" );


   double dXStaBmp, dYStaBmp, dXEndBmp, dYEndBmp;
   dXStaBmp = (pScenaRender->dMinX - 1) * dSc;
   dYStaBmp = (pScenaRender->dMaxY + 1) * dSc;
   pScenaRender->SpecialPointLPtoDP( dXStaBmp, dYStaBmp, r.right, r.bottom, dSc );		
   dXStaBmp--; dYStaBmp--;
   pScenaRender->SpecialBounds( dXStaBmp, dYStaBmp, r );
   dXStaBmp = floor( dXStaBmp );
   dYStaBmp = ceil( dYStaBmp );

   dXEndBmp = (pScenaRender->dMaxX + 1)* dSc;
   dYEndBmp = (pScenaRender->dMinY - 1) * dSc;
   pScenaRender->SpecialPointLPtoDP( dXEndBmp, dYEndBmp, r.right, r.bottom, dSc );
   dXEndBmp++; dYEndBmp++;
   pScenaRender->SpecialBounds( dXEndBmp, dYEndBmp, r );
   dXEndBmp = ceil( dXEndBmp );
   dYEndBmp = floor( dYEndBmp );

   double dXSta = dXStaBmp, dYSta = dYStaBmp,
	  dXEnd = dXEndBmp, dYEnd = dYEndBmp;	   
   pScenaRender->SpecialPointDPtoLP( dXSta, dYSta, r.right, r.bottom, dSc );
   pScenaRender->SpecialPointDPtoLP( dXEnd, dYEnd, r.right, r.bottom, dSc );
   dXSta /= dSc; dYSta /= dSc;
   dXEnd /= dSc; dYEnd /= dSc;

   double dPix = (fabs(dXEnd - dXSta)) / (fabs(dXEndBmp - dXStaBmp) + 1);
   double dHalfPix = dPix /  2;
   double x_ = dXSta + dHalfPix, y_ = dYSta - dHalfPix;
      

   RECT rTrBounds;
   rTrBounds.left = dXStaBmp; rTrBounds.right = dXEndBmp;
   rTrBounds.top = dYStaBmp; rTrBounds.bottom = dYEndBmp;

   tr.Bounds( rTrBounds );

   rTLog.StartInitBkGnd();

   pInd->UpdateStr( 2, "Init backgraund..." );
   tr.Start( mrcBackGraund(), pInd );

   rTLog.EndInitBkGnd();

   
   PTSurface pNearstSurf = NULL;
   PTFacet pNearstFac = NULL;
   double z = INT_MIN;
   LONG  lWorking = 1;

   static DWORD dwMeanCount = 0, dwNumbTr = 0;   


   rTLog.StartCopyShadows();

   TAutoMemoryRc<PTScena> mrcShade;
   if( bFlShadows == TRUE )
    {
      pInd->UpdateStr( 2, "Transforming objects" );
      pInd->Init( pScenaRender->GetBytesSize() );
      mrcShade = new TScena( *pScenaRender, pInd, TRUE, FALSE );
      if( !mrcShade )
       {
	 delete pScenaRender;
         return  NULL;
       }

      RemoveHorizAdgesItems( mrcShade()->listSurf, TestHoriz );
      mrcShade()->bFlLeftUCS = FALSE;
      mrcShade()->InitStrScanParamOfAdgeItems( INT_MAX );
      mrcShade()->pFIntersect = &TScena::FindIntersectReal;

      mrcShade()->CalcAllTraceObol( pInd );
    }

   rTLog.EndCopyShadows();

   tlLog2.Reset();
   tlLog2.StartPrepareRender();

   RemoveBackFacesAndHorizAdgesItems( pScenaRender->listSurf, TestVisibleRayTraceReal, TestHoriz );
   DoubleList dblRemovTrace;

   pScenaRender->InitStrScanParamOfAdgeItems( dYSta + 100 );

   tlLog2.EndPrepareRender();
   rTLog += tlLog2;

   

   int iEY = dYEndBmp, iEX = dXEndBmp;
   if( dYStaBmp != iEY && dXStaBmp != iEX )
    {
      tlLog2.Reset();
      tlLog2.StartPrepareRender();

      trPhoto.IntBuf( tr.Buffer() );

      tlLog2.EndPrepareRender();
      rTLog += tlLog2;

      rTLog.StartRender();

      pInd->UpdateStr( 2, "Render..." );
      pInd->Init(  labs( (LONG)rTrBounds.bottom - (LONG)rTrBounds.top + 1L ) *
	           labs( (LONG)rTrBounds.right - (LONG)rTrBounds.left + 1L )
		);

      BOOL bFlTransparency = pScenaRender->TransparencyTest();
      TIntersectBuf interBuf( 50, 10 );
      interBuf.Direction( FALSE );

      if( bFlAntiAliasing == TRUE )
       {
	 /*pScenaRender->iMaxCornerDelta = 40;
	 pScenaRender->iMaxLevel = 4;

	 TAutoMemoryRc<PTRGBKey> mrcRGBKeyStr = new TRGBKey[ abs(iEX - dXStaBmp) + 1 ];
	 int iKeyStrInd = 0;
	 if( !mrcRGBKeyStr )
	  {
	    delete pScenaRender;
            BWCCMessageBox( pWin->HWindow, "Not enought memory for transform",
              "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL
			  );
	    return NULL;
	  }

	 for( int str = dYStaBmp; str <= iEY; str++, y_ -= dPix )
	  {
	    x_ = dXSta + dHalfPix; iKeyStrInd = 0;
	    for( int clm = dXStaBmp; clm <= iEX; clm++, lWorking++, x_ += dPix, iKeyStrInd++ )
	     {
	       BOOL bFlObject = FALSE;

	       TVector rgbVec = TraceWarnok( dblLights,
		 pScenaRender, x_, y_, dPix,
		 mrcRGBKeyStr(), iKeyStrInd, (str == dYStaBmp) ? TRUE:FALSE,
		 bFlObject,
		 trPhoto, clm, str, 1 );


	       if( bFlObject == TRUE )
		 trPhoto.PixelAbs( clm, str, rgbVec.x, rgbVec.y, rgbVec.z );

	       if( pInd->WorkMessage( lWorking ) == TRUE )
		{
		  delete pScenaRender;
		  return NULL;
		}
	     }
          }*/


	  double dCell = dPix / double(iNumbPointsAntiAlias);
	 //double dCell = dPix / (double)TRACER_i_AntialiasingNumbCells;
	 randomize();
	 double dVariance = double(iNumbPointsAntiAlias) * double(iNumbPointsAntiAlias) *
			    dStrengtsAntialias;

	 for( int str = dYStaBmp; str <= iEY; str++, y_ -= dPix )
	  {
	    x_ = dXSta + dHalfPix;
	    for( int clm = dXStaBmp; clm <= iEX; clm++, lWorking++, x_ += dPix )
	     {

	       double dPixLeftX = x_ - dHalfPix,
		      dPixLeftY = y_ + dHalfPix;
	       int iCount = 0;
	       TVector vecColor, vecSumm, vecBuffer;
	       double dDispers = 0;               
	       double d;
				     dwNumbTr++;
	       BOOL bFlObj = FALSE;
	       do
		{                     
		  double dCellX = dPixLeftX, dCellY = dPixLeftY;
		  for( int iCellStr = iNumbPointsAntiAlias; iCellStr > 0; iCellStr--, dCellY -= dCell )
		   {
		     dCellX = dPixLeftX;
		     for( int iCellClm = iNumbPointsAntiAlias; iCellClm > 0; iCellClm--, dCellX += dCell )
		      {
			double dDx = Rnd() * dCell,
			       dDy = Rnd() * dCell;

			
			if( bFlTransparency == FALSE )
			 {
			   Trace( pNearstSurf, pNearstFac, z, pScenaRender, dCellX + dDx, dCellY + dDy, dblRemovTrace, FALSE );
			   if( pNearstSurf )
			    {
			      double dZNorm;

			      bFlObj = TRUE;

			      TTracData dtaRay( pNearstSurf, pNearstFac, z );
			      TRGBd rgbPix = pScenaRender->Shadow( dblLights, dtaRay, dCellX + dDx, dCellY + dDy, mrcShade(), dZNorm );
			      vecColor = TVector( rgbPix.dR, rgbPix.dG, rgbPix.dB );

			      vecSumm += vecColor;
			      dDispers += vecColor & vecColor;
			    }
			   else
			    {
			      vecColor = trPhoto.GetVectorBkGnd( clm, str );
			      vecSumm += vecColor;
			      dDispers += vecColor & vecColor;
			    }
			 }
			else
			 {
			   TraceAll( pNearstSurf, pNearstFac, z, pScenaRender, dCellX + dDx, dCellY + dDy, dblRemovTrace, FALSE, interBuf );
			   if( interBuf.Status() == FALSE )
			    {
			      delete pScenaRender;
			      BWCCMessageBox( NULL, "Not enought memory for trace",
				"Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

			      return NULL;
			    }

			   if( pNearstSurf )
			    {
			      bFlObj = TRUE;

			      int iIndBottom = interBuf.BottomIndex();
			      PTTracData pBotom = interBuf.Get( iIndBottom );

			      TVector vecRefract;
			      if( !pBotom ) vecRefract = trPhoto.GetVectorBkGnd( clm, str );
			      else
			       {
				 double dZNorm;
			         
				 TRGBd rgbPix = pScenaRender->Shadow( dblLights, *pBotom, dCellX + dDx, dCellY + dDy, mrcShade(), dZNorm );
				 vecRefract = TVector( rgbPix.dR, rgbPix.dG, rgbPix.dB );
			       }

			      PTTracData pCurDta;
			      interBuf.Start( iIndBottom );
			      while( (pCurDta = interBuf.Prev()) )
			       {
				 double dZNorm;
			                              
				 TRGBd rgbPix = pScenaRender->Shadow( dblLights, *pCurDta, dCellX + dDx, dCellY + dDy, mrcShade(), dZNorm );
				 TVector vecCur( rgbPix.dR, rgbPix.dG, rgbPix.dB );
				 double t = pCurDta->psfSurf->GdKt( dZNorm );
				 vecRefract = vecCur * (1 - t) + vecRefract * t;
			       }

			      vecSumm += vecRefract;
			      dDispers += vecRefract & vecRefract;			      
			    }
			   else
			    {
			      vecColor = trPhoto.GetVectorBkGnd( clm, str );
			      vecSumm += vecColor;
			      dDispers += vecColor & vecColor;
                            }
			 }
			iCount++;
		      }
		   }

		  vecBuffer = vecSumm / double(iCount);

		  d = double(iCount) *
		    (dDispers / double(iCount) - (vecBuffer & vecBuffer)) /
		    double(iCount - 1);

		} while( bFlObj == TRUE && d >= dVariance && iCount < 99 );
				      dwMeanCount += iCount;
	       if( bFlObj == TRUE )
		 trPhoto.PixelAbs( clm, str, vecBuffer.x, vecBuffer.y, vecBuffer.z );

	       if( pInd->WorkMessage( lWorking ) == TRUE )
		{
		  delete pScenaRender;
		  return NULL;
		}
	     }
	  }
       }
      else
       {
         for( int str = dYStaBmp; str <= iEY; str++, y_ -= dPix )
	  {

	    x_ = dXSta + dHalfPix;
	    for( int clm = dXStaBmp; clm <= iEX; clm++, lWorking++, x_ += dPix )
	     {

	       if( bFlTransparency == FALSE )
                {
		  Trace( pNearstSurf, pNearstFac, z, pScenaRender, x_, y_, dblRemovTrace, TRUE );

		  if( pNearstSurf )
		   {
		     double dZNorm;

		     TTracData dtaRay( pNearstSurf, pNearstFac, z );
		     TRGBd rgbPix = pScenaRender->Shadow( dblLights, dtaRay, x_, y_, mrcShade(), dZNorm );

		     trPhoto.PixelAbs( clm, str, rgbPix.dR, rgbPix.dG, rgbPix.dB );
		   }
		}
	       else
		{          
		  TraceAll( pNearstSurf, pNearstFac, z, pScenaRender, x_, y_, dblRemovTrace, TRUE, interBuf );
		  if( interBuf.Status() == FALSE )
		   {
		     delete pScenaRender;
		     BWCCMessageBox( NULL, "Not enought memory for trace",
		       "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

		     return NULL;
		   }

		  if( pNearstSurf )
                   {
		     int iIndBottom = interBuf.BottomIndex();
		     PTTracData pBotom = interBuf.Get( iIndBottom );

		     TVector vecRefract;
		     if( !pBotom ) vecRefract = trPhoto.GetVectorBkGnd( clm, str );
		     else
		      {
			double dZNorm;

			TRGBd rgbPix = pScenaRender->Shadow( dblLights, *pBotom, x_, y_, mrcShade(), dZNorm );
			vecRefract = TVector( rgbPix.dR, rgbPix.dG, rgbPix.dB );
		      }

		     PTTracData pCurDta;
		     interBuf.Start( iIndBottom );
		     while( (pCurDta = interBuf.Prev()) )
		      {
			double dZNorm;

			TRGBd rgbPix = pScenaRender->Shadow( dblLights, *pCurDta, x_, y_, mrcShade(), dZNorm );
			TVector vecCur( rgbPix.dR, rgbPix.dG, rgbPix.dB );
			double t = pCurDta->psfSurf->GdKt( dZNorm );
			vecRefract = vecCur * (1 - t) + vecRefract * t;
		      }

		     trPhoto.PixelAbs( clm, str, vecRefract.x, vecRefract.y, vecRefract.z );
		   }
		}

	       if( pInd->WorkMessage( lWorking ) == TRUE )
		{
		  delete pScenaRender;
		  return NULL;
		}
	     }
	  }
       }

      rTLog.EndRender();

      trPhoto.ConvertToRGB( tr.Buffer(), bFlStrictGamma );
      trPhoto.Free();
      dblRemovTrace.flush( TShouldDelete::Delete );
    }

	if( dwNumbTr ) dwMeanCount = dwMeanCount / dwNumbTr;

   pInd->WorkIs100();   

   mrcShade.FreeRc();

   TNoisyDta ndDta;   pScenaRender->GetNoisy( ndDta );
   delete pScenaRender;

   //PTColorTable pctColorTbl = pScenaRender->CreateColorTbl();

   if( !iPaletteSize )
    {
      rpPctTbl = NULL;

      return new TDIB( tr.Buffer(), r.right, r.bottom, NULL, 0, TRUE, pApplicat );
      //return tr.CreateGetBitmap( NULL, 0 );
    }


   int iPS = iPaletteSize ? ((iPaletteSize == -16) ? 16:iPaletteSize):236;
   TConvertAlho alho = (iPaletteSize == -16) ? CA_System:caAlhoPalette;

   rTLog.StartCalcPalette();

   if( iPS )
    {
      pInd->SetTitle( "Convert image to indexed" );
      rpPctTbl = TrueColorTo256( tr.Buffer(), tr.SizeBuf(), iPS,
	iPaletteSize ? FALSE:TRUE, alho, r.right, r.bottom,
        &ndDta,
	pInd );
    }
   else rpPctTbl = NULL;

   rTLog.EndCalcPalette();

   PTDIB pRetDIB = new TDIB( tr.Buffer(), r.right, r.bottom,
     iPaletteSize ? rpPctTbl:NULL,
     (iPaletteSize == -16) ? 16:iPaletteSize,
     TRUE, pApplicat );
   //HGLOBAL hRetDIB = tr.CreateGetBitmap( iPaletteSize ? rpPctTbl:NULL,
     //(iPaletteSize == -16) ? 16:iPaletteSize );
   if( rpPctTbl && iPaletteSize )
    {
      delete []rpPctTbl; rpPctTbl = NULL;
    }

   return pRetDIB;

 }
#pragma option -G.

#pragma option -G

TTracer::~TTracer()
 {
   if( pBuf[0] ) { delete [](pBuf[0]); pBuf[0] = NULL; }
   if( pBuf[1] ) { delete [](pBuf[1]); pBuf[1] = NULL; }

   if( videoBuf )
    {
      farfree( videoBuf );
      videoBuf = NULL;
    }
 }

TTracer::TTracer( RECT& r, HDC hdc, RGBQUAD& rgbAdgeColor,
		  RGBQUAD& rgbIntersectColor ):
  rgbAdge( rgbAdgeColor ),
  rgbIntersect( rgbIntersectColor )
 {
   this->hdc = hdc;
   wid = r.right; high = r.bottom;
    
   if( !(videoBuf = (TRGB huge *)farcalloc( 1, sizeof(TRGB) * (DWORD)r.right * (DWORD)r.bottom)) )
     Status = TRUE;
   else Status = FALSE;

   pBuf[0] = new TTracData[ wid + 1 ];
   pBuf[1] = new TTracData[ wid  + 1 ];
   if( !pBuf[0] || !pBuf[1] ) Status = TRUE;

   if( pBuf[0] ) _fmemset( pBuf[0], 0, sizeof(TTracData) * (wid + 1) );
   if( pBuf[1] ) _fmemset( pBuf[1], 0, sizeof(TTracData) * (wid + 1) );

   bounds.left = bounds.top = 0;
   bounds.right = r.right - 1;
   bounds.bottom = r.bottom - 1;
 }

/*
HGLOBAL  near pascal TTracer::CreateGetBitmap( PTColorTable pctTbl, int iPalSize ) const
 {
   HGLOBAL      hbmInfo;
   LPTDiskDIBBitmap  pbiInfo;


   DWORD  dwLenPal = iPalSize;
   DWORD  dwBytePerPixel = pctTbl ? 1:3;
   DWORD  dwBitCount = dwBytePerPixel * 8;


   DWORD dwSize;		
   if( !(hbmInfo = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
			       (dwSize =
			       sizeof(BITMAPINFOHEADER) +
                               sizeof(BITMAPFILEHEADER) +
			       sizeof(RGBQUAD) * dwLenPal +
			       (((DWORD)dwBitCount * (DWORD)wid + 31UL) / 32UL*4UL) * (DWORD)high
			       //(DWORD)wid * (DWORD)high * dwBytePerPixel
                                )
			      )
	)
     )  return NULL;
   pbiInfo = (LPTDiskDIBBitmap)GlobalLock( hbmInfo );

   if( !pbiInfo )
    {
      GlobalUnlock( hbmInfo );
      GlobalFree( hbmInfo );

      BWCCMessageBox( NULL, "Not enought memory for bitmap",
	"Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return NULL;
    }


   pbiInfo->bmFile.bfType = 0x4D42;
   pbiInfo->bmFile.bfSize = dwSize;
   pbiInfo->bmFile.bfOffBits = 
                               sizeof(BITMAPINFOHEADER) +
                               sizeof(BITMAPFILEHEADER) +
			       sizeof(RGBQUAD) * dwLenPal;

   pbiInfo->bmInf.bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
   pbiInfo->bmInf.bmiHeader.biWidth          = wid;
   pbiInfo->bmInf.bmiHeader.biHeight         = high;
   pbiInfo->bmInf.bmiHeader.biPlanes         = 1;
   //pbiInfo->bmInf.bmiHeader.biBitCount       = GetDeviceCaps( hdc, BITSPIXEL ) *
   //					       GetDeviceCaps( hdc, PLANES );
   pbiInfo->bmInf.bmiHeader.biBitCount       = dwBitCount;
   pbiInfo->bmInf.bmiHeader.biCompression    = BI_RGB;
   //pbiInfo->bmInf.bmiHeader.biSizeImage      = (DWORD)wid * (DWORD)high;
   //pbiInfo->bmInf.bmiHeader.biSizeImage      = (DWORD)wid * (DWORD)high * dwBytePerPixel;
   pbiInfo->bmInf.bmiHeader.biSizeImage      = (((DWORD)dwBitCount * (DWORD)wid + 31UL) / 32UL*4UL) * (DWORD)high;
   pbiInfo->bmInf.bmiHeader.biXPelsPerMeter  = 0;
   pbiInfo->bmInf.bmiHeader.biYPelsPerMeter  = 0;
   pbiInfo->bmInf.bmiHeader.biClrUsed        = dwLenPal;
   pbiInfo->bmInf.bmiHeader.biClrImportant   = pctTbl ? 0:256;
	


   if( pctTbl )
     _fmemcpy( pbiInfo->bmInf.bmiColors, pctTbl, sizeof(RGBQUAD) * dwLenPal );


   //BYTE huge *pBits = ((BYTE huge *)pbiInfo) + pbiInfo->bmFile.bfOffBits;


   int widAlign = wid * dwBytePerPixel;
   while( widAlign % 4 ) widAlign++;

   if( pctTbl )
    {
      for( int iStr = 0; iStr < high; iStr++ )
       {

         TRGB huge *pSrc = videoBuf + (DWORD)wid * (DWORD)high -
			   (DWORD)iStr * (DWORD)wid - (DWORD)wid;
	 BYTE huge *pBits = ((BYTE huge *)pbiInfo) + (DWORD)pbiInfo->bmFile.bfOffBits +
			    (DWORD)iStr * (DWORD)widAlign;

	 for( int iClm = 0; iClm < wid; iClm++, pBits++, pSrc++  )
	   *pBits = pSrc->index;
       }
    }
   else
    {
      for( int iStr = 0; iStr < high; iStr++ )
       {

         TRGB huge *pSrc = videoBuf + (DWORD)wid * (DWORD)high -
			   (DWORD)iStr * (DWORD)wid - (DWORD)wid;
	 BYTE huge *pBits = ((BYTE huge *)pbiInfo) + (DWORD)pbiInfo->bmFile.bfOffBits +
			    (DWORD)iStr * (DWORD)widAlign;

	 for( int iClm = 0; iClm < wid; iClm++, pBits += 3, pSrc++  )
	  {
	    //*(TRGB huge *)pBits = *pSrc;
	    *pBits = pSrc->b;
	    *(pBits+1) = pSrc->g;
	    *(pBits+2) = pSrc->r;
          }
       }
    }

   GlobalUnlock( hbmInfo );

   return hbmInfo;
 }*/

/*TRGB _FAR* NEAR TTracer::GetCol( BOOL checkX, BOOL checkY ) const
 {
   static _FAR TRGB rgb;

   if( checkX == FALSE && checkY == FALSE )
    {
      if( !pBuf[str][clm].pFac ) return NULL;//coFon;
      //else return pBuf[str][clm].psfSurf->GetFaceColorIndex();      
    }
   else
   if( checkX == TRUE && checkY == FALSE )
    {
      if( pBuf[str][clm].psfSurf != pBuf[str][clm-1].psfSurf )
       {
	 if( !pBuf[str][clm].psfSurf || !pBuf[str][clm-1].psfSurf )
	   rgb = rgbAdge;
	 else if( fabs(pBuf[str][clm].z - pBuf[str][clm-1].z) < 1 )
	   rgb = rgbIntersect;
	 else rgb = rgbAdge;
         return &rgb;
	//return coIntersect;
       }
      else if( pBuf[str][clm].pFac != pBuf[str][clm-1].pFac )
       {
	 rgb = rgbAdge;
	 return &rgb;
	//return coAdge;
       }
      else if( !pBuf[str][clm].pFac ) return NULL;//coFon;
      //else return pBuf[str][clm].psfSurf->GetFaceColorIndex();      
    }
   else
   if( checkX == FALSE && checkY == TRUE )
    {
      if( pBuf[str][clm].psfSurf != pBuf[0][clm].psfSurf )
       {
	 if( !pBuf[str][clm].psfSurf || !pBuf[0][clm].psfSurf )
	   rgb = rgbAdge;
	 else if( fabs(pBuf[str][clm].z - pBuf[0][clm].z) < 1 )
	   rgb = rgbIntersect;
	 else rgb = rgbAdge;
         return &rgb;
	//return coIntersect;
       }
      else if( pBuf[str][clm].pFac != pBuf[0][clm].pFac )
       {
	 rgb = rgbAdge;
	 return &rgb;
	//return coAdge;
       }
      else if( !pBuf[str][clm].pFac ) return NULL;//coFon;
      //else return pBuf[str][clm].psfSurf->GetFaceColorIndex();     
    }
   else
   if( checkX == TRUE && checkY == TRUE )
    {
      if( pBuf[1][clm].psfSurf != pBuf[0][clm].psfSurf )
       {
	 if( !pBuf[1][clm].psfSurf || !pBuf[0][clm].psfSurf )
	   rgb = rgbAdge;
	 else if( fabs(pBuf[1][clm].z - pBuf[0][clm].z) < 1 )
	   rgb = rgbIntersect;
	 else rgb = rgbAdge;
         return &rgb;
	//return coIntersect;
       }
      else if( pBuf[1][clm].pFac != pBuf[0][clm].pFac )
       {
	 rgb = rgbAdge;
	 return &rgb;
	//return coAdge;
       }
      else if( pBuf[str][clm].psfSurf != pBuf[str][clm-1].psfSurf )
       {	 
         if( !pBuf[str][clm].psfSurf || !pBuf[str][clm-1].psfSurf )
	   rgb = rgbAdge;
	 else if( fabs(pBuf[str][clm].z - pBuf[str][clm-1].z) < 1 )
	   rgb = rgbIntersect;
	 else rgb = rgbAdge;
         return &rgb;
	//return coIntersect;
       }
      else if( pBuf[str][clm].pFac != pBuf[str][clm-1].pFac )
       {
	 rgb = rgbAdge;
	 return &rgb;
	//return coAdge;
       }
      else if( !pBuf[str][clm].pFac ) return NULL;//coFon;
      //else return pBuf[str][clm].psfSurf->GetFaceColorIndex();
    }

   pBuf[str][clm].psfSurf->GetRGB( rgb );
   return &rgb;

 }*/

void near pascal TTracer::Start( PTBackGraund pBkGnd, PTIndicator pInd )
 {
   //clm = str = 0;
   clm = bounds.left;
   str = bounds.top;
   if( pBkGnd )
     pBkGnd->Init( videoBuf, NULL, 0, wid, high, pInd );
 }

TRGB near pascal TTracer::GetColor( PTTracData p1, PTTracData p2, TColorClass& tc ) const
 {
   /*switch( tc )
    {
      case TC_Surface:
	return TRGB( rSf.GetFaceRGBColor() );

      case TC_Fon:
	return TRGB( 0, 0, 0 );

      case TC_Adge:
	return rgbAdge;

      case TC_Intersect:
	return rgbIntersect;

      default:
	return TRGB( 0, 0, 0 );
    }*/


   if( p2 )
    {	  
      if( p1->psfSurf != p2->psfSurf )
       {
         if( !p1->psfSurf || !p2->psfSurf )
          {
	    tc = TTracer::TC_Adge;
	    return rgbAdge;
          }		
         else
          {
	    tc = (fabs(p1->z - p2->z) <= 1) ? TTracer::TC_Intersect:
		 TTracer::TC_Adge;
	    return rgbIntersect;
          }      
       }
      else if( p1->psfSurf && p1->pFac != p2->pFac )
       {
	 tc = TTracer::TC_Adge;
         return rgbAdge;
       }
      else if( p1->psfSurf )
       {
	 tc = TTracer::TC_Surface;
	 return TRGB( p1->psfSurf->GetFaceRGBColor() );
       }
      else
       {
	 tc = TTracer::TC_Fon;
	 return TRGB( 0, 0, 0 );
       }
    }
   else if( p1->psfSurf )
    {
      tc = TTracer::TC_Surface;
      return TRGB( p1->psfSurf->GetFaceRGBColor() );
    }    
   else
    {
      tc = TTracer::TC_Fon;
      return TRGB( 0, 0, 0 );
    }
 }

void near pascal TTracer::Pixel( int x, int y, PTSurface pF, PTFacet pFac, double z )
 {
   pBuf[1][x] = TTracData( pF, pFac, z );

   //if( x == wid - 1 )
   if( x == bounds.right ) NextStr( y );
 }


void near pascal TTracer::NextStr( int iYCurStr, BOOL bFlInitBuf )
 {   
   if( iYCurStr != bounds.top )
    {
      //PTTracData p1 = pBuf[0],
      //	    p2 = pBuf[1];
      PTTracData p1 = pBuf[0] + bounds.left,
	         p2 = pBuf[1] + bounds.left;
      TRGB huge* pDst = videoBuf + (DWORD)(iYCurStr - 1) * (DWORD)wid + (DWORD)bounds.left;
      //for( int i = wid; i > 0; i--, p1++, p2++, pDst++ )
      for( int i = bounds.right - bounds.left + 1; i > 0; i--, p1++, p2++, pDst++ )
       {
         TRGB rgb1, rgb2;
	 TColorClass tc1, tc2;

	 if( i > 1 ) rgb1 = GetColor( p1, p1 + 1, tc1 );
	 else rgb1 = GetColor( p1, NULL, tc1 );

	 rgb2 = GetColor( p1, p2, tc2 );

	 if( tc1 == TTracer::TC_Adge || tc1 == TTracer::TC_Intersect ) *pDst = rgb1;
	 else if( tc2 == TTracer::TC_Adge || tc2 == TTracer::TC_Intersect ) *pDst = rgb2;
	 else if( tc1 != TTracer::TC_Fon ) *pDst = rgb1;
       }
    }

   _fmemcpy( pBuf[0], pBuf[1], sizeof(TTracData) * wid );
   if( bFlInitBuf == TRUE )
     _fmemset( pBuf[1], 0, sizeof(TTracData) * wid );
 }

void  near pascal TTracer::PixelSmart( int x, PTSurface pF, PTFacet pFac, double z )
 {
   RTTracData rDta = *(pBuf[1] + x);

   if( !rDta.psfSurf || rDta.z < z ) rDta = TTracData( pF, pFac, z );
 }

void near pascal TTracer::PixelAbs( int x, int y, RTRGB rRGB )
 {
   videoBuf[ DWORD(y) * DWORD(wid) + DWORD(x) ] = rRGB;
 }

void near pascal TTracer::End()
 {
   //PTTracData p1 = pBuf[0];
   PTTracData p1 = pBuf[0] + bounds.left;
   //TRGB huge* pDst = videoBuf + (DWORD)(high - 1) * (DWORD)wid;
   //TRGB huge* pDst = videoBuf + (DWORD)(high - 1) * (DWORD)wid + (DWORD)bounds.left;
   TRGB huge* pDst = videoBuf + (DWORD)bounds.bottom * (DWORD)wid + (DWORD)bounds.left;
   //for( int i = wid; i > 0; i--, p1++, pDst++ )
   for( int i = bounds.right - bounds.left + 1; i > 0; i--, p1++, pDst++ )
    {
      TRGB rgb1;
      TColorClass tc1;

      if( i > 1 ) rgb1 = GetColor( p1, p1 + 1, tc1 );
      else rgb1 = GetColor( p1, NULL, tc1 );
	    
      if( tc1 != TC_Fon ) *pDst = rgb1;
    }
 }

void near pascal TTracer::NextStrBack( int iYCurStr, BOOL bFlInitBuf )
 {
   if( iYCurStr != bounds.bottom )
    {
      PTTracData p1 = pBuf[1] + bounds.left,
	         p2 = pBuf[0] + bounds.left;
      TRGB huge* pDst = videoBuf + (DWORD)(iYCurStr + 1) * (DWORD)wid + (DWORD)bounds.left;

      for( int i = bounds.right - bounds.left + 1; i > 0; i--, p1++, p2++, pDst++ )
       {
         TRGB rgb1, rgb2;
	 TColorClass tc1, tc2;

	 if( i > 1 ) rgb1 = GetColor( p1, p1 + 1, tc1 );
	 else rgb1 = GetColor( p1, NULL, tc1 );

	 rgb2 = GetColor( p1, p2, tc2 );

	 if( tc1 == TTracer::TC_Adge || tc1 == TTracer::TC_Intersect ) *pDst = rgb1;
	 else if( tc2 == TTracer::TC_Adge || tc2 == TTracer::TC_Intersect ) *pDst = rgb2;
	 else if( tc1 != TTracer::TC_Fon ) *pDst = rgb1;
       }
    }

   _fmemcpy( pBuf[1], pBuf[0], sizeof(TTracData) * wid );
   if( bFlInitBuf == TRUE )
     _fmemset( pBuf[0], 0, sizeof(TTracData) * wid );
 }

void near pascal TTracer::EndBack()
 {
   PTTracData p1 = pBuf[0] + bounds.left;
   
   TRGB huge* pDst = videoBuf + (DWORD)bounds.top * (DWORD)wid + (DWORD)bounds.left;
   for( int i = bounds.right - bounds.left + 1; i > 0; i--, p1++, pDst++ )
    {
      TRGB rgb1;
      TColorClass tc1;

      if( i > 1 ) rgb1 = GetColor( p1, p1 + 1, tc1 );
      else rgb1 = GetColor( p1, NULL, tc1 );
	    
      if( tc1 != TC_Fon ) *pDst = rgb1;
    }
 }

/*void NEAR TTracer::Pixel( int x, int y, PTSurface pF, PTFacet pFac, double z )
 {
   TRGB _FAR* prgbCur = NULL;

   if( !str )
    {
      pBuf[0][x].psfSurf = pF; clm = x;
      pBuf[0][x].pFac = pFac;
      pBuf[0][x].z = z;
      if( clm < 1 ) prgbCur = GetCol( FALSE, FALSE );//cur. status
      else prgbCur = GetCol( TRUE, FALSE );
    }
   else
    {
      pBuf[1][x].psfSurf = pF; clm = x;
      pBuf[1][x].pFac = pFac;
      pBuf[1][x].z = z;
      if( clm < 1 ) prgbCur = GetCol( FALSE, TRUE );
      else prgbCur = GetCol( TRUE, TRUE );
    }

   //SetPixel( hdc, x, y, coCur );
   if( prgbCur )
     videoBuf[(DWORD)y * (DWORD)wid + (DWORD)x] = *prgbCur;

   if( clm == wid - 1 )
    {
      if( str ) _fmemmove( pBuf[0], pBuf[1], sizeof(TTracData) * wid );
      clm = 0; str = 1;
    }
 }*/




void near pascal TTracerPhoto::IntBuf( TRGB  huge* pRGBBuf )
 {
   PTRGBd pBuf = rgbBuf();
   for( DWORD i = dwSize; i > 0; i--, pBuf++, pRGBBuf++ )
    {
      TRGB t; hmemcpy( &t, pRGBBuf, sizeof(TRGB) );
      *pBuf = TRGBd( float(t.r) / 255.0,
		     float(t.g) / 255.0,
		     float(t.b) / 255.0
		   );
    }
 }

void near pascal TTracerPhoto::CalcMaxVal()
 {
   PTRGBd pBuf = rgbBuf();
   dMax = INT_MIN;
   for( DWORD i = dwSize; i > 0; i--, pBuf++ )
    {
      TRGBd t; hmemcpy( &t, pBuf, sizeof(TRGBd) );
      dMax = max( float(dMax), max(t.dR, max(t.dG, t.dB)) );
    }
 }

inline float RGBFilter( float dVal )
 {
   return (dVal < 0) ? 0:((dVal > 255) ? 255:dVal);
 }


void near pascal TTracerPhoto::ConvertToRGB( TRGB  huge* pRGBBuf, BOOL bFlStrictGamma )
 {   
   PTRGBd pBuf = rgbBuf();

   if( bFlStrictGamma == FALSE )
    {
      CalcMaxVal();
      double dMax_ = dMax ? dMax:1;
      double dM = 255.0 / dMax_;
       
      for( DWORD i = dwSize; i > 0; i--, pBuf++, pRGBBuf++ )
       {
	 TRGBd t; hmemcpy( &t, pBuf, sizeof(TRGBd) );
	 *pRGBBuf = TRGB( t.dR * dM,
			  t.dG * dM,
		          t.dB * dM
			);
       }
    }
   else   
     for( DWORD i = dwSize; i > 0; i--, pBuf++, pRGBBuf++ )
      {
	TRGBd t; hmemcpy( &t, pBuf, sizeof(TRGBd) );
        *pRGBBuf = TRGB( RGBFilter( t.dR * 255.0 ),
		         RGBFilter( t.dG * 255.0 ),
		         RGBFilter( t.dB * 255.0 )
		       );
      }
 }

TTracerPhoto::TTracerPhoto( DWORD dwWid, DWORD dwHigh ):
  rgbBuf( (PTRGBd)farcalloc( (dwSize = dwWid * dwHigh), sizeof(TRGBd) ) )
 {
   dwWidth = dwWid;
   dMax = 255;
   pBuf = new TStrTracData[ dwWidth  + 1 ];

   if( pBuf ) _fmemset( pBuf, 0, sizeof(TStrTracData) * (dwWidth + 1) );
 }


TTracerPhoto::~TTracerPhoto()
 {
   if( pBuf ) { delete []pBuf; pBuf = NULL; }
 }

void  near pascal TTracerPhoto::PixelSmart( int iX, double x, double y, double z, PTSurface pF, PTFacet pFac )
 {
   RTStrTracData rDta = *(pBuf + iX);

   if( !rDta.psfSurf || rDta.z < z ) rDta = TStrTracData( pF, pFac, x, y, z );
 }


PTStrTracData TTracerPhoto::operator++(int)
 {
   PTStrTracData pTmp = pBufIter++;
   iCountIter++;

   return pTmp;
 }

TVector near pascal TTracerPhoto::GetVectorBkGnd( int clm, int str )
 {
   TRGBd rgb = rgbBuf()[ DWORD(str) * dwWidth + DWORD(clm) ];
   return TVector( rgb.dR, rgb.dG, rgb.dB );
 }

RTRGBd TRGBd::operator=( RTRGBd rgb )
 {
   hmemcpy( this, &rgb, sizeof(TRGBd) );

   return *(PTRGBd)this;
 }

TRGBd::TRGBd( RGBQUAD rgbQ )
 {
   TRGBd t1;

   t1.dR = float(rgbQ.rgbRed) / 255.0;
   t1.dG = float(rgbQ.rgbGreen) / 255.0;
   t1.dB = float(rgbQ.rgbBlue) / 255.0;

   hmemcpy( this, &t1, sizeof(TRGBd) );
 }

TRGBd::TRGBd( RTRGBd rgb )
 {
   //dR = rgb.dR;  dG = rgb.dG;  dB = rgb.dB;
   hmemcpy( this, &rgb, sizeof(TRGBd) );
 }


#pragma option -G.
#pragma option -v.
