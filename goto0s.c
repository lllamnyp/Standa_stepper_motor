#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

#include "helpers.h"

#define AXIS_TO_USE	0


#include "main.c"