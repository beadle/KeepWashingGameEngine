#pragma once

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

float RANDOM_0_1()
{
	return (float)rand() / RAND_MAX;
}

float RANDOM_MINUS_1_1()
{
	return RANDOM_0_1() * 2.0f - 1.0f;
}