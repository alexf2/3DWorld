#pragma option -v-

#include "surface.hpp"
#include "INC\errors.inc"
#include "fheader.hpp"
#include "scena.hpp"

#include <stdio.h>
#include <values.h>
#include <io.h>


TSphere::TSphere( PTFaceData pFD ):
  TSurface( pFD ),
  TInitShape( TSphere::InitShape )
  {
  }

TSphere::TSphere( int ind, PTFaceData pFD ):
  TSurface( ind, pFD ),
  TInitShape( TSphere::InitShape )
  {
  }

/*int nMerid, nHoriz;

  int radius;
*/

void TSphere::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   PTSphereInit  pSphereData = (PTSphereInit)pFD;

   int n = pSphereData->nHoriz, m = pSphereData->nMerid;
   int iVerNum = (n - 1) * m + 2,
       iLinNum = m * ( 2 * n - 1 ),
       iFacNum = m * n;
   double  deltaFi = M_PI / double(n);

   for( int v = 1; v <= n - 1; v++ )
    {
      PTVertex pVert;      

      pVert = new TVertex( v,
	double(pSphereData->radius) * sin(deltaFi * double(v)),
	double(pSphereData->radius) * cos(deltaFi * double(v)),
	0 );

      if( !pVert ) { pSurface->Status = ER_CREATESURF; return; }

      pSurface->listVertex.add( *pVert );
    }


   double deltaPs = 2.0 * M_PI / double(m);
   for( int k = 1, num = n; k <= m - 1; k++ )
    {
      DoubleListIterator dbIt = DoubleListIterator( pSurface->listVertex, FALSE );
      PTVertex pVert = (PTVertex)&dbIt.current();
      for( int i = 0 ; int(dbIt) && i < n - 1; pVert = (PTVertex)&--dbIt, i++ )
       {
	 PTVertex pV;

         pV = new TVertex( num++,
	   pVert->x * cos(deltaPs * double(k)),
	   pVert->y,
	   pVert->x * sin(deltaPs * double(k))
	                );
	 if( !pV ) { pSurface->Status = ER_CREATESURF; return; }

	 pSurface->listVertex.add( *pV );
       }
    }  

   PTVertex pVPolus;

   pVPolus = new TVertex( num++, 0, pSphereData->radius, 0 );			
   if( !pVPolus ) { pSurface->Status = ER_CREATESURF; return; }
   pSurface->listVertex.add( *pVPolus );

   pSurface->pfacInitAlignFac = NULL;
   pSurface->pvInitAlignVert = pVPolus;

   pVPolus = new TVertex( num, 0, -pSphereData->radius, 0 );
   if( !pVPolus ) { pSurface->Status = ER_CREATESURF; return; }
   pSurface->listVertex.add( *pVPolus );

   DoubleListIterator dbIt1 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert1;
   DoubleListIterator dbIt2 = DoubleListIterator( pSurface->listVertex, FALSE );
   PTVertex pVert2;

   pSurface->listVertex.CreateChache();

   //int it = pSurface->listVertex.getItemsInContainer();

   int count2 = 1, count;
   pSurface->listVertex.SetIterator( 1, dbIt1 );
   for( k = 0, count = 1; k < m; k++ )
    {
      count = count2;

      pSurface->listVertex.SetIterator( count+1, dbIt2 );
      for( int g = count; g < count + (n-1) - 1; g++, count2++ )
       {
         pVert1 = (PTVertex)&dbIt1--;
         pVert2 = (PTVertex)&dbIt2--;
      
         if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

         PTAdge pAdge = new TAdge( count2, pVert1, pVert2 );
         if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
         pSurface->listAdges.add( *pAdge );
       }      
      
      pSurface->listVertex.SetIterator( num, dbIt2 );

      pVert1 = (PTVertex)&dbIt1--;
      pVert2 = (PTVertex)&dbIt2--;
      
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( count2++, pVert1, pVert2 );
      if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
      pSurface->listAdges.add( *pAdge );
    }

   pSurface->listVertex.SetIterator( 1, dbIt1 );
   pSurface->listVertex.SetIterator( n, dbIt2 );
   for( k = 0; k < (m - 1) * (n - 1); k++ )
    {
      pVert1 = (PTVertex)&dbIt1--;
      pVert2 = (PTVertex)&dbIt2--;
      
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( count2++, pVert1, pVert2 );
      if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
      pSurface->listAdges.add( *pAdge );
    }

   pSurface->listVertex.SetIterator( 1, dbIt2 );
   for( k = 0; k < n - 1; k++ )
    {
      pVert1 = (PTVertex)&dbIt1--;
      pVert2 = (PTVertex)&dbIt2--;
      
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( count2++, pVert1, pVert2 );
      if( !pAdge ) { pSurface->Status = ER_CREATESURF; return; }
      pSurface->listAdges.add( *pAdge );
    }

   pSurface->listVertex.SetIterator( num - 1, dbIt1 );
   pSurface->listVertex.SetIterator( 1, dbIt2 );
   for( k = 0; k < m; k++ )
    {
      pVert1 = (PTVertex)&dbIt1.current();
      if( k > 0 )     
	pSurface->listVertex.SetIterator( k * n - (k - 1), dbIt2 );
      pVert2 = (PTVertex)&dbIt2--;
      
      if( *pVert1 == NOOBJECT || *pVert2 == NOOBJECT ) { pSurface->Status = ER_CREATESURF; return; }

      PTAdge pAdge = new TAdge( count2++, pVert1, pVert2 );
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

   int p = (n - 1) * m;
   count2 = 1;
   pSurface->listAdges.SetIterator( 1, dbIt1 );
   pSurface->listAdges.SetIterator( p + 1, dbIt2 );
   pSurface->listAdges.SetIterator( n, dbIt3 );
   pSurface->listAdges.SetIterator( p + 2, dbIt4 );
   for( k = 0, count = 1; k < m; k++ )
    {
      count = count2;
           
      for( int g = count; g < count + (n-1) - 1; g++, count2++ )
       {
	 if( count2 == 1 + (m - 1) * (n - 1) )
	   pSurface->listAdges.SetIterator( 1, dbIt3 );

	 pAge1 = (PTAdge)&dbIt1--;
         pAge2 = (PTAdge)&dbIt2--;
         pAge3 = (PTAdge)&dbIt3--;
         pAge4 = (PTAdge)&dbIt4--;

	 if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT || *pAge4 == NOOBJECT )
	   { pSurface->Status = ER_CREATESURF; return; }

	 PTFacet pFacet = new TFacet( count2 );
         PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, -1 );
         if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
         pFacet->listAgesItem.add( *pAIt );
         pAIt = new TAdgeItem( -1, pAge2, 1 );
         pFacet->listAgesItem.add( *pAIt );
         pAIt = new TAdgeItem( -1, pAge3, 1 );
	 pFacet->listAgesItem.add( *pAIt );
         pAIt = new TAdgeItem( -1, pAge4, -1 );
         pFacet->listAgesItem.add( *pAIt );
         //listFacet
    
	 pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge2 );
	 pSurface->listFacets.add( *pFacet );
       }

       pAge1 = (PTAdge)&dbIt1--;
       pAge2 = (PTAdge)&dbIt2--;
       pAge3 = (PTAdge)&dbIt3--;
       dbIt4--;

       if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT  )
         { pSurface->Status = ER_CREATESURF; return; }

       PTFacet pFacet = new TFacet( count2++ );
       PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, -1 );
       if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge2, 1 );
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge3, 1 );
       pFacet->listAgesItem.add( *pAIt );     

       pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge2 );
       pSurface->listFacets.add( *pFacet );
    }

   pSurface->listAdges.SetIterator( 2 * m * (n - 1) + 1, dbIt1 );
   pSurface->listAdges.SetIterator( 2 * m * (n - 1) + 2, dbIt2 );
   pSurface->listAdges.SetIterator( p + 1, dbIt3 );
   for( k = 0; k < m; k++ )
    {
      pAge1 = (PTAdge)&dbIt1--;
      if( k == m - 1 )
	pSurface->listAdges.SetIterator( 2 * m * (n - 1) + 1, dbIt2 );
      pAge2 = (PTAdge)&dbIt2--;
      if( k > 0 )
	pSurface->listAdges.SetIterator( p + 1 + (n - 1) * k, dbIt3 );
      pAge3 = (PTAdge)&dbIt3--; 

      if( *pAge1 == NOOBJECT || *pAge2 == NOOBJECT || *pAge3 == NOOBJECT  )
       { pSurface->Status = ER_CREATESURF; return; }

       PTFacet pFacet = new TFacet( count2++ );
       PTAdgeItem  pAIt = new TAdgeItem( -1, pAge1, -1 );
       if( !pAIt || !pFacet ) { pSurface->Status = ER_CREATESURF; return; }
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge2, 1 );
       pFacet->listAgesItem.add( *pAIt );
       pAIt = new TAdgeItem( -1, pAge3, -1 );
       pFacet->listAgesItem.add( *pAIt );
       
       pFacet->vecOutNormal = pSurface->CreateNormal( pAge1, pAge2 );
       pSurface->listFacets.add( *pFacet );
    }

   pSurface->listFacets.CreateChache();

   //pSurface->CalcNormals();
   //pSurface->Move( pFD->insertPoint.x, pFD->insertPoint.y, pFD->insertPoint.z );
  }
