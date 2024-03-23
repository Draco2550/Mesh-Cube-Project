#pragma once
#include "vector.h"
#ifndef TRIANGLE_H
#define TRINAGLE_H

typedef struct
{
	int a;
	int b;
	int c;
} face_t;

typedef struct
{
	vec2_t points[3];
}triangle_t;



#endif // !TRIANGLE_H
