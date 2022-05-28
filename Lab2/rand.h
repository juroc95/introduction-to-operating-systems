#pragma once

#ifndef __RAND_H
# define __RAND_H

#ifndef RAND_MAX
# define RAND_MAX (1 << 31)
#endif

unsigned int rand(void);
void srand(unsigned int seed);

#endif // __RAND_H
