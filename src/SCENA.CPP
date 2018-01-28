//#pragma option -v-

#include "threshol.hpp"
#include <values.h>
#include <stdio.h>
#include <bwcc.h>
#include <math.h>
#include <io.h>
#include <string.h>

#include "scena.hpp"
#include "infdlg.hpp"
#include "fheader.hpp"

#include "autorc.hpp"
#include "INC\io_em.inc"


extern HINSTANCE hInstRC;
extern PTFileManeger pGlobalFManeger;



TClassFilterFunc  FilterAll
 {
   if( !pObj ) return FALSE;
   return TRUE;
 }

TClassFilterFunc  FilterBody
 {
   if( !pObj ) return FALSE;

   classType cl = pObj->isA();
   return  cl != TYP_TOMNI && cl!= TYP_TCAMERA && cl!= TYP_TOMNIOBOL &&
	   cl != TYP_TCAMERAOBOL;
 }

TClassFilterFunc  FilterCameras
 {
   if( !pObj ) return FALSE;
   return pObj->isA() == TYP_TCAMERA;
 }

TClassFilterFunc  FilterLights
 {
   if( !pObj ) return FALSE;
   return pObj->isA() == TYP_TOMNI;
 }

TClassFilterFunc  FilterBodyAndLights
 {
   if( !pObj ) return FALSE;
   classType cl = pObj->isA();

   return cl != TYP_TCAMERA && cl != TYP_TCAMERAOBOL;
 }


static PTSurface near pascal CloneObj( RTSurface rSf, PTIndicator pI )
 {
   if( rSf == NOOBJECT ) return NULL;

   PTSurface pSu = NULL;
   switch( rSf.isA() )
    {
      case TYP_FACE:
	pSu = new TSurface( (RTSurface)rSf, pI );
	break;

      case TYP_CYLINDER:
	pSu = new TCylinder( (RTCylinder)rSf, pI );
	break;

      case TYP_CONE:
	pSu = new TCone( (RTCone)rSf, pI );
	break;

      case TYP_TUBE:
	pSu = new TTube( (RTTube)rSf, pI );
	break;

      case TYP_SPHERE:
	pSu = new TSphere( (RTSphere)rSf, pI );
	break;

      case TYP_TORUS:
	pSu = new TTorus( (RTTorus)rSf, pI );
	break;

      case TYP_TOBOL:
	pSu = new TObol( *(PTObol)rSf.TopPtr(), pI );
	break;

      case TYP_TOMNI:
	pSu = new TOmni( *(PTOmni)rSf.TopPtr(), pI );
	break;

      case TYP_TCAMERA:
	pSu = new TCamera( *(PTCamera)rSf.TopPtr(), pI );
	break;

      case TYP_TOMNIOBOL:
       pSu = new TOmniObol( *(PTOmniObol)rSf.TopPtr() );
       break;

      case TYP_TCAMERAOBOL:
       pSu = new TCameraObol( *(PTCameraObol)rSf.TopPtr() );
       break;
    }

   return pSu;
 }


void TScena::GetPhotoVariables( RTPhotoSetupTransfer rT ) const
 {
   rT.bFlAntiAliasing = bFlAntiAliasing;
   rT.bFlShadows      = bFlShadows;

   //rT.dPixelSize      = dPixelSize;
   rT.iNumbPointsAntiAlias = iNumbPointsAntiAlias;
   rT.dStrengtsAntialias = dStrengtsAntialias;

   rT.saShadingLimit  = saShadingLimit;
   rT.tmTraceModel    = tmTraceModel;
   rT.polyMethod      = polyMethod;

   rT.bFlStrictGamma = bFlStrictGamma;
 }

void TScena::SetPhotoVariables( const RTPhotoSetupTransfer rT )
 {
   bFlAntiAliasing = rT.bFlAntiAliasing;
   bFlShadows      = rT.bFlShadows;   

   iNumbPointsAntiAlias = rT.iNumbPointsAntiAlias;
   dStrengtsAntialias = rT.dStrengtsAntialias;

   saShadingLimit  = rT.saShadingLimit;
   tmTraceModel    = rT.tmTraceModel;
   polyMethod      = rT.polyMethod;

   bFlStrictGamma = rT.bFlStrictGamma;
 }

static LPCSTR near pascal DwordAscii( DWORD dwVal )
 {
   static char cBuf[25];

   sprintf( cBuf, "%lu", dwVal );

   return cBuf;
 }

static LPCSTR near pascal FlagASCII( BOOL bFl )
 {
   return (bFl == FALSE) ? "Off":"On";
 }

LPCSTR TScena::BodyStr() const
 {
   int iBody = 0;

   DoubleListIterator dbIter = DoubleListIterator( listSurf, TRUE );
   PTSurface pSerf = (PTSurface)&dbIter.current();
   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     if( FilterBody( pSerf ) == TRUE ) iBody++;

   return DwordAscii( iBody );
 }

LPCSTR TScena::LightsStr() const
 {
   int iLi = 0;

   DoubleListIterator dbIter = DoubleListIterator( listSurf, TRUE );
   PTSurface pSerf = (PTSurface)&dbIter.current();
   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     if( FilterLights( pSerf ) == TRUE ) iLi++;

   return DwordAscii( iLi );
 }

LPCSTR TScena::VertexStr() const
 {
   unsigned long ulTotalVert = 0;

   DoubleListIterator dbIter = DoubleListIterator( listSurf, TRUE );
   PTSurface pSerf = (PTSurface)&dbIter.current();
   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     if( FilterBody( pSerf ) == TRUE )
       ulTotalVert += pSerf->listVertex.getItemsInContainer();

   return DwordAscii( ulTotalVert );
 }

LPCSTR TScena::AdgesStr() const
 {
   unsigned long ulTotalAdges = 0;

   DoubleListIterator dbIter = DoubleListIterator( listSurf, TRUE );
   PTSurface pSerf = (PTSurface)&dbIter.current();
   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     if( FilterBody( pSerf ) == TRUE )
       ulTotalAdges += pSerf->listAdges.getItemsInContainer();
	   
   return DwordAscii( ulTotalAdges );
 }

LPCSTR TScena::FacetsStr() const
 {
   unsigned long ulTotalFace = 0;

   DoubleListIterator dbIter = DoubleListIterator( listSurf, TRUE );
   PTSurface pSerf = (PTSurface)&dbIter.current();
   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     if( FilterBody( pSerf ) == TRUE )
       ulTotalFace += pSerf->listFacets.getItemsInContainer();
 
   return DwordAscii( ulTotalFace );
 }


LPCSTR TScena::AlhoNameStr() const
 {
   switch( polyMethod )
    {
      case PM_RayTracing:
	return "Ray tracing";

      case PM_StringScan:
	return "String scanning";

      default:
	return "Unknown";
    };
 }

LPCSTR TScena::ModelNameStr() const
 {
   switch( tmTraceModel )
    {
      case TM_Local:
	return "Local";

      case TM_GlobalHolls:
	return "Global Holl's";

      default:
	return "Unknown";
    };
 }

LPCSTR TScena::AntialiasingStatusStr() const
 {
   return FlagASCII( bFlAntiAliasing );
 }

LPCSTR TScena::ShadowsStatusStr() const
 {
   return FlagASCII( bFlShadows );
 }

LPCSTR TScena::GammaStatusStr() const
 {
   return FlagASCII( bFlStrictGamma );
 }

LPCSTR TScena::ShadingNameStr() const
 {
   switch( saShadingLimit )
    {
      case SA_Flat:
	return "Flat";

      case SA_Phong:
       return "Phong";

      default:
	return "Unknown";
    }
 }


LPCSTR TScena::IntersectStatusStr() const
 {
   return FlagASCII( bFlMarkIntersect );
 }

LPCSTR TScena::PaletteNameStr() const
 {
   if( !iPaletteSize ) return "True colour";
   else if( iPaletteSize == -16 ) return "System 16";
   else
    {
      static char cBuf[10];
      sprintf( cBuf, "%d", int(iPaletteSize) );
      return cBuf;
    }
 }

LPCSTR TScena::SizeNameStr() const
 {
   switch( ifFormat )
    {
      case IF_Windowed:
	return "As viewport";

      case IF_FullScreen:
	return "Full screen";

      case IF_640x480:
	return "640x480";

      case IF_800x600:
	return "800x600";

      case IF_1024x768:
	return "1024x768";

      default:
        return "Unknown";
    };
 }

LPCSTR TScena::PaletteAlhoNameStr() const
 {
   if( caAlhoPalette & CA_MedianCutGeom ) return "Median cut geometric";
   else if( caAlhoPalette & CA_MedianCutDiscr ) return "Median cut discretic";
   else if( caAlhoPalette & CA_Quantize ) return "Quantize";
   else return "Use system palette";    
 }

LPCSTR TScena::DitherStatusStr() const
 {
   if( caAlhoPalette & CA_Diffuzion ) return "On";
   else return "Off";
 }

LPCSTR TScena::NoisyStatusStr() const
 {
   switch( nNoisy )
    {
      case N_Marginal:
        return "Marginal";

      case N_Uniform:
        return "Uniform";

      case N_None:
	return "None";

      default:
        return "Unknown";
    };
 }

LPCSTR TScena::BkGndNameStr() const
 {
   if( !mrcBackGraund ) return "None";
   else
     switch( mrcBackGraund()->isA() )
      {
	case BK_Flat:
	  return "Flat";

	case BK_Linear:
	  return "Linear";

	case BK_Rectangular:
	  return "Rectangular";

	case BK_Circular:
          return "Circular";

	default:
          return "Unknown";
      };
 }


TScena::TScena( int IdFile, PTApplication pApp ):

  rgbAdges(),
  rgbIntersect(),
  rgbAmbient(),
  polyMethod( PM_RayTracing ),
  tmTraceModel( PM_RayTracing ),
  saShadingLimit( SA_Phong ),
  bFlMarkIntersect( TRUE ),
  iPaletteSize( 256 ),
  ifFormat( IF_Windowed ),
  caAlhoPalette( CA_MedianCutGeom ),
  pFIntersect( NULL )
 {
   bFlStrictGamma = TRUE;

   nNoisy = N_None;
   iMarginalAmplitude = 1;
   iUniformAmplitude  = 2;
   iUniformThreshold  = 20;

   iNumbPointsAntiAlias = 2;
   dStrengtsAntialias = 0.01;

   dAngleSmothGrad = 60;

   bFlAntiAliasing = FALSE;
   bFlShadows = FALSE;
   
   //dPixelSize = 1.1;

   pApplicat = pApp;

   pUcs = NULL;
   hdc = NULL;
   pWin = NULL;

   RGBQUAD rgbTmp;
   rgbTmp.rgbRed   = 0;
   rgbTmp.rgbGreen = 128;
   rgbTmp.rgbBlue  = 128;
   mrcBackGraund = new TBackGraundFlat( 236, rgbTmp, rgbTmp );

   rgbAdges.rgbBlue = 0xFF;
   rgbAdges.rgbGreen = 0xFF;
   rgbAdges.rgbRed = 0xFF;

   rgbIntersect.rgbBlue = 0;
   rgbIntersect.rgbGreen = 0;
   rgbIntersect.rgbRed = 0;

   rgbAmbient.rgbBlue = 10;
   rgbAmbient.rgbGreen = 10;
   rgbAmbient.rgbRed = 10;
   dAmbMult = 1;

   Status = 0;
   bFlChanged = FALSE; this->IdFile = IdFile;
   Lock( FALSE );
   bFlShowBackFaces = TRUE; bFlFullDetal = TRUE;
   bFlVisyal = TRUE;
   proector = ProectOrtho;
   bFlHold = FALSE;

   bFlCriticalLock = FALSE;

   bFlBaseUCS = TRUE;
 }

TScena::~TScena()
 {
   //if( bgBackGraund ) { delete bgBackGraund; bgBackGraund = NULL; }
 }

TScena::TScena( RTScena rS, PTIndicator pI, BOOL bFlOnlyFace, BOOL bFlOnlySelected ):

  rgbAdges( rS.rgbAdges ),
  rgbIntersect( rS.rgbIntersect ),
  rgbAmbient( rS.rgbAmbient ),
  polyMethod( rS.polyMethod ),
  tmTraceModel( rS.tmTraceModel ),
  saShadingLimit( rS.saShadingLimit ),
  bFlMarkIntersect( rS.bFlMarkIntersect ),
  iPaletteSize( rS.iPaletteSize ),
  ifFormat( rS.ifFormat ),
  caAlhoPalette( rS.caAlhoPalette ),
  pFIntersect( rS.pFIntersect )
 {
   bFlStrictGamma = rS.bFlStrictGamma;

   nNoisy = rS.nNoisy;
   iMarginalAmplitude = rS.iMarginalAmplitude;
   iUniformAmplitude  = rS.iUniformAmplitude;
   iUniformThreshold  = rS.iUniformThreshold;

   iNumbPointsAntiAlias = rS.iNumbPointsAntiAlias;
   dStrengtsAntialias = rS.dStrengtsAntialias;

   dAngleSmothGrad = rS.dAngleSmothGrad;

   bFlAntiAliasing = rS.bFlAntiAliasing;
   bFlShadows = rS.bFlShadows;
   

   //dPixelSize = rS.dPixelSize; 

   dAmbMult = rS.dAmbMult;

   //if( bgBackGraund ) {delete bgBackGraund; bgBackGraund = NULL; }
   if( !!rS.mrcBackGraund )
     mrcBackGraund = CreateCopy( *(rS.mrcBackGraund()) );

   pApplicat = rS.pApplicat;

   bFlBaseUCS = rS.bFlBaseUCS;

   bFlCriticalLock = FALSE;
   bFlHold = FALSE;
   Status = 0;
   bFlChanged = FALSE; IdFile = rS.IdFile;
   Lock( FALSE );
   bFlShowBackFaces = rS.bFlShowBackFaces;
   bFlFullDetal = rS.bFlFullDetal;
   bFlVisyal = rS.bFlVisyal;
   proector =  rS.proector;

   pUcs = rS.pUcs;
   hdc =  rS.hdc;
   pWin = rS.pWin;

   dMinX = rS.dMinX;
   dMaxX = rS.dMaxX;
   dMinY = rS.dMinY;
   dMaxY = rS.dMaxY;
   dMinZ = rS.dMinZ;
   dMaxZ = rS.dMaxZ;


   Status = Copy( listSurf, rS.listSurf, pI, bFlOnlyFace, bFlOnlySelected );
   if( !Status )
     Status = Copy( listObl, rS.listObl, pI, bFlOnlyFace, bFlOnlySelected );

   if( pI ) pI->WorkIs100();
 }

int TScena::Copy( RTIndexedDoubleList rD, RTIndexedDoubleList rS, PTIndicator pI,
  BOOL bFlOnlyFace, BOOL bFlOnlySelected )
 {
   DoubleListIterator dblIt = DoubleListIterator( rS );
   PTIndexedItem pIt = (PTIndexedItem)&dblIt.current();

   for( ; int(dblIt); pIt = (PTIndexedItem)&++dblIt )
    {
      if( /*(bFlOnlyFace == TRUE && (pIt->isA() == TYP_TOMNI || pIt->isA() == TYP_TCAMERA)) ||*/
	  bFlOnlySelected == TRUE && PTSurface(pIt)->IsSelected() == FALSE ) continue;

      PTSurface  pSu = CloneObj( *(PTSurface)pIt, pI );

      if( !pSu ) return -1;
      rD.addAtTail( *pSu );
      if( pSu->Status ) return pSu->Status;

    }
   //TIndexedItem( int indAssign )
   return 0;
 }

//static void  ProectOrtho( PCTScena pSc, PT3DPoint pPoint )
static TProectorFunc_ ProectOrtho
 {
   MoveTo( pSc->hdc, pPoint[0].x, pPoint[0].y );
   LineTo( pSc->hdc, pPoint[1].x, pPoint[1].y );
 }

//static void  ProectPerspective( PCTScena pSc, PT3DPoint pPoint )
static TProectorFunc_ ProectPerspective
 {

 }

void TScena::SetHardwareEnviron( PTUCS pUcs, HDC hdc, PTWindow pWin )
 {
   TViewPoint vpPoint;
   double anH, anV;

   this->pUcs = pUcs; this->hdc = hdc; this->pWin = pWin;

   pUcs->GetVpoint( vpPoint, anH, anV );
   if( vpPoint == TUCS::VP_Camera ) proector = ProectPerspective;
   else proector = ProectOrtho;
 }

int TScena::isEqual( const Object& rObj ) const
 {
   if( (TScena huge *)this != (TScena huge *)&rObj ) return FALSE;
   //if( IdFile != ((RTScena)rObj).IdFile ) return FALSE;

   return TRUE;
 }


void TScena::DrawSpecial() const
 {
   if( !listSurf.getItemsInContainer() ) return;

   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   int iROPOld = SetROP2( hdc, R2_XORPEN );
   HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
   HPEN hpOld = SelectPen( hdc, hpFon );

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     pSerf->DrawSpecial( bFlShowBackFaces, (PTScena)this );

   SelectPen( hdc, hpOld );
   SetROP2( hdc, iROPOld );
   DeleteObject( hpFon );

 }

void TScena::DrawFacet( RTFacet rF, BOOL bFlDrawBack ) const
 {
   if( bFlShowBackFaces == FALSE && bFlDrawBack == FALSE )
    {   
      TVector vecOutNorm = rF.GetNewelNormal( pUcs );
      if( vecOutNorm.z < SCENA_d_VectorZAbsOutNormalVisible ) return;
    }

   DoubleListIterator dbAdges = DoubleListIterator( rF.listAgesItem, FALSE );
   PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();

   for( ; int(dbAdges); pAge = (PTAdgeItem)&--dbAdges )
    {
          
      T3DPoint  tmpP[2];

      /*tmpP[0] =T3DPoint( pAge->pAdge->pV1->x,
			 pAge->pAdge->pV1->y,
		         pAge->pAdge->pV1->z );

      tmpP[1] =T3DPoint( pAge->pAdge->pV2->x,
	                 pAge->pAdge->pV2->y,
			 pAge->pAdge->pV2->z );*/
      tmpP[0] = T3DPoint( *(PT3DPoint)pAge->pAdge->pV1 );
      tmpP[1] = T3DPoint( *(PT3DPoint)pAge->pAdge->pV2 );

      pUcs->RecalcUserToBase( tmpP, 2 );
      proector( this, tmpP );
    }
 }

/*
void TScena::DrawBody( RTSurface rSurf ) const
 {
   if( rSurf == NOOBJECT ) return;
   rSurf.listAdges.ResetActionFlag();

   DoubleListIterator dbFacet = DoubleListIterator( rSurf.listFacets, FALSE );
   PTFacet  pFacet = (PTFacet)&dbFacet.current();
   for( ; int(dbFacet); pFacet = (PTFacet)&--dbFacet )
    {
      if( bFlShowBackFaces == FALSE )
       {   
	 TVector vecOutNorm = pFacet->GetNewelNormal( pUcs );
	 if( vecOutNorm.z < 0 ) continue;
       }

      DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem, FALSE );
      PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();
      for( ; int(dbAdges); pAge = (PTAdgeItem)&--dbAdges )
       {
         if( pAge->pAdge->bFlAtion == TRUE ) continue;
	 else pAge->pAdge->bFlAtion = TRUE;

	 T3DPoint  tmpP[2];

	 tmpP[0] =T3DPoint( pAge->pAdge->pV1->x,
			    pAge->pAdge->pV1->y,
		            pAge->pAdge->pV1->z );

	 tmpP[1] =T3DPoint( pAge->pAdge->pV2->x,
	                    pAge->pAdge->pV2->y,
		            pAge->pAdge->pV2->z );

	 pUcs->RecalcUserToBase( tmpP, 2 );
	 proector( this, tmpP );
       }
    }

   if( rSurf.bFlObolVis == TRUE && rSurf.NeedCreateObol() == TRUE )   
     DrawBody( *(PTSurface)listObl[rSurf.Index()] );
 }
 */

void TScena::DrawObol( RTSurface rSurf ) const
 {
   PTSurface pSurf = PTSurface( listObl[ rSurf.Index() ] );

   if( rSurf.NeedCreateObol() == FALSE || !pSurf ) return;

   HPEN  hpOld = SelectPen( hdc, GetStockObject(WHITE_PEN) );   
   pSurf->Draw( bFlShowBackFaces, (PTScena)this );
   SelectPen( hdc, hpOld );
 }

void TScena::Draw( PTSurface pSf ) const
 {
   if( bFlVisyal == FALSE ) return;

   if( !listSurf.getItemsInContainer() ) return;

   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   if( bFlFullDetal == TRUE )
     dbIter = DoubleListIterator( listSurf );
   else
     dbIter = DoubleListIterator( listObl );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   HPEN  hpOld = SelectPen( hdc, GetStockObject(WHITE_PEN) );
   HPEN  hpNormal = (HPEN)GetStockObject(WHITE_PEN);
   HPEN  hpOmni = CreatePen( PS_SOLID, 0, RGB(0xFF, 0xFF, 0) );
   HPEN  hpCamera = CreatePen( PS_SOLID, 0, RGB(0, 0x80, 0x80) );
   HPEN  hpSelect = CreatePen( PS_SOLID, 0, PALETTEINDEX(1) );

   TViewPoint vpPoint;
   double anH, anV;
   pUcs->GetVpoint( vpPoint, anH, anV );

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
       if( pSf ) pSerf = pSf;

       /*if( pSerf->IsSelected() == FALSE )
	 SelectPen( hdc, GetStockObject(WHITE_PEN) );
       else
	 SelectPen( hdc, hpSelect );
      DrawBody( *pSerf );*/

      if( !(vpPoint == TUCS::VP_Camera && pSerf->isA() == TYP_TCAMERA) )
       {
         SelectPen( hdc, (pSerf->IsSelected() == TRUE) ? hpSelect:
		    ((pSerf->isA() == TYP_TOMNI) ? hpOmni:
		    ((pSerf->isA() == TYP_TCAMERA) ? hpCamera:hpNormal))
	          );
	 pSerf->Draw( bFlShowBackFaces, (PTScena)this );
       }

      if( pSf ) break;
    }

   SelectPen( hdc, hpOld );
   DeleteObject( hpSelect );
   DeleteObject( hpOmni );
   DeleteObject( hpCamera );
 }

void TScena::ZoomToFit( RECT *pRect ) const
 {
   if( !pUcs || !listSurf.getItemsInContainer()) return;

   double  dMinX = MAXINT, dMaxX = -MAXINT,
	   dMinY = MAXINT, dMaxY = -MAXINT;

   double scale = pUcs->GetScale();
   pUcs->ChangeScale( 1 );
   pUcs->ShiftAbs( 0, 0 );

   if( !pRect )
    {
      DoubleListIterator  dbIter = DoubleListIterator( listSurf );
      PTSurface  pSerf = (PTSurface)&dbIter.current();

      for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
       {
         //if( *pSerf == NOOBJECT ) break;

	 DoubleListIterator dbVert = DoubleListIterator( pSerf->listVertex, FALSE );
         PTVertex  pVert = (PTVertex)&dbVert.current();
         for( ; int(dbVert); pVert = (PTVertex)&--dbVert )
          {	
	    //T3DPoint  tmpP = T3DPoint( pVert->x, pVert->y, pVert->z );
	    T3DPoint  tmpP = T3DPoint( *(PT3DPoint)pVert );
	    pUcs->RecalcUserToBase( &tmpP, 1 );	   

	    dMinX = (tmpP.x < dMinX) ? tmpP.x:dMinX;
	    dMaxX = (tmpP.x > dMaxX) ? tmpP.x:dMaxX;
	    dMinY = (tmpP.y < dMinY) ? tmpP.y:dMinY;
	    dMaxY = (tmpP.y > dMaxY) ? tmpP.y:dMaxY;
          }
       }
    }
   else
    {
      dMinX = pRect->left; dMaxX = pRect->right;
      dMinY = pRect->top; dMaxY = pRect->bottom;      
    }

   static const double dCantX = SCENA_d_ZoomToFitBorderX,
		       dCantY = SCENA_d_ZoomToFitBorderY;
   RECT r;
   GetClientRect( pWin->HWindow, &r );
   double widt, high;
   double scX, scY;

   widt = dMaxX - dMinX;  high = dMaxY - dMinY;
   if( widt )
     scX = (double(r.right) * (1 - dCantX)) / widt;
   else scX = 1;
   if( high )
     scY = (double(r.bottom) * (1 - dCantY)) / high;
   else scY = 1;

   double sc = (scX < scY) ? scX:scY;

   double cX = sc * (dMinX + widt / 2.0),
	  cY = sc * (dMinY + high / 2.0);

   if( pRect ) sc *= scale;

   /*pUcs->ShiftAbs( (double(r.right) - sc*widt) / 2.0 + sc*widt / 2.0,
		   (double(r.bottom) - sc*high) / 2.0 + sc*high / 2.0
		 );*/
   pUcs->ShiftAbs( -cX, cY );

   pUcs->ChangeScale( sc );
 }

void TScena::AddBody( PTSurface pSurf )
 {
   /*if( pUcs ) //Make aligment
    {
      DoubleListIterator dbIt = DoubleListIterator( pSurf->listVertex, FALSE );
      PTVertex pV = (PTVertex)&dbIt.current();

      for( ; int(dbIt); pV = (PTVertex)&--dbIt )
       {
	 //T3DPoint ptV = T3DPoint ( pV->x, pV->y, pV->z );
	 pUcs->RecalcUserToBase( (PT3DPoint)pV, 1 );
       }

      dbIt = DoubleListIterator( pSurf->listFacets, FALSE );
      PTFacet  pF = (PTFacet)&dbIt.current();
      for( ; int(dbIt); pF = (PTFacet)&--dbIt )
       {
	 T3DPoint ptV = T3DPoint ( pF->vecOutNormal.x,
				   pF->vecOutNormal.y,
				   pF->vecOutNormal.z );
	 pUcs->RecalcUserToBase( &ptV, 1 );
	 pF->vecOutNormal = TVector( ptV.x, ptV.y, ptV.z );
       }

    }*/

   listSurf.add(*pSurf); bFlChanged = TRUE;
   if( pSurf->NeedCreateObol() == TRUE && !listObl[pSurf->Index()] )
    {
      PTObol pObl = new TObol( pSurf->Index(), pSurf );
      if( pObl )
	listObl.add( *pObl );
    }
 }

static void near pascal LiAddSeparator( List& rList )
 {
   TAutoStringRc s1_ = TAutoStringRc( hInstRC, ID_MSG_72, 256 );
   rList.add( *new String(s1_()) );
 }

static void near pascal MergeLists( List& rlDest, List& rlSource )
 {
   ListIterator lIt = ListIterator( rlSource );
   PString pStr = (PString)&lIt.current();

   for( ; int(lIt); pStr = (PString)&++lIt )
     rlDest.add( *pStr );

   rlSource.flush( TShouldDelete::NoDelete );
 }

void TScena::FillInfoDlg( List& rList, LPSTR lpName )
 {
   char chBuf[256];
   //PTInfDialog pDlg = (PTInfDialog)pDlg_;
   List liBodys, liLights, liCameras;
   long lBodyCnt, lLightCnt, lCameraCnt;
   lBodyCnt = lLightCnt = lCameraCnt = 0;

   TAutoStringRc s1_;

   s1_ = TAutoStringRc( hInstRC, ID_MSG_70 );
   sprintf( chBuf, /*"The file: '%s'"*/s1_(), lpName );
   rList.add( *new String(chBuf) );
   sprintf( chBuf, " " );   
   rList.add( *new String(chBuf) );


   s1_ = TAutoStringRc( hInstRC, ID_MSG_73 );
   unsigned long ulTotalVert = 0,
		 ulTotalAdges = 0,
		 ulTotalFace  = 0;
   DoubleListIterator dbIter = DoubleListIterator( listSurf, TRUE );
   PTSurface pSerf = (PTSurface)&dbIter.current();
   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      switch( pSerf->isA() )
       {
	 case TYP_FACE:
	 case TYP_CYLINDER:
	 case TYP_CONE:
	 case TYP_TUBE:
	 case TYP_SPHERE:
	 case TYP_TORUS:
	   sprintf( chBuf, /*"%4d: %10d  %10d  %10d"*/s1_(), (int)pSerf->Index(),
	   pSerf->listVertex.getItemsInContainer(),
	   pSerf->listAdges.getItemsInContainer(),
	   pSerf->listFacets.getItemsInContainer()
	     );	   
	   liBodys.add( *new String(chBuf) );

           ulTotalVert += pSerf->listVertex.getItemsInContainer();
           ulTotalAdges += pSerf->listAdges.getItemsInContainer();
	   ulTotalFace += pSerf->listFacets.getItemsInContainer();

	   lBodyCnt++;
           break;

	 case TYP_TOMNI:
	   PTOmni pTmp__;
	   PTOmni pO = TSPtr( pSerf, pTmp__ );  
	   sprintf( chBuf, "%3d  %3d  %3d  %3s  %7s", (int)pO->R(), (int)pO->G(), (int)pO->B(),
		    (pO->IsOn() == TRUE) ? "On":"Off",
		    (pO->Shadow() == TRUE) ? "+":"-"
		  );
	   liLights.add( *new String(chBuf) );

	   lLightCnt++;
	   break;
       };
    }

   if( lBodyCnt )
    {
      sprintf( chBuf, "Bodys:" );
      rList.add( *new String(chBuf) );
   
      s1_ = TAutoStringRc( hInstRC, ID_MSG_71 );
      sprintf( chBuf, /*"      %10s  %10s  %10s"*/s1_(), "Vertex", "Adges", "Faces" );   
      rList.add( *new String(chBuf) );

      LiAddSeparator( rList );
      MergeLists( rList, liBodys );
      LiAddSeparator( rList );

      //s1_ = TAutoStringRc( hInstRC, ID_MSG_75 );
      sprintf( chBuf, "In [%lu] bodys total:", lBodyCnt );
      rList.add( *new String(chBuf) );

      s1_ = TAutoStringRc( hInstRC, ID_MSG_76 );
      sprintf( chBuf, /*"      Vertex  %-10lu"*/s1_(),
        ulTotalVert
	     );   
      rList.add( *new String(chBuf) );

      s1_ = TAutoStringRc( hInstRC, ID_MSG_77 );
      sprintf( chBuf, /*"      Adges   %-10lu"*/s1_(),
        ulTotalAdges   
	  );   
      rList.add( *new String(chBuf) );

      s1_ = TAutoStringRc( hInstRC, ID_MSG_78 );
      sprintf( chBuf, /*"      Faces   %-10lu"*/s1_(),
        ulTotalFace
	  );   
      rList.add( *new String(chBuf) );

      sprintf( chBuf, " " );
      rList.add( *new String(chBuf) );
    }

   if( lLightCnt )
    {
      sprintf( chBuf, "Lights:" );
      rList.add( *new String(chBuf) );  

      sprintf( chBuf, "   R     G     B         Shadows" );   
      rList.add( *new String(chBuf) );

      LiAddSeparator( rList );
      MergeLists( rList, liLights );
      LiAddSeparator( rList );

      sprintf( chBuf, "Total lights: %lu", lLightCnt );
    }
 }

void TScena::FindObjectAndPoint( int x_, int y_, RTOPData rDta )
 {
   POINT p_; p_.x = x_; p_.y = y_;
   DPtoLP( rDta.hdc, &p_, 1 );

   int x = p_.x, y = p_.y;


   DoubleListIterator  dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {    
      if( FilterBody(pSerf) == FALSE ) continue;

      DoubleListIterator dbFacet = DoubleListIterator( pSerf->listFacets );
      PTFacet  pFacet = (PTFacet)&dbFacet.current();
      for( ; int(dbFacet); pFacet = (PTFacet)&++dbFacet )
       {
	 double dZ = 0;
	 TVector vecOutNormal = pFacet->GetNewelNormal( rDta.pUcs );

	 if( vecOutNormal.z < SCENA_d_VectorZAbsOutNormalVisible ) continue;

         int intersectCount = 0;
	 int x1, x2, y1, y2;
         double z1;

         DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem, TRUE );
         PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();  

         for( ; int(dbAdges); pAge = (PTAdgeItem)&++dbAdges )
          {
	    PTAdge pA = pAge->pAdge;
	    T3DPoint tmpP[2];

	    tmpP[0] = T3DPoint( *(PT3DPoint)pA->pV1 );
	    tmpP[1] = T3DPoint( *(PT3DPoint)pA->pV2 );

	    rDta.pUcs->RecalcUserToBase( tmpP, 2 );

	    /*POINT p[2];
	    p[0].x = tmpP[0].x; p[0].y = tmpP[0].y;
	    p[1].x = tmpP[1].x; p[1].y = tmpP[1].y;
	    LPtoDP( rDta.hdc, p, 2 );
            x1 = p[0].x; x2 = p[1].x;
	    y1 = p[0].y; y2 = p[1].y;
	    z1 = tmpP[0].z;*/

	    x1 = tmpP[0].x; x2 = tmpP[1].x;
	    y1 = tmpP[0].y; y2 = tmpP[1].y;
	    z1 = tmpP[0].z;

            if( y1 == y2 ) continue;

            if( (y < y1 && y < y2) ||
	        (y > y1 && y > y2)
	      ) continue;

            if( (x <= x1 && y == y1 && y1 > y2) ||
	        (x <= x2 && y == y2 && y2 > y1)
	      ) continue;

            double xt = double(x1) + double(y1 - y) * ( double(x2 - x1) / double(y1 - y2) );

            if( double(x) > xt ) continue;

            intersectCount++;
          }   

         if( !(intersectCount % 2) ) continue;

	 double d = -vecOutNormal.x * double(x1) -
		    vecOutNormal.y * double(y1) -
		    vecOutNormal.z * double(z1);

	 dZ = -( (d + vecOutNormal.x * double(x) + vecOutNormal.y * double(y) ) /
	   vecOutNormal.z );

	 if( dZ > rDta.dZ )
	  {
	    rDta.dZ = dZ;
	    rDta.pSfFound = pSerf;
	    rDta.pFacetFound = pFacet;
	  }
       }
    }

   if( rDta.pFacetFound && rDta.pSfFound )
    {
      POINT p; p.x = x; p.y = y;
      //DPtoLP( rDta.hdc, &p, 1 );
      rDta.ptFound = T3DPoint( p.x, p.y, rDta.dZ );
      rDta.pUcs->RecalcBaseToUser( &rDta.ptFound, 1 );
    }
 }


PTSurface TScena::FindObject( double x, double y, PTClassFilterFunc pFiltr )//DP in client
 {
   const double dcR = SCENA_i_SelectObjectDelta;
   const double dcR2 = dcR * dcR;


   RECT  delta;
   delta.left = x - dcR; delta.right = x + dcR;
   delta.top = y - dcR; delta.bottom = y + dcR;

   RECT rWin; GetClientRect( pWin->HWindow, &rWin );
   if( IntersectRect(&delta, &delta, &rWin) == FALSE ) return NULL;

   PTSurface pFoundSerf = NULL;
   PTAdge    pNearstAdge = NULL;
   long  deltaAdge = LONG_MAX;

   DoubleListIterator  dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      //if( pSerf->isAPrepare() != ctActClass ) continue;
      if( pFiltr(pSerf) == FALSE ) continue;

      if( pSerf->isA() == TYP_TOMNI )
       {	 
	 T3DPoint tmpP = (RT3DPoint)RTVertex( pSerf->listVertex.peekAtHead() );
	 //tmpP = T3DPoint( (RT3DPoint)(pSerf->listVertex.peekAtHead()) );
	 pUcs->RecalcUserToBase( &tmpP, 1 );

	 POINT p;
	 p.x = tmpP.x; p.y = tmpP.y;
	 LPtoDP( hdc, &p, 1 );

	 if( p.x > delta.right ||
	     p.x < delta.left ||
	     p.y > delta.bottom ||
	     p.y < delta.top
	   ) continue;

	 double dDist2 = long(p.x - x) * long(p.x - x) +
			 long(p.y - y) * long(p.y - y);

	 if( deltaAdge > dDist2 )
	  {
	    deltaAdge = dDist2;
	    pFoundSerf = pSerf;
	    pNearstAdge = NULL;
	  }
	 continue;
       }

      DoubleListIterator dbAg = DoubleListIterator( pSerf->listAdges );
      PTAdge  pAg = (PTAdge)&dbAg.current();
      for( ; int(dbAg); pAg = (PTAdge)&++dbAg )
       {
	 T3DPoint  tmpP[2];

	 /*tmpP[0] =T3DPoint( pAg->pV1->x,
			    pAg->pV1->y,
		            pAg->pV1->z );

	 tmpP[1] =T3DPoint( pAg->pV2->x,
			    pAg->pV2->y,
			    pAg->pV2->z );*/
	 tmpP[0] = T3DPoint( *(PT3DPoint)pAg->pV1 );
	 tmpP[1] = T3DPoint( *(PT3DPoint)pAg->pV2 );

	 pUcs->RecalcUserToBase( tmpP, 2 );

	 POINT p[2];
	 p[0].x = tmpP[0].x; p[0].y = tmpP[0].y;
	 p[1].x = tmpP[1].x; p[1].y = tmpP[1].y;
	 LPtoDP( hdc, p, 2 );

	 if( (p[0].x >  delta.right && p[1].x > delta.right) ||
	     (p[0].x <  delta.left && p[1].x < delta.left) ||
	     (p[0].y >  delta.bottom && p[1].y > delta.bottom) ||
	     (p[0].y <  delta.top && p[1].y < delta.top)
	   ) continue;

	 long iDiv = long(p[1].x - p[0].x) * long(p[1].x - p[0].x) +
		     long(p[1].y - p[0].y) * long(p[1].y - p[0].y);


	 double dDist2;
	 if( iDiv )
	  {
	    double m;
	    double dXc, dYc;
	    double d2;

	    int iSx1, iSx2,
		iSy1, iSy2;
	    m = double(long(iSy1 = (p[0].y - p[1].y)) * long(iSy2 = (p[0].y - y)) +
		       (long(iSx1 = p[0].x - p[1].x)) * long(iSx2 = (p[0].x - x))) / double(iDiv);

	    dXc = double(iSx2) + double(-iSx1) * m;
	    dYc = double(iSy2) + double(-iSy1) * m;

	    d2 = dXc * dXc + dYc * dYc;

	    if( d2 > dcR2 ) continue;

	    if( m >= 0 && m <= 1 ) dDist2 = d2;
	    else if( m < 0 )
	     {
	       if( (dDist2 = long(iSx2) * long(iSx2) + long(iSy2) * long(iSy2)) > dcR2 ) continue;
	     }
	    else if( m > 1 )
	     {
	       if( (dDist2 = long(p[1].x - x) * long(p[1].x - x) +
	           long(p[1].y - y) * long(p[1].y - y)) > dcR2 ) continue;
	     }
	  }
	 else
	   if( (dDist2 = long(p[1].x - x) * long(p[1].x - x) +
		    long(p[1].y - y) * long(p[1].y - y)) > dcR2 ) continue;

	 if( deltaAdge > dDist2 )
	  {
	    deltaAdge = dDist2;
	    pFoundSerf = pSerf;
	    pNearstAdge = pAg;
	  }
       }
    }

   return pFoundSerf;

/*   const double dt = 2;
   RECT  delta;
   delta.left = x - 2; delta.right = x + 2;
   delta.top = y - 2; delta.bottom = y + 2;

   PTSurface pFoundSerf = NULL;
   PTAdge    pNearstAdge = NULL;
   int  deltaAdge = MAXINT;

   DoubleListIterator  dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      if( *pSerf == NOOBJECT ) break;

      double prMinX = MAXINT, prMinY = MAXINT,
	     prMaxX = -MAXINT, prMaxY = -MAXINT;


	 PTObol pObl = (PTObol)listObl[ pSerf->Index() ];

	 DoubleListIterator dbVIt = DoubleListIterator( pObl->listVertex );
	 PTVertex pV = (PTVertex)&dbVIt.current();

	 for( ; int(dbVIt); pV = (PTVertex)&++dbVIt )
	  {
	    T3DPoint p = T3DPoint( pV->x, pV->y, pV->z );
	    pUcs->RecalcUserToBase( &p, 1 );

	    prMinX = (p.x < prMinX) ? p.x:prMinX;
	    prMinY = (p.y < prMinY) ? p.y:prMinY;

	    prMaxX = (p.x > prMaxX) ? p.x:prMaxX;
	    prMaxY = (p.y > prMaxY) ? p.y:prMaxY;
	  }

		  //Rectangle( hdc, prMinX, prMinY, prMaxX, prMaxY );
      if( x < prMinX || x > prMaxX ||
	  y < prMinY || y > prMaxY
	)  continue;

      DoubleListIterator dbAg = DoubleListIterator( pSerf->listAdges, FALSE );
      PTAdge  pAg = (PTAdge)&dbAg.current();
      for( ; int(dbAg); pAg = (PTAdge)&--dbAg )
       {
         
	    T3DPoint  tmpP[2];

	    tmpP[0] =T3DPoint( pAg->pV1->x,
			       pAg->pV1->y,
			       pAg->pV1->z );

	    tmpP[1] =T3DPoint( pAg->pV2->x,
			       pAg->pV2->y,
			       pAg->pV2->z );

	    pUcs->RecalcUserToBase( tmpP, 2 );

	    int xMin = (tmpP[0].x < tmpP[1].x) ?
		       tmpP[0].x:tmpP[1].x;
	    int xMax = (tmpP[0].x > tmpP[1].x) ?
		       tmpP[0].x:tmpP[1].x;

	    int yMin = (tmpP[0].y < tmpP[1].y) ?
			tmpP[0].y:tmpP[1].y;
	    int yMax = (tmpP[0].y > tmpP[1].y) ?
			tmpP[0].y:tmpP[1].y;

	    if( yMin > delta.bottom || xMin > delta.right ||
		xMax < delta.left   || yMax < delta.top
	      )
	      continue;

	    tmpP[0].x -= x;  tmpP[0].y -= y;
	    tmpP[1].x -= x;  tmpP[1].y -= y;

	    double keepX = tmpP[0].x, keepY = tmpP[0].y;
	    double l = tmpP[1].x - tmpP[0].x,
		   m = tmpP[1].y - tmpP[0].y,
		   r = m * m + l * l;

	    tmpP[0].x = l/r * keepX + m/r * keepY;
	    tmpP[0].y = -m/r * keepX + l/r * keepY;

	    keepX = tmpP[1].x; keepY = tmpP[1].y;
	    tmpP[1].x = l/r * keepX + m/r * keepY;
	    tmpP[1].y = -m/r * keepX + l/r * keepY;

	    if( fabs(tmpP[1].y) > dt ) continue;

	    if( !((tmpP[0].x > dt && tmpP[1].x > dt) ||
		  (tmpP[0].x < - dt && tmpP[1].x < -dt)) )
	     {
	       if( deltaAdge > fabs(tmpP[1].y) )
		{
		  pFoundSerf = pSerf;
		  pNearstAdge = pAg;
		  deltaAdge = fabs(tmpP[1].y);
                }
	     }             
	 
       }
    }

   return pFoundSerf;*/

 }


void TScena::Move( RTSurface rSurf, double dx, double dy, double dz )
 {
   if( rSurf == NOOBJECT ) return;

   bFlChanged = TRUE;

   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();

   if( bFlBaseUCS == TRUE ) RecalcBodyUserToBase( rSurf );
   rSurf.Move( dx, dy, dz );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );

   if( rSurf.NeedCreateObol() == TRUE )
    {
      PTSurface  pObol = (PTSurface)listObl[ rSurf.Index() ];
      if( pObol ) pObol->Move( dx, dy, dz );
    }
 }

struct TMoveData
 {
  double dx, dy, dz;
  PTScena pScena;

  TMoveData() { dx = dy = dz = 0; pScena = NULL; }
  TMoveData( PTScena pSc, double dx, double dy, double dz )
   {
     this->dx = dx; this->dy = dy; this->dz = dz;
     pScena = pSc;
   }
 };

typedef TMoveData _FAR & RTMoveData;
typedef TMoveData _FAR * PTMoveData;


static void MoveInstance( Object _FAR &rFac, void _FAR *pDta )
 {
   if( RTSurface(rFac).IsSelected() == TRUE )
     PTMoveData(pDta)->pScena->Move( RTSurface(rFac),
				     PTMoveData(pDta)->dx,
				     PTMoveData(pDta)->dy,
				     PTMoveData(pDta)->dz
                                   );
 }


void TScena::MoveAllSelected( double dx, double dy, double dz )
 {
   TMoveData mdDta( this, dx, dy, dz );

   listSurf.forEach( MoveInstance, Pvoid(&mdDta) );
 }


 void TScena::RotateZ( RTSurface rSurf, double angle )
  {
   if( rSurf == NOOBJECT ) return;
   angle = GRAD_TO_RAD( angle );

   bFlChanged = TRUE;

   //BOOL keepAbsFlag = rSurf.GetAbsFlag();
   //rSurf.SetAbsFlag( FALSE );

   //if( keepAbsFlag == TRUE ) rSurf.ResetVertex();
   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();
   T3DPoint pt = rSurf.GetGeomCenter();

   rSurf.Move( 0 - pt.x, 0 - pt.y, 0 - pt.z );
   if( bFlBaseUCS == TRUE ) RecalcBodyUserToBase( rSurf );
   rSurf.RotateZ( angle );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );
   rSurf.Move( pt.x - 0, pt.y - 0, pt.z - 0 );

   //rSurf.SetAbsFlag( keepAbsFlag );
  }

 void TScena::RotateX( RTSurface rSurf, double angle )
  {
   if( rSurf == NOOBJECT ) return;
   angle = GRAD_TO_RAD( angle );

   bFlChanged = TRUE;

   //BOOL keepAbsFlag = rSurf.GetAbsFlag();
   //rSurf.SetAbsFlag( FALSE );

   //if( keepAbsFlag == TRUE ) rSurf.ResetVertex();
   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();
   T3DPoint pt = rSurf.GetGeomCenter();

   rSurf.Move( 0 - pt.x, 0 - pt.y, 0 - pt.z );
   if( bFlBaseUCS == TRUE ) RecalcBodyUserToBase( rSurf );
   rSurf.RotateX( angle );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );
   rSurf.Move( pt.x - 0, pt.y - 0, pt.z - 0 );

   //rSurf.SetAbsFlag( keepAbsFlag );

   /*if( rSurf.NeedCreateObol() == TRUE )
    {
      PTSurface  pObol = (PTSurface)listObl[ rSurf.Index() ];
      if( pObol ) pObol->RotateX( angle );
    }*/
  }

 void TScena::RotateY( RTSurface rSurf, double angle )
  {
  if( rSurf == NOOBJECT ) return;
   angle = GRAD_TO_RAD( angle );

   bFlChanged = TRUE;

   //BOOL keepAbsFlag = rSurf.GetAbsFlag();
   //rSurf.SetAbsFlag( FALSE );

   //if( keepAbsFlag == TRUE ) rSurf.ResetVertex();
   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();
   T3DPoint pt = rSurf.GetGeomCenter();

   rSurf.Move( 0 - pt.x, 0 - pt.y, 0 - pt.z );
   if( bFlBaseUCS == TRUE ) RecalcBodyUserToBase( rSurf );
   rSurf.RotateY( angle );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );
   rSurf.Move( pt.x - 0, pt.y - 0, pt.z - 0 );

   //rSurf.SetAbsFlag( keepAbsFlag );
  }

void TScena::RotateXYZ( RTSurface rSurf, double anglX, double anglY, double anglZ, T3DPoint ptBase )
 {
   if( rSurf == NOOBJECT ) return;

   double anX = GRAD_TO_RAD( anglX ),
          anY = GRAD_TO_RAD( anglY ),
	  anZ = GRAD_TO_RAD( anglZ );

   bFlChanged = TRUE;

   //BOOL keepAbsFlag = rSurf.GetAbsFlag();
   //rSurf.SetAbsFlag( FALSE );

   //if( keepAbsFlag == TRUE ) rSurf.ResetVertex();
   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();


   rSurf.Move( 0 - ptBase.x, 0 - ptBase.y, 0 - ptBase.z );
   if( bFlBaseUCS == TRUE ) RecalcBodyUserToBase( rSurf );
   rSurf.RotateX( anX );
   rSurf.RotateY( anY );
   rSurf.RotateZ( anZ );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );
   rSurf.Move( ptBase.x - 0, ptBase.y - 0, ptBase.z - 0 );

   //rSurf.SetAbsFlag( keepAbsFlag );

   if( rSurf.NeedCreateObol() == TRUE )
     NewObol( rSurf );
 }

struct TRotateData {
  double anX, anY, anZ;
  T3DPoint pCent;
  PTScena pSc;

  TRotateData( PTScena pScena, double anglX, double anglY, double anglZ, RT3DPoint rCen )
   {
     pSc = pScena;
     anX = anglX; anY = anglY; anZ = anglZ;
     pCent = rCen;
   }
 };
typedef TRotateData _FAR *PTRotateData;


static void RotateXYZAllSelectedInstance( Object _FAR &rObj, void _FAR *pDta )
 {
   if( RTSurface(rObj).IsSelected() == FALSE ) return;

   PTRotateData(pDta)->pSc->RotateXYZ( RTSurface(rObj),
     PTRotateData(pDta)->anX,
     PTRotateData(pDta)->anY,
     PTRotateData(pDta)->anZ,
     PTRotateData(pDta)->pCent );
 }


void TScena::RotateXYZAllSelected( double anglX, double anglY, double anglZ, T3DPoint ptBase )
 {
   TRotateData rdDta( this, anglX, anglY, anglZ, ptBase );
   listSurf.forEach( RotateXYZAllSelectedInstance, Pvoid(&rdDta) );
 }

void TScena::ScaleXYZ( RTSurface rSurf, double scX, double scY, double scZ, T3DPoint cent )
  {
    if( rSurf == NOOBJECT ) return;

   bFlChanged = TRUE;

   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();

   if( bFlBaseUCS == TRUE )
    {
      RecalcBodyUserToBase( rSurf );
      pUcs->RecalcUserToBase( &cent, 1 );
    }
   rSurf.ScaleXYZ( scX, scY, scZ, cent );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );

   if( rSurf.NeedCreateObol() == TRUE )
     NewObol( rSurf );
    /*{
      PTSurface  pObol = (PTSurface)listObl[ rSurf.Index() ];
      if( pObol ) pObol->Scale3D( sc, pObol->GetGeomCenter() );
    }*/
  }

struct TScaleData {
  PTScena pScena;
  double scX, scY, scZ;
  T3DPoint pCent;

  TScaleData( PTScena pSc, double scX, double scY, double scZ, RT3DPoint rCen )
   {
     pScena = pSc;
     this->scX = scX;
     this->scY = scY;
     this->scZ = scZ;
     pCent = rCen;
   }
 };
typedef TScaleData _FAR *PTScaleData;


static void ScaleXYZAllSelectedInstance( Object _FAR &rObj, void _FAR *pDta )
 {
   if( RTSurface(rObj).IsSelected() == FALSE ) return;

   PTScaleData(pDta)->pScena->ScaleXYZ( RTSurface(rObj),
     PTScaleData(pDta)->scX,
     PTScaleData(pDta)->scY,
     PTScaleData(pDta)->scZ,
     PTScaleData(pDta)->pCent );
 }

void TScena::ScaleXYZAllSelected( double scX, double scY, double scZ, T3DPoint pCenterScale )
 {
   //&&&
   TScaleData scdDta( this, scX, scY, scZ, pCenterScale );
   listSurf.forEach( ScaleXYZAllSelectedInstance, Pvoid(&scdDta) );
 }


void TScena::Scale2D( RTSurface rSurf, double sc, T3DPoint pCenterScale )
  {
    if( rSurf == NOOBJECT ) return;

   bFlChanged = TRUE;

   if( rSurf.GetAbsFlag() == TRUE ) rSurf.ResetVertex();

   if( bFlBaseUCS == TRUE )
    {
      RecalcBodyUserToBase( rSurf );
      pUcs->RecalcUserToBase( &pCenterScale, 1 );
    }
   rSurf.Scale2D( sc, pCenterScale );
   if( bFlBaseUCS == TRUE ) RecalcBodyBaseToUser( rSurf );


   /*pUcs->RecalcUserToBase( &pCenterScale, 1 );
   RecalcBodyUserToBase( rSurf );
   rSurf.Scale2D( sc, pCenterScale );
   RecalcBodyBaseToUser( rSurf );*/

   if( rSurf.NeedCreateObol() == TRUE )
     NewObol( rSurf );
  }

static void Scale2DAllSelectedInstance( Object _FAR &rObj, void _FAR *pDta )
 {
   if( RTSurface(rObj).IsSelected() == FALSE ) return;

   PTScaleData(pDta)->pScena->Scale2D( RTSurface(rObj), PTScaleData(pDta)->scX, PTScaleData(pDta)->pCent );
 }

void TScena::Scale2DAllSelected( double sc, T3DPoint pCenterScale )
 {
   //&&&
   TScaleData scdDta( this, sc, 100, 100, pCenterScale );
   listSurf.forEach( Scale2DAllSelectedInstance, Pvoid(&scdDta) );
 }


void TScena::RecalcLPtoDP()
 {
   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      DoubleListIterator dbV = DoubleListIterator( pSerf->listVertex );
      PTVertex  pVert = (PTVertex)&dbV.current();

      for( ; int(dbV); pVert = (PTVertex)&++dbV )
       {
	 POINT p;
	 p.x = pVert->x;
	 p.y = pVert->y;
	 LPtoDP( hdc, &p, 1 );
	 pVert->x = p.x;
	 pVert->y = p.y;
       }

      /*if( pSerf->isA() == TYP_TOMNI )
       {
	 PTOmni pOmni = (PTOmni)(pSerf->TopPtr());	 
	 POINT p;
	 p.x = 0;
	 p.y = pOmni->GetMult();
	 LPtoDP( hdc, &p, 1 );	
	 pOmni->Mult( sqrt(double(p.x) * double(p.x) + double(p.y) * double(p.y)) );
       }*/
    } 
 }


void TScena::RecalcDPtoLP()
 {
   DoubleListIterator dbIter = DoubleListIterator( listSurf );
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
    {
      DoubleListIterator dbV = DoubleListIterator( pSerf->listVertex );
      PTVertex  pVert = (PTVertex)&dbV.current();

      for( ; int(dbV); pVert = (PTVertex)&++dbIter )
       {
	 POINT p;
	 p.x = pVert->x;
	 p.y = pVert->y;
	 DPtoLP( hdc, &p, 1 );
	 pVert->x = p.x;
	 pVert->y = p.y;
       }
    } 
 }


static void RecalcUserToBaseInstance( Object _FAR &rObj, void _FAR *pSc )
 {
   PTScena(pSc)->RecalcBodyUserToBase( RTSurface(rObj) );
 }

void TScena::RecalcUserToBase()
 {

   listSurf.forEach( RecalcUserToBaseInstance, Pvoid(this) );

   /*DoubleListIterator dbIter = DoubleListIterator( listSurf );   
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     RecalcBodyUserToBase( *pSerf );*/
 }

static void RecalcBaseToUserInstance( Object _FAR &rObj, void _FAR *pSc )
 {
   PTScena(pSc)->RecalcBodyBaseToUser( RTSurface(rObj) );
 }


void TScena::RecalcBaseToUser()
 {
   /*DoubleListIterator dbIter = DoubleListIterator( listSurf );   
   PTSurface  pSerf = (PTSurface)&dbIter.current();

   for( ; int(dbIter); pSerf = (PTSurface)&++dbIter )
     RecalcBodyBaseToUser( *pSerf );*/

   listSurf.forEach( RecalcBaseToUserInstance, Pvoid(this) );
 }



static void RecalcBodyUserToBaseInstance( Object _FAR &rObj, void _FAR *pUcs )
 {
   PTUCS(pUcs)->RecalcUserToBase( &RT3DPoint(RTVertex(rObj)), 1 );
 }

void TScena::RecalcBodyUserToBase( RTSurface rSurf )
 {
   rSurf.listVertex.forEach( RecalcBodyUserToBaseInstance, Pvoid(pUcs) );
   /*if( rSurf.isA() == TYP_TOMNI )
    {
      PTOmni pOmni = (PTOmni)(rSurf.TopPtr());
      T3DPoint pt( 0, 0, pOmni->GetMult() );
      pUcs->RecalcUserToBase( &pt, 1 );
      pOmni->Mult( sqrt(pt.x * pt.x + pt.y * pt.y + pt.y * pt.y) );
    }*/
 }

static void RecalcBodyBaseToUserInstance( Object _FAR &rObj, void _FAR *pUcs )
 {
   PTUCS(pUcs)->RecalcBaseToUser( &RT3DPoint(RTVertex(rObj)), 1 );
 }


void TScena::RecalcBodyBaseToUser( RTSurface rSurf )
 {
   rSurf.listVertex.forEach( RecalcBodyBaseToUserInstance, Pvoid(pUcs) );

   /*DoubleListIterator dbVert = DoubleListIterator( rSurf.listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbVert.current();
   for( ; int(dbVert); pVert = (PTVertex)&--dbVert )
    {
      T3DPoint pt = T3DPoint( pVert->x, pVert->y, pVert->z );
      pUcs->RecalcBaseToUser( &pt, 1 );
      pVert->x = pt.x; pVert->y = pt.y; pVert->z = pt.z;
    }*/

   //rSurf.CalcNormals();
 }



_CLASSDEF(TDeleteData)
class TDeleteData
 {
public:
  BOOL bFlOnlySelected;
  PTIndexedDoubleList pListObl;
  PTIndexedDoubleList pListSurf;
  HWND hwnd;
  PTApplication pApp;

  TDeleteData( PTIndexedDoubleList pLO, PTIndexedDoubleList pLS, BOOL bFlOS, HWND hw,
	       PTApplication pApp_ )
   {
     bFlOnlySelected = bFlOS; hwnd = hw;
     pListObl = pLO;
     pListSurf = pLS;
     pApp = pApp_;
   }
 };

static void DeleteBodyInstance( Object _FAR & rObj, void _FAR * pDta )
 {
   PTIndexedItem pTmp = (*PTDeleteData(pDta)->pListObl)[ RTSurface(rObj).Index() ];
   if( RTSurface(rObj).NeedCreateObol() == TRUE && !pTmp )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_13 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( PTDeleteData(pDta)->hwnd, str_(),
		      ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: scena cann't find obol."
      return;
    }

   //PTObol pObl = pTmp ? (PTObol)pTmp->TopPtr():NULL;

   PTObol pTmp__;
   PTObol pObl = TSPtr( (PTSurface)pTmp, pTmp__ );

   if( PTDeleteData(pDta)->bFlOnlySelected == FALSE ||
       (PTDeleteData(pDta)->bFlOnlySelected == TRUE && RTSurface(rObj).IsSelected() == TRUE)
     )
    {
      if( pObl ) PTDeleteData(pDta)->pListObl->detach( *pObl, TShouldDelete::Delete );
      PTDeleteData(pDta)->pListSurf->detach( RTSurface(rObj), TShouldDelete::Delete );
    }
 }

void TScena::DeleteBody( RTSurface rSurf )
 {
   if( rSurf == NOOBJECT ) return;

  /* PTObol pObl = (PTObol)listObl[ rSurf.Index() ];
   if( *pObl == NOOBJECT )
    {
      BWCCMessageBox( pWin->HWindow, "Internal application error: scena cann't find obol.",
                      "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      return;
    }

   listObl.detach( *pObl, TShouldDelete::Delete );
   listSurf.detach( rSurf, TShouldDelete::Delete );*/

   TDeleteData ddDta( &listObl, &listSurf, FALSE, pWin->HWindow, pApplicat );
   DeleteBodyInstance( rSurf, Pvoid(&ddDta) );
   bFlChanged = TRUE;
 }



void TScena::DeleteAllSelected()
 {
   if( SelectedCount(FilterAll) ) bFlChanged = TRUE;
   TDeleteData ddDta( &listObl, &listSurf, TRUE, pWin->HWindow, pApplicat );
   listSurf.forEach( DeleteBodyInstance, Pvoid(&ddDta) );
 }

void TScena::NewObol( RTSurface rSurf )
 {
   if( rSurf == NOOBJECT ) return;

   PTIndexedItem pTmp = listObl[ rSurf.Index() ];
   if( pTmp )
    {
      //PTObol pObl = (PTObol)pTmp->TopPtr();
      PTObol pTmp__;
      PTObol pObl = TSPtr( (PTSurface)pTmp, pTmp__ );
      listObl.detach( *pObl, TShouldDelete::Delete );
    }


   PTObol pObl = new TObol( rSurf.Index(), &rSurf );
   if( pObl )
     listObl.add( *pObl );
 }


static void AssignColorIndexesInstance( Object _FAR &rObj, void _FAR *pInd )
 {
   RTSurface(rObj).AssignFaceColorIndex( (*(int _FAR *)pInd)++ );
   if( *(int _FAR *)pInd > 255 ) *(int _FAR *)pInd = 255;
 }

void TScena::AssignColorIndexes() 
 {
   int index = 3; // 0, 1, 2 - fon, adges, intersect

   listSurf.forEach( AssignColorIndexesInstance, Pvoid(&index) );
 }

/*static void CreateColorTblInstance( Object _FAR &rObj, void _FAR *pTbl )
 {
   (PTColorTable(pTbl))[ RTSurface(rObj).GetFaceColorIndex() ] =
     RTSurface(rObj).GetFaceRGBColor();
 }


PTColorTable TScena::CreateColorTbl()
 {
   PTColorTable pctTbl = new TColorTable[256];
   if( !pctTbl ) return NULL;

   _fmemset( pctTbl, 0, sizeof(RGBQUAD) * 256 );
   listSurf.forEach( CreateColorTblInstance, Pvoid(pctTbl) );

   PALETTEENTRY p1;
   GetSystemPaletteEntries( hdc, 6, 1, &p1 );

   pctTbl[0].rgbBlue = p1.peBlue;
   pctTbl[0].rgbGreen = p1.peGreen;
   pctTbl[0].rgbRed = p1.peRed;

  //typedef RGBQUAD TColorTable;
   pctTbl[1] = rgbAdges;
  

   pctTbl[2] = (bFlMarkIntersect == TRUE) ? rgbIntersect:rgbAdges;

   return pctTbl;
 }*/


void TScena::GetSwap( PTIndicator pInd )
 {
/*   pInd->UpdateStr( 2, "Fetch data from disk" );

   Flush();

   OFSTRUCT of;
   HFILE hf = OpenFile( "3dw.swp", &of, OF_READ );

   lseek( hf, 0, SEEK_END ); 
   pInd->Init( tell( hf ) );
   lseek( hf, 0, SEEK_SET );

   pInd->SetStatusButton( FALSE );
   int res;
   if( (res = Load( hf, pInd )) && res != 1 )
    {
      _lclose( hf );
      OpenFile( "3dw.swp", &of, OF_DELETE );

      TAutoStringRc str_( *pApplicat, ID_MSG_14 );
      TAutoStringRc ttl_( *pApplicat, ID_TITLE_ERROR );
      BWCCMessageBox( pWin->HWindow, str_(), ttl_(),
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Error reading swap file"
      return;
    }

   _lclose( hf );
   OpenFile( "3dw.swp", &of, OF_DELETE );

   pInd->SetStatusButton( TRUE );*/
 }


BOOL TScena::Flush( BOOL bFlAsk )
 {
   if( bFlAsk == TRUE && bFlChanged == TRUE )
    {
      char cBuf[256];
      TAutoStringRc s1_ = TAutoStringRc( hInstRC, ID_MSG_79 );
      wsprintf( cBuf, /*"File: ""%s"" is not saved !  Reastore ?"*/s1_(),
		pGlobalFManeger->GetInfo( GetId() )->LpFileNameFull );
      TAutoStringRc ttl_ = TAutoStringRc( hInstRC, ID_TITLE_WARNING, 10 );
      int res = BWCCMessageBox( NULL, cBuf, ttl_(),
        MB_ICONEXCLAMATION | MB_OKCANCEL | MB_APPLMODAL );
      if( res == IDCANCEL ) return FALSE;
    }

   listSurf.flush( TShouldDelete::Delete );
   listObl.flush( TShouldDelete::Delete );

   return TRUE;
 }



  //-1 - internal
  //-2 - memory
  //-3 - hard
  //-4 - CRC         /***********************************
  //-5 - Cancel

struct TSaveDta {
  HFILE hf;
  PTIndicator pInd;

  TSaveDta( HFILE hf_, PTIndicator pI )
   {
     hf = hf_; pInd = pI;
   };
 };
typedef TSaveDta _FAR *PTSaveDta;

static int SaveInstance( const Object _FAR &rObj, void _FAR *pSD )
 {
   classType iClass = RTSurface(rObj).isAPrepare();

   if( _lwrite(PTSaveDta(pSD)->hf, &iClass, sizeof(classType)) == HFILE_ERROR )
    {
      RTSurface(rObj).Status = -3; return 1;
    } 

   int res;
   if( (res = RTSurface(rObj).Save( PTSaveDta(pSD)->hf, PTSaveDta(pSD)->pInd )) < 0 )
    {
      RTSurface(rObj).Status = res; return 1;
    }

   return 0;
 }

int TScena::Save( HFILE hf, PTIndicator pInd  )
 {
   TScenaHdr shHdr( listSurf.getItemsInContainer(),
     bFlShowBackFaces,
     bFlFullDetal,
     bFlVisyal,

     rgbAdges,
     rgbIntersect,
     rgbAmbient,
     dAmbMult,
     polyMethod,
     bFlMarkIntersect,

     iPaletteSize,
     (int)ifFormat,
     caAlhoPalette,

     bFlAntiAliasing,
     bFlShadows,
     

     iNumbPointsAntiAlias,
     dStrengtsAntialias,

     saShadingLimit,
     tmTraceModel,

     dAngleSmothGrad,

     (int)nNoisy, 
     iMarginalAmplitude, 
     iUniformAmplitude,  
     iUniformThreshold,

     bFlStrictGamma
		  );

   shHdr.InitCRC();

   if( shHdr.Save(hf) == HFILE_ERROR ) return -3;

   int iClassTyp = !!mrcBackGraund ? mrcBackGraund()->isA():-1;
   if( _lwrite( hf, &iClassTyp, sizeof(int) ) == HFILE_ERROR ) return -3;
   if( iClassTyp != -1 )
     if( mrcBackGraund()->Save(hf) == HFILE_ERROR ) return -3;

   TSaveDta sd( hf, pInd );
   RTSurface rSurf = (RTSurface)listSurf.firstThat( SaveInstance, Pvoid(&sd) );

   int ret = 0;
   if( rSurf != NOOBJECT ) { ret = rSurf.Status; rSurf.Status = 0; }

   if( !ret ) bFlChanged = FALSE;

   return ret;
 }

int TScena::Load( HFILE hf, PTIndicator pInd )
 {
   bFlChanged = FALSE;

   TScenaHdr shHdr;

   if( shHdr.Read(hf) == HFILE_ERROR ) return -3;
   if( shHdr.CheckCRC() == FALSE ) return -4;

   bFlShowBackFaces = shHdr.d.bFlShowBackFaces;
   bFlFullDetal = shHdr.d.bFlFullDetal;
   bFlVisyal = shHdr.d.bFlVisyal;

   rgbAdges = shHdr.d.rgbAdges;
   rgbIntersect = shHdr.d.rgbIntersect;
   rgbAmbient = shHdr.d.rgbAmbient;
   dAmbMult = shHdr.d.dAmbMult;
   polyMethod = shHdr.d.polyMethod;
   bFlMarkIntersect = shHdr.d.bFlMarkIntersect;
   ifFormat = (TImageFormat)shHdr.d.ifFormat;

   iPaletteSize = shHdr.d.iPaletteSize;
   caAlhoPalette = (TConvertAlho)shHdr.d.caAlhoPalette;

   bFlAntiAliasing = shHdr.d.bFlAntiAliasing;
   bFlShadows = shHdr.d.bFlShadows;



   iNumbPointsAntiAlias = shHdr.d.iNumbPointsAntiAlias;
   dStrengtsAntialias =   shHdr.d.dStrengtsAntialias;

   saShadingLimit = (TShadingAlho)shHdr.d.saShadingLimit;
   tmTraceModel = (TTraceModel)shHdr.d.tmTraceModel;

   dAngleSmothGrad = shHdr.d.dAngleSmothGrad;

   nNoisy = (TNoisy)shHdr.d.nNoisy;
   iMarginalAmplitude = shHdr.d.iMarginalAmplitude;
   iUniformAmplitude = shHdr.d.iUniformAmplitude;
   iUniformThreshold = shHdr.d.iUniformThreshold;

   bFlStrictGamma = shHdr.d.bFlStrictGamma;

   int iClassTyp;
   UINT l;
   if( (l = _lread( hf, &iClassTyp, sizeof(int) )) == HFILE_ERROR ) return -3;
   if( l != sizeof(int) ) return -3;

   if( iClassTyp == -1 )
    {
      mrcBackGraund.FreeRc(); 
    }
   else
    {     
      mrcBackGraund = CreateBkForClass( iClassTyp );

      if( !!mrcBackGraund && mrcBackGraund()->Load(hf) == HFILE_ERROR )
       {
	 //if( bgBackGraund ) delete bgBackGraund; bgBackGraund = NULL;
	 mrcBackGraund.FreeRc();
	 return -3;
       }
    }

   for( int i = 0; i < shHdr.d.iNumSurf; i++ )
    {
      classType iClass;

      int res;
      if( (res = _lread(hf, &iClass, sizeof(classType))) == HFILE_ERROR ||
	  res != sizeof(classType)
	)
        return -3;

      PTSurface pSf;
      switch( iClass )
       {
	 case TYP_FACE:
	   pSf = new TSurface( i + 1, NULL );
           break;
	   
	 case TYP_TOMNI:
	   pSf = new TOmni( i + 1, NULL );
	   break;
	   
	 case TYP_TCAMERA:
	   pSf = new TCamera( i + 1, NULL );
	   break;

	 default:
           return -4;
       };

      if( !pSf ) return -2;

      res = 0;
      if( (res = pSf->Load( hf, pInd )) < 0 )
       {delete pSf; return res; }

      AddBody( pSf );
      bFlChanged = FALSE;
    }

   return 0;
 }


_CLASSDEF(TSizeData)

class TSizeData
 {
public:
   unsigned long size;
   BOOL bFlOnlySelected;
   BOOL bFlOnlyFace;

   TSizeData( BOOL bFlOF, BOOL bFlOS )
    {
      size = 0;
      bFlOnlyFace = bFlOF;
      bFlOnlySelected = bFlOS;
    }
 };

static void GetBytesSizeInstance( Object _FAR & rObj, void _FAR * pPar )
 {
   if( PTSizeData(pPar)->bFlOnlyFace == TRUE &&
      (rObj.isA() == TYP_TOMNI || rObj.isA() == TYP_TCAMERA) )
     return;

   if( PTSizeData(pPar)->bFlOnlySelected == FALSE )
     PTSizeData(pPar)->size += RTSurface(rObj).GetBytesSize();
   else if( RTSurface(rObj).IsSelected() == TRUE )
     PTSizeData(pPar)->size += RTSurface(rObj).GetBytesSize();
 }


long TScena::GetBytesSize( BOOL bFlOnlyFace, BOOL bFlOnlySelected ) const
 {
   TSizeData sdDta( bFlOnlyFace, bFlOnlySelected );
   listSurf.forEach( GetBytesSizeInstance, Pvoid(&sdDta) );

   return sdDta.size;
 }


long TScena::GetBytesSizeWithObol( BOOL bFlOnlyFace ,BOOL bFlOnlySelected ) const
 {
   TSizeData sdDta( bFlOnlyFace, bFlOnlySelected );
   listObl.forEach( GetBytesSizeInstance, Pvoid(&sdDta) );

   return  (unsigned long)GetBytesSize( bFlOnlyFace, bFlOnlySelected ) + sdDta.size;
 }





_CLASSDEF(TSelWinData)

class  TSelWinData
 {
public:
  RECT* r;
  BOOL  bFlSel;
  PTUCS  pUcs;
  BOOL bFlChanged;

  TSelWinData( RECT& r, BOOL bFl, PTUCS pU )
   {
     this->r = &r; bFlSel = bFl; pUcs = pU; bFlChanged = FALSE;
   }

 };


static void Selector( Object _FAR& rObj, void _FAR* pSelDta )
 {
   BOOL bFlKeySelState = RTSurface(rObj).IsSelected();

   RTSurface(rObj).Select( PTSelWinData(pSelDta)->bFlSel );

   if( RTSurface(rObj).IsSelected() != bFlKeySelState )
     PTSelWinData(pSelDta)->bFlChanged = TRUE;
 }


BOOL TScena::SelectAll( BOOL bFlInvert )
 {
   RECT rRect; _fsetmem( &rRect, sizeof(RECT), 0 );
   TSelWinData swdDta( rRect, (bFlInvert == FALSE) ? TRUE:FALSE, pUcs );

   listSurf.forEach( Selector, Pvoid(&swdDta) );

   return swdDta.bFlChanged;
 }

static void InvertInstance( Object _FAR& rObj, void _FAR* pSelDta )
 {
   BOOL bFlKeySelState = RTSurface(rObj).IsSelected();

   RTSurface(rObj).Select( (bFlKeySelState == FALSE) ? TRUE:FALSE );

   if( RTSurface(rObj).IsSelected() != bFlKeySelState )
     PTSelWinData(pSelDta)->bFlChanged = TRUE;
 }


BOOL TScena::SelectInvert()
 {
   RECT rRect; _fsetmem( &rRect, sizeof(RECT), 0 );
   TSelWinData swdDta( rRect, 0, pUcs );

   listSurf.forEach( InvertInstance, Pvoid(&swdDta) );

   return swdDta.bFlChanged;
 }

/*struct TDelta {
  double dX, dY, dZ;
  double minX, maxX, minY, maxY, minZ, maxZ;

  TDelta()
   {
     dX = dY = dZ = 0;
     minX = MAXINT; maxX = -MAXINT,
     minY = MAXINT; maxY = -MAXINT,
     minZ = MAXINT; maxZ = -MAXINT;
   }
 };
typedef TDelta _FAR *PTDelta;*/

struct TDeltaEx: public TDelta
 {
   PTUCS pUcs;

   TDeltaEx( PTUCS pUc ):TDelta() { pUcs = pUc; }
 };
typedef TDeltaEx _FAR *PTDeltaEx;


static void WinSelectInstance2( Object _FAR &rObj, void _FAR *pD )
 {
   //T3DPoint  tp = T3DPoint( RTVertex(rObj).x, RTVertex(rObj).y, RTVertex(rObj).z );
   T3DPoint  tp = T3DPoint( (RT3DPoint)RTVertex(rObj) );
   PTDeltaEx(pD)->pUcs->RecalcUserToBase( &tp, 1 );

   PTDeltaEx(pD)->minX = (tp.x < PTDeltaEx(pD)->minX) ? tp.x:PTDeltaEx(pD)->minX;   
   PTDeltaEx(pD)->maxX = (tp.x > PTDeltaEx(pD)->maxX) ? tp.x:PTDeltaEx(pD)->maxX;
   PTDeltaEx(pD)->minY = (tp.y < PTDeltaEx(pD)->minY) ? tp.y:PTDeltaEx(pD)->minY;
   PTDeltaEx(pD)->maxY = (tp.y > PTDeltaEx(pD)->maxY) ? tp.y:PTDeltaEx(pD)->maxY;
 }

static void WinSelectInstance( Object _FAR & rObj, void _FAR * pData )
 {
   PTSurface pSf = (PTSurface)&rObj;
   PTSelWinData  pDta = (PTSelWinData)pData;
   
   /*double  dMinX = MAXINT, dMaxX = -MAXINT,
	   dMinY = MAXINT, dMaxY = -MAXINT;*/
					       //&&&
   TDeltaEx delta( pDta->pUcs );
   pSf->listVertex.forEach( WinSelectInstance2, Pvoid(&delta) );

/*   DoubleListIterator dbIter = DoubleListIterator( pSf->listVertex );
   PTVertex pV = (PTVertex)&dbIter.current();

   for( ; int(dbIter); pV = (PTVertex)&++dbIter )
    {
      T3DPoint  tp = T3DPoint( pV->x, pV->y, pV->z );
      pDta->pUcs->RecalcUserToBase( &tp, 1 );

      dMinX = (tp.x < dMinX) ? tp.x:dMinX;
      dMaxX = (tp.x > dMaxX) ? tp.x:dMaxX;
      dMinY = (tp.y < dMinY) ? tp.y:dMinY;
      dMaxY = (tp.y > dMaxY) ? tp.y:dMaxY;
    }
  */

   BOOL bFlKeySelState = pSf->IsSelected();

   //minX, maxX, minY, maxY, minZ, maxZ;
   if( pDta->r->left <= delta.minX && pDta->r->right >= delta.maxX &&
       pDta->r->top <= delta.minY && pDta->r->bottom >= delta.maxY
     )
     pSf->Select( pDta->bFlSel );
    

   if( pSf->IsSelected() != bFlKeySelState ) pDta->bFlChanged = TRUE;
 }

static int CrossSelectInstance2( const Object _FAR &rObj, void _FAR *pD )
 {
   //T3DPoint  tp = T3DPoint( RTVertex(rObj).x, RTVertex(rObj).y, RTVertex(rObj).z );
   T3DPoint  tp = T3DPoint( (RT3DPoint)RTVertex(rObj) );
   PTDeltaEx(pD)->pUcs->RecalcUserToBase( &tp, 1 );

   return  int(tp.x >= PTDeltaEx(pD)->minX && tp.x <= PTDeltaEx(pD)->maxX &&
	       tp.y >= PTDeltaEx(pD)->minY && tp.y <= PTDeltaEx(pD)->maxY);
 }


static void CrossSelectInstance( Object _FAR & rObj, void _FAR * pData )
 {
   PTSurface pSf = (PTSurface)&rObj;
   PTSelWinData  pDta = (PTSelWinData)pData;
   
   BOOL bFlKeySelState = pSf->IsSelected();

   TDeltaEx delta( pDta->pUcs );
   delta.minX = pDta->r->left;
   delta.minY = pDta->r->top;
   delta.maxX = pDta->r->right;
   delta.maxY = pDta->r->bottom;
   if( pSf->listVertex.firstThat( CrossSelectInstance2, Pvoid(&delta) ) != NOOBJECT )
     pSf->Select( pDta->bFlSel );  
   

   if( pSf->IsSelected() != bFlKeySelState ) pDta->bFlChanged = TRUE;
 }


BOOL TScena::SelectWithWindow( RECT& rRect, BOOL bFlSelector )
 {
   TSelWinData swdDta( rRect, bFlSelector, pUcs );

   listSurf.forEach( WinSelectInstance, Pvoid(&swdDta) );

   return swdDta.bFlChanged;
 }   

BOOL TScena::SelectWithCrossing( RECT& rRect, BOOL bFlSelector )
 {
   TSelWinData swdDta( rRect, bFlSelector, pUcs );

   listSurf.forEach( CrossSelectInstance, Pvoid(&swdDta) );

   return swdDta.bFlChanged;
 }

void TScena::Merge( RTScena rSc )
 {

   DoubleListIterator dbIt = DoubleListIterator( rSc.listSurf );
   PTSurface pSf = (PTSurface)&dbIt.current();

   for( ; int(dbIt); pSf = (PTSurface)&++dbIt )
    {
      int ind = GetFreeIndex();
      pSf->ChangeIndex( ind );

      PTIndexedItem pTmp = rSc.listObl[ pSf->Index() ];
      //PTObol pObl = pTmp ? ((PTObol)pTmp->TopPtr()):NULL;
      PTObol pTmp__;
      PTObol pObl = TSPtr( (PTSurface)pTmp, pTmp__ );

      if( pObl )
       {
	 listSurf.add( *pSf );
	 rSc.listObl.detach( *pObl, TShouldDelete::NoDelete );
	 pObl->ChangeIndex( ind );
	 listObl.add( *pObl );
       }
      else AddBody( pSf );
    }

   rSc.listSurf.flush( TShouldDelete::NoDelete );
   bFlChanged = TRUE;
 }

struct TSelectData
 {
   unsigned long count;
   PTClassFilterFunc pFiltr;

   TSelectData( PTClassFilterFunc pFiltr_ )
    {
      pFiltr = pFiltr_;
      count = 0;
    }
 };
typedef TSelectData* PTSelectData;

static void SelectedCountInstance( Object _FAR & rObj, void _FAR * pTsd )
 {
   if( PTSelectData(pTsd)->pFiltr( &RTSurface(rObj) ) == TRUE &&
       RTSurface(rObj).IsSelected() == TRUE
     )
     PTSelectData(pTsd)->count++;
 }

unsigned long TScena::SelectedCount( PTClassFilterFunc pFiltr ) const
 {
   TSelectData tsD( pFiltr );
   listSurf.forEach( SelectedCountInstance, Pvoid(&tsD) );

   return tsD.count;
 }

static void GetGeomCenterSelectedInstance( Object _FAR &rObj, void _FAR *pD )
 {
   if( RTSurface(rObj).IsSelected() == FALSE ) return;
   T3DPoint pCent = RTSurface(rObj).GetGeomCenter();

   PTDelta(pD)->minX = (pCent.x < PTDelta(pD)->minX) ? pCent.x:PTDelta(pD)->minX;
   PTDelta(pD)->maxX = (pCent.x > PTDelta(pD)->maxX) ? pCent.x:PTDelta(pD)->maxX;

   PTDelta(pD)->minY = (pCent.y < PTDelta(pD)->minY) ? pCent.y:PTDelta(pD)->minY;
   PTDelta(pD)->maxY = (pCent.y > PTDelta(pD)->maxY) ? pCent.y:PTDelta(pD)->maxY;

   PTDelta(pD)->minZ = (pCent.z < PTDelta(pD)->minZ) ? pCent.z:PTDelta(pD)->minZ;
   PTDelta(pD)->maxZ = (pCent.z > PTDelta(pD)->maxZ) ? pCent.z:PTDelta(pD)->maxZ;
 } 

T3DPoint TScena::GetGeomCenterSelected() const
 {
   TDelta delta;
   listSurf.forEach( GetGeomCenterSelectedInstance, Pvoid(&delta) );

   return  T3DPoint( delta.minX + fabs(delta.maxX - delta.minX) / 2.0,
		     delta.minY + fabs(delta.maxY - delta.minY) / 2.0,
		     delta.minZ + fabs(delta.maxZ - delta.minZ) / 2.0
		   );
 }

static void GetDeltaInstance( Object _FAR &rObj, void _FAR *pD )
 {
   PTDelta(pD)->minX = (RTVertex(rObj).x < PTDelta(pD)->minX) ? RTVertex(rObj).x:PTDelta(pD)->minX;
   PTDelta(pD)->maxX = (RTVertex(rObj).x > PTDelta(pD)->maxX) ? RTVertex(rObj).x:PTDelta(pD)->maxX;

   PTDelta(pD)->minY = (RTVertex(rObj).y < PTDelta(pD)->minY) ? RTVertex(rObj).y:PTDelta(pD)->minY;
   PTDelta(pD)->maxY = (RTVertex(rObj).y > PTDelta(pD)->maxY) ? RTVertex(rObj).y:PTDelta(pD)->maxY;

   PTDelta(pD)->minZ = (RTVertex(rObj).z < PTDelta(pD)->minZ) ? RTVertex(rObj).z:PTDelta(pD)->minZ;
   PTDelta(pD)->maxZ = (RTVertex(rObj).z > PTDelta(pD)->maxZ) ? RTVertex(rObj).z:PTDelta(pD)->maxZ;
 }

TDelta TScena::GetDelta( RTSurface rSf )
 {
   TDelta delta;
   rSf.listVertex.forEach( GetDeltaInstance, Pvoid(&delta) );

   delta.dX = fabs( delta.maxX - delta.minX ),
   delta.dY = fabs( delta.maxY - delta.minY ),
   delta.dZ = fabs( delta.maxZ - delta.minZ );

   return delta;
 }

static void GetDeltaScenaInstance( Object _FAR &rObj, void _FAR *pD )
 {
   if( RTSurface(rObj).IsSelected() == FALSE ) return;
   RTSurface(rObj).listVertex.forEach( GetDeltaInstance, pD );
 }

TDelta TScena::GetDeltaSelected() const
 {
   TDelta delta;
   listSurf.forEach( GetDeltaScenaInstance, Pvoid(&delta) );

   delta.dX = fabs( delta.maxX - delta.minX ),
   delta.dY = fabs( delta.maxY - delta.minY ),
   delta.dZ = fabs( delta.maxZ - delta.minZ );

   return delta;
 }

   //&&&&&&&&&&&&&&&&&&

PTScena TScena::Clone( RTSurface rSf ) const
 {
   PTScena pscScena = new TScena( 0, pApplicat);
   if( !pscScena ) return NULL;
   pscScena->SetHardwareEnviron( pUcs, hdc, pWin );

   //PTSurface  pSu = new TSurface( rSf, NULL );
   PTSurface  pSu = CloneObj( rSf, NULL );
   if( !pSu ) { delete pscScena; return NULL; }          
   pSu->Select( TRUE );

   pscScena->AddBody( pSu );

   return pscScena;
 }


static void CloneAllSelectedInstance( Object _FAR &rObj, void _FAR *pSc )
 {
   if( RTSurface(rObj).IsSelected() == FALSE ) return;

   //PTSurface  pSu = new TSurface( RTSurface(rObj), NULL );
   PTSurface  pSu = CloneObj( RTSurface(rObj), NULL );
   if( !pSu ) return;          
   pSu->Select( TRUE );

   PTScena(pSc)->AddBody( pSu );
 }

PTScena TScena::CloneAllSelected() const
 {
   PTScena pscScena = new TScena( 0, pApplicat );
   if( !pscScena ) return NULL;
   pscScena->SetHardwareEnviron( pUcs, hdc, pWin );

   listSurf.forEach( CloneAllSelectedInstance, Pvoid(pscScena) );
   if( !pscScena->listSurf.getItemsInContainer() )
    {
      delete pscScena;
      return NULL;
    }

   return pscScena;
 }

/*struct TVertexData
 {
   RECT      _FAR *pRect;
   POINT     _FAR *pPoint;
   PTSurface pSf;
   TVertex   pVert;
   double    dDistance;
   PTUCS     pUcs;

   TVertexData( RECT _FAR *pR, POINT _FAR *pP )
    {
      pRect = pR; pPoint = pP; pSf = NULL; pVert = NULL;
      dDistance = MAXINT;
    }
 };
typedef TVertexData _FAR *PTVertexData;*/


static void GetNearstVertexInRangeInstance2( Object _FAR &rObj, void _FAR *pVD )
 {
   RTVertex rV = RTVertex(rObj);
   TVertexData _FAR &rpVD = *PTVertexData( pVD );

   //T3DPoint tp( rV.x, rV.y, rV.z );
   T3DPoint tp( (RT3DPoint)rV );
   rpVD.pUcs->RecalcUserToBase( &tp, 1 );

   POINT p; p.x = tp.x; p.y = tp.y;

   if( rpVD.pRect->InRect( p ) == TRUE )
    {
      double dist = sqrt( (p.x - rpVD.pPoint->x)*(p.x - rpVD.pPoint->x) +
			  (p.y - rpVD.pPoint->y)*(p.y - rpVD.pPoint->y)
			);
      if( dist < rpVD.dDistance )
       {
	 rpVD.dDistance = dist;
	 rpVD.pVert = &rV;
	 rpVD.pSf = rpVD.pCurSf;
       }
    }
 }

static void GetNearstVertexInRangeInstance( Object _FAR &rObj, void _FAR *pVD )
 {
   PTVertexData(pVD)->pCurSf = &RTSurface(rObj);
   RTSurface(rObj).listVertex.forEach( GetNearstVertexInRangeInstance2, pVD );
 }

PTVertexData TScena::GetNearstVertexInRange( TRECT _FAR &rCursor, POINT _FAR &rPoint ) const
 {
   TVertexData vdDta( &rCursor, &rPoint, pUcs );
   listSurf.forEach( GetNearstVertexInRangeInstance, Pvoid(&vdDta) );

   if( vdDta.pVert ) return new TVertexData( vdDta );

   return NULL;
 }

static int GetSelectedFirstInstance( const Object _FAR &rObj, void _FAR* pF )
 {
   return (RTSurface(rObj).IsSelected() == TRUE &&
	   PTClassFilterFunc(pF)( &RTSurface(rObj) ) == TRUE) ? 1:0;
 }

PTSurface TScena::GetSelectedFirst( PTClassFilterFunc pFiltr ) const
 {
   RTSurface rSf = (RTSurface)listSurf.firstThat( GetSelectedFirstInstance, pFiltr );

   if( rSf == NOOBJECT ) return NULL;
   else return &rSf;
 }

static void AssignMediumAllSelectedInstance( Object _FAR &rObj, void _FAR *pMed )
 {
   if( RTSurface(rObj).IsSelected() == TRUE &&
       FilterBody(&RTSurface(rObj)) == TRUE )
     RTSurface(rObj).medium = *PTMedium(pMed);
 }

void TScena::AssignMediumAllSelected( RTMedium rMed )
 {
   listSurf.forEach( AssignMediumAllSelectedInstance, Pvoid(&rMed) );
 }

struct TAligmentZDta
 {
   double dCosA, dSinA, dCosB, dSinB;

   TAligmentZDta( double dCosA_, double dSinA_, double dCosB_, double dSinB_ )
    {
      dCosA = dCosA_; dSinA = dSinA_;
      dCosB = dCosB_; dSinB = dSinB_;
    }
 };
typedef TAligmentZDta _FAR *PTAligmentZDta;


static void AligmentZInstance( Object _FAR &rV, void _FAR *pDta )
 {
   double x = RTVertex(rV).x, y = RTVertex(rV).y, z = RTVertex(rV).z;

   RTVertex(rV).x = x * PTAligmentZDta(pDta)->dCosA -		    
		    z * PTAligmentZDta(pDta)->dSinA; 

   RTVertex(rV).y = -x * PTAligmentZDta(pDta)->dSinB * PTAligmentZDta(pDta)->dSinA +
		    y * PTAligmentZDta(pDta)->dCosB -
		    z * PTAligmentZDta(pDta)->dSinB * PTAligmentZDta(pDta)->dCosA;
		   
   RTVertex(rV).z = x * PTAligmentZDta(pDta)->dSinA * PTAligmentZDta(pDta)->dCosB +
		    y * PTAligmentZDta(pDta)->dSinB +
		    z * PTAligmentZDta(pDta)->dCosA * PTAligmentZDta(pDta)->dCosB;

 }

void TScena::AligmentZ( RTSurface rSurf, RTVector rV )
 {
   double dMA = sqrt( rV.x * rV.x + rV.z * rV.z );
   double dMB = sqrt( rV.y * rV.y + dMA * dMA );
   double dCosA, dSinA, dCosB, dSinB;

   if( dMA < SCENA_d_AligmentZNormalZero && dMB > SCENA_d_AligmentZNormalZero )
    {
      rSurf.RotateX( GRAD_TO_RAD(90) );
      return;
    }
   else if( dMB < SCENA_d_AligmentZNormalZero )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_15 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( pWin->HWindow, str_(),
		      ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error(AligmentZ): invalid normal vector"
      return;
    }
   else if( fabs(dMA - dMB) < SCENA_d_AligmentZNormalZero  )
    {
      /*BWCCMessageBox( pWin->HWindow, "Body is already aligmented",
		      "Message", MB_ICONINFORMATION | MB_OK | MB_APPLMODAL );
      return;*/
    }
      
   dCosA = rV.z / dMA; dSinA = rV.x / dMA;
   dCosB = dMA / dMB; dSinB = rV.y / dMB;

   TAligmentZDta azDta( dCosA, dSinA, dCosB, dSinB );
   rSurf.listVertex.forEach( AligmentZInstance, Pvoid(&azDta) );
 }

struct TSortFacDta
 {
   int _export (*pCmpF)(const void *, const void *);
   PTIndicator pInd;

   TSortFacDta( int _export (*pCmpF_)(const void *, const void *), PTIndicator pInd_ )
    {
      pCmpF = pCmpF_;
      pInd = pInd_;
    }
 };
typedef TSortFacDta* PTSortFacDta;
typedef TSortFacDta& RTSortFacDta;


static void SortFacetsInstance( Object _FAR &rObj, void _FAR *pD )
 {
   //RTSurface(rObj).listFacets.Sort( CompareFacetY, PTIndicator(pI) );
   RTSurface(rObj).listFacets.Sort( PTSortFacDta(pD)->pCmpF, PTSortFacDta(pD)->pInd );
 }

static void SortAdgesInstanceFac( Object _FAR &rObj, void _FAR *pD )
 {
   //RTFacet(rObj).listAgesItem.Sort( (PTCompareFunc)&TAdgeItem::CompareY, PTIndicator(pI) );
   RTFacet(rObj).listAgesItem.Sort( PTSortFacDta(pD)->pCmpF, PTSortFacDta(pD)->pInd );
 }


static void SortAdgesInstance( Object _FAR &rObj, void _FAR *pD )
 {
   RTSurface(rObj).listFacets.forEach( SortAdgesInstanceFac, pD );
 }


void TScena::SortFacets( int _export (*pCmpF)(const void *, const void *), PTIndicator pInd )
 {
   TSortFacDta sortDta( pCmpF, pInd );

   if( pInd ) pInd->Init( 2 * SummOfNumbFacet() );
   listSurf.forEach( SortFacetsInstance, Pvoid(&sortDta) );
 }

void TScena::SortAdges( int _export (*pCmpF)(const void *, const void *), PTIndicator pInd )
 {
   TSortFacDta sortDta( pCmpF, pInd );

   if( pInd ) pInd->Init( 2 * SummOfNumbAdges() );
   listSurf.forEach( SortAdgesInstance, Pvoid(&sortDta) );
 }

void TScena::SortSurfFacAdg( PTIndicator pI )
 {
   if( pI ) pI->Init( listSurf.getItemsInContainer() );
   //listSurf.Sort( (PTCompareFunc)&TSurface::CompareY, PTIndicator(pI) );
   listSurf.Sort( CompareSurfaceY, PTIndicator(pI) );

   TSortFacDta sortDta( CompareFacetY, pI );
   if( pI ) pI->Init( 2 * SummOfNumbFacet() );
   listSurf.forEach( SortFacetsInstance, Pvoid(&sortDta) );

   sortDta = TSortFacDta( CompareAdgeItemY, pI );
   if( pI ) pI->Init( 2 * SummOfNumbAdges() );
   listSurf.forEach( SortAdgesInstance, Pvoid(&sortDta) );
 }

BOOL TScena::ListAdgeNormalyzeRev( PTIndicator pInd )
 {
   DoubleListIterator dbIt = DoubleListIterator( listSurf );
   PTSurface pSurf = (PTSurface)&dbIt.current();

   if( pInd ) pInd->Init( 2 * listSurf.getItemsInContainer() );

   for( ; int(dbIt); pSurf = (PTSurface)&++dbIt )
    {
      pSurf->listAdges.ResetActionFlag();

      DoubleListIterator dbIt2 = DoubleListIterator( pSurf->listAdges );
      PTAdge pA = (PTAdge)&dbIt2.current();

      for( ; int(dbIt2); pA = (PTAdge)&++dbIt2 )
       {
	 if( pA->pV1->y < pA->pV2->y )
	  {
	    PTVertex pV = pA->pV1; pA->pV1 = pA->pV2; pA->pV2 = pV;
	    pA->bFlAtion = TRUE;
          }
       }

      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   BOOL res = FlipAdgesItems( pInd );
   if( pInd ) pInd->WorkIs100();

   return res;
 }

BOOL TScena::ListAdgeNormalyzeRevInt( PTIndicator pInd )
 {
   DoubleListIterator dbIt = DoubleListIterator( listSurf );
   PTSurface pSurf = (PTSurface)&dbIt.current();

   if( pInd ) pInd->Init( 2 * listSurf.getItemsInContainer() );

   for( ; int(dbIt); pSurf = (PTSurface)&++dbIt )
    {
      pSurf->listAdges.ResetActionFlag();

      DoubleListIterator dbIt2 = DoubleListIterator( pSurf->listAdges );
      PTAdge pA = (PTAdge)&dbIt2.current();

      for( ; int(dbIt2); pA = (PTAdge)&++dbIt2 )
       {
	 if( int(pA->pV1->y) < int(pA->pV2->y) )
	  {
	    PTVertex pV = pA->pV1; pA->pV1 = pA->pV2; pA->pV2 = pV;
	    pA->bFlAtion = TRUE;
          }
       }

      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   BOOL res = FlipAdgesItems( pInd );
   if( pInd ) pInd->WorkIs100();

   return res;
 }


BOOL near pascal TScena::FlipAdgesItems( PTIndicator pInd )
 {
   DoubleListIterator dbIt = DoubleListIterator( listSurf );
   PTSurface pSurf = (PTSurface)&dbIt.current();

   for( ; int(dbIt); pSurf = (PTSurface)&++dbIt )
    {      
      DoubleListIterator dbIt2 = DoubleListIterator( pSurf->listFacets );
      PTFacet pF = (PTFacet)&dbIt2.current();

      for( ; int(dbIt2); pF = (PTFacet)&++dbIt2 )
       {
	 DoubleListIterator dbIt3 = DoubleListIterator( pF->listAgesItem );
	 PTAdgeItem pAI = (PTAdgeItem)&dbIt3.current();

	 for( ; int(dbIt3); pAI = (PTAdgeItem)&++dbIt3 )
	  {
	    PTAdge pA = pAI->pAdge;

	    if( pA->bFlAtion == TRUE ) pAI->clockVise = -pAI->clockVise;
	  }
       }

      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   return TRUE;
 }


static void SummOfNumbFacetInstance( Object _FAR& rSf, void _FAR* dwS )
 {
   (*(DWORD*)dwS) += RTSurface(rSf).listFacets.getItemsInContainer();
 }

DWORD TScena::SummOfNumbFacet() const
 {
   DWORD dwSumm = 0;
   listSurf.forEach( SummOfNumbFacetInstance, Pvoid(&dwSumm) );

   return dwSumm;
 }

static void SummOfNumbAdgesInstance( Object _FAR& rSf, void _FAR* dwS )
 {
   (*(DWORD*)dwS) += RTSurface(rSf).listAdges.getItemsInContainer();
 }


DWORD TScena::SummOfNumbAdges() const
 {
   DWORD dwSumm = 0;
   listSurf.forEach( SummOfNumbAdgesInstance, Pvoid(&dwSumm) );

   return dwSumm;
 }

void TScena::TestToFileBounds( LPSTR lpName )
 {
  FILE *f = fopen( lpName , "w+t" );

  DoubleListIterator dbIt = DoubleListIterator( listSurf );
  PTSurface pSurf = (PTSurface)&dbIt.current();

  char cStr[80] = "Bounds of Bodys:\n\n\tleft \ttop \tright \tbottom\n\n";
  //_lwrite( Hf, cStr, strlen(cStr) );

  for( ; int(dbIt); pSurf = (PTSurface)&++dbIt )
   {
     fprintf( f, "[%d]: %5d  %5d  %5d  %5d\n", pSurf->Index(),
       pSurf->iMinX,
       pSurf->iMinY,
       pSurf->iMaxX,
       pSurf->iMaxY
	    ); 
     //_lwrite( Hf, cStr, strlen(cStr) );
   }

  fclose( f );
 }

void TScena::Separate( RTIndexedDoubleList rdblLights, RTIndexedDoubleList rdblCameras ) const
 {
LBL_RESTART:
   DoubleListIterator dbIt = DoubleListIterator( listSurf );
   PTSurface pSurf = (PTSurface)&dbIt.current();
  
   for( ; int(dbIt); pSurf = (PTSurface)&++dbIt )
    {
      if( pSurf->isA() == TYP_TOMNI )
       {
	 listSurf.detach( *pSurf, TShouldDelete::NoDelete );
	 rdblLights.add( *pSurf );
	 //dbIt = DoubleListIterator( listSurf );
	 goto LBL_RESTART;
       }
      else if( pSurf->isA() == TYP_TCAMERA )
       {
	 listSurf.detach( *pSurf, TShouldDelete::NoDelete );
	 rdblCameras.add( *pSurf );
	 //dbIt = DoubleListIterator( listSurf );
	 goto LBL_RESTART;
       }
    }
 }

void TScena::CalcBounds()
 {
   DoubleListIterator dbIt = DoubleListIterator( listSurf, FALSE );
   PTSurface pSf = (PTSurface)&dbIt.current();

   dMinY = dMinZ = dMinX = INT_MAX;
   dMaxY = dMaxZ = dMaxX = INT_MIN;

   for( ; int(dbIt); pSf = (PTSurface)&--dbIt )
    {
      classType cl = pSf->isA();
      if( cl == TYP_TOMNI || cl == TYP_TCAMERA || cl == TYP_TOMNIOBOL ||
	  cl == TYP_TCAMERAOBOL
	) continue;

      dMinX = (pSf->minX < dMinX) ? pSf->minX:dMinX;
      dMaxX = (pSf->minX > dMaxX) ? pSf->minX:dMaxX;

      dMinY = (pSf->minY < dMinY) ? pSf->minY:dMinY;
      dMaxY = (pSf->minY > dMaxY) ? pSf->minY:dMaxY;

      dMinZ = (pSf->minZ < dMinZ) ? pSf->minZ:dMinZ;
      dMaxZ = (pSf->minZ > dMaxZ) ? pSf->minZ:dMaxZ;


      dMinX = (pSf->maxX < dMinX) ? pSf->maxX:dMinX;
      dMaxX = (pSf->maxX > dMaxX) ? pSf->maxX:dMaxX;

      dMinY = (pSf->maxY < dMinY) ? pSf->maxY:dMinY;
      dMaxY = (pSf->maxY > dMaxY) ? pSf->maxY:dMaxY;

      dMinZ = (pSf->maxZ < dMinZ) ? pSf->maxZ:dMinZ;
      dMaxZ = (pSf->maxZ > dMaxZ) ? pSf->maxZ:dMaxZ;
    }
 }

static void CalcMaxAdgesInstanceFac( Object _FAR &rFac, void _FAR *pCount )
 {
   int len = RTFacet(rFac).listAgesItem.getItemsInContainer();
   if( *((int*)pCount) < len )  *((int*)pCount) = len;
 }

static void CalcMaxAdgesInstance( Object _FAR &rSf, void _FAR *pCount )
 {
   RTSurface(rSf).listFacets.forEach( CalcMaxAdgesInstanceFac, pCount );
 }

int TScena::GetMaxAdges() const
 {
   int iCountMax = 0;

   listSurf.forEach( CalcMaxAdgesInstance, Pvoid(&iCountMax) );

   return iCountMax;
 }

BOOL TScena::RenderPrepareBodys( PTIndicator pInd )
 {
   DoubleListIterator dbIt = DoubleListIterator( listSurf, FALSE );
   PTSurface pSf = (PTSurface)&dbIt.current();

   if( pInd ) pInd->Init( 2 * listSurf.getItemsInContainer() );
      
   for( ; int(dbIt); pSf = (PTSurface)&--dbIt )
    {
      if( saShadingLimit == SA_Flat || pSf->medium.Shading() == SA_Flat )
       {
	 pSf->pGetTraceNormalFunc = &TFacet::GetTraceNormal;
	 // GetTraceNormal( double x, double y ) const;
         //TVector GetTraceNormalPhong( double x, double y ) const;
	 pSf->tsAlho = SA_Flat;
       }
      else
        {
	 pSf->pGetTraceNormalFunc = &TFacet::GetTraceNormalPhong;
	 pSf->tsAlho = SA_Phong;
       }

      pSf->AssignTransparencyFunc();

      pSf->dIAmbientR = double(pSf->medium.GetRGBColor().rgbRed) / 255.0 * pSf->medium.GdKa() * double(rgbAmbient.rgbRed) / 255.0;
      pSf->dIAmbientG = double(pSf->medium.GetRGBColor().rgbGreen) / 255.0 * pSf->medium.GdKa() * double(rgbAmbient.rgbGreen) / 255.0;
      pSf->dIAmbientB = double(pSf->medium.GetRGBColor().rgbBlue) / 255.0 * pSf->medium.GdKa() * double(rgbAmbient.rgbBlue) / 255.0;

      pSf->CalcNormalsAllVertex( NULL );
      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;

      pSf->CalcNormalsAllVertexPhongAdges();
      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   if( pInd ) pInd->WorkIs100();

   return TRUE;
 }

static void InitSInst3( Object _FAR &rAI, void _FAR *pStrScan )
 {
   RTAdgeItem(rAI).dStrScan = *(double*)pStrScan;
 }

static void InitSInst2( Object _FAR &rF, void _FAR *pStrScan )
 {
   RTFacet(rF).listAgesItem.forEach( InitSInst3, pStrScan  );
 }

static void InitSInst( Object _FAR &rSf, void _FAR *pStrScan )
 {
   RTSurface(rSf).listFacets.forEach( InitSInst2, pStrScan  );
 }

void TScena::InitStrScanParamOfAdgeItems( double dStrScan )
 {
   listSurf.forEach( InitSInst, Pvoid(&dStrScan) );
 }

struct TCalcObolDta
 {
   PTIndicator pI;
   double dMaxRad;
   TVector vecCent;
   PTVertex pV;
 };
typedef TCalcObolDta* PTCalcObolDta;

static void CalcAllTraceObolInst2( Object _FAR &rV, void _FAR *pD )
 {
   double dDist =
     fabs(RTVertex(rV).x - PTCalcObolDta(pD)->vecCent.x) +
     fabs(RTVertex(rV).y - PTCalcObolDta(pD)->vecCent.y) +
     fabs(RTVertex(rV).z - PTCalcObolDta(pD)->vecCent.z);

   if( dDist > PTCalcObolDta(pD)->dMaxRad )
    {
      PTCalcObolDta(pD)->dMaxRad = dDist;
      PTCalcObolDta(pD)->pV = &RTVertex(rV);
    }
 }

static void CalcAllTraceObolInst( Object _FAR &rSf, void _FAR *pD )
 {
   if( FilterBody(&RTSurface(rSf)) == FALSE ) return;
  

  RTSurface(rSf).vecCenter = TVector(
    RTSurface(rSf).minX + fabs(RTSurface(rSf).maxX - RTSurface(rSf).minX) / 2.0,
    RTSurface(rSf).minY + fabs(RTSurface(rSf).maxY - RTSurface(rSf).minY) / 2.0,
    RTSurface(rSf).minZ + fabs(RTSurface(rSf).maxZ - RTSurface(rSf).minZ) / 2.0
                                    );

  PTCalcObolDta(pD)->dMaxRad = 0;
  PTCalcObolDta(pD)->vecCent = RTSurface(rSf).vecCenter;
  PTCalcObolDta(pD)->pV = NULL;
  RTSurface(rSf).listVertex.forEach( CalcAllTraceObolInst2, pD );

  if( PTCalcObolDta(pD)->pV )
    RTSurface(rSf).dRadSphricObol =
      !(TVector( PTCalcObolDta(pD)->pV->x,
	         PTCalcObolDta(pD)->pV->y,
		 PTCalcObolDta(pD)->pV->z
	       ) - RTSurface(rSf).vecCenter);

  if( PTCalcObolDta(pD)->pI ) PTCalcObolDta(pD)->pI->WorkMessageAdd( 1 );
 }

void TScena::CalcAllTraceObol( PTIndicator pInd )
 {
   if( pInd ) pInd->Init( listSurf.getItemsInContainer() );

   TCalcObolDta dta; dta.pI = pInd;
   listSurf.forEach( CalcAllTraceObolInst, Pvoid(&dta) );

   if( pInd ) pInd->WorkIs100();
 }

BOOL TScena::TransparencyTest()
 {
   DoubleListIterator dbIt = DoubleListIterator( listSurf, FALSE );
   PTSurface pSf = (PTSurface)&dbIt.current();   
      
   for( ; int(dbIt); pSf = (PTSurface)&--dbIt )
     if( FilterBody(pSf) == TRUE && pSf->IsTransparency() == TRUE ) return TRUE;

   return FALSE;
 }
