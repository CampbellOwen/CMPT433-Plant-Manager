#include <include/plant_manager.h>
#include <include/plant_array.h>

#define PLANT_MANAGER_DEFAULT_SIZE 64

void PlantManager_Init( void )
{
	PlantArray_Init( PLANT_MANAGER_DEFAULT_SIZE );
}

void PlantManager_Register( struct sockaddr_in* addr, uint64_t id )
{
	plant_t newPlant = {
		id,
		addr,
		ALIVE
	};
	
	PlantArray_Put( &newPlant );

}
