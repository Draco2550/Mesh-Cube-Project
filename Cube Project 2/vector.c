#include "vector.h";
#include <math.h>;

vec3_t vec3_rotate_z(vec3_t v, float angle)
{
	vec3_t rotated_vector = {
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};
	return rotated_vector;

}

vec3_t vec3_rotate_y(vec3_t v, float angle)
{
	vec3_t rotated_vector = {
		.x = v.x * cos(angle) - v.z * sin(angle),
		.z = v.x * sin(angle) + v.z * cos(angle),
		.y = v.y
	};
	return rotated_vector;

}

vec3_t vec3_rotate_x(vec3_t v, float angle)
{
	vec3_t rotated_vector = {
		.z = v.z * cos(angle) - v.y * sin(angle),
		.y = v.z * sin(angle) + v.y * cos(angle),
		.x = v.x
	};
	return rotated_vector;

}


float vec3_length(vec3_t v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

vec3_t vec3_add(vec3_t a, vec3_t b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

vec3_t vec3_multi(vec3_t v, float scalar)
{
	vec3_t result =
	{
		.x = v.x * scalar,
		.y = v.y * scalar,
		.z = v.z * scalar
	};
	return result;
}

vec3_t vec3_div(vec3_t v, float scalar)
{

	vec3_t result =
	{
		.x = v.x / scalar,
		.y = v.y / scalar,
		.z = v.z / scalar
	};
	return result;
}

vec3_t vec3_cross(vec3_t c, vec3_t v)
{
	vec3_t result =
	{
		.x = (c.y * v.z) - (c.z * v.y),
		.y = (c.z * v.x) - (c.x * v.z),
		.z = (c.x * v.y) - (c.y * v.x)
	};
	return result;
}


float vec3_dot(vec3_t c, vec3_t v)
{
	return (c.x * v.x) + (c.y * v.y) + (c.z * v.z);
}

//2d functions

float vec2_length(vec2_t v)
{
	return sqrt((v.x * v.x) + (v.y * v.y));
}

vec2_t vec2_add(vec2_t a, vec2_t b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

vec2_t vec2_sub(vec2_t a, vec2_t b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

vec2_t vec2_multi(vec2_t v, float scalar)
{
	vec2_t result =
	{
		.x = v.x * scalar,
		.y = v.y * scalar
	};
	return result;
}

vec2_t vec2_div(vec2_t v, float scalar)
{
	vec2_t result =
	{
		.x = v.x / scalar,
		.y = v.y / scalar
	};
	return result;
}

float vec2_dot(vec2_t c, vec2_t v)
{
	return (c.x * v.x) + (c.y * v.y);
}
