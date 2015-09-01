#include "mOperationCategory.h"

bool operator ==(int i, const mOperationCategory&mm)
{
	return i==mm.OperationCategory;
}
bool operator !=(int i, const mOperationCategory&mm)
{
	return i!=mm.OperationCategory;
}