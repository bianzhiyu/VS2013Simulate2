#include "mStockStreamListener.h"

double StockStreamListener::getInstantPremiumRate()
{
	double EstimatedNAVM=MFundState.estimateNetValue(ContempIndexState.getPrice(),InitialIndexState.getPrice());
	return (ContempSFundAState.Price+ContempSFundBState.Price-2*EstimatedNAVM)/(2*EstimatedNAVM);
}