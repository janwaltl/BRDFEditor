#ifndef BRDFEDITOR_KERNEL_RANDOM_HEADER
#define BRDFEDITOR_KERNEL_RANDOM_HEADER

typedef uint2 RNGState;

#include "Geometry.cl"

//From http://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
float MWC64X(RNGState *state)
{
    enum { A=4294883355U};
    uint x=(*state).x, c=(*state).y;  // Unpack the state
    uint res=x^c;                     // Calculate the result
    uint hi=mul_hi(x,A);              // Step the RNG
    x=x*A+c;
    c=hi+(x<c);
    *state=(uint2)(x,c);               // Pack the state back up
	//Assumes floats have 23-bit mantissa.
	res &= 0x7FFFFF;
    return (float)res/(1 << 23);      // Return the next result
}
float PCG(RNGState* state)
{
	ulong s = upsample(state->x,state->y);

	uint res =  (s ^ (s >> 18)) >> 27;
	uint rotAmount = s >> 59;
	res = res >> rotAmount | ( res << (32 - rotAmount));

	//Advance the state. Uses magic constant from 64-bit Mersenne twister
	s = s * 6364136223846793005UL + 1442695040888963407UL;

	state->x = s >> 32;
	state->y = s & 0xFFFFFFFF;
	//Assumes floats have 23-bit mantissa.
	res &= 0x7FFFFF;
	return (float)res/(1 << 23);
}

float uniformRNG(RNGState* state)
{
	return PCG(state);
}

//Returns randomly generated direction in a hemisphere from cosine-weighted distribution.
float3 genCosineDir(float3 wOut, float3 n, RNGState* rngState, float* pdf)
{
	float e1=uniformRNG(rngState);
	float e2=uniformRNG(rngState);
	float angle = TWOPI*e1;
	//Tangent space
	float3 tn1,tn2;
	genBasis(n,&tn1,&tn2);

	float3 wIn= sqrt(1-e2)*n + sqrt(e2)*(cos(angle)*tn1 + sin(angle)*tn2);
	*pdf = dot(n,wIn)/PI;
	return wIn;
}
float3 genCosineDirN(float3 n, RNGState* rngState, float* pdf)
{
	float e1=uniformRNG(rngState);
	float e2=uniformRNG(rngState);
	float angle = TWOPI*e1;
	//Tangent space
	float3 tn1,tn2;
	genBasis(n,&tn1,&tn2);

	float3 wIn= sqrt(1-e2)*n + sqrt(e2)*(cos(angle)*tn1 + sin(angle)*tn2);
	*pdf = dot(n,wIn)/PI;
	return wIn;
}

#endif
