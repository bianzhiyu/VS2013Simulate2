#include "mOperation.h"

mAsset mOperation::buyPartA(const mAsset &mSFS) const
{
	llint share = OperationAmoutLlint;		//+
	double pricea = mSFS.PartA.getPrice();			//+

	if (mSFS.CashFlow[0] - (double)share*pricea<0) return mSFS;

	mAsset tmp(mSFS);

	tmp.CashFlow[0] -= (double)share*pricea;
	tmp.PositionA[1] += share;

	return tmp;
}
mAsset mOperation::buyPartB(const mAsset &mSFS) const
{
	llint share = OperationAmoutLlint;
	double priceb = mSFS.PartB.getPrice();

	if (mSFS.CashFlow[0] - (double)share*priceb<0) return mSFS;

	mAsset tmp(mSFS);

	tmp.CashFlow[0] -= (double)share*priceb;
	tmp.PositionB[1] += share;

	return tmp;
}
mAsset mOperation::sellPartA(const mAsset& mSFS) const
{
	llint share = OperationAmoutLlint;
	double pricea = mSFS.PartA.getPrice();

	if (mSFS.PositionA[0] - share<0) return mSFS;

	mAsset tmp(mSFS);

	tmp.CashFlow[0] += (double)share*pricea;
	tmp.PositionA[0] -= share;

	return tmp;
}
mAsset mOperation::sellPartB(const mAsset& mSFS) const
{
	llint share = OperationAmoutLlint;
	double priceb = mSFS.PartB.getPrice();

	if (mSFS.PositionB[0] - share<0) return mSFS;

	mAsset tmp(mSFS);

	tmp.CashFlow[0] += (double)share*priceb;
	tmp.PositionB[0] -= share;

	return tmp;
}
///***
mAsset mOperation::buyMotherFund(const mAsset&mSFS)	const	//time check needed before being called
{
	double amount = OperationAmoutDouble;
	double price = mSFS.MotherFund.getClosingPrice();
	llint share = (long long int)(amount / price);
	share = share / 100 * 100;

	if (mSFS.CashFlow[0] - share*price<0) return mSFS;

	mAsset tmp(mSFS);

	tmp.CashFlow[0] -= share*price;
	tmp.PositionM[3] += share;

	return tmp;
}
mAsset mOperation::redeemMotherFund(const mAsset& mA, int Daysdelay) const	//time check needed before being called
{
	llint share = OperationAmoutLlint;
	double price = mA.MotherFund.getClosingPrice();

	if (mA.PositionM[0]<share)	return mA;

	mAsset tmp(mA);

	if (Daysdelay>7) Daysdelay = 7;
	if (Daysdelay<0) Daysdelay = 0;

	tmp.PositionM[0] -= share;
	tmp.CashFlow[Daysdelay] += (double)share*price;

	return tmp;
}
mAsset mOperation::splitMotherFund(const mAsset& mA)	const//time check needed before being called
{
	llint share = OperationAmoutLlint;
	if (share % 2 != 0) share--;

	if (mA.PositionM[0]<share) return mA;

	mAsset tmp(mA);

	tmp.PositionM[0] -= share;
	tmp.PositionA[1] += share / 2;
	tmp.PositionB[1] += share / 2;

	return tmp;
}
mAsset mOperation::combinePartAB(const mAsset& mA) const
{
	llint share = OperationAmoutLlint;

	if ((mA.PositionA[0]<share) || (mA.PositionB[0]<share))  return mA;

	mAsset tmp(mA);

	tmp.PositionA[0] -= share;
	tmp.PositionB[0] -= share;
	tmp.PositionM[1] += 2 * share;

	return tmp;
}
mAsset mOperation::ActOn(const mAsset& mSFS) const
{
	//StructuredFundStateCopy=(mStructuredFundState)mSFS;

	if (OperationCategory == mOperationCategory::buyPartA)  return buyPartA(mSFS);
	if (OperationCategory == mOperationCategory::buyPartB)  return buyPartB(mSFS);
	if (OperationCategory == mOperationCategory::sellPartA) return sellPartA(mSFS);
	if (OperationCategory == mOperationCategory::sellPartB) return sellPartB(mSFS);

	if (OperationCategory == mOperationCategory::buyMotherFund) return buyMotherFund(mSFS);
	if (OperationCategory == mOperationCategory::redeemMotherFund) return redeemMotherFund(mSFS);
	if (OperationCategory == mOperationCategory::splitMotherFund) return splitMotherFund(mSFS);
	if (OperationCategory == mOperationCategory::combinePartAB) return combinePartAB(mSFS);

	if (OperationCategory == mOperationCategory::buyMotherFundInClosingMarket) return buyMotherFundInClosingMarket(mSFS);
	if (OperationCategory == mOperationCategory::buyMotherFundInOpenMarket) return buyMotherFundInOpenMarket(mSFS);
	if (OperationCategory == mOperationCategory::redeemMotherFundInClosingMarket) return redeemMotherFundInClosingMarket(mSFS);
	if (OperationCategory == mOperationCategory::redeemMotherFundInOpenMarket) return redeemMotherFundInOpenMarket(mSFS);
	if (OperationCategory == mOperationCategory::splitMotherFundInOpenMarket) return splitMotherFundInOpenMarket(mSFS);
	if (OperationCategory == mOperationCategory::splitMotherFundInClosingMarket) return splitMotherFundInClosingMarket(mSFS);
	if (OperationCategory == mOperationCategory::combinePartABInOpenMarket) return combinePartABInOpenMarket(mSFS);
	if (OperationCategory == mOperationCategory::combinePartABInClosingMarket) return combinePartABInClosingMarket(mSFS);

	return mSFS;
}
///***
mAsset mOperation::buyMotherFundInOpenMarket(const mAsset&ma) const
{
	double amount = OperationAmoutDouble;
	double price = ma.MotherFund.getClosingPrice();
	llint share = (long long int)(amount / price);
	share = share / 100 * 100;

	mAsset tmp(ma);

	tmp.CashFlow[0] -= share*price;
	//tmp.PositionM[3]+=share;

	return tmp;
}
mAsset mOperation::buyMotherFundInClosingMarket(const mAsset& ma) const
{
	double amount = OperationAmoutDouble;
	double price = ma.MotherFund.getClosingPrice();
	llint share = (long long int)(amount / price);
	share = share / 100 * 100;

	mAsset tmp(ma);

	//tmp.CashFlow[0]-=share*price;
	tmp.PositionM[3] += share;

	return tmp;
}
mAsset mOperation::redeemMotherFundInOpenMarket(const mAsset&mA, int Daysdelay) const
{
	llint share = OperationAmoutLlint;
	double price = mA.MotherFund.getClosingPrice();

	mAsset tmp(mA);

	if (Daysdelay>7) Daysdelay = 7;
	if (Daysdelay<0) Daysdelay = 0;

	tmp.PositionM[0] -= share;
	//tmp.CashFlow[Daysdelay]+=(double)share*price;

	return tmp;
}
mAsset mOperation::redeemMotherFundInClosingMarket(const mAsset&mA, int Daysdelay) const
{
	llint share = OperationAmoutLlint;
	double price = mA.MotherFund.getClosingPrice();

	mAsset tmp(mA);

	if (Daysdelay>7) Daysdelay = 7;
	if (Daysdelay<0) Daysdelay = 0;

	//tmp.PositionM[0]-=share;
	tmp.CashFlow[Daysdelay] += (double)share*price;

	return tmp;
}
mAsset mOperation::splitMotherFundInOpenMarket(const mAsset&mA) const
{
	llint share = OperationAmoutLlint;
	if (share % 2 != 0) share--;

	mAsset tmp(mA);

	tmp.PositionM[0] -= share;
	//tmp.PositionA[1]+=share/2;
	//tmp.PositionB[1]+=share/2;

	return tmp;
}
mAsset mOperation::splitMotherFundInClosingMarket(const mAsset&mA) const
{
	llint share = OperationAmoutLlint;
	if (share % 2 != 0) share--;

	mAsset tmp(mA);

	//tmp.PositionM[0]-=share;
	tmp.PositionA[1] += share / 2;
	tmp.PositionB[1] += share / 2;

	return tmp;
}
mAsset mOperation::combinePartABInOpenMarket(const mAsset&mA) const
{
	llint share = OperationAmoutLlint;

	mAsset tmp(mA);

	tmp.PositionA[0] -= share;
	tmp.PositionB[0] -= share;
	//tmp.PositionM[1]+=2*share;

	return tmp;
}
mAsset mOperation::combinePartABInClosingMarket(const mAsset&mA) const
{
	llint share = OperationAmoutLlint;

	mAsset tmp(mA);

	//tmp.PositionA[0]-=share;
	//tmp.PositionB[0]-=share;
	tmp.PositionM[1] += 2 * share;

	return tmp;
}
///***
std::ostream& operator << (std::ostream& osm, const mOperation &ope)
{
	osm << "Time = " << ope.Time.toString(mTime::StandardFormat) << "  , Operation = ";
	if (ope.OperationCategory == mOperationCategory::buyPartA)
		osm << "buy PartA\nShare= " << ope.OperationAmoutLlint
		<< "\nPrice= " << ope.StructuredFundStateCopy.PartA.getPrice();

	if (ope.OperationCategory == mOperationCategory::buyPartB)
		osm << "buy PartB\nShare= " << ope.OperationAmoutLlint
		<< "\nPrice= " << ope.StructuredFundStateCopy.PartB.getPrice();

	if (ope.OperationCategory == mOperationCategory::sellPartA)
		osm << "sellPartA\nShare= " << ope.OperationAmoutLlint
		<< "\nPrice= " << ope.StructuredFundStateCopy.PartA.getPrice();

	if (ope.OperationCategory == mOperationCategory::sellPartB)
		osm << "sellPartB\nShare= " << ope.OperationAmoutLlint
		<< "\nPrice= " << ope.StructuredFundStateCopy.PartB.getPrice();

	//---------------------------------

	if (ope.OperationCategory == mOperationCategory::buyMotherFund)
		osm << "buyMotherFund\nCapitalAmount= " << ope.OperationAmoutDouble
		<< "\nEstimated Net Value=" << ope.StructuredFundStateCopy.MotherFund.getEstimatedNetValue();

	if (ope.OperationCategory == mOperationCategory::redeemMotherFund)
		osm << "redeemMotherFund\nShare(M)= " << ope.OperationAmoutLlint
		<< "\nEstimated Net Value=" << ope.StructuredFundStateCopy.MotherFund.getEstimatedNetValue();

	if (ope.OperationCategory == mOperationCategory::splitMotherFund)
		osm << "splitMotherFund\nShare of MotherFund= " << ope.OperationAmoutLlint
		<< "\n   ( Estimated Net Value=" << ope.StructuredFundStateCopy.MotherFund.getEstimatedNetValue() << "  ) ";

	if (ope.OperationCategory == mOperationCategory::combinePartAB)
		osm << "combinePartAB\nShare of A (or B)= " << ope.OperationAmoutLlint;

	//-------------------------------


	if (ope.OperationCategory == mOperationCategory::buyMotherFundInOpenMarket)
		osm << "buyMotherFundInOpenMarket\nAmount= " << ope.OperationAmoutDouble;

	if (ope.OperationCategory == mOperationCategory::buyMotherFundInClosingMarket)
		osm << "buyMotherFundInClosingMarket\nAmount= " << ope.OperationAmoutDouble;

	if (ope.OperationCategory == mOperationCategory::redeemMotherFundInOpenMarket)
		osm << "redeemMotherFundInOpenMarket\nShare of MotherFund= " << ope.OperationAmoutLlint;

	if (ope.OperationCategory == mOperationCategory::redeemMotherFundInClosingMarket)
		osm << "redeemMotherFundInClosingMarket\nShare of MotherFund= " << ope.OperationAmoutLlint;

	if (ope.OperationCategory == mOperationCategory::splitMotherFundInOpenMarket)
		osm << "splitMotherFundInOpenMarket\nShare of MotherFund= " << ope.OperationAmoutLlint;

	if (ope.OperationCategory == mOperationCategory::splitMotherFundInClosingMarket)
		osm << "splitMotherFundInClosingMarket\nShare of MotherFund= " << ope.OperationAmoutLlint;

	if (ope.OperationCategory == mOperationCategory::combinePartABInOpenMarket)
		osm << "combinePartABInOpenMarket\nShare of A (or B)= " << ope.OperationAmoutLlint;

	if (ope.OperationCategory == mOperationCategory::combinePartABInClosingMarket)
		osm << "combinePartABInClosingMarket\nShare of A (or B)= " << ope.OperationAmoutLlint;

	return osm;
}