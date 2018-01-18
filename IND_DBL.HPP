#if !defined(_INDEXED_DBLIST_)
#define _INDEXED_DBLIST_

#include <owl.h>
#include <dbllist.h>
//#include "indicat.hpp"

#define TYP_INDEXED_ITEM 127

_CLASSDEF(TIndicator)

_CLASSDEF(TIndexedDoubleList)
_CLASSDEF(TIndexedItem)


#undef _EXPORT2
#if defined(_INDDBLDLL) || defined(_ALLLIBDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_INDDBLDLL) || defined(_ALLLIBDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


typedef int (TIndexedItem::* PTCompareFunc)( RTIndexedItem rObjForCmp );

class _EXPORT2 TIndexedItem: public Object
 {
public:
  BOOL bFlAtion;

  TIndexedItem( int indAssign ) { index = indAssign; }

  //virtual Pvoid TopPtr() { return this; }

  int Index() const { return index; }
  void ChangeIndex( int indAssign ) { index = indAssign; }

  virtual classType isA() const { return TYP_INDEXED_ITEM; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& rObj ) const;
  virtual char *nameOf() const { return "IndexedItem"; }
  virtual void printOn( ostream& ) const {};


private:
  int index;
 };


class _EXPORT2 TIndexedDoubleList: public DoubleList
 {
public:
  TIndexedDoubleList(): DoubleList()
   {
     bFlChacheState = FALSE;
     pChacheArray = NULL;
     wSizeChache = 0;
   }

  virtual ~TIndexedDoubleList();

  int GetMaxIndex() const;

  PTIndexedItem operator[]( int ind );
  int GetFreeIndex() const;

  void SetIterator( int index, DoubleListIterator& rIter );
  void ResetActionFlag();


  void CreateChache( WORD wSize = 0 );//0 - auto detect
  void DestroyChache();
  void ChacheOnOff( BOOL bFl ) { bFlChacheState = bFl; }
  int GetChacheSize() const { return wSizeChache; }

  virtual void add( Object _FAR &toAdd );
  void addAtHead( Object _FAR &toAdd );
  void addAtTail( Object _FAR &toAdd );

  virtual void detach( Object _FAR &toDetach, DeleteType dt = NoDelete );
  void detachFromHead( Object _FAR &toDetach, DeleteType dt = NoDelete );
  void detachFromTail( Object _FAR &toDetach, DeleteType dt = NoDelete);

  virtual void flush( DeleteType dt = DefDelete );

  void destroyFromHead( Object _FAR &toDestroy );
  void destroyFromTail( Object _FAR &toDestroy );

  RTIndexedItem GetMaximum( PTCompareFunc );
  //void Sort( PTCompareFunc, PTIndicator );
  void Sort( int _export (*pCmpF)(const void *, const void *), PTIndicator pInd );

private:
  BOOL bFlChacheState;
  int wSizeChache;
  PTIndexedItem huge  *pChacheArray;

  void PutToChache( RTIndexedItem toAdd );
  void RemoveFromChache( int ind );
 };

#endif
