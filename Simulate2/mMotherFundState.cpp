#include "mMotherFundState.h"

double mMotherFundState::estimateNetValue(double PriceT,double Price0) 
{
	if (abs(Price0)<1e-14)
	{
		EstimatedNetValue=0;
		return 0;
	}
	EstimatedNetValue=getClosingNetValue()*investPct*PriceT/Price0+getClosingNetValue()*(1-investPct) ;
	return EstimatedNetValue;
}