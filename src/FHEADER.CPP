#pragma option -v-

#include "fheader.hpp"



char TData::c3DWVersion[ iLenVersion ] = "3DWorld 1.0";

TData tdZer = "";
PTData TData::ZERO = &tdZer;

TDataInit::~TDataInit()
 {
   if( d_ != NODATA ) delete &d_;
 }


unsigned long TFileHeader::CalcHeadCRC() const
 {
   LPSTR lpData = (LPSTR)&d_;
   unsigned long ulCRC = 0;

   for( int i = 0; i < GetDataSIze();
	ulCRC += (unsigned long)(*lpData++), i++
      );

   return ulCRC;
 }

BOOL TFileHeader::CheckCRC() const
 {
   return BOOL(CalcHeadCRC() == ulCRChead);
 }

int TFileHeader::Save( HFILE hf ) const
 {
   UINT res;
	
   if( _lwrite( hf, &d_, GetDataSIze() ) == HFILE_ERROR )
     return HFILE_ERROR;
   res = _lwrite( hf, &ulCRChead, sizeof(unsigned long) );

   return res;
 }

int TFileHeader::Read( HFILE hf )
 {
   UINT res;

   if( _lread( hf, &d_, GetDataSIze() ) == HFILE_ERROR )
     return HFILE_ERROR;

   res = _lread( hf, &ulCRChead, sizeof(unsigned long) );

   return res;
 }

int TFileHeader::GetDataSIze() const
 {
   return sizeof(TData);
 }

void TFileHeader::InitCRC()
 {
   ulCRChead = CalcHeadCRC();
 }

int TMediumHeader::GetDataSIze() const
 {
   return sizeof(TDataMedium);
 }

int TMDISave::GetDataSIze() const
 {
   return sizeof(TDataMDISave);
 }

TMDIChHeader::TMDIChHeader(
  RECT _FAR &rBounds,
  int iSpcX, int iSpcY,
  BOOL bFlVG,
  double dScale,
  double dShX, double dShY,
  double dH, double dV,
  TViewPoint rVP,
  BOOL bFlVA,
  WINDOWPLACEMENT _FAR rPlace,
  int iDivI,
  BOOL bAbFit,
  BOOL bFlPopup,
  RT3DPoint bp,
  BOOL bFlBPV
			  ):
  TDataInit( new TDataChild() ),
  TFileHeader(),
  d( RTDataChild(d_) )
 {
   d.bounds = rBounds;
   d.spcX = iSpcX;
   d.spcY = iSpcY;
   d.bFlVisyalGreed = bFlVG;
   d.scale = dScale;
   d.shiftX = dShX;
   d.shiftY = dShY;
   d.h = dH;
   d.v = dV;
   d.vpViewPoint = rVP;
   d.bFlVisyalAxis = bFlVA;
   d.place = rPlace;
   d.iDivIndex = iDivI;
   d.bAutoFit = bAbFit;
   d.bFlPopupAllViews = bFlPopup;
   d.globalBasePoint = bp;
   d.bFlIsBasePointVisible = bFlBPV;
 };


int TMDIChHeader::GetDataSIze() const
 {
   return sizeof(TDataChild);
 }

int TScenaHdr::GetDataSIze() const
 {
   return sizeof(TDataScena);
 }

int TSurfHdr::GetDataSIze() const
 {
   return sizeof(TDataSurf);
 }

#pragma option -v.
