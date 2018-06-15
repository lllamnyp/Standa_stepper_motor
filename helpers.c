#include <stdio.h>
#include <ctype.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

#include "helpers.h"

//	Modulo function that is never negative
int mod(int x, int y) {
	return (x % y + y) % y;
}

//	Integer division with rounding to -Inf; assumes positive divisor
int quotient(int x, int y) {
	return x / y - ((x%y) < 0);
}

//	Find rotation direction (1 if positive, 0 if negative)
int rotation_direction(int currPos, int target) {
	int modPos = mod(currPos, STEPS_PER_TURN);
	int modTar = mod(target, STEPS_PER_TURN);

	// No zero crossing involved if true
	if (abs(modTar - modPos) < STEPS_PER_TURN - abs(modTar - modPos)) {
		return modTar - modPos > 0;
	}
	return modTar - modPos < 0;
}

//	Find zero crossing position
int zero_crossing(int currPos, int dir) {
	return STEPS_PER_TURN * (quotient(currPos, STEPS_PER_TURN) + dir);
}

//	Movement algorithm
void goto_custom(device_t device, int currPos, int target) {
	int dir = rotation_direction(currPos, target);
	int modPos = mod(currPos, STEPS_PER_TURN);
	int modTar = mod(target, STEPS_PER_TURN);
	int zero = zero_crossing(currPos, dir);

	printf("Encoder position %d; target %d; dir %d; modPos %d; modTar %d; zero %d\n", currPos, target, dir, modPos, modTar, zero);

	// If shift less than 0.05 degrees, ignore command
	if (abs(modPos - modTar) < 5) return;

	// If zero will not be crossed
	if (abs(modTar - modPos) < STEPS_PER_TURN - abs(modTar - modPos)) {
		printf("Zero will not be crossed\n");
		int correctedTar = STEPS_PER_TURN * quotient(currPos, STEPS_PER_TURN) + modTar;
		printf("Now moving to %d\n", correctedTar);
		command_move(device, correctedTar, 0);
		command_wait_for_stop(device, 500);
	}
	else {
		printf("Zero will be crossed; moving to %d\n", zero);
		command_move(device, zero, 0);
		command_wait_for_stop(device, 500);
		printf("Setting current position to zero\n");
		command_zero(device);
		command_wait_for_stop(device, 500);
		int correctedTar = modTar - STEPS_PER_TURN * (1 - dir);
		printf("Now moving to %d\n", correctedTar);
		command_move(device, correctedTar, 0);
		command_wait_for_stop(device, 500);
	}
	return;
}

//	Test if string represents valid integer
int validateArg(char * arg) {
	int i = 0;
	if (arg[i] == '-') i++;
	while (arg[i] != 0) {
		if (isdigit(arg[i])) { i++; }
		else { return 0; }
	}
	return 1;
}