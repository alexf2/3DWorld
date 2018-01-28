#pragma option -v-

#include "surface.hpp"
#include "INC\errors.inc"
#include "fheader.hpp"
#include "scena.hpp"

#include <stdio.h>
#include <values.h>
#include <io.h>


TCone::TCone( PTFaceData pFD ):
  TSurface( pFD ),
  TInitShape( TCone::InitShape )
  {
  }

TCone::TCone( int ind, PTFaceData pFD ):
  TSurface( ind, pFD ),
  TInitShape( TCone::InitShape )
  {
  }

/*  int nSides;

  int radius;
  int high;*/
void TCone::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   PTConeInit  pConeData = (PTConeInit)pFD;
   int iVerNum = pConeData->nSides + 1,
       iLinNum = 2 * pConeData->nSides,
       iFacNum =  pConeData->nSides + 1;
   double  deltaFi = 2.0 * M_PI / double(pConeData->nSides);

   for( int v = 0; v < iVerNum - 1; v++ )
    {
      PTVertex pVert = new TVertex( v + 1,
	double(pConeData->radius) * cos(deltaFi * double(v)),
	double(pConeData->radius) * sin(deltaFi * double(v)),
        0 );
      if( !pVert ) { pSurface->Status = ER_CREATESURF; return; }

      pSurface->listVertex.add( *pVert );
    }

   PTVertex pVert = new TVertex( iVerNum, 0, 0, -pConeData->high );
   if( !pVert ) { pSurface->Status = ER_CREATESURF; return; }
   pSurface->listVertex.add( *pVert );

   pSurface->listVertex.CreateChache();

   DoubleListIterator dbIt1 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert1;
   DoubleListIterator dbIt2 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert2;

   for( int k = 0; k < iLinNum; k++ )
    {
      if( k == 0 )
       {
	 pSurface->listVertex.SetIterator( 1, dbIt1 );
	 pSurface->listVertex.SetIterator( 2, dbIt2 );
       }
      else
      if( k == pConeData->nSides - 1 )
	pSurface->listVertex.SetIterator( 1, dbIt2 );
      else
      if( k == pConeData->nSides )
       {
	 pSurface->listVertex.SetIterator( 1, dbIt1 );
	 pSurface->listVertex.SetIterator( pConeData->nSides + 1, dbIt2 );
       }    

      pVert1 = (PTVertex)&dbIt1--;
      if( k < pConeData->nSides  )
	pVert2 = (PTVertex)&dbIt2--;
      else
	pVert2 = (PTVertex)&dbIt2.current();
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( k + 1, pVert1, pVert2 );

      if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }

      pSurface->listAdges.add( *pAdge );
    }

   pSurface->listAdges.CreateChache();

   dbIt1 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge pAge1;
   dbIt2 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge pAge2;
   DoubleListIterator dbIt3 = DoubleListIterator( pSurface->listAdges, FALSE );
   PTAdge pAge3;

   for( int fac = 0; fac < iFacNum - 1; fac++ )
    {
      if( fac == 0 )
       {
	 pSurface->listAdges.SetIterator( pConeData->nSides + 1, dbIt1 );
	 pSurface->listAdges.SetIterator( pConeData->nSides + 2, dbIt2 );
	 pSurface->listAdges.SetIterator( 1, dbIt3 );	
       }
      else if( fac == pConeData->nSides - 1 )
	pSurface->listAdges.SetIterator( pConeData->nSides + 1, dbIt2 );

      pAge1 = (PTAdge)&dbIt1--;
      pAge2 = (PTAdge)&dbIt2--;
      pAge3 = (PTAdge)&dbIt3--;

      if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT  )
	{ pSurface->Status = ER_CREATESURF; return; }

      PTFacet pFacet = new TFacet( fac + 1 );
      PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, 1 );
      if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
      pFacet->listAgesItem.add( *pAIt );
      pAIt = new TAdgeItem( -1, pAge2, -1 );
      pFacet->listAgesItem.add( *pAIt );
      pAIt = new TAdgeItem( -1, pAge3, -1 );
      pFacet->listAgesItem.add( *pAIt );

      pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge3 );

      pSurface->listFacets.add( *pFacet );
    }


   PTFacet pFacet1 = new TFacet( iFacNum );
   pSurface->pfacInitAlignFac = pFacet1;
   pSurface->pvInitAlignVert = NULL;
   pSurface->listAdges.SetIterator( 1, dbIt1 );
   for( int age = 0; age < pConeData->nSides; age++ )
    {
      pAge1 = (PTAdge)&dbIt1--;

      if( *pAge1 == NOOBJECT  )
	{ pSurface->Status = ER_CREATESURF; return; }

      PTAdgeItem  pAIt1 = new TAdgeItem( -1, pAge1, 1 );

      if( !pAIt1 ) { pSurface->Status = ER_CREATESURF; return; }

      pFacet1->listAgesItem.add( *pAIt1 );
    }

   pFacet1->vecOutNormal = TVector( 0, 0, 1 );
   pSurface->listFacets.add( *pFacet1 );

   pSurface->listFacets.CreateChache();

   //pSurface->CalcNormals();
   //pSurface->Move( pFD->insertPoint.x, pFD->insertPoint.y, pFD->insertPoint.z );
 }
