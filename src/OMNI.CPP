//#pragma option -v-

#include "surface.hpp"
#include "scena.hpp"

extern HINSTANCE hInstRC;

TOmni::TOmni( PTOmniInit pInit ):
  TSurface( pInit ),
  TInitShape( TOmni::InitShape )
 {
   SetObol( FALSE );

   if( pInit )
    {
      rgbColor = pInit->rgbColor;
      mult =     pInit->mult;
      bFlOn =    pInit->bFlOn;      
    }
   else
    {
      _fsetmem( &rgbColor, sizeof(rgbColor), 0 );
      mult =     1;
      bFlOn =    TRUE;      
    }

   bFlCastShadows = TRUE;
 }

TOmni::TOmni( int ind, PTOmniInit pInit ):
  TSurface( ind, pInit ),
  TInitShape( TOmni::InitShape )
 {
   SetObol( FALSE );

   if( pInit )
    {
      rgbColor = pInit->rgbColor;
      mult =     pInit->mult;
      bFlOn =    pInit->bFlOn;      
    }
   else
    {
      _fsetmem( &rgbColor, sizeof(rgbColor), 0 );
      mult =     1;
      bFlOn =    TRUE;      
    }

   bFlCastShadows = TRUE;
 }


void TOmni::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   //PTOmni pOmni = (PTOmni)pSurface;
   PTOmniInit pInit = PTOmniInit(pFD);

   if( pFD )
    {
      /*pOmni->rgbColor = pInit->rgbColor;
      pOmni->mult =     pInit->mult;
      pOmni->bFlOn =    pInit->bFlOn;*/
      pSurface->listVertex.add( *new TVertex(1, pInit->pt) );
    }
   else
    {
      /*_fsetmem( &pOmni->rgbColor, sizeof(pOmni->rgbColor), 0 );
      pOmni->mult =     1;
      pOmni->bFlOn =    TRUE;*/
      pSurface->listVertex.add( *new TVertex(1, 0, 0, 0) );
    }
 }

static BYTE RGBMult( BYTE b, double mult )
 {
   int res = double(b) * mult;

   return BYTE((res < 0) ? 0:((res > 255) ? 255:res));
 }

RGBQUAD TOmni::GetReductedColor() const
 {
   RGBQUAD rgbTmp; 

   rgbTmp.rgbRed = RGBMult( rgbColor.rgbRed, mult );
   rgbTmp.rgbGreen = RGBMult( rgbColor.rgbGreen, mult );
   rgbTmp.rgbBlue = RGBMult( rgbColor.rgbBlue, mult );

   return rgbTmp;
 }


TOmniObol::TOmniObol( PTOmni pOmni ):  
  TInitShape( NULL ),
  TSurface( *pOmni, NULL ),
  TOmni( *pOmni, NULL ),
  TObol( (PTSurface)NULL )
 {
 }

TOmniObol::TOmniObol( int ind, PTOmni pOmni ):
  TInitShape( NULL ),
  TSurface( *pOmni, NULL ),
  TOmni( *pOmni, NULL ),
  TObol( ind, NULL )
 {
 }

/*void TOmniObol::InitShape( PTSurface pSurface, PTFaceData pFD )
 {
   PTOmniObol pObl = (PTOmniObol)pSurface;
   pObl->bFlAbsTransf = FALSE;
   PTOmni pOmni = (PTOmni)((PTExtFaceData)pFD)->pSf;

   pObl->rgbColor = pOmni->rgbColor;
   pObl->mult =     pOmni->mult;
   pObl->bFlOn =    pOmni->bFlOn;
   pObl->listVertex.add( *new TVertex(1, *(pOmni->listVertex[1])) );
 }*/

PTObol TOmni::CreateObol() const
 {
   PTOmniObol pO = new TOmniObol( (PTOmni)this );
   if( pO ) pO->GetGeomCenter();

   return pO;
 }

struct TOmniSave
 {
   RGBQUAD rgbColor;
   double mult;
   BOOL bFlOn;
   BOOL bFlCastShadows;

   TOmniSave() {};
   TOmniSave( RGBQUAD& rRGB, double mult_, BOOL bFlOn_, BOOL bFlCastShadows_ )
    {
      rgbColor = rRGB; mult = mult_; bFlOn = bFlOn_;
      bFlCastShadows = bFlCastShadows_;
    }
 };

//-1 - internal
  //-2 - memory
  //-3 - hard
  //-4 - CRC   
  //-5 - Cancel

int TOmni::Save( HFILE hf, PTIndicator pInd )
 {
   int res = TSurface::Save( hf, pInd );
   if( res < 0 ) return res;

   TOmniSave omSave( rgbColor, mult, bFlOn, bFlCastShadows );
   UINT l;
   if( (l = _lwrite(hf, &omSave, sizeof(TOmniSave))) == HFILE_ERROR ||
       l != sizeof(TOmniSave)
     ) return -3;

   if( pInd && pInd->WorkMessageAdd( sizeof(TOmniSave) ) == TRUE ) return -5;

   return 0;
 }

int TOmni::Load( HFILE hf, PTIndicator pInd  )
 {
   int res = TSurface::Load( hf, pInd );
   if( res < 0 ) return res;

   TOmniSave omSave;
   UINT l;
   if( (l = _lread(hf, &omSave, sizeof(TOmniSave))) == HFILE_ERROR ||
       l != sizeof(TOmniSave)
     ) return -3;

   if( pInd && pInd->WorkMessageAdd( sizeof(TOmniSave) ) == TRUE ) return -5;

   rgbColor = omSave.rgbColor;
   mult = omSave.mult;
   bFlOn = omSave.bFlOn;
   bFlCastShadows = omSave.bFlCastShadows;

   r = float(rgbColor.rgbRed) / 255.0;
   g = float(rgbColor.rgbGreen) / 255.0;
   b = float(rgbColor.rgbBlue) / 255.0;

   return 0;
 }

void TOmni::Draw( BOOL bFlShowBackFaces, PTScena pSc ) const
 {
   PTUCS pUcs = pSc->GetUcs();
		     
   T3DPoint  tmpP = T3DPoint( (RT3DPoint)RTVertex(listVertex.peekAtHead()) );
   pUcs->RecalcUserToBase( &tmpP, 1 );

   //const int iLength = 5.0 / ((pUcs->GetScale() < 1 && pUcs->GetScale()) ? pUcs->GetScale():1);
   const int iLength = 5.0;
   //const int iLengthSec = sqrt( 2 * iLength * iLength );

   T3DPoint  line[2];

   line[0] = line[1] = tmpP;
   line[0].x -= iLength;  line[1].x += iLength;
   pSc->CallProector( line );

   line[0] = line[1] = tmpP;
   line[0].y -= iLength;  line[1].y += iLength;
   pSc->CallProector( line );

   line[0] = line[1] = tmpP;
   line[0].x -= iLength;  line[1].x += iLength;
   line[0].y -= iLength;  line[1].y += iLength;
   pSc->CallProector( line );

   line[0] = line[1] = tmpP;
   line[0].x += iLength;  line[1].x -= iLength;
   line[0].y -= iLength;  line[1].y += iLength;
   pSc->CallProector( line );
 }

void TOmni::DrawSpecial( BOOL bFlShowBackFaces, PTScena pSc )
 {
   Draw( bFlShowBackFaces, pSc );
 }

void TOmni::Place( RT3DPoint rPt )
 {
   (RT3DPoint)RTVertex(listVertex.peekAtHead()) = rPt;
 }

