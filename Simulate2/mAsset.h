#pragma once
#ifndef __MASSET
#define __MASSET

#include "mTime.h"
#include "mStructuredFundState.h"
#include "mPositionState.h"

typedef long long int llint;

/*
* aim to describe one asset, inheritating: public mStructuredFundState, public mPositionState
*/
class mAsset : public mStructuredFundState, public mPositionState
{
public:
	mAsset();
	mAsset(const mAsset&);
	mAsset& operator =(const mAsset&);

	void updatePositionForNewDay();  //Only update Positions

	void showAsset(std::ostream& osm=std::cout) const;

	double getTotalCapital() const;
	
	double estimateNetValueOfMotherFund(double ContempIndexPrice,double ClosingIndexPrice)
	{		return MotherFund.estimateNetValue(ContempIndexPrice,ClosingIndexPrice);	}

	double getEstimatedNetValueOfMotherFund() const
	{		return MotherFund.getEstimatedNetValue();	}

};

#endif