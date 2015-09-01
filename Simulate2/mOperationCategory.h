#pragma once
#ifndef __mOperationCategory
#define __mOperationCategory

class mOperationCategory
{
public:
	static const int startcode=0;
	static const int endcode=15;

	static const int buyPartA=0;	//optnum : +
	static const int sellPartA=1;	//optnum : +
	static const int buyPartB=2;	//optnum : +
	static const int sellPartB=3;	//optnum : +
	///4-7 may be deserted
	static const int buyMotherFund=4;	//optnum : +
	static const int redeemMotherFund=5;	//optnum : +
	static const int splitMotherFund=6;	//optnum : +
	static const int combinePartAB=7;	//optnum : +

	static const int buyMotherFundInClosingMarket=8;
	static const int redeemMotherFundInClosingMarket=9;
	static const int buyMotherFundInOpenMarket=10;
	static const int redeemMotherFundInOpenMarket=11;
	static const int splitMotherFundInOpenMarket=12;
	static const int splitMotherFundInClosingMarket=13;
	static const int combinePartABInOpenMarket=14;
	static const int combinePartABInClosingMarket=15;
private:
	int OperationCategory;
public:
	mOperationCategory():OperationCategory(0) {}
	mOperationCategory(int d):OperationCategory(d) {}
	mOperationCategory(const mOperationCategory& mm) {OperationCategory=mm.OperationCategory;}
	operator int () const {return OperationCategory;}
	mOperationCategory& operator = (mOperationCategory i) {OperationCategory=i.OperationCategory; return *this;}
	mOperationCategory& operator = (int i) {OperationCategory=i; return *this;}
	bool operator == (int d) const {return (OperationCategory==d);}
	bool operator != (int d) const {return (OperationCategory!=d);}

	friend bool operator ==(int , const mOperationCategory&);
	friend bool operator !=(int , const mOperationCategory&);
};

bool operator ==(int , const mOperationCategory&);
bool operator !=(int , const mOperationCategory&);


#endif