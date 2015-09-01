#include "mObjectCategory.h"

bool operator == (int i,const mObjectCategory&mm)
{
	return  i==mm.ObjectCategory;
}

bool operator != (int i,const mObjectCategory&mm)
{
	return  i!=mm.ObjectCategory;
}