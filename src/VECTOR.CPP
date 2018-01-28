#pragma option -v-

#include "vector.hpp"
#include <stdlib.h>



TVector Normalyze( RTVector v )
 {
   return v / (!v);
 }

TVector RndVector()
 {
   TVector v = TVector( rand() - 0.5*RAND_MAX,
			rand() - 0.5*RAND_MAX,
			rand() - 0.5*RAND_MAX
                      );

   return Normalyze( v );
 }

#pragma option -v.
