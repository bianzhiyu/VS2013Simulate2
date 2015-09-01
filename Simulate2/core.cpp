/*
*Author	:	bianzhiyu@126.com
*IDE	:	VS2013
*Time	:	2015/08/27 , 13:36
*Head Files: Controlling files, Data structure files, Processing files
*	Controlling files: 
*		AbsoluteVariety.h   (not included in this version)
*	Date structure fiels:
*		mTime.h		:Custom Common Library
*		mQueue.h	:Custom Common Library
*		mTradeDetail.h	:	Input Date
*		mStockState.h
*		mETFState.h
*		mMotherFundState.h
*		mSonFundState.h
*		mObjectCategory.h
*		mOperationCategory.h
*		mOperation.h
*		mStructuredFundState.h
*		mPositionState.h
*		mAsset.h
*		mNStrategy.h
*		mStockStreamListener.h
*	Pocessing files:
*		mSimulateMethods.h
*/
#include <iostream>
#include <iomanip>
#include <string>

#include "mTime.h"
#include "mTradeDetail.h"
#include "mSonFundState.h"
#include "mMotherFundState.h"
#include "mAsset.h"
#include "mNStrategy.h"
#include "mSimulateMethods.h"

#define _Method_1
//0,1

void setInitialAsset(mAsset& ma)
{
	mTime InitTime;
	InitTime.transfer("20150826090000");
	ma.setTime(InitTime);

	mMotherFundState InitMF;
	/*mStockState::
	* void setAll(int ind, const mTime& ot, double p, double closingprice)
	* void setAll(int ind, const mTime& ot, double p, double closingprice,double openingprice)
	* void setAll(int ind, const mTime& ot, double p, double preclosingprice, double closingprice, double openingprice)
	*/
	InitMF.setAll(161812, InitTime, 0.865, 0.865);
	InitMF.setinvestPct(0.95);
	ma.setMotherFund(InitMF);

	mSonFundState InitPA;
	InitPA.setAll(150018, InitTime, 0.924, 0.924);
	ma.setPartA(InitPA);

	mSonFundState PB;
	PB.setAll(150019, InitTime, 0.986, 0.986);
	ma.setPartB(PB);

	mStockState Ind;
	Ind.setAll(399330, InitTime, 3504.36, 3492.09, 3504.36);
	ma.setTrackIndex(Ind);

	ma.setCashFlow(0,0,0,0,0,0,0,0);
	ma.setPositionA(50000,0);	
	ma.setPositionB(50000,0);
	ma.setPositionM(0,0,0,0);
}

void PrintResult(const VectorStrategy&StrategyStreamInstant,
				 const VectorStrategy&StrategyStreamHung,
				 const mAsset& InitAsset,
				 const mAsset& TerminalAsset
				 )
{
	using std::cout;
	using std::endl;
	using std::ofstream;

	ofstream outfile("Result.txt");

	outfile.setf(std::ios::fixed);			//set output format of double : fixed number
	outfile << std::setprecision(6);			//set precision : 6-digit : *.??????

	outfile << "InitialAsset:\n";
	InitAsset.showAsset(outfile);
	outfile << "\n----------------------------\n";;

	outfile << "TerminalAsset:\n";
	TerminalAsset.showAsset(outfile);
	outfile << "\n----------------------------\n";

	outfile << "StrategyStreamInstant.size() = " << StrategyStreamInstant.size() << "\n\n";	//test
	outfile << "StrategyStreamInstant:\n";
	for (auto i = StrategyStreamInstant.begin(); i != StrategyStreamInstant.end(); i++)
		outfile << (*i) << endl << endl;
	outfile << "----------------------------\n";

	outfile << "StrategyStreamHung.size() = " << StrategyStreamHung.size() << "\n\n";		//tests
	outfile << "StrategyStreamHung:\n";
	for (auto i = StrategyStreamHung.begin(); i != StrategyStreamHung.end(); i++)
		outfile << (*i) << endl << endl;
	outfile << "----------------------------\n";

	outfile.close();
}

int main(int argc,char *argv)
{
	VectorTradeDetail DataSFundA,DataSFundB,DataIndex;		//A:150085, B:150086, M:163111, Index:159902
	
	inputRawData(DataSFundA, "sz150018.csv");
	inputRawData(DataSFundB, "sz150019.csv");
	inputRawData(DataIndex, "sz399330.csv");

	//showVectorTradeDetailToCout(ETF);		//Test for input

	mAsset InitAsset;		//Initial state
	setInitialAsset(InitAsset);

	mAsset TerminalAsset;  //save AssetState after one-day action
	VectorStrategy StrategyStreamInstant,StrategyStreamHung;
		//StrategyStreamInstant:  performed instantly in open market
		//StrategyStreamHung:     performed just after closing market

	//update position of mAsset at 00:00:00  !!
#ifdef _Method_0
	SimulateOneDay(InitAsset, DataSFundA, DataSFundB, DataIndex, StrategyStreamInstant, StrategyStreamHung, TerminalAsset);
#else ifdef _Method_1
	SimulateOneDay2(InitAsset, DataSFundA, DataSFundB, DataIndex, StrategyStreamInstant, StrategyStreamHung, TerminalAsset);
#endif
	//update closing price of mAsset at closing market !!

	PrintResult(StrategyStreamInstant, StrategyStreamHung, InitAsset, TerminalAsset);
	
	system("pause");
	return 0;
	return 0;
}