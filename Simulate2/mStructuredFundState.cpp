#include "mStructuredFundState.h"

double mStructuredFundState::estimatePremiumRate()
{
	double env=MotherFund.estimateNetValue(TrackIndex.getPrice(),TrackIndex.getClosingPrice());
	return (PartA.getPrice()+PartB.getPrice()-2*env)/(2*env);
}

void mStructuredFundState::updateAllTime(const mTime& mt)
{
	MotherFund.setTime(mt);
	PartA.setTime(mt);
	PartB.setTime(mt);
	TrackIndex.setTime(mt);
	setTime(mt);
}