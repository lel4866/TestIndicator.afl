////////////////////////////////////////////////////
// Functions.cpp
////////////////////////////////////////////////////

#include "pch.h" // must be first

#include <iostream>
#include <fstream>
#include <cmath>
#include "Plugin.h"

#undef LOG
int SkipEmptyValues(int nSize, float* Src, float* Dst);

////////////////////////////////////////////////////
// Each AFL function has the following prototype:
// AmiVar FunctionName( int NumArgs, AmiVar *ArgsTable )
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// PeakValley( array, switch_pct, neutral_pct )
// this function returns an array with values -1, 0, or 1 to indicate whether you're in a
// downtrend, neutral, or uptrend. 
// it does this by keeping track of the peaks and valleys of array. The peaks and valleys
// can only be found "after the fact". So, a peak is defined when the value in the array is
// x% less than the prior high value, and a valley is found when the value in the array is
// x% more than the prior low value. A neutral region (where the returned value is 0) is found
// when the value in the array is y% less than the prior high value or y% greater than the 
// prior low value, where y% is strictly less than x%
////////////////////////////////////////////////////

AmiVar LEL_PeakValley(int NumArgs, AmiVar* ArgsTable) {
#ifdef LOG
	static int call_num = 0; // don't need logging right now
	static std::ofstream log("C:/Users/lel48/amilog.txt");
#endif

	AmiVar result = gSite.AllocArrayResult();
	int nSize = gSite.GetArraySize();
	float* SrcArray = ArgsTable[0].array;
	float pct_param = (float)ArgsTable[1].val * 0.01f;
	float neutral_pct_param = (float)ArgsTable[2].val * 0.01f;

	int iStart = SkipEmptyValues(nSize, SrcArray, result.array);
#ifdef LOG
	if (call_num < 2) {
		log << "std::endl << nSize=" << nSize << std::endl;
		log << "pct_param=" << pct_param << std::endl;
		log << "iStart=" << iStart << std::endl;
		log << std::endl;
	}
#endif
	if (iStart >= nSize)
		return result;
#ifdef LOG
	if (call_num < 2) 
		log << "line 111" << std::endl;	
#endif
	int state = 0;
	float prev_value = SrcArray[iStart];
	float prev_extreme = prev_value;
	float cur_value, pct_change;
	for (int i=iStart+1; i< nSize; i++) {
		cur_value = SrcArray[i];
		if (cur_value == EMPTY_VAL) {
			result.array[i] = (float)state;
			continue;
		}

		switch (state) {
		case 0: // neutral state
			pct_change = cur_value / prev_extreme - 1.0f; // prev_extreme is SrcArray[iStart]
#ifdef LOG
			if (call_num < 2)
				log << "line 129 case 0: " << "pct_change=" << pct_change << std::endl;
#endif
			if (std::abs(pct_change) > pct_param)
				state = (pct_change > 0.0f) ? 1 : -1;
			result.array[i] = (float)state;
#ifdef LOG
			if (call_num < 2)
				log << "line 136 case 0: " << "state=" << state << std::endl;
#endif
			break;

		case -1: // we are in down trend (will check if it's broken)
			// prev_extreme is lowest value so far in this down trend
#ifdef LOG
			if (call_num < 2)
				log << "line 144 case -1: " << "prev_extreme=" << prev_extreme << " cur_value=" << cur_value << std::endl;
#endif
			if (cur_value <= prev_extreme) {
				// we have a new lowest value in this down trend
				prev_extreme = cur_value;
				result.array[i] = (float)state;
				break;
			}

			pct_change = cur_value / prev_extreme - 1.0f; // cur_value > prev_extreme, so pct_change is positive
#ifdef LOG
			if (call_num < 2)
				log << "line 156 case 0: " << "pct_change=" << pct_change << std::endl;
#endif
			if (pct_change < pct_param) {
				// value is higher than previous lowest value, but not high enough to trigger state change
				result.array[i] = (float)state;
				break;
			}

			// value is higher than previous lowest value...high enough to trigger state change to up trend
			state = 1;
			result.array[i] = (float)state;
#ifdef LOG
			if (call_num < 2)
				log << "line 169 case -1: " << "state=" << state << std::endl;
#endif
			break;

		case 1: // we are in uptrend (will check if it's broken)
			// prev_extreme is highest value so far in this up trend
#ifdef LOG
			if (call_num < 2)
				log << "line 177 case 1: " << "prev_extreme=" << prev_extreme << " cur_value=" << cur_value << std::endl;
#endif
			if (cur_value >= prev_extreme) {
				// we have a new highest value in this up trend
				prev_extreme = cur_value;
				result.array[i] = (float)state;
				break;
			}

			pct_change = -(cur_value / prev_extreme - 1.0f); // cur_value < prev_extreme, so pct_change is positive
#ifdef LOG
			if (call_num < 2)
				log << "line 189 case 0: " << "pct_change=" << pct_change << std::endl;
#endif
			if (pct_change < pct_param) {
				// value is lower than previous highest value, but not low enough to trigger state change
				result.array[i] = (float)state;
				break;
			}

			// value is lower than previous highest value...low enough to trigger state change to down trend
			state = -1;
			result.array[i] = (float)state;
			break;
		}
	}
#ifdef LOG
	if (call_num++ == 2)
		log.close();
#endif
	return result;
}

// Helper function
int SkipEmptyValues(int nSize, float* Src, float* Dst) {
	int i = 0;
	for (i = 0; i < nSize && IS_EMPTY(Src[i]); i++)
		Dst[i] = EMPTY_VAL;
	return i;
}

// "Function name", { FunctionPtr, <no. of array args>, <no. of string args>, <no. of float args>, <no. of default args>, <pointer to default values table float *>
FunctionTag gFunctionTable[] = {
								"LEL_PeakValley", {LEL_PeakValley, 1, 0, 2, 0, NULL}
};

int gFunctionTableSize = sizeof( gFunctionTable )/sizeof( FunctionTag );
