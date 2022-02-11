//
// Sample.cpp : Defines the entry point for the DLL application.
//

#include "pch.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "Plugin.h"

#undef LOG
int SkipEmptyValues(int nSize, float* Src, float* Dst);

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	return TRUE;
}

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
	for (int i = iStart + 1; i < nSize; i++) {
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

struct V {
	float f0 = 0.0f, f8 = 0.0f, f10 = 0.0f, f18 = 0.0f, f20 = 0.0f, f28 = 0.0f, f30 = 0.0f, f38 = 0.0f, f40 = 0.0f,
		f48 = 0.0f, f50 = 0.0f, f58 = 0.0f, f60 = 0.0f, f68 = 0.0f, f70 = 0.0f, f78 = 0.0f, f80 = 0.0f;
	float v4 = 0.0f, v8 = 0.0f, v10 = 0.0f, v14 = 0.0f, v18 = 0.0f, v20 = 0.0f, vC = 0.0f, v1C = 0.0f;
};

AmiVar LEL_RSX(int NumArgs, AmiVar* ArgsTable) {
	AmiVar result = gSite.AllocArrayResult();
	int nSize = gSite.GetArraySize();
	float* SrcArray = ArgsTable[0].array;
	float Len = (float)ArgsTable[1].val;

	int iStart = SkipEmptyValues(nSize, SrcArray, result.array);
	if (iStart >= nSize)
		return result;

	V v;
	V pV;

	for (int i = iStart + 1; i < nSize; i++) {
		float Close = SrcArray[i];
		if (Close == EMPTY_VAL) {
			result.array[i] = SrcArray[i - 1];
			continue;
		}

		v.f8 = 100.0f * Close;
		v.f18 = 3.0f / (Len + 2.0f);
		v.f20 = 1.0f - v.f18;

		v.f8 = 100.0f * Close;
		v.f10 = pV.f8;
		v.v8 = v.f8 - v.f10;
		v.f28 = v.f20 * pV.f28 + v.f18 * v.v8;
		v.f30 = v.f18 * v.f28 + v.f20 * pV.f30;
		v.vC = v.f28 * 1.5f - v.f30 * 0.5f;
		v.f38 = v.f20 * pV.f38 + v.f18 * v.vC;
		v.f40 = v.f18 * v.f38 + v.f20 * pV.f40;
		v.v10 = v.f38 * 1.5f - v.f40 * 0.5f;
		v.f48 = v.f20 * pV.f48 + v.f18 * v.v10;
		v.f50 = v.f18 * v.f48 + v.f20 * pV.f50;
		v.v14 = v.f48 * 1.5f - v.f50 * 0.5f;
		v.f58 = v.f20 * pV.f58 + v.f18 * std::abs(v.v8);
		v.f60 = v.f18 * v.f58 + v.f20 * pV.f60;
		v.v18 = v.f58 * 1.5f - v.f60 * 0.5f;
		v.f68 = v.f20 * pV.f68 + v.f18 * v.v18;
		v.f70 = v.f18 * v.f68 + v.f20 * pV.f70;
		v.v1C = v.f68 * 1.5f - v.f70 * 0.5f;
		v.f78 = v.f20 * pV.f78 + v.f18 * v.v1C;
		v.f80 = v.f18 * v.f78 + v.f20 * pV.f80;
		v.v20 = v.f78 * 1.5f - v.f80 * 0.5f;

		float v4_ = v.v20 > 0.0f ? (v.v14 / v.v20 + 1.0f) * 50.0f : 50.0f;
		float rsx = v4_ > 100.0f ? 100.0f : v4_ < 0.0f ? 0.0f : v4_;

		result.array[i] = rsx;

		pV = v;
	}

	//return rsx coloured(255, 0, 255) style(line, 3) as "Jurik RSX", 30 coloured(100, 100, 100) style(dottedline), 50 coloured(100, 100, 100) style(dottedline), 70 coloured(100, 100, 100) style(dottedline)
	return result;
}

float nz(float value) {
	return (value == EMPTY_VAL) ? 0.0f : value;
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
								"LEL_PeakValley", {LEL_PeakValley, 1, 0, 2, 0, NULL},
								"LEL_RSX", {LEL_RSX, 1, 0, 1, 0, NULL}
};

int gFunctionTableSize = sizeof(gFunctionTable) / sizeof(FunctionTag);
