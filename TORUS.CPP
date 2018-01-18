 #pragma option -v-

#include "surface.hpp"
#include "INC\errors.inc"
#include "fheader.hpp"
#include "scena.hpp"

#include <stdio.h>
#include <values.h>
#include <io.h>


TTorus::TTorus( PTFaceData pFD ):
  TSurface( pFD ),
  TInitShape( TTorus::InitShape )
  {
  }

TTorus::TTorus( int ind, PTFaceData pFD ):
  TSurface( ind, pFD ),
  TInitShape( TTorus::InitShape )
  {
  }

/*int nMerid, nHoriz;

   int rad1, rad2;
*/

void TTorus::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   PTTorusInit  pTorusData = (PTTorusInit)pFD;

   int n = pTorusData->nHoriz, m = pTorusData->nMerid;
   int iVerNum = m * n,
       iLinNum = 2 * m * n,
       iFacNum = m * n;
   double  deltaFi = 2.0 * M_PI / double(n),
	   deltaPs = 2.0 * M_PI / double(m);
   DoubleList  dblTmp;

   if( pTorusData->rad2 < pTorusData->rad1 )
    {
      int r = pTorusData->rad2; pTorusData->rad2 = pTorusData->rad1;
      pTorusData->rad1 = r;
    }


   for( int k = 0;  k < n; k++ )
    {
      PTVertex pVert;


      pVert = new TVertex( -1,
	double(pTorusData->rad1) * cos(deltaFi * double(k)) + double(pTorusData->rad2),
	double(pTorusData->rad1) * sin(deltaFi * double(k)),
	0 );

      if( !pVert ) { pSurface->Status = ER_CREATESURF; return; }

      dblTmp.add( *pVert );

    }


   pSurface->pvInitAlignVert = NULL;
   pSurface->pfacInitAlignFac = NULL;

   DoubleListIterator dbIt = DoubleListIterator( dblTmp, FALSE );
   PTVertex pVert = (PTVertex)&dbIt.current();
   int count = 1;
   for( ; int(dbIt) ; pVert = (PTVertex)&--dbIt )
    {
      for( k = 0; k < m; k++, count++ )
       {
	 PTVertex pV;

	 pV = new TVertex( count, pVert->x * sin(deltaPs * k),
	   pVert->y,
	   pVert->x * cos(deltaPs * k)
                         );
	 if( !pV ) { pSurface->Status = ER_CREATESURF; return; }

	 pSurface->listVertex.add( *pV );

	 if( !pSurface->pvInitAlignVert || pSurface->pvInitAlignVert->y < pV->y )
	   pSurface->pvInitAlignVert = pV;
       }
    }
   dblTmp.flush( TShouldDelete::Delete );

   pSurface->listVertex.CreateChache();

   DoubleListIterator dbIt1 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert1;
   DoubleListIterator dbIt2 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert2;

   count = 1;
   for( k = 0; k < n; k++ )
    {
      pSurface->listVertex.SetIterator( count + 1, dbIt2 );
      for( int i = 0; i < m - 1; i++ )
       {
         pVert1 = (PTVertex)&dbIt1--;
         pVert2 = (PTVertex)&dbIt2--;
      
         if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

         PTAdge pAdge = new TAdge( count++, pVert1, pVert2 );
         if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
	 pSurface->listAdges.add( *pAdge );
       }

      pSurface->listVertex.SetIterator( m * k + 1, dbIt2 );

      pVert1 = (PTVertex)&dbIt1--;
      pVert2 = (PTVertex)&dbIt2--;
      
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( count++, pVert1, pVert2 );
      if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
      pSurface->listAdges.add( *pAdge );
    }


   for( k = 1; k <= m; k++ )
    {

      for( int i = 0; i < n - 1; i++ )
       {
	 pSurface->listVertex.SetIterator( m * i + k, dbIt1 );
	 pSurface->listVertex.SetIterator( m * (i + 1) + k, dbIt2 );

         pVert1 = (PTVertex)&dbIt1--;
         pVert2 = (PTVertex)&dbIt2--;
      
         if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

         PTAdge pAdge = new TAdge( count++, pVert1, pVert2 );
         if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
	 pSurface->listAdges.add( *pAdge );
       }

      pSurface->listVertex.SetIterator( (n - 1) * m + k, dbIt1 );
      pSurface->listVertex.SetIterator( k, dbIt2 );

      pVert1 = (PTVertex)&dbIt1--;
      pVert2 = (PTVertex)&dbIt2--;
      
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( count++, pVert1, pVert2 );
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

   int p = n * m;   
   count = 1;
   for( k = 0; k < m; k++ )
    {

      if( k == m - 1 )
	pSurface->listAdges.SetIterator( p + 1, dbIt2 );
      else
        pSurface->listAdges.SetIterator( p + 1 + (k + 1) * n, dbIt2 );
      pSurface->listAdges.SetIterator( p + 1 + k * n, dbIt4 );
      for( int i = 0; i < n - 1; i++ )
       {
	 pSurface->listAdges.SetIterator( i * m + k + 1, dbIt1 );
	 if( k == m - 1 )
	   pSurface->listAdges.SetIterator( (i + 2) * m, dbIt3 );
         else
	   pSurface->listAdges.SetIterator( (i + 1) * m + k + 1, dbIt3 );

	 pAge1 = (PTAdge)&dbIt1--;
         pAge2 = (PTAdge)&dbIt2--;
	 pAge3 = (PTAdge)&dbIt3--;
	 pAge4 = (PTAdge)&dbIt4--; 

	 if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT || *pAge4 == NOOBJECT )
           { pSurface->Status = ER_CREATESURF; return; }

         PTFacet pFacet = new TFacet( count++ );
         PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, 1 );
         if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
         pFacet->listAgesItem.add( *pAIt );
         pAIt = new TAdgeItem( -1, pAge2, 1 );
         pFacet->listAgesItem.add( *pAIt );
         pAIt = new TAdgeItem( -1, pAge3, -1 );
	 pFacet->listAgesItem.add( *pAIt );
	 pAIt = new TAdgeItem( -1, pAge4, -1 );
	 pFacet->listAgesItem.add( *pAIt );

         pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge2 );
         pSurface->listFacets.add( *pFacet );
       }

      pSurface->listAdges.SetIterator( (n - 1) * m + k + 1, dbIt1 );
      if( k == m - 1 )
	pSurface->listAdges.SetIterator( m, dbIt3 );
      else
	pSurface->listAdges.SetIterator( k + 1, dbIt3 );

      pAge1 = (PTAdge)&dbIt1--;
      pAge2 = (PTAdge)&dbIt2--;

      pAge3 = (PTAdge)&dbIt3--;
      pAge4 = (PTAdge)&dbIt4--; 

      if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT || *pAge4 == NOOBJECT )
       { pSurface->Status = ER_CREATESURF; return; }

       PTFacet pFacet = new TFacet( count++ );
       PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, 1 );
       if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge2, 1 );
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge3, -1 );
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge4, -1 );
       pFacet->listAgesItem.add( *pAIt );

       pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge2 );
       pSurface->listFacets.add( *pFacet );
    }

   pSurface->listFacets.CreateChache();

   //pSurface->Move( pFD->insertPoint.x, pFD->insertPoint.y, pFD->insertPoint.z );
 }

