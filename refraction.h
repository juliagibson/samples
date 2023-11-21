/*
Constants
*/
#ifndef PI
#define PI 3.14159265359
#endif 

/*
Globals
*/



/* 
Function prototypes
*/
void satProjection(double x, double y, double z, double *satProj_x, double *satProj_y, double *satProj_z);

double deltaAngle(double x, double y, double z, double a, double b, double c);

double zenithAngle(double x, double y, double z, double a, double b, double c, double *distGS2satProj);

double refractiveIndex(double q, double r, double s);

void main();


