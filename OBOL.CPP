//#pragma option -v-

#include "surface.hpp"
#include "scena.hpp"
#include <stdio.h>
#include <values.h>

extern HINSTANCE hInstRC;

void TObol::GetVertex()
 {
   if( listVertex.getItemsInContainer() != listVK.getItemsInContainer() )
    {
      listVK.flush( TShouldDelete::Delete );

      DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
      PTVertex pVert = (PTVertex)&dbIt.current();

      for( ; int(dbIt); pVert = (PTVertex)&--dbIt ) //add	
	listVK.addAtHead( *new TVertex(pVert->Index(), pVert->x, pVert->y, pVert->z) );
    }
   else
    {
      DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
      PTVertex pVert = (PTVertex)&dbIt.current();
      DoubleListIterator  dbItVK = DoubleListIterator( listVK, FALSE );
      PTVertex pVertVK = (PTVertex)&dbItVK.current();

      for( ; int(dbIt) && int(dbItVK); pVert = (PTVertex)&--dbIt,
				       pVertVK = (PTVertex)&--dbItVK
         )
	*pVertVK = *pVert;
    }
 }

TObol::TObol( PTSurface psfSurf ):
  TSurface( new TExtFaceData(psfSurf) ),
  TInitShape( TObol::InitShape )
  {
    SetObol( FALSE );
    bFlAbsTransf = FALSE;

    Setup( PTExtFaceData(pfdKey) );

    if( pfdKey ) delete pfdKey; pfdKey = NULL;

    GetVertex();
  }

TObol::TObol( PTScena pScena ):
  TSurface( new TExtFaceData(pScena) ),
  TInitShape( TObol::InitShape )
  {
    SetObol( FALSE );
    bFlAbsTransf = FALSE;

    Setup( PTExtFaceData(pfdKey) );

    if( pfdKey ) delete pfdKey; pfdKey = NULL;

    GetVertex();
  }


TObol::TObol( RTObol rObol, PTIndicator  pInd  ):
  //TSurface( rObol ),
  TSurface( rObol, pInd ),
  TInitShape( NULL )
 {
   bFlAbsTransf = rObol.bFlAbsTransf;
   //pSurf = rObol.pSurf;
 };

TObol::TObol( int ind, PTSurface psfSurf ):
  TSurface( ind, new TExtFaceData(psfSurf) ),
  TInitShape( TObol::InitShape )
  {
    SetObol( FALSE );
    bFlAbsTransf = FALSE;

    Setup( PTExtFaceData(pfdKey) );

    if( pfdKey ) delete pfdKey; pfdKey = NULL;

    GetVertex();
  }



void TObol::InitShape( PTSurface pSurface, PTFaceData pFD )
 {

   //PTObol pObl = (PTObol)pSurface;
   //pObl->bFlAbsTransf = FALSE;
   PTSurface pSurf = ((PTExtFaceData)pFD)->pSf;


   TDelta delta = PTExtFaceData(pFD)->pSf ? TScena::GetDelta( *PTExtFaceData(pFD)->pSf ):
					    PTExtFaceData(pFD)->pScena->GetDeltaSelected();

   /*double minX = MAXINT, maxX = -MAXINT,
	  minY = MAXINT, maxY = -MAXINT,
	  minZ = MAXINT, maxZ = -MAXINT;

   DoubleListIterator dbIt = DoubleListIterator( pSurf->listVertex, FALSE );
   PTVertex pVert = (PTVertex)&dbIt.current();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      minX = (pVert->x < minX) ? pVert->x:minX;
      maxX = (pVert->x > maxX) ? pVert->x:maxX;

      minY = (pVert->y < minY) ? pVert->y:minY;
      maxY = (pVert->y > maxY) ? pVert->y:maxY;

      minZ = (pVert->z < minZ) ? pVert->z:minZ;
      maxZ = (pVert->z > maxZ) ? pVert->z:maxZ;
    }

   double dX = fabs( maxX - minX ),
	  dY = fabs( maxY - minY ),
	  dZ = fabs( maxZ - minZ );
     */

  pSurface->listVertex.add( *new TVertex(1, delta.maxX, delta.maxY, delta.maxZ) );
  pSurface->listVertex.add( *new TVertex(2, delta.maxX, delta.minY, delta.maxZ) );
  pSurface->listVertex.add( *new TVertex(3, delta.maxX, delta.minY, delta.minZ) );
  pSurface->listVertex.add( *new TVertex(4, delta.maxX, delta.maxY, delta.minZ) );

  pSurface->listVertex.add( *new TVertex(5, delta.minX, delta.maxY, delta.maxZ) );
  pSurface->listVertex.add( *new TVertex(6, delta.minX, delta.minY, delta.maxZ) );
  pSurface->listVertex.add( *new TVertex(7, delta.minX, delta.minY, delta.minZ) );
  pSurface->listVertex.add( *new TVertex(8, delta.minX, delta.maxY, delta.minZ) );


  pSurface->listAdges.add( *new TAdge(1, pSurface->listVertex[1], pSurface->listVertex[2]) );
  pSurface->listAdges.add( *new TAdge(2, pSurface->listVertex[2], pSurface->listVertex[3]) );
  pSurface->listAdges.add( *new TAdge(3, pSurface->listVertex[3], pSurface->listVertex[4]) );
  pSurface->listAdges.add( *new TAdge(4, pSurface->listVertex[4], pSurface->listVertex[1]) );

  pSurface->listAdges.add( *new TAdge(5, pSurface->listVertex[6], pSurface->listVertex[5]) );
  pSurface->listAdges.add( *new TAdge(6, pSurface->listVertex[7], pSurface->listVertex[6]) );
  pSurface->listAdges.add( *new TAdge(7, pSurface->listVertex[8], pSurface->listVertex[7]) );
  pSurface->listAdges.add( *new TAdge(8, pSurface->listVertex[5], pSurface->listVertex[8]) );

  pSurface->listAdges.add( *new TAdge(9, pSurface->listVertex[1], pSurface->listVertex[5]) );
  pSurface->listAdges.add( *new TAdge(10, pSurface->listVertex[4], pSurface->listVertex[8]) );
  pSurface->listAdges.add( *new TAdge(11, pSurface->listVertex[3], pSurface->listVertex[7]) );
  pSurface->listAdges.add( *new TAdge(12, pSurface->listVertex[2], pSurface->listVertex[6]) );



  PTFacet pFacet = new TFacet( 1 );
  PTIndexedItem pA1, pA2;
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA1=pSurface->listAdges[5]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA2=pSurface->listAdges[12]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[1], -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[9], 1 ) );

  pFacet->vecOutNormal = pSurface->CreateNormal( (PTAdge)pA1, (PTAdge)pA2 );
  pSurface->listFacets.add( *pFacet );

  pFacet = new TFacet( 2 );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA1=pSurface->listAdges[4]), 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA2=pSurface->listAdges[1]), 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[2], 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[3], 1 ) );

  pFacet->vecOutNormal = pSurface->CreateNormal( (PTAdge)pA1, (PTAdge)pA2 );
  pSurface->listFacets.add( *pFacet );

  pFacet = new TFacet( 3 );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA1=pSurface->listAdges[10]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA2=pSurface->listAdges[3]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[11], 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[7], -1 ) );

  pFacet->vecOutNormal = pSurface->CreateNormal( (PTAdge)pA1, (PTAdge)pA2 );
  pSurface->listFacets.add( *pFacet );

  pFacet = new TFacet( 4 );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA1=pSurface->listAdges[8]), 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA2=pSurface->listAdges[7]), 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[6], 1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[5], 1 ) );

  pFacet->vecOutNormal = pSurface->CreateNormal( (PTAdge)pA1, (PTAdge)pA2 );
  pSurface->listFacets.add( *pFacet );

  pFacet = new TFacet( 5 );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA1=pSurface->listAdges[8]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA2=pSurface->listAdges[9]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[4], -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[10], 1 ) );

  pFacet->vecOutNormal = pSurface->CreateNormal( (PTAdge)pA1, (PTAdge)pA2 );
  pSurface->listFacets.add( *pFacet );

  pFacet = new TFacet( 6 );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA1=pSurface->listAdges[6]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, (pA2=pSurface->listAdges[11]), -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[2], -1 ) );
  pFacet->listAgesItem.add( *new TAdgeItem( -1, pSurface->listAdges[12], 1 ) );

  pFacet->vecOutNormal = pSurface->CreateNormal( (PTAdge)pA1, (PTAdge)pA2 );
  pSurface->listFacets.add( *pFacet );


  //pSurface->TestToFile( "G:\\tbl.fce" );

  T3DPoint ptCent = pSurface->GetGeomCenter();
  pSurface->listVertex.addAtHead( *new TVertex(pSurface->listVertex.GetFreeIndex(), ptCent.x, ptCent.y, ptCent.z) );

  //pObl->Setup( PTExtFaceData(pFD) );
 }

T3DPoint TObol::GetGeomCenterObol()
 {
   return (RT3DPoint)(RTVertex)listVertex.peekAtHead();
 }

void TObol::Setup( PTExtFaceData pFd )
 {

   //RotateZ( GRAD_TO_RAD(45) );

   if( !pFd ) return;

   T3DPoint ptSurfCen = pFd->pSf ? pFd->pSf->GetGeomCenter():
				   pFd->pScena->GetGeomCenterSelected();
   T3DPoint ptOblCen = GetGeomCenter();

   TSurface::Move( ptSurfCen.x - ptOblCen.x, ptSurfCen.y - ptOblCen.y,
		  ptSurfCen.z - ptOblCen.z
		);
 }

void TObol::Move( double dx, double dy, double dz )
 {
   //if( bFlAbsTransf == TRUE ) ResetVertex();
   TSurface::Move( dx, dy, dz );
   //CalcBounds();
 }

void TObol::RotateZ( double angleRad, BOOL bFlFlipNormals )
 {
   //if( bFlAbsTransf == TRUE ) ResetVertex();
   TSurface::RotateZ( angleRad, bFlFlipNormals );
   //CalcBounds();
 }
 
void TObol::RotateX( double angleRad, BOOL bFlFlipNormals )
 {
   //if( bFlAbsTransf == TRUE ) ResetVertex();
   TSurface::RotateX( angleRad, bFlFlipNormals );
 }

void TObol::RotateY( double angleRad, BOOL bFlFlipNormals )
 {
   //if( bFlAbsTransf == TRUE ) ResetVertex();
   TSurface::RotateY( angleRad, bFlFlipNormals );
 }

void TObol::ScaleXYZ( double scX, double scY, double scZ, T3DPoint center )
 {
   //if( bFlAbsTransf == TRUE ) ResetVertex();
   TSurface::ScaleXYZ( scX, scY, scZ, center );
 }

void TObol::Scale2D( double sc, T3DPoint center )
 {
   //if( bFlAbsTransf == TRUE ) ResetVertex();
   TSurface::Scale2D( sc, center );
 }

void TObol::ResetVertex()
 {
   DoubleListIterator dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex pVert = (PTVertex)&dbIt.current();
   DoubleListIterator dbItVK = DoubleListIterator( listVK, FALSE );
   PTVertex pVertVK = (PTVertex)&dbItVK.current();

   for( ; int(dbIt) && int(dbItVK); pVert = (PTVertex)&--dbIt,
				    pVertVK = (PTVertex)&--dbItVK
      )
     *pVert = *pVertVK;
 
 }


#pragma option -v.

