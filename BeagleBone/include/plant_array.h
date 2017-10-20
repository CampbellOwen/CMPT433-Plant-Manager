#ifndef _PLANT_ARRAY_H_
#define _PLANT_ARRAY_H_

#include <include/plant_manager.h>

void PlantArray_Init( int initialSize );

void PlantArray_Destroy();

void PlantArray_Put( plant_t* p );

plant_t* PlantArray_Get( int index );

#endif
