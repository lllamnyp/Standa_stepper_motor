#ifndef _GOTO_HELPERS_H_
#define _GOTO_HELPERS_H_

#define	LIMIT_POSITION	12
#define	ANA_STEP		18
#define STEPS_PER_DEG	100
#define STEPS_PER_TURN	36000

//	Modulo function that is never negative
int mod(int x, int y);

//	Integer division with rounding to -Inf; assumes positive divisor
int quotient(int x, int y);

//	Find rotation direction (1 if positive, 0 if negative)
int rotation_direction(int currPos, int target);

//	Find zero crossing position
int zero_crossing(int currPos, int dir);

//	Movement algorithm
void goto_custom(device_t device, int currPos, int target);

//	Test if string represents valid integer
int validateArg(char * arg);

#endif // !_GOTO_HELPERS_H_

