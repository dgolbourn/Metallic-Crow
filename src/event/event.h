#ifndef EVENT_H_
#define EVENT_H_

#include "signal.h"

namespace event
{

extern Signal quit;

extern Signal up;
extern Signal down;
extern Signal left;
extern Signal right;

extern Signal trigger;

void Events(void);

}

#endif