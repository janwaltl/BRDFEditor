#ifndef BRDFEDITOR_KERNEL_CONSTANTS_HEADER
#define BRDFEDITOR_KERNEL_CONSTANTS_HEADER

#define PI 3.1415926535f
#define TWOPI (2.0f*PI)
#define HALFPI (PI/2.0f)
#define EPSILON 0.000001f

#define NOT_LI -1.0f
//If we've hit a light
#define IS_LI(LI) (LI.x>=0.0f && LI.y >=0.0f && LI.z>=0.0f)
#endif
