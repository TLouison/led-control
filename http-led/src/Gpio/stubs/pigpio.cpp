#include "pigpio.h"
#include <cstdio>

int gpioInitialise(void)
{
    printf("[pigpio stub] gpioInitialise (no-op)\n");
    return 0;
}

void gpioTerminate(void)
{
    printf("[pigpio stub] gpioTerminate (no-op)\n");
}

int gpioSetMode(unsigned gpio, unsigned mode)
{
    printf("[pigpio stub] gpioSetMode(%u, %u) (no-op)\n", gpio, mode);
    return 0;
}

int gpioGetMode(unsigned gpio)
{
    printf("[pigpio stub] gpioGetMode(%u) (no-op)\n", gpio);
    return PI_INPUT;
}

int gpioWrite(unsigned gpio, unsigned level)
{
    printf("[pigpio stub] gpioWrite(%u, %u) (no-op)\n", gpio, level);
    return 0;
}

int gpioRead(unsigned gpio)
{
    printf("[pigpio stub] gpioRead(%u) (no-op)\n", gpio);
    return 0;
}

int gpioSetAlertFuncEx(unsigned gpio, gpioAlertFuncEx_t f, void *userdata)
{
    printf("[pigpio stub] gpioSetAlertFuncEx(%u) (no-op)\n", gpio);
    return 0;
}
