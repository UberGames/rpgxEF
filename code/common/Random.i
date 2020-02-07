%module common
%{
#include "Random.h"
%}

namespace common {

int getRandomInt();
int getRandomInt(int min);
int getRandomInt(int min, int max);

double getRandomDouble();
double getRandomDouble(double min);
double getRandomDouble(double min, double max);

} // namespace common