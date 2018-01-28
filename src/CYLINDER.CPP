#pragma option -v-

#include "surface.hpp"
#include "INC\errors.inc"
#include "fheader.hpp"
#include "scena.hpp"

#include <stdio.h>
#include <values.h>
#include <io.h>


TCylinder::TCylinder( PTFaceData pFD ):
  TSurface( pFD ),
  TInitShape( TCylinder::InitShape )
  {
  }

TCylinder::TCylinder( int ind, PTFaceData pFD ):
  TSurface( ind, pFD ),
  TInitShape( TCylinder::InitShape )
  {
  }
/*
TCylinder::TCylinder( int ind, PTSurface pSf )
  TSurface( ind, (PTFaceData)pSf ),
  TInitShape( TCylinder::InitShape )
  {
  }
  */

void TCylinder::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   PTCylinderInit  pCylindData = (PTCylinderInit)pFD;
   int iVerNum = 2 * pCylindData->nSides,
       iLinNum = 3 * pCylindData->nSides,
       iFacNum =  pCylindData->nSides + 2;
   double  deltaFi = 2.0 * M_PI / double(pCylindData->nSides);


   for( int k = 0; k < pCylindData->nSides; k++ )
    {
      PTVertex pVert = new TVertex( k + 1,
	double(pCylindData->radius) * cos(deltaFi * double(k)),
	double(pCylindData->radius) * sin(deltaFi * double(k)),
        0 );
      if( !pVert ) { pSurface->Status = ER_CREATESURF; return; }

      pSurface->listVertex.add( *pVert );
    }

   int number = pCylindData->nSides + 1;
   for( k = 0; k < pCylindData->nSides; k++, number++ )
    {
      PTVertex pVert = new TVertex( number,
	double(pCylindData->radius) * cos(deltaFi * double(k)),
	double(pCylindData->radius) * sin(deltaFi * double(k)),
	-pCylindData->high );

      if( !pVert ) { pSurface->Status = ER_CREATESURF; return; }

      pSurface->listVertex.add( *pVert );
    }

   pSurface->listVertex.CreateChache();

   DoubleListIterator dbIt1 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert1;
   DoubleListIterator dbIt2 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert2;

   for( k = 0; k < iLinNum; k++ )
    {
      if( k == 0 )
       {
	 pSurface->listVertex.SetIterator( 1, dbIt1 );
	 pSurface->listVertex.SetIterator( 2, dbIt2 );
       }
      else
      if( k == pCylindData->nSides - 1 )
	pSurface->listVertex.SetIterator( 1, dbIt2 );
      else
      if( k == pCylindData->nSides )
       {
	 pSurface->listVertex.SetIterator( pCylindData->nSides + 1, dbIt1 );
	 pSurface->listVertex.SetIterator( pCylindData->nSides + 2, dbIt2 );
       }
      else
      if( k == 2 * pCylindData->nSides - 1 )
	pSurface->listVertex.SetIterator( pCylindData->nSides + 1, dbIt2 );
      else
      if( k == 2 * pCylindData->nSides )
       {
         pSurface->listVertex.SetIterator( 1, dbIt1 );
	 pSurface->listVertex.SetIterator( pCylindData->nSides + 1, dbIt2 );
       }

      pVert1 = (PTVertex)&dbIt1--;
      pVert2 = (PTVertex)&dbIt2--;
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( k + 1, pVert1, pVert2 );

      if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }

      pSurface->listAdges.add( *pAdge );
    }

   pSurface->listAdges.CreateChache();

   dbIt1 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge  pAge1;
   dbIt2 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge pAge2;
   DoubleListIterator dbIt3 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge pAge3;
   DoubleListIterator dbIt4 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge pAge4;

   for( int fac = 0; fac < iFacNum - 2; fac++ )
    {
      if( fac == 0 )
       {
	 pSurface->listAdges.SetIterator( 2 * pCylindData->nSides + 1, dbIt1 );
	 pSurface->listAdges.SetIterator( pCylindData->nSides + 1, dbIt2 );
	 pSurface->listAdges.SetIterator( 2 * pCylindData->nSides + 2, dbIt3 );
	 pSurface->listAdges.SetIterator( 1, dbIt4 );
       }
      else if( fac == pCylindData->nSides - 1 )
	pSurface->listAdges.SetIterator( 2 * pCylindData->nSides + 1, dbIt3 );

      pAge1 = (PTAdge)&dbIt1--;
      pAge2 = (PTAdge)&dbIt2--;
      pAge3 = (PTAdge)&dbIt3--;
      pAge4 = (PTAdge)&dbIt4--;

      if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT || *pAge4 == NOOBJECT )
	{ pSurface->Status = ER_CREATESURF; return; }

      PTFacet pFacet = new TFacet( fac + 1 );
      PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, 1 );
      if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
      pFacet->listAgesItem.add( *pAIt );
      pAIt = new TAdgeItem( -1, pAge2, 1 );
      pFacet->listAgesItem.add( *pAIt );
      pAIt = new TAdgeItem( -1, pAge3, -1 );
      pFacet->listAgesItem.add( *pAIt );
      pAIt = new TAdgeItem( -1, pAge4, -1 );
      pFacet->listAgesItem.add( *pAIt );
      //listFacets

      pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge4 );
      pSurface->listFacets.add( *pFacet );
    }


   PTFacet pFacet1 = new TFacet( iFacNum - 1 );
   PTFacet pFacet2 = new TFacet( iFacNum );
   if( !pFacet1 || !pFacet2 ) { pSurface->Status = ER_CREATESURF; return; }

   pSurface->pfacInitAlignFac = pFacet1;
   pSurface->pvInitAlignVert = NULL;

   pSurface->listAdges.SetIterator( 1, dbIt1 );
   pSurface->listAdges.SetIterator( 2 * pCylindData->nSides, dbIt2 );
   for( int age = 0; age < pCylindData->nSides; age++ )
    {
      pAge1 = (PTAdge)&dbIt1--;
      pAge2 = (PTAdge)&dbIt2++;

      if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT )
	{ pSurface->Status = ER_CREATESURF; return; }

      PTAdgeItem  pAIt1 = new TAdgeItem( -1, pAge1, 1 );
      PTAdgeItem  pAIt2 = new TAdgeItem( -1, pAge2, -1 );

      if( !pAIt1 || !pAIt2 ) { pSurface->Status = ER_CREATESURF; return; }

      pFacet1->listAgesItem.add( *pAIt1 );
      pFacet2->listAgesItem.add( *pAIt2 );
    }


   pFacet1->vecOutNormal = TVector( 0, 0, 1 );
   pFacet2->vecOutNormal = TVector( 0, 0, -1 );
   pSurface->listFacets.add( *pFacet1 );
   pSurface->listFacets.add( *pFacet2 );

   pSurface->listFacets.CreateChache();

   //pSurface->CalcNormals();
   //pSurface->Move( pFD->insertPoint.x, pFD->insertPoint.y, pFD->insertPoint.z );
 }
