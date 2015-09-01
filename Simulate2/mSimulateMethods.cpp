#include "mSimulateMethods.h"

long long minim(long long l1, long long l2)
{
	if (l1 < l2)	return l1;
	return l2;
}

/* This simulation function below only act in two ways:
* 1: premium arbitrage:
*		buy FoFs(MotherFund), sell PartA, PartB
* 2: discount arbitrage:
*		redeem (sell) FoFs, buy PartA, PartB
*  And in one day, only one kind of these two action will be performed (but can be performed more than once)
*/
void SimulateOneDay(const mAsset& InitialAsset,
	const VectorTradeDetail& SFAStream,
	const VectorTradeDetail& SFBStream,
	const VectorTradeDetail& IndexStream,
	VectorStrategy & StrategyStreamInstant,
	VectorStrategy & StrategyStreamHung,
	mAsset& TerminalAsset
	)
{
#ifdef DeBug
	cout << "Begin\n";
	for (auto i = SFAStream.cbegin(); i != SFAStream.cend(); i++)
		cout << (*i).getTime().toString(mTime::StandardFormat) << endl;
	system("pause");
#endif
	if ((SFAStream.size() == 0) || (SFBStream.size() == 0) || (IndexStream.size() == 0)) return;

	mAsset tmpAssetState(InitialAsset);

	VectorStrategy tmpStrategyStreamInstant;	//Strategy stream for today
	VectorStrategy tmpStrategyStreamHung;

	mTime openMarketTime, closeMarketTime;
	openMarketTime = SFAStream[0].Time;
	if (SFBStream[0].Time>openMarketTime) openMarketTime = SFBStream[0].Time;
	if (IndexStream[0].Time>openMarketTime) openMarketTime = IndexStream[0].Time;
	closeMarketTime = SFAStream[SFAStream.size() - 1].Time;
	if (SFBStream[SFBStream.size() - 1].Time<closeMarketTime) closeMarketTime = SFBStream[SFBStream.size() - 1].Time;
	if (IndexStream[IndexStream.size() - 1].Time<closeMarketTime) closeMarketTime = IndexStream[IndexStream.size() - 1].Time;

	mTime StartActionTime("20150826143000"), EndActionTime("20150826150000"); //set action time window

	StockStreamListener SSL;
	//Listener is the bridge between input port and inner datastructure
	// it receive data from the port, transform data form, and send data to inner datastructure
	SSL.setInitialIndexState(IndexStream[0].toStockState());
	SSL.setMFundState(InitialAsset.MotherFund);
	SSL.setInitialSFundAState(InitialAsset.PartA);
	SSL.setInitialSFundBState(InitialAsset.PartB);

	unsigned int pointA = 0, pointB = 0, pointI = 0;

	int TradeDir = TradeDirectionNsp::NoneAction;

	double PositiveThreshold = 0.05, NegativeThreshold = -0.05;

	for (mTime iTime = openMarketTime; !(iTime>closeMarketTime); iTime = iTime + 10)
	{
		while ((pointA<SFAStream.size() - 1) && (SFAStream[pointA + 1].Time<=iTime)) pointA++;
		while ((pointB<SFBStream.size() - 1) && (SFBStream[pointB + 1].Time<=iTime)) pointB++;
		while ((pointI<IndexStream.size() - 1) && (IndexStream[pointI + 1].Time<=iTime)) pointI++;

		//Listener Working : receive data, transform form
		SSL.setContempIndexState(IndexStream[pointI].toStockState());
		SSL.setContempSFundAState(SFAStream[pointA].toSonFundState());
		SSL.setContempSFundBState(SFBStream[pointB].toSonFundState());

		//Update Situation, the price A B in tmpAssetState influence calculation with Operation
		//Should separate information of A,B by comparing Index 
		tmpAssetState.setPartA(SSL.getContempSFundAState());
		tmpAssetState.setPartB(SSL.getContempSFundBState());
		tmpAssetState.estimateNetValueOfMotherFund(SSL.ContempIndexState.getPrice(), SSL.InitialIndexState.getPrice());

		if ((iTime<StartActionTime) || (iTime>EndActionTime)) continue;

		//Blow is the decision-maker
		if ((SSL.getInstantPremiumRate()>PositiveThreshold) && (TradeDir != TradeDirectionNsp::redeemMotherFund))
			//Premium Arbitrage is activated
			//buy FoFs(MotherFund), sell PartA, PartB
		{
			long long int share = minim(tmpAssetState.getPositionA(0), tmpAssetState.getPositionB(0));
			double returnrate = SSL.getInstantPremiumRate();
			//the way to calculate cost is needed
			//profit will be sure only after closing market,

			if (share>0)
			{
				TradeDir = TradeDirectionNsp::buyMotherFund;

				mOperation oprA, oprB, oprM, oprMH;
				oprA.setTime(iTime);
				oprA.setOperationCategory(mOperationCategory::sellPartA);
				oprA.setOperatedObjectCategory(mObjectCategory::PartA);
				oprA.setOperatedObjectIndex(tmpAssetState.getIndexA());
				oprA.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprA.setOperationAmoutLlint(share);

				mNStrategy ts(oprA, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprA.ActOn(tmpAssetState);

				oprB.setTime(iTime);
				oprB.setOperationCategory(mOperationCategory::sellPartB);
				oprB.setOperatedObjectCategory(mObjectCategory::PartB);
				oprB.setOperatedObjectIndex(tmpAssetState.getIndexB());
				oprB.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprB.setOperationAmoutLlint(share);

				ts.setAll(oprB, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprB.ActOn(tmpAssetState);

				oprM.setTime(iTime);
				oprM.setOperationCategory(mOperationCategory::buyMotherFundInOpenMarket);
				oprM.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprM.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprM.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprM.setOperationAmoutDouble(share*(SSL.getContempPriceA() + SSL.getContempPriceB()));

				ts.setAll(oprM, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprM.ActOn(tmpAssetState);

				oprMH.setTime(closeMarketTime);
				oprMH.setOperationCategory(mOperationCategory::buyMotherFundInClosingMarket);
				oprMH.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprMH.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprMH.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprMH.setOperationAmoutDouble(share*(SSL.getContempPriceA() + SSL.getContempPriceB()));

				ts.setAll(oprMH, returnrate, 0, 0);
				tmpStrategyStreamHung.push_back(ts);
			}
		}
		if ((SSL.getInstantPremiumRate()<NegativeThreshold) && (TradeDir != TradeDirectionNsp::buyMotherFund))
			//Discount Arbitrage is activated
			//redeem (sell) FoFs, buy PartA, PartB
		{
			long long int share = minim(tmpAssetState.getPositionM(0) / 2,
				(long long)(tmpAssetState.getCashFlow(0) / (SSL.getContempPriceA() + SSL.getContempPriceB()) - 1)
				);
			if (share<0)  share = 0;

			if (share>0)
			{
#ifdef DeBug
				cout << "PointA = " << pointA << endl;
#endif
				TradeDir = TradeDirectionNsp::redeemMotherFund;

				double returnrate = -SSL.getInstantPremiumRate();
				//the way to calculate cost is needed
				//profit can be sure only after closing market,

				mOperation oprA, oprB, oprM, oprMH;
				oprA.setTime(iTime);
				oprA.setOperationCategory(mOperationCategory::buyPartA);
				oprA.setOperatedObjectCategory(mObjectCategory::PartA);
				oprA.setOperatedObjectIndex(tmpAssetState.getIndexA());
				oprA.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprA.setOperationAmoutLlint(share);

				mNStrategy ts(oprA, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprA.ActOn(tmpAssetState);

				oprB.setTime(iTime);
				oprB.setOperationCategory(mOperationCategory::buyPartB);
				oprB.setOperatedObjectCategory(mObjectCategory::PartB);
				oprB.setOperatedObjectIndex(tmpAssetState.getIndexB());
				oprB.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprB.setOperationAmoutLlint(share);

				ts.setAll(oprB, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprB.ActOn(tmpAssetState);

				oprM.setTime(iTime);
				oprM.setOperationCategory(mOperationCategory::redeemMotherFundInOpenMarket);
				oprM.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprM.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprM.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprM.setOperationAmoutLlint(2 * share);

				ts.setAll(oprM, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprM.ActOn(tmpAssetState);

				oprMH.setTime(closeMarketTime);
				oprMH.setOperationCategory(mOperationCategory::redeemMotherFundInClosingMarket);
				oprMH.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprMH.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprMH.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprMH.setOperationAmoutLlint(2 * share);

				ts.setAll(oprMH, returnrate, 0, 0);
				tmpStrategyStreamHung.push_back(ts);
			}
		}
		//End of Desicion maker
	}

	/*need to get closing price of motherfund, in order to :
	* 1: calculate all profit, (cost,) real return rate of tmpStrategyStreamHung , here -- before it is poped into tmpStrategyStreamHung !!
	* 2: update the state of fund (closing price of MotherFund)
	* 3: perform all actions in tmpStrategy, whose calculation depends on new closing price  !!
	*/

	//update Closing Price of Mother Fund by hand....
	//tmpAssetState.updateClosingPriceOfMotherFund(0.7469);
	tmpAssetState.updateClosingPriceOfMotherFund(
		tmpAssetState.estimateNetValueOfMotherFund(SSL.getContempIndexPrice(), SSL.getInitialIndexPrice()
		)
		);

	for (auto iss = tmpStrategyStreamInstant.begin(); iss != tmpStrategyStreamInstant.end(); iss++)
	{
		StrategyStreamInstant.push_back(*iss);
	}

	for (auto iss = tmpStrategyStreamHung.begin(); iss != tmpStrategyStreamHung.end(); iss++)
	{
		tmpAssetState = (*iss).ActOn(tmpAssetState);
		StrategyStreamHung.push_back(*iss);
	}

	TerminalAsset = tmpAssetState;
}

/*Compared with former one,
* the Method below taking volume limit into consideration
* In order to record turnover, define a queue class
*/

void SimulateOneDay2(const mAsset& InitialAsset,
	const VectorTradeDetail& SFAStream,
	const VectorTradeDetail& SFBStream,
	const VectorTradeDetail& IndexStream,
	VectorStrategy & StrategyStreamInstant,
	VectorStrategy & StrategyStreamHung,
	mAsset& TerminalAsset
	)
{
#ifdef DeBug
	cout << "Begin\n";
	for (auto i = SFAStream.cbegin(); i != SFAStream.cend(); i++)
		cout << (*i).getTime().toString(mTime::StandardFormat) << endl;
	system("pause");
#endif
	if ((SFAStream.size() == 0) || (SFBStream.size() == 0) || (IndexStream.size() == 0)) return;

	mAsset tmpAssetState(InitialAsset);

	//Strategy stream for today
	VectorStrategy tmpStrategyStreamInstant;	
	VectorStrategy tmpStrategyStreamHung;

	mTime openMarketTime, closeMarketTime;
	openMarketTime = SFAStream[0].Time;
	if (SFBStream[0].Time>openMarketTime) openMarketTime = SFBStream[0].Time;
	if (IndexStream[0].Time>openMarketTime) openMarketTime = IndexStream[0].Time;
	closeMarketTime = SFAStream[SFAStream.size() - 1].Time;
	if (SFBStream[SFBStream.size() - 1].Time<closeMarketTime) closeMarketTime = SFBStream[SFBStream.size() - 1].Time;
	if (IndexStream[IndexStream.size() - 1].Time<closeMarketTime) closeMarketTime = IndexStream[IndexStream.size() - 1].Time;

	mTime StartActionTime("20150826143000"), EndActionTime("20150826150000"); //set action time window
	StartActionTime=SFAStream[0].getTime();
	StartActionTime.setHour(14);
	StartActionTime.setMinute(30);
	StartActionTime.setSecond(0);
	EndActionTime = StartActionTime;
	EndActionTime.setHour(15);
	EndActionTime.setMinute(0);

	StockStreamListener SSL;
	//Listener is the bridge between input port and inner datastructure
	// it receive data from the port, transform data form, and send data to inner datastructure
	SSL.setInitialIndexState(IndexStream[0].toStockState());
	SSL.setMFundState(InitialAsset.MotherFund);
	SSL.setInitialSFundAState(InitialAsset.PartA);
	SSL.setInitialSFundBState(InitialAsset.PartB);

	unsigned int pointA = 0, pointB = 0, pointI = 0;

	int TradeDir = TradeDirectionNsp::NoneAction;

	double PositiveThreshold = 0.05, NegativeThreshold = -0.05;

	mTradeDetailQueue CacheA, CacheB;	//To get latest turnover for a while
	const int CacheTimeSpan = 60 * 5;

	for (mTime iTime = openMarketTime; !(iTime>closeMarketTime); iTime = iTime + 10)
	{
		while ((pointA<SFAStream.size() - 1) && (SFAStream[pointA + 1].Time<=iTime)) pointA++;
		while ((pointB<SFBStream.size() - 1) && (SFBStream[pointB + 1].Time<=iTime)) pointB++;
		while ((pointI<IndexStream.size() - 1) && (IndexStream[pointI + 1].Time<=iTime)) pointI++;
#ifdef DeBug
		cout << iTime.toString(mTime::StandardFormat) << endl;
		cout << "PointA=" << pointA << endl;
		
#endif

		//Listener Working : receive data, transform form
		SSL.setContempIndexState(IndexStream[pointI].toStockState());
		SSL.setContempSFundAState(SFAStream[pointA].toSonFundState());
		SSL.setContempSFundBState(SFBStream[pointB].toSonFundState());

		//Update Situation, the price A B in tmpAssetState influence calculation with Operation
		//Should separate information of A,B by comparing Index 
		tmpAssetState.setPartA(SSL.getContempSFundAState());
		tmpAssetState.setPartB(SSL.getContempSFundBState());
		tmpAssetState.estimateNetValueOfMotherFund(SSL.ContempIndexState.getPrice(), SSL.InitialIndexState.getPrice());

		//update CacheA,CacheB to get latest turnover%
		//Should separate information of A,B by comparing Index 
		CacheA.pushin(SFAStream[pointA]);
		CacheB.pushin(SFBStream[pointB]);
		CacheA.filter(iTime - CacheTimeSpan, iTime);
		CacheB.filter(iTime - CacheTimeSpan, iTime);
#ifdef DeBug
		cout << "CacheA: Size= " << CacheA.getSize() << endl;
		cout << "CacheA.Head,Time= " << CacheA.getHead().getTime().toString(mTime::StandardFormat) << endl;
		cout << "CacheA.Tail,Time= " << CacheA.getTail().getTime().toString(mTime::StandardFormat) << endl;
		cout << "CacheA.getMaxTotalTurnover()=" << CacheA.getMaxTotalTurnover() << endl;
		cout << "CacheA.getMinTotalTurnover()=" << CacheA.getMinTotalTurnover() << endl;
		//cout << CacheA << endl;
		system("pause");
#endif

		//Suppose 10% others Trading Volume is the max Volume limit
		int MaxShareLimitA = (CacheA.getMaxTotalTurnover() - CacheA.getMinTotalTurnover()) / 10 + 10;
		int MaxShareLimitB = (CacheB.getMaxTotalTurnover() - CacheB.getMinTotalTurnover()) / 10 + 10;

		if ((iTime<StartActionTime) || (iTime>EndActionTime)) continue;

		//Blow is the decision-maker
		if ((SSL.getInstantPremiumRate()>PositiveThreshold) && (TradeDir != TradeDirectionNsp::redeemMotherFund))
			//Premium Arbitrage is activated
			//buy FoFs(MotherFund), sell PartA, PartB
		{
			long long int share = minim(tmpAssetState.getPositionA(0), tmpAssetState.getPositionB(0));
			double returnrate = SSL.getInstantPremiumRate();
			//the way to calculate cost is needed
			//profit will be sure only after closing market,

			//Volume Control
			if (share > MaxShareLimitA) share = MaxShareLimitA;
			if (share > MaxShareLimitB) share = MaxShareLimitB;

			share = (share / 100) * 100;

			if (share>0)
			{
#ifdef DeBug
		//		cout << "PointA = " << pointA << endl;
#endif
				TradeDir = TradeDirectionNsp::buyMotherFund;

				mOperation oprA, oprB, oprM, oprMH;
				oprA.setTime(iTime);
				oprA.setOperationCategory(mOperationCategory::sellPartA);
				oprA.setOperatedObjectCategory(mObjectCategory::PartA);
				oprA.setOperatedObjectIndex(tmpAssetState.getIndexA());
				oprA.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprA.setOperationAmoutLlint(share);

				mNStrategy ts(oprA, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprA.ActOn(tmpAssetState);

				oprB.setTime(iTime);
				oprB.setOperationCategory(mOperationCategory::sellPartB);
				oprB.setOperatedObjectCategory(mObjectCategory::PartB);
				oprB.setOperatedObjectIndex(tmpAssetState.getIndexB());
				oprB.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprB.setOperationAmoutLlint(share);

				ts.setAll(oprB, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprB.ActOn(tmpAssetState);

				oprM.setTime(iTime);
				oprM.setOperationCategory(mOperationCategory::buyMotherFundInOpenMarket);
				oprM.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprM.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprM.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprM.setOperationAmoutDouble(share*(SSL.getContempPriceA() + SSL.getContempPriceB()));

				ts.setAll(oprM, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprM.ActOn(tmpAssetState);

				oprMH.setTime(closeMarketTime);
				oprMH.setOperationCategory(mOperationCategory::buyMotherFundInClosingMarket);
				oprMH.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprMH.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprMH.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprMH.setOperationAmoutDouble(share*(SSL.getContempPriceA() + SSL.getContempPriceB()));

				ts.setAll(oprMH, returnrate, 0, 0);
				tmpStrategyStreamHung.push_back(ts);
			}
		}
		if ((SSL.getInstantPremiumRate()<NegativeThreshold) && (TradeDir != TradeDirectionNsp::buyMotherFund))
			//Discount Arbitrage is activated
			//redeem (sell) FoFs, buy PartA, PartB
		{
			long long int share = minim(tmpAssetState.getPositionM(0) / 2,
				(long long)(tmpAssetState.getCashFlow(0) / (SSL.getContempPriceA() + SSL.getContempPriceB()) - 1)
				);
			if (share<0)  share = 0;

			//Volume Control
			if (share > MaxShareLimitA) share = MaxShareLimitA;
			if (share > MaxShareLimitB) share = MaxShareLimitB;

			share = (share / 100) * 100;

			if (share>0)
			{
				TradeDir = TradeDirectionNsp::redeemMotherFund;

				double returnrate = -SSL.getInstantPremiumRate();
				//the way to calculate cost is needed
				//profit can be sure only after closing market,

				mOperation oprA, oprB, oprM, oprMH;
				oprA.setTime(iTime);
				oprA.setOperationCategory(mOperationCategory::buyPartA);
				oprA.setOperatedObjectCategory(mObjectCategory::PartA);
				oprA.setOperatedObjectIndex(tmpAssetState.getIndexA());
				oprA.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprA.setOperationAmoutLlint(share);

				mNStrategy ts(oprA, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprA.ActOn(tmpAssetState);

				oprB.setTime(iTime);
				oprB.setOperationCategory(mOperationCategory::buyPartB);
				oprB.setOperatedObjectCategory(mObjectCategory::PartB);
				oprB.setOperatedObjectIndex(tmpAssetState.getIndexB());
				oprB.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprB.setOperationAmoutLlint(share);

				ts.setAll(oprB, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprB.ActOn(tmpAssetState);

				oprM.setTime(iTime);
				oprM.setOperationCategory(mOperationCategory::redeemMotherFundInOpenMarket);
				oprM.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprM.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprM.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprM.setOperationAmoutLlint(2 * share);

				ts.setAll(oprM, returnrate, 0, 0);
				tmpStrategyStreamInstant.push_back(ts);
				tmpAssetState = oprM.ActOn(tmpAssetState);

				oprMH.setTime(closeMarketTime);
				oprMH.setOperationCategory(mOperationCategory::redeemMotherFundInClosingMarket);
				oprMH.setOperatedObjectCategory(mObjectCategory::MotherFund);
				oprMH.setOperatedObjectIndex(tmpAssetState.getIndexM());
				oprMH.setStructuredFundStateCopy((mStructuredFundState)tmpAssetState);
				oprMH.setOperationAmoutLlint(2 * share);

				ts.setAll(oprMH, returnrate, 0, 0);
				tmpStrategyStreamHung.push_back(ts);
			}
		}
		//End of Desicion maker
	}

	/*need to get closing price of motherfund, in order to :
	* 1: calculate all profit, (cost,) real return rate of tmpStrategyStreamHung , here -- before it is poped into tmpStrategyStreamHung !!
	* 2: update the state of fund (closing price of MotherFund)
	* 3: perform all actions in tmpStrategy, whose calculation depends on new closing price  !!
	*/

	//update Closing Price of Mother Fund by hand....
	//tmpAssetState.updateClosingPriceOfMotherFund(0.7469);
	tmpAssetState.updateClosingPriceOfMotherFund(
		tmpAssetState.estimateNetValueOfMotherFund(SSL.getContempIndexPrice(), SSL.getInitialIndexPrice()
		)
		);
	//tmpAssetState.updateClosingPriceOfMotherFund();

	for (auto iss = tmpStrategyStreamInstant.begin(); iss != tmpStrategyStreamInstant.end(); iss++)
		StrategyStreamInstant.push_back(*iss);

	for (auto iss = tmpStrategyStreamHung.begin(); iss != tmpStrategyStreamHung.end(); iss++)
	{
		tmpAssetState = (*iss).ActOn(tmpAssetState);
		StrategyStreamHung.push_back(*iss);
	}

	TerminalAsset = tmpAssetState;
}
