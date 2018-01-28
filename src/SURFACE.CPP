//#pragma option -v-

#include "surface.hpp"
#include "INC\errors.inc"
#include "fheader.hpp"
#include "scena.hpp"

#include <stdio.h>
#include <values.h>
#include <io.h>


int TSurface::isEqual( const Object& rObj ) const
 {
   return (TSurface huge *)this == (TSurface huge *)&rObj;
 }

int CompareSurfaceY( const void *p1, const void *p2 )
 {
   RTSurface rSf1 = *(PTSurface)(*(PTIndexedItem*)p1),
	     rSf2 = *(PTSurface)(*(PTIndexedItem*)p2);

   if( rSf1.minY < rSf2.minY ) return -1;
   else if( rSf1.minY > rSf2.minY ) return 1;
   else return 0;
 }

int CompareSurfaceYRev( const void *p1, const void *p2 )
 {
   RTSurface rSf1 = *(PTSurface)(*(PTIndexedItem*)p1),
	     rSf2 = *(PTSurface)(*(PTIndexedItem*)p2);

   if( rSf1.maxY > rSf2.maxY ) return -1;
   else if( rSf1.maxY < rSf2.maxY ) return 1;
   else return 0;
 }


int CompareSurfaceYRevInt( const void *p1, const void *p2 )
 {
   RTSurface rSf1 = *(PTSurface)(*(PTIndexedItem*)p1),
	     rSf2 = *(PTSurface)(*(PTIndexedItem*)p2);

   if( rSf1.iMaxY > rSf2.iMaxY ) return -1;
   else if( rSf1.iMaxY < rSf2.iMaxY ) return 1;
   else return 0;
 }


int TSurface::CompareY( RTSurface rSfForCmp )
 {
   if( minY < rSfForCmp.minY ) return -1;
   else if( minY > rSfForCmp.minY ) return 1;
   else return 0;
 }
     

PTObol TSurface::CreateObol() const
 {
   return new TObol( (PTSurface)this );
 }

struct TDetectCommonDta
 {
   PTSurface pSf;
   PTFacet pF;
   PTAdge  pA;
 };
typedef TDetectCommonDta* PTDetectCommonDta;


static int FindCommonFacInst2( const Object _FAR& rObj, void _FAR* pD )
 {
   return RTAdgeItem(rObj).pAdge == PTDetectCommonDta(pD)->pA;
 }

static int FindCommonFacInst( const Object _FAR& rObj, void _FAR* pD )
 {
   if( PTDetectCommonDta(pD)->pF == &RTFacet(rObj) ) return 0;
   return RTFacet(rObj).listAgesItem.firstThat( FindCommonFacInst2, pD ) != NOOBJECT;
 }

static RTFacet near pascal FindCommonFac( RTAdge rA, PTDetectCommonDta pD )
 {
   pD->pA = &rA;
   return (RTFacet)pD->pSf->listFacets.firstThat( FindCommonFacInst, Pvoid(pD) );
 }

static void DetectCommonFacetsInst2( Object _FAR& rObj, void _FAR* pD )
 {
   RTFacet rFacCommon = FindCommonFac( *RTAdgeItem(rObj).pAdge, (PTDetectCommonDta)pD );
   if( rFacCommon == NOOBJECT ) RTAdgeItem(rObj).pFCommon = NULL;
   else RTAdgeItem(rObj).pFCommon = &rFacCommon;
 } 

static void DetectCommonFacetsInst( Object _FAR& rObj, void _FAR* pD )
 {
   PTDetectCommonDta(pD)->pF = (PTFacet)&rObj;
   RTFacet(rObj).listAgesItem.forEach( DetectCommonFacetsInst2, pD );
 }

void TSurface::DetectCommonFacets()
 {
   TDetectCommonDta dta; dta.pSf = this;
   listFacets.forEach( DetectCommonFacetsInst, Pvoid(&dta) );
 }

static void ResetSmothGrpInst( Object _FAR& rObj, void _FAR* )
 {
   RTFacet(rObj).iSmothGroup = 0;
 }

void TSurface::ResetSmothGroups()
 {
   listFacets.forEach( ResetSmothGrpInst, 0 );
 }

struct TTstAngleDta
 {
   double dAngle;
   PTFacet pF;
   BOOL bFlHaveSmothAdges;
 };
typedef TTstAngleDta* PTTstAngleDta;

static void MarkNoSmothInst2( Object _FAR& rObj, void _FAR* pD )
 {
   if( !RTAdgeItem(rObj).pFCommon ) return;

   double dA = acos(RTAdgeItem(rObj).pFCommon->vecOutNormal & PTTstAngleDta(pD)->pF->vecOutNormal);
   RTAdgeItem(rObj).dAngleRadNormalsSelfAndFCommon = dA;

   if( dA < PTTstAngleDta(pD)->dAngle ) PTTstAngleDta(pD)->bFlHaveSmothAdges = TRUE;
 }

static void MarkNoSmothInst( Object _FAR& rObj, void _FAR* pD )
 {
   PTTstAngleDta(pD)->pF = &RTFacet(rObj);
   PTTstAngleDta(pD)->bFlHaveSmothAdges = FALSE;

   RTFacet(rObj).listAgesItem.forEach( MarkNoSmothInst2, Pvoid(pD) );

   if( PTTstAngleDta(pD)->bFlHaveSmothAdges == FALSE ) RTFacet(rObj).iSmothGroup = -1;
 }

static int FindNoSmothedInst( const Object _FAR& rObj, void _FAR* )
 {
   return !RTFacet(rObj).iSmothGroup;
 }


void TSurface::AutoSmoth( double dGradAngleMinSmoth )
 {
   ResetSmothGroups();
   double dCosSmoth = GRAD_TO_RAD(dGradAngleMinSmoth);

   TTstAngleDta dta; dta.dAngle = dCosSmoth;
   listFacets.forEach( MarkNoSmothInst, Pvoid(&dta) );
   

   for( int iCurSmothGrp = 1; 1; iCurSmothGrp++ )
    {
      RTFacet rFirstNoSmothed = (RTFacet)listFacets.firstThat( FindNoSmothedInst, 0 );
      if( rFirstNoSmothed == NOOBJECT ) break;

      rFirstNoSmothed.iSmothGroup = iCurSmothGrp;

      BOOL bFlHaveNoSmothedInGrp = TRUE;
      while( bFlHaveNoSmothedInGrp == TRUE )
       {
	 DoubleListIterator dblIt( listFacets );
	 PTFacet pF = (PTFacet)&dblIt.current();

         BOOL bFlSmoth = FALSE;
	 for( ; int(dblIt); pF = (PTFacet)&++dblIt )
	  {
	    if( pF->iSmothGroup != iCurSmothGrp ) continue;

	    DoubleListIterator dblIt2( pF->listAgesItem );
	    PTAdgeItem pAIt = (PTAdgeItem)&dblIt2.current();

	    for( ; int(dblIt2); pAIt = (PTAdgeItem)&++dblIt2 )
	     {
	       if( !pAIt->pFCommon || pAIt->pFCommon->iSmothGroup ||
		    pAIt->dAngleRadNormalsSelfAndFCommon >= dCosSmoth
		   //acos(pAIt->pFCommon->vecOutNormal & pF->vecOutNormal) >= dCosSmoth
		 ) continue;

	       bFlSmoth = TRUE;
	       pAIt->pFCommon->iSmothGroup = iCurSmothGrp;
             }
	  }

	 bFlHaveNoSmothedInGrp = bFlSmoth;
       }
    }
 }

static void SmothAllInOneGroupInst( Object _FAR& rObj, void _FAR* pGrp )
 {
   RTFacet(rObj).iSmothGroup = *(int*)pGrp;
 }

void TSurface::SmothAllInOneGroup( int idGrp )
 {
   listFacets.forEach( SmothAllInOneGroupInst, Pvoid(&idGrp) );
 }

float TSurface::TransparencyLinear( float fZNorm ) const
 {
   return medium.GdKt();
 }

float TSurface::TransparencyFalloff( float fZNorm ) const
 {
   return medium.GdRefr() + ( medium.GdBetta() - medium.GdRefr() ) *
     ( 1 - (1 - fabs(fZNorm)) );
 }

void TSurface::AssignTransparencyFunc()
 {
   pFuncTransparency = (medium.IsLineTransp() == TRUE) ?
     &TSurface::TransparencyLinear:&TSurface::TransparencyFalloff;
 }


TSurface::TSurface( PTFaceData pFD ):
  TIndexedItem( -1 ),
  TInitShape( TSurface::InitShape )
  {
    bFlSelect = FALSE;
    Status = 0;
    pfdKey = pFD;
    bFlCreateObol = TRUE;
    bFlObolVis = FALSE;
    if( initShape ) initShape( this, pFD );

    if( pFD && pFD->bFlSmoothed == TRUE )
     {
       CalcNormals();
       DetectCommonFacets();
       AutoSmoth( pFD->dAngleSmothGrad );
     }

    pFuncTransparency = &TSurface::TransparencyLinear;
  }

TSurface::TSurface( int ind, PTFaceData pFD ):
  TIndexedItem( ind ),
  TInitShape( TSurface::InitShape )
  {
    bFlSelect = FALSE;
    Status = 0;
    pfdKey = pFD;
    bFlCreateObol = TRUE;
    bFlObolVis = FALSE;
    if( initShape ) initShape( this, pFD );

    if( pFD && pFD->bFlSmoothed == TRUE )
     {
       CalcNormals();
       DetectCommonFacets();
       AutoSmoth( pFD->dAngleSmothGrad );
     }

    pFuncTransparency = &TSurface::TransparencyLinear;
  };


TSurface::TSurface( RTSurface rSurf, PTIndicator pInd ):
  TIndexedItem( rSurf.Index() ),
  TInitShape( NULL ),
  medium( rSurf.medium )
 {
   pFuncTransparency = rSurf.pFuncTransparency;

   pGetTraceNormalFunc = rSurf.pGetTraceNormalFunc;
   tsAlho = rSurf.tsAlho;
   dIAmbientR = rSurf.dIAmbientR;
   dIAmbientG = rSurf.dIAmbientG;
   dIAmbientB = rSurf.dIAmbientB;


   bFlSelect = rSurf.bFlSelect;
   Status = rSurf.Status;////////////////
   pfdKey = rSurf.pfdKey;
   //pfdKey = NULL;
   minX = rSurf.minX; maxX = rSurf.maxX;
   minY = rSurf.minY; maxY = rSurf.maxY;
   minZ = rSurf.minZ; maxZ = rSurf.maxZ;
   iMinX = rSurf.iMinX; iMaxX = rSurf.iMaxX;
   iMinY = rSurf.iMinY; iMaxY = rSurf.iMaxY;
   iMinZ = rSurf.iMinZ; iMaxZ = rSurf.iMaxZ;

   bFlObolVis = rSurf.bFlObolVis;
   bFlCreateObol = rSurf.bFlCreateObol;

   DoubleListIterator dblIt = DoubleListIterator( rSurf.listVertex );
   PTVertex pV = (PTVertex)&dblIt.current();
   for( ; int(dblIt); pV = (PTVertex)&++dblIt )
    {
      PTVertex pTmpV = new TVertex( *pV );
      if( !pTmpV ) { Status = -1; return; }

      listVertex.addAtTail( *pTmpV );
      if( pInd && pInd->WorkMessageAdd(TVertex::SizeData()) == TRUE )
       { Status = 1; return; }
    }

   listVertex.CreateChache();

   dblIt = DoubleListIterator( rSurf.listAdges );
   PTAdge pA = (PTAdge)&dblIt.current();
   for( ; int(dblIt); pA = (PTAdge)&++dblIt )
    {
      PTAdge pTmpA = new TAdge( pA->Index(), listVertex[pA->pV1->Index()],
	                        listVertex[pA->pV2->Index()] );
      if( !pTmpA ) { Status = -1; return; }

      listAdges.addAtTail( *pTmpA );
      if( pInd && pInd->WorkMessageAdd(TAdge::SizeData()) == TRUE )
       { Status = 1; return; }
    }

   listAdges.CreateChache();

   dblIt = DoubleListIterator( rSurf.listFacets );
   PTFacet pF = (PTFacet)&dblIt.current();
   for( ; int(dblIt); pF = (PTFacet)&++dblIt )
    {
      PTFacet  pTmpF = new TFacet( pF->Index() );
      if( !pTmpF ) { Status = -1; return; }

      pTmpF->pSPParent = pF;

      pTmpF->vecOutNormal = pF->vecOutNormal;
      pTmpF->iSmothGroup = pF->iSmothGroup;

      pTmpF->iMinX = pF->iMinX;
      pTmpF->iMaxX = pF->iMaxX;
      pTmpF->iMinY = pF->iMinY;
      pTmpF->iMaxY = pF->iMaxY;
      pTmpF->iMinZ = pF->iMinZ;
      pTmpF->iMaxZ = pF->iMaxZ;

      pTmpF->minY = pF->minY;
      pTmpF->minZ = pF->minZ;
      pTmpF->minX = pF->minX;
      pTmpF->maxY = pF->maxY;
      pTmpF->maxZ = pF->maxZ;
      pTmpF->maxX = pF->maxX;

      pTmpF->d = pF->d;

      DoubleListIterator dblIt2 = DoubleListIterator( pF->listAgesItem );
      PTAdgeItem pIt = (PTAdgeItem)&dblIt2.current();
      for( ; int(dblIt2); pIt = (PTAdgeItem)&++dblIt2 )
       {
		 
	 PTAdgeItem  pTmpIt = new TAdgeItem( pIt->Index(),
	   (PTAdge)listAdges[pIt->pAdge->Index()], pIt->clockVise );                     
	 if( !pTmpIt ) { Status = -1; return; }

	 pTmpF->listAgesItem.addAtTail( *pTmpIt );
	 //if( pInd && pInd->WorkMessageAdd(TAdgeItem::SizeData()) == TRUE )
	   //{ Status = 1; return; } 
  
	 pTmpIt->dAngleRadNormalsSelfAndFCommon = pIt->dAngleRadNormalsSelfAndFCommon;
	 pTmpIt->vecNorm1 = pIt->vecNorm1;
	 pTmpIt->vecNorm2 = pIt->vecNorm2;

	 pTmpIt->dStrScan  =  pIt->dStrScan;
	 pTmpIt->dTestParam = pIt->dTestParam;
	 pTmpIt->bTestParam = pIt->bTestParam;
       }

      listFacets.addAtTail( *pTmpF );
      if( pInd && pInd->WorkMessageAdd(TFacet::SizeData() + pF->GetBytesSize()) == TRUE )
       { Status = 1; return; }
    }

   listFacets.CreateChache();
 }

static void CalcBoundsAllFacetsInstance( Object _FAR &rFac, void _FAR * )
 {
   RTFacet(rFac).CalcBounds();
 }

void TSurface::CalcBoundsAllFacets()
 {
   listFacets.forEach( CalcBoundsAllFacetsInstance, NULL );
 }

void TSurface::CalcBounds()
 {
   classType cl = isA();

   if( cl == TYP_TOMNI || cl == TYP_TOMNIOBOL )
    {
      RT3DPoint rPt = (RT3DPoint)RTVertex( listVertex.peekAtHead() );
      maxX = minX = iMinX = iMaxX = rPt.x;
      maxY = minY = iMinY = iMaxY = rPt.y;
      maxZ = minZ = iMinZ = iMaxZ = rPt.z;

      return;
    }

   DoubleListIterator dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex pVert = (PTVertex)&dbIt.current();

   minY = minZ = minX = INT_MAX;
   maxY = maxZ = maxX = INT_MIN;

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      minX = (pVert->x < minX) ? pVert->x:minX;
      maxX = (pVert->x > maxX) ? pVert->x:maxX;

      minY = (pVert->y < minY) ? pVert->y:minY;
      maxY = (pVert->y > maxY) ? pVert->y:maxY;

      minZ = (pVert->z < minZ) ? pVert->z:minZ;
      maxZ = (pVert->z > maxZ) ? pVert->z:maxZ;
    }

   iMinX = floor( minX );
   iMaxX = ceil( maxX );
   iMinY = floor( minY );
   iMaxY = ceil( maxY );
   iMinZ = floor( minZ );
   iMaxZ = ceil( maxZ );
 }

T3DPoint TSurface::GetGeomCenter()
 {

   CalcBounds();   
   return  T3DPoint( minX + fabs(maxX - minX) / 2.0,
		     minY + fabs(maxY - minY) / 2.0,
		     minZ + fabs(maxZ - minZ) / 2.0
		   );
 }          

void TSurface::InitShape( PTSurface, PTFaceData )
 {
 }

TVector TSurface::CreateNormal( PTAdge pAdge1, PTAdge pAdge2 )
 {
   TVector  v1 = TVector( pAdge1->pV2->x - pAdge1->pV1->x,
                          pAdge1->pV2->y - pAdge1->pV1->y,
			  pAdge1->pV2->z - pAdge1->pV1->z
	                ),
	    v2 = TVector( pAdge2->pV2->x - pAdge2->pV1->x,
			  pAdge2->pV2->y - pAdge2->pV1->y,
			  pAdge2->pV2->z - pAdge2->pV1->z
		        );   

   //return  Normalyze( VecMul( v1, v2 ) );
   return  Normalyze( v1 ^ v2  );
 }

//&&&


static void CalcNormalsInstance( Object _FAR &rObj, void _FAR *pDta )
 {
   RTFacet(rObj).vecOutNormal = RTFacet(rObj).GetNewelNormal();


   RTAdgeItem rA = (RTAdgeItem)RTFacet(rObj).listAgesItem.peekAtHead();
   if( rA == NOOBJECT ) return;
   PTVertex pV = rA.pAdge->pV1;

   RTFacet(rObj).d = -RTFacet(rObj).vecOutNormal.x * pV->x -
	             RTFacet(rObj).vecOutNormal.y * pV->y -
		     RTFacet(rObj).vecOutNormal.z * pV->z;
 }

void TSurface::CalcNormals()
 {
   listFacets.forEach( CalcNormalsInstance, NULL );

   /*DoubleListIterator dbIt = DoubleListIterator( listFacets, FALSE );
   PTFacet pFacet = (PTFacet)&dbIt.current();

   for( ; int(dbIt);  pFacet = (PTFacet)&--dbIt )
     pFacet->vecOutNormal = pFacet->GetNewelNormal();*/
 }

struct TCalcNAV
 {
   PTIndicator pInd;
   PTSurface pSf;

   TCalcNAV( PTIndicator pInd_, PTSurface pSf_ )
    {
      pInd = pInd_;
      pSf = pSf_;
    }
 };

typedef TCalcNAV* PTCalcNAV;
typedef TCalcNAV& RTCalcNAV;

static void CalcNormalsAllVertexInstance( Object _FAR &rObj, void _FAR *pD )
 {
   RTVertex(rObj).vecMeanPhongNormal = PTCalcNAV(pD)->pSf->CalcVertexNormal( RTVertex(rObj) );

   if( PTCalcNAV(pD)->pInd ) PTCalcNAV(pD)->pInd->WorkMessageAdd( 1 );
 }

void TSurface::CalcNormalsAllVertex( PTIndicator pInd )
 {
 //CalcVertexNormal
   TCalcNAV dta( pInd, this );
   listVertex.forEach( CalcNormalsAllVertexInstance, Pvoid(&dta) );
 }

static void near pascal ResetActionFacetVertex( RTFacet rF )
 {
   DoubleListIterator dblIt2( rF.listAgesItem );
   PTAdgeItem pAIt = (PTAdgeItem)&dblIt2.current();

   for( ; int(dblIt2); pAIt = (PTAdgeItem)&++dblIt2 )
    {
      pAIt->pAdge->pV1->bFlAtion = FALSE;
      pAIt->pAdge->pV2->bFlAtion = FALSE;
    }
 }

static void near pascal AssignFacetPhongNormal( RTFacet rF, RTVertex rV, RTVector rvecNorm )
 {
   DoubleListIterator dblIt2( rF.listAgesItem );
   PTAdgeItem pAIt = (PTAdgeItem)&dblIt2.current();

   for( ; int(dblIt2); pAIt = (PTAdgeItem)&++dblIt2 )
    {
      PTAdge pA = pAIt->pAdge;

      if( pA->pV1 == &rV ) pAIt->vecNorm1 = rvecNorm;
      else if( pA->pV2 == &rV ) pAIt->vecNorm2 = rvecNorm;
    }
 }

void TSurface::CalcNormalsAllVertexPhongAdges()
 {  
   DoubleListIterator dblIt( listFacets );
   PTFacet pF = (PTFacet)&dblIt.current();

   for( ; int(dblIt); pF = (PTFacet)&++dblIt )
    {
      ResetActionFacetVertex( *pF );

      DoubleListIterator dblIt2( pF->listAgesItem );
      PTAdgeItem pAIt = (PTAdgeItem)&dblIt2.current();

      for( ; int(dblIt2); pAIt = (PTAdgeItem)&++dblIt2 )
       {
	 PTAdge pA = pAIt->pAdge;

	 if( pF->iSmothGroup == -1 || pF->iSmothGroup == 0 )
	  {
	    pAIt->vecNorm1 = pF->vecOutNormal;
	    pAIt->vecNorm2 = pF->vecOutNormal;
            continue;
	  }

	 if( pA->pV1->bFlAtion == FALSE )
          {
	    TVector vecMeanPhongNormal = CalcVertexNormal( *pA->pV1, pF->iSmothGroup );
	    pA->pV1->bFlAtion = TRUE;
	    AssignFacetPhongNormal( *pF, *pA->pV1, vecMeanPhongNormal );
	  }

	 if( pA->pV2->bFlAtion == FALSE )
          {
	    TVector vecMeanPhongNormal = CalcVertexNormal( *pA->pV2, pF->iSmothGroup );
	    pA->pV2->bFlAtion = TRUE;
	    AssignFacetPhongNormal( *pF, *pA->pV2, vecMeanPhongNormal );
	  }
       }
    }
 }

struct TCalcVertexNormDta
 {
   PTVertex pVertex;
   PTVector pVector;
   int iSmothGrp;

   TCalcVertexNormDta( PTVertex pV, PTVector pVec, int iSmothGrp_ )
    {
      pVertex = pV; pVector = pVec; iSmothGrp = iSmothGrp_;
    }
 };
typedef TCalcVertexNormDta _FAR *PTCalcVertexNormDta;

static int TestVertexInstance( const Object _FAR &rObj, void _FAR *pV )
 {
   return (void huge *)(RTAdgeItem(rObj).pAdge->pV1) == (void huge *)pV ||
	  (void huge *)(RTAdgeItem(rObj).pAdge->pV2) == (void huge *)pV;
 }

static void CalcVertexNormalInstance( Object _FAR &rFac, void _FAR *pDta )
 {
   RTAdge rA = (RTAdge)RTFacet(rFac).listAgesItem.firstThat( TestVertexInstance,
     Pvoid(PTCalcVertexNormDta(pDta)->pVertex) );
   if( rA == NOOBJECT ) return;

   if( PTCalcVertexNormDta(pDta)->iSmothGrp != -1 &&
       PTCalcVertexNormDta(pDta)->iSmothGrp != RTFacet(rFac).iSmothGroup
     ) return;

   PTCalcVertexNormDta(pDta)->pVector->x += RTFacet(rFac).vecOutNormal.x;
   PTCalcVertexNormDta(pDta)->pVector->y += RTFacet(rFac).vecOutNormal.y;
   PTCalcVertexNormDta(pDta)->pVector->z += RTFacet(rFac).vecOutNormal.z;
 }

TVector TSurface::CalcVertexNormal( RTVertex rV, int iSmothGrp ) const //need initialized facet normals
 {  
   TVector vNormal;
   if( !listFacets.getItemsInContainer() ) return vNormal;

   if( !listVertex.hasMember(rV) ) return vNormal;

   TCalcVertexNormDta cvnDta( &rV, &vNormal, iSmothGrp );
   listFacets.forEach( CalcVertexNormalInstance, Pvoid(&cvnDta) );


   return Normalyze( *cvnDta.pVector );
 }

struct TGetCommonFacetsDta
 {
   PTVertex pVertex;
   DoubleList _FAR *pDbl;

   TGetCommonFacetsDta( PTVertex pV, DoubleList _FAR *pDbl_ )
    {
      pVertex = pV; pDbl = pDbl_;
    }
 };
typedef TGetCommonFacetsDta _FAR *PTGetCommonFacetsDta;


static void GetCommonFacetsListInstance( Object _FAR &rFac, void _FAR *pDta )
 {
   RTAdge rA = (RTAdge)RTFacet(rFac).listAgesItem.firstThat( TestVertexInstance,
     Pvoid(PTGetCommonFacetsDta(pDta)->pVertex) );
   if( rA == NOOBJECT ) return;

   PTGetCommonFacetsDta(pDta)->pDbl->add( *new TFacetItem(RTFacet(rFac)) );
 }


void TSurface::GetCommonFacetsList( DoubleList _FAR &rdblList, RTVertex rV ) const
 {
   if( !listVertex.hasMember(rV) ) return;

   TGetCommonFacetsDta gcfDta( &rV, &rdblList );
   listFacets.forEach( GetCommonFacetsListInstance, Pvoid(&gcfDta) );

   return;
 }

void TSurface::GetCreationAlignDataFacet( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   if( !pfacInitAlignFac )
    {
      GetCreationAlignData( rvAlign, rvertAlign );
      return;
    }

   rvAlign = pfacInitAlignFac->GetNewelNormal();

   rvertAlign = pfacInitAlignFac->GetGeomCenter();
 }

void TSurface::GetCreationAlignDataVertex( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   if( !pvInitAlignVert )
    {
      GetCreationAlignData( rvAlign, rvertAlign );
      return;
    }

   rvAlign = CalcVertexNormal( *pvInitAlignVert );
   rvertAlign = *pvInitAlignVert;
 }


void TCylinder::GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   GetCreationAlignDataFacet( rvAlign, rvertAlign );
 }

void TCone::GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   GetCreationAlignDataFacet( rvAlign, rvertAlign );
 }

void TTube::GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   GetCreationAlignDataFacet( rvAlign, rvertAlign );
 }

void TSphere::GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   GetCreationAlignDataVertex( rvAlign, rvertAlign );
 }

void TTorus::GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const
 {
   //GetCreationAlignDataVertex( rvAlign, rvertAlign );
   rvAlign = CalcVertexNormal( *pvInitAlignVert );
   rvertAlign = *pvInitAlignVert;

   T3DPoint pC = GetGeomCenter();
   rvertAlign.x = pC.x; rvertAlign.z = pC.z;
 }



void TSurface::TestToFile( LPSTR lpName )
 {
    /*TIndexedDoubleList  listVertex;
  TIndexedDoubleList  listAdges;
  TIndexedDoubleList  listFacets;*/
              //fprintf
  //HFILE    Hf;
  //OFSTRUCT of; 
  //if( (Hf = OpenFile(lpName, &of, OF_WRITE | OF_CREATE)) == HFILE_ERROR ) { return; }
  FILE *f = fopen( lpName , "w+t" );

  DoubleListIterator dbIt = DoubleListIterator( listVertex, FALSE );
  PTVertex pVert = (PTVertex)&dbIt.current();

  char cStr[80] = "Vertex:\n\n\tX \tY \tZ\n\n";
  //_lwrite( Hf, cStr, strlen(cStr) );

  for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
   {
     fprintf( f, "[%d]: %5.2f  %5.2f  %5.2f\n", pVert->Index(),
       float(pVert->x),
       float(pVert->y),  
       float(pVert->z)
	    );
     //_lwrite( Hf, cStr, strlen(cStr) );
   }

  fprintf( f, "\n\t\tAdges:  \tSta \tEnd\n\n" );
  //_lwrite( Hf, cStr, strlen(cStr) );
  dbIt = DoubleListIterator( listAdges, FALSE );
  PTAdge pAge = (PTAdge)&dbIt.current();

  for( ; int(dbIt); pAge = (PTAdge)&--dbIt )
   {
     fprintf( f, "[%d]: %5d(%5.2f  %5.2f  %5.2f)  %5d(%5.2f  %5.2f  %5.2f)\n", pAge->Index(),
       pAge->pV1->Index(),

       float(pAge->pV1->x),
       float(pAge->pV1->y),
       float(pAge->pV1->z),

       pAge->pV2->Index(),

       float(pAge->pV2->x),
       float(pAge->pV2->y),
       float(pAge->pV2->z)
	    );
     //_lwrite( Hf, cStr, strlen(cStr) );
   }

  fprintf( f, "\n\t\tFaces:  \n\n" );
  //_lwrite( Hf, cStr, strlen(cStr) );
  dbIt = DoubleListIterator( listFacets, FALSE );
  PTFacet pFac = (PTFacet)&dbIt.current();
  for( ; int(dbIt); pFac = (PTFacet)&--dbIt )
   {
     DoubleListIterator dbIt2 = DoubleListIterator( pFac->listAgesItem, FALSE );
     PTAdgeItem pItem = (PTAdgeItem)&dbIt2.current();

     fprintf( f, "Face [%d]:  ", pFac->Index() );
     //_lwrite( Hf, cStr, strlen(cStr) );
      for( ; int(dbIt2); pItem = (PTAdgeItem)&--dbIt2 )
       {
	 fprintf( f, "%5d(%d)  ", pItem->pAdge->Index(),
	   pItem->clockVise );
	 //_lwrite( Hf, cStr, strlen(cStr) );
       }

     fprintf( f, " Normal: [%2.2f  %2.2f  %2.2f]\n",
       float(pFac->vecOutNormal.x),
       float(pFac->vecOutNormal.y),
       float(pFac->vecOutNormal.z)
     );
     //_lwrite( Hf, cStr, strlen(cStr) );
   }

  fclose( f );
 }


int TSurface::Save( HFILE hf, PTIndicator pInd )
 {
   TSurfHdr shHdr(
		   listVertex.getItemsInContainer(),
		   listAdges.getItemsInContainer(),
		   listFacets.getItemsInContainer(),
		   minX, maxX, minY, maxY, minZ, maxZ,
		   bFlCreateObol,
		   bFlSelect
		 );
   shHdr.InitCRC();

   if( shHdr.Save(hf) == HFILE_ERROR ) return -3;
   if( pInd && pInd->WorkMessageAdd( sizeof(TSurfHdr) ) == TRUE ) return -5;

   if( medium.Save(hf) == HFILE_ERROR ) return -3;

   DoubleListIterator dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex pVert = (PTVertex)&dbIt.current();
   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      if( pVert->Save(hf) == -1 ) return -3;
      if( pInd && pInd->WorkMessageAdd(TVertex::SizeData()) == TRUE ) return -5;
    }


   dbIt = DoubleListIterator( listAdges, FALSE );
   PTAdge pAg = (PTAdge)&dbIt.current();
   for( ; int(dbIt); pAg = (PTAdge)&--dbIt )
    {
      if( pAg->Save(hf) == -1 ) return -3;
      if( pInd && pInd->WorkMessageAdd(TAdge::SizeData()) == TRUE ) return -5;
    }


   dbIt = DoubleListIterator( listFacets, FALSE );
   PTFacet pFac = (PTFacet)&dbIt.current();
   for( ; int(dbIt); pFac = (PTFacet)&--dbIt )
    {
      if( pFac->Save(hf) == -1 ) return -3;

      if( pInd && pInd->WorkMessageAdd(pFac->GetBytesSize() + TFacet::SizeData()) == TRUE ) return -5;
    }


   return 0;
 }

long TSurface::GetBytesSize() const
 {
   long size = (long)listVertex.getItemsInContainer() * TVertex::SizeData() +
	       (long)listAdges.getItemsInContainer() * TAdge::SizeData() +
	       (long)listFacets.getItemsInContainer() * TFacet::SizeData();

   DoubleListIterator  dblIter = DoubleListIterator( listFacets );
   PTFacet  pFacet = (PTFacet)&dblIter.current();

   for( ; int(dblIter); pFacet = (PTFacet)&++dblIter )
     size += pFacet->GetBytesSize();

   return size;
 }

  //-1 - internal
  //-2 - memory
  //-3 - hard
  //-4 - CRC         /***********************************
  //-5 - Cancel


int TSurface::Load( HFILE hf, PTIndicator pInd  )
 {
   TSurfHdr shHdr;

   if( shHdr.Read(hf) == HFILE_ERROR ) return -3;
   if( shHdr.CheckCRC() == FALSE ) return -4;


   bFlCreateObol = shHdr.d.bFlCreateObol;
   maxZ = shHdr.d.maxZ;
   minZ = shHdr.d.minZ;
   maxY = shHdr.d.maxY;
   minY = shHdr.d.minY;
   maxX = shHdr.d.maxX;
   minX = shHdr.d.minX;


   bFlSelect = shHdr.d.bFlSelect;


   if( medium.Load(hf) == HFILE_ERROR ) return -3;

   for( int i = 0; i < shHdr.d.iNumVertex; i++ )
    {
      PTVertex pVert = new TVertex( i + 1, 0, 0, 0 );
      if( !pVert ) return -2;
      pVert->Load( hf );
      listVertex.add ( *pVert );

      if( pInd && pInd->WorkMessage( tell(hf) ) == TRUE ) return -5;
    }

   listVertex.CreateChache();

   for( i = 0; i < shHdr.d.iNumAdges; i++ )
    {
      int ind1, ind2;
      UINT l;
      if( (l = _lread( hf, &ind1, sizeof(int) )) == HFILE_ERROR ) return -3;
      if( (l = _lread( hf, &ind2, sizeof(int) )) == HFILE_ERROR ) return -3;
      if( l != sizeof(int) ) return -3;

      PTAdge pAg = new TAdge( i + 1, listVertex[ind1], listVertex[ind2] );
      if( !pAg ) return -2;

      listAdges.add ( *pAg );

      if( pInd && pInd->WorkMessage( tell(hf) ) == TRUE ) return -5;
    }

   listAdges.CreateChache();

   for( i = 0; i < shHdr.d.iNumFacets; i++ )
    {
      PTFacet pF = new TFacet( i + 1 );
      if( !pF ) return -2;
      UINT l;
      if( (l = _lread( hf, &(pF->vecOutNormal), sizeof(TVector) )) == HFILE_ERROR ) return -3;
      if( l != sizeof(TVector) ) return -3;

      if( (l = _lread( hf, &(pF->iSmothGroup), sizeof(int) )) == HFILE_ERROR ) return -3;
      if( l != sizeof(int) ) return -3;
      

      int numAdges;
      if( (l = _lread( hf, &numAdges, sizeof(int) )) == HFILE_ERROR ) return -3;
      if( l != sizeof(int) ) return -3;
      for( int j = 0; j < numAdges; j++ )
       {
	 PTAdgeItem pA = new TAdgeItem();
	 if( !pA ) return -2;
	 if( (l = _lread( hf, &(pA->clockVise), sizeof(int) )) == HFILE_ERROR ) return -3;
	 if( l != sizeof(int) ) return -3;
	 int ind;
	 if( (l = _lread( hf, &ind, sizeof(int) )) == HFILE_ERROR ) return -3;
	 if( l != sizeof(int) ) return -3;
	 pA->pAdge = (PTAdge)listAdges[ ind ];

	 pF->listAgesItem.add( *pA );
       }

      listFacets.add( *pF );

      listFacets.CreateChache();

      if( pInd && pInd->WorkMessage( tell(hf) ) == TRUE ) return -5;
    }

   return 0;
 }



void TSurface::Draw( BOOL bFlShowBackFaces, PTScena pSc ) const
 {
   PTUCS pUcs = pSc->GetUcs();
     
   listAdges.ResetActionFlag();

   DoubleListIterator dbFacet = DoubleListIterator( listFacets, FALSE );
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

	 /*tmpP[0] =T3DPoint( pAge->pAdge->pV1->x,
			    pAge->pAdge->pV1->y,
		            pAge->pAdge->pV1->z );

	 tmpP[1] =T3DPoint( pAge->pAdge->pV2->x,
	                    pAge->pAdge->pV2->y,
			    pAge->pAdge->pV2->z );*/
	 tmpP[0] = T3DPoint( *(PT3DPoint)pAge->pAdge->pV1 );
	 tmpP[1] = T3DPoint( *(PT3DPoint)pAge->pAdge->pV2 );

	 pUcs->RecalcUserToBase( tmpP, 2 );
	 pSc->CallProector( tmpP );	
       }
    }

   if( bFlObolVis == TRUE && NeedCreateObol() == TRUE )
     pSc->GetObol( Index() )->Draw( bFlShowBackFaces, pSc );
     //((PTSurface)listObl[rSurf.Index()])->Draw();
     //DrawBody( *(PTSurface)listObl[rSurf.Index()] );
 }

struct TSpecAdge
 {
   POINT p1, p2;

   TSpecAdge() {};
   TSpecAdge( POINT& rP1, POINT& rP2 )
    {
      p1 = rP1; p2 = rP2;  
    }
 };
typedef TSpecAdge _FAR *PTSpecAdge;

static BOOL FindAdge( PTSpecAdge keyDraw, POINT* pt, int iLimit )
 {
   for( int i = 0; i < iLimit; i++ )
     if( (pt[0].x == keyDraw[i].p1.x && pt[0].y == keyDraw[i].p1.y &&
	  pt[1].x == keyDraw[i].p2.x && pt[1].y == keyDraw[i].p2.y) ||

	 (pt[1].x == keyDraw[i].p1.x && pt[1].y == keyDraw[i].p1.y &&
	  pt[0].x == keyDraw[i].p2.x && pt[0].y == keyDraw[i].p2.y)
       )  return TRUE;

   return FALSE;
 }


inline int min( const int a1, int a2 )
 {
   return (a1 < a2) ? a1:a2;
 }


void TSurface::DrawSpecial( BOOL bFlShowBackFaces, PTScena pSc )
 {
   PTUCS pUcs = pSc->GetUcs();
   HDC hdc = pSc->GetHdc();

   //if( bFlVisyal == FALSE ) return;
   const int iSizeKey = 50;
   static TSpecAdge keyDraw[iSizeKey];

   listAdges.ResetActionFlag();

   iMinX = iMinY = INT_MAX;
   iMaxX = iMaxY = INT_MIN;

   int iKeyIndex = 0;

   DoubleListIterator dbFacet = DoubleListIterator( listFacets, TRUE );
   PTFacet  pFacet = (PTFacet)&dbFacet.current();
   for( ; int(dbFacet); pFacet = (PTFacet)&++dbFacet )
    {
      if( bFlShowBackFaces == FALSE )
       {
         TVector vecOutNorm = pFacet->GetNewelNormal( pUcs );
	 if( vecOutNorm.z < 0 ) continue;
       }


      DoubleListIterator dbAdges = DoubleListIterator( pFacet->listAgesItem, TRUE );
      PTAdgeItem  pAge = (PTAdgeItem)&dbAdges.current();
      for( ; int(dbAdges); pAge = (PTAdgeItem)&++dbAdges )
       {
         if( pAge->pAdge->bFlAtion == TRUE ) continue;
	 else pAge->pAdge->bFlAtion = TRUE;

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
	 POINT p[2];
	 p[0].x = tmpP[0].x;
	 p[0].y = tmpP[0].y;
	 p[1].x = tmpP[1].x;
	 p[1].y = tmpP[1].y;
         LPtoDP( hdc, p, 2 );

	 if( FindAdge( keyDraw, p, min(iSizeKey, iKeyIndex) ) == TRUE ) continue;

	 MoveTo( hdc, tmpP[0].x, tmpP[0].y );
         LineTo( hdc, tmpP[1].x, tmpP[1].y );
	 //proector( this, tmpP );

	 if( iKeyIndex < iSizeKey )
	   keyDraw[ iKeyIndex++ ] = TSpecAdge( p[0], p[1] );

	 if( iMinX > p[0].x ) iMinX = p[0].x;
	 if( iMinY > p[0].y ) iMinY = p[0].y;
	 if( iMaxX < p[0].x ) iMaxX = p[0].x;
	 if( iMaxY < p[0].y ) iMaxY = p[0].y;

	 if( iMinX > p[1].x ) iMinX = p[1].x;
	 if( iMinY > p[1].y ) iMinY = p[1].y;
	 if( iMaxX < p[1].x ) iMaxX = p[1].x;
	 if( iMaxY < p[1].y ) iMaxY = p[1].y;
       }
    }   
 }

#pragma option -v.
