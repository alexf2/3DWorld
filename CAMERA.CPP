#pragma option -v-

#include "surface.hpp"
#include "scena.hpp"

extern HINSTANCE hInstRC;

TCamera::TCamera( PTCameraInit pInit ):
  TSurface( pInit ),
  TInitShape( TCamera::InitShape )
 {
   SetObol( FALSE );

   if( pInit )
    {
      dAOV = pInit->dAOV;
      dRoll = pInit->dRoll;
      bFlShowCone = pInit->bFlShowCone;
    }
   else
    {
      dAOV = 45;
      dRoll = 0;
      bFlShowCone = TRUE;
    }
 }

TCamera::TCamera( int ind, PTCameraInit pInit ):
  TSurface( ind, pInit ),
  TInitShape( TCamera::InitShape )
 {
   SetObol( FALSE );

   if( pInit )
    {
      dAOV = pInit->dAOV;
      dRoll = pInit->dRoll;
      bFlShowCone = pInit->bFlShowCone;
    }
   else
    {
      dAOV = 45;
      dRoll = 0;
      bFlShowCone = TRUE;
    }

 }

/*
  T3DPoint ptCamera, ptDir;
  double dAOV;
  double dRoll;
  BOOL bFlShowCone;

*/

void TCamera::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   //PTCamera pCamera = PTCamera(pSurface);
   PTCameraInit pInit = PTCameraInit(pFD);

   if( pFD )
    {
      /*pCamera->dAOV = pInit->dAOV;
      pCamera->dRoll = pInit->dRoll;
      pCamera->bFlShowCone = pInit->bFlShowCone;*/

      PTVertex pV1, pV2;
      pSurface->listVertex.add( *(pV1 = new TVertex(1, pInit->ptCamera)) );
      pSurface->listVertex.add( *(pV2 = new TVertex(2, pInit->ptDir)) );
      pSurface->listAdges.add( *new TAdge(1, pV1, pV2) );
    }
   else
    {
      /*pCamera->dAOV = 45;
      pCamera->dRoll = 0;
      pCamera->bFlShowCone = TRUE;*/

      PTVertex pV1, pV2;
      pSurface->listVertex.add( *(pV1 = new TVertex(1, 0, 0, 100)) );
      pSurface->listVertex.add( *(pV2 = new TVertex(2, 0, 0, 0)) );
      pSurface->listAdges.add( *new TAdge(1, pV1, pV2) );
    }
 }


TCameraObol::TCameraObol( PTCamera pCamera ):
  TInitShape( NULL ),
  TSurface( *pCamera, NULL ),
  TCamera( *pCamera, NULL ),
  TObol( (PTSurface)NULL )
 {
 }

TCameraObol::TCameraObol( int ind, PTCamera pCamera ):
  TInitShape( NULL ),
  TSurface( *pCamera, NULL ),
  TCamera( *pCamera, NULL ),
  TObol( ind, NULL )
 {
 }

/*void TCameraObol::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   PTCameraObol pObl = (PTCameraObol)pSurface;
   pObl->bFlAbsTransf = FALSE;
   PTCamera pCamera = (PTCamera)((PTExtFaceData)pFD)->pSf;

   pObl->dAOV = pCamera->dAOV;
   pObl->dRoll = pCamera->dRoll;
   pObl->bFlShowCone = pCamera->bFlShowCone;
   PTVertex pV1, pV2;
   pObl->listVertex.addAtTail( *(pV1 = new TVertex(1, *(pCamera->listVertex[1]))) );
   pObl->listVertex.addAtTail( *(pV2 = new TVertex(2, *(pCamera->listVertex[2]))) );
   pObl->listAdges.add( *new TAdge(1, pV1, pV2) );
 }*/


PTObol TCamera::CreateObol() const
 {
   return new TCameraObol( (PTCamera)this );
 }

struct TCameraSave
 {
   double dAOV;
   double dRoll;
   BOOL bFlShowCone;

   TCameraSave() {};
   TCameraSave( double dAOV_, double dRoll_, BOOL bFl )
    {
      dAOV = dAOV_;
      dRoll = dRoll_;
      bFlShowCone = bFl;
    }
 };


int TCamera::Save( HFILE hf, PTIndicator pInd )
 {
   int res = TSurface::Save( hf, pInd );
   if( res < 0 ) return res;

   TCameraSave caSave( dAOV, dRoll, bFlShowCone );
   UINT l;
   if( (l = _lwrite(hf, &caSave, sizeof(TCameraSave))) == HFILE_ERROR ||
       l != sizeof(TCameraSave)
     ) return -3;

   if( pInd && pInd->WorkMessageAdd( sizeof(TCameraSave) ) == TRUE ) return -5;

   return 0;
 }

int TCamera::Load( HFILE hf, PTIndicator pInd  )
 {
   int res = TSurface::Load( hf, pInd );
   if( res < 0 ) return res;

   TCameraSave caSave;
   UINT l;
   if( (l = _lread(hf, &caSave, sizeof(TCameraSave))) == HFILE_ERROR ||
       l != sizeof(TCameraSave)
     ) return -3;

   if( pInd && pInd->WorkMessageAdd( sizeof(TCameraSave) ) == TRUE ) return -5;

   dAOV = caSave.dAOV;
   dRoll = caSave.dRoll;
   bFlShowCone = caSave.bFlShowCone;

   return 0;
 }

void TCamera::Draw( BOOL bFlShowBackFaces, PTScena pSc ) const
 {
   PTUCS pUcs = pSc->GetUcs();

   HBRUSH hbrOld = SelectBrush( pSc->GetHdc(), (HBRUSH)GetStockObject(NULL_BRUSH) );

   double dDiv = ((pUcs->GetScale() < 1 && pUcs->GetScale()) ? pUcs->GetScale():1);
   const int iLenCamera = 3.0 / dDiv, iRadDir = 2.0 / dDiv;
   T3DPoint  tmpP[2];
   tmpP[0] = T3DPoint( *(PT3DPoint)(PTVertex)listVertex[1] );
   tmpP[1] = T3DPoint( *(PT3DPoint)(PTVertex)listVertex[2] );

   pUcs->RecalcUserToBase( tmpP, 2 );
   pSc->CallProector( tmpP );

   Rectangle( pSc->GetHdc(), tmpP[0].x - iLenCamera, tmpP[0].y - iLenCamera,
	      tmpP[0].x + iLenCamera, tmpP[0].y + iLenCamera );

   Ellipse( pSc->GetHdc(), tmpP[0].x - iRadDir, tmpP[0].y - iRadDir,
	      tmpP[0].x + iRadDir, tmpP[0].y + iRadDir );

   SelectBrush( pSc->GetHdc(), hbrOld );
 }


void TCamera::DrawSpecial( BOOL bFlShowBackFaces, PTScena pSc )
 {
   Draw( bFlShowBackFaces, pSc );
 }

