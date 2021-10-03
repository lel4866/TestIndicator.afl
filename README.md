# TestIndicator.afl
C++ dll that implements a custom Amibroker indicator: LEL_PeakValley(array, percent, neutral_percent)

This indicator is a peak-valley follower. It outputs values [-1, 0, 1] to indicate whether the specified price/indicator array is in active
confirmed uptred (1), confirmed downtrend (-1), or in between (0)

The indicator switches to 1 when an array value is greater than or equal to percent greater than the prior low, and switches to -1 when an
array value is less than or equal to a prior high

While neurtral percent isn't implemented yet, it will be used to output a zero when, for instance, the array is in an uptrend, goes more than
neutral_percent lower, but has not yet gone percent lower

This indicator uses files Plugin.h and Plugin.cpp from the Amibroker Development Kit

# Programming comments:
This is written using C++ 20 and Visual Studio 2022 Preview