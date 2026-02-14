/*
 * pigpio stub header for non-Linux development.
 * Provides no-op declarations so the project compiles on macOS.
 * On the Pi, the real pigpio.h is used instead.
 */
#ifndef PIGPIO_STUB_H
#define PIGPIO_STUB_H

#include <cstdint>

#define PI_INPUT  0
#define PI_OUTPUT 1

#define PI_MIN_GPIO 0
#define PI_MAX_GPIO 53

typedef void (*gpioAlertFuncEx_t)(int gpio, int level, uint32_t tick, void *userdata);

int gpioInitialise(void);
void gpioTerminate(void);

int gpioSetMode(unsigned gpio, unsigned mode);
int gpioGetMode(unsigned gpio);
int gpioWrite(unsigned gpio, unsigned level);
int gpioRead(unsigned gpio);
int gpioSetAlertFuncEx(unsigned gpio, gpioAlertFuncEx_t f, void *userdata);

#endif
