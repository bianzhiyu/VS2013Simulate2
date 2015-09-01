#include "mNStrategy.h"

std::ostream& operator << (std::ostream& osm, const mNStrategy & NS)
{
	osm<<(mOperation)(NS)<<"\nReturnRate= "<<NS.getReturnRate();
	return osm;
}