
#ifndef COPRO_H
#define COPRO_H

#define pi 3.1415926536

#ifdef __cplusplus
extern "C" {
#endif

void finit(int);
float sin(float);
float cos(float);
float arccos(float);
float sqr(float);
float sqrt(float);
float fabs(float);
float pow(float,float);
float arctan(float,float);

int fexp(float);
float fscale(float,int);

float cos2sin(float);

void setrc(int);
int ceil(float);
int floor(float);

float round(float);

#ifdef __cplusplus
};
#endif

#endif //COPRO_H
