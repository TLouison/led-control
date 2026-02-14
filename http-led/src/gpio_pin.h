#ifndef GPIO_PIN_H
#define GPIO_PIN_H

#include <pigpio.h>
#include <functional>
#include <cstdint>
#include <iostream>

/*
 * Class copied from the flashing-led project
 */

class GpioPin
{
public:
    GpioPin(int pin)
    {
        this->pin = pin;
        gpioSetMode(pin, this->mode);
        std::cout << "Created GPIO Pin " << pin << " in mode " << this->mode << "\n";
    }
    GpioPin(int pin, int mode)
    {
        this->pin = pin;
        this->mode = mode;
        gpioSetMode(pin, mode);
        std::cout << "Created GPIO Pin " << pin << " in mode " << mode << "\n";
    }
    ~GpioPin()
    {
        if (this->pin >= PI_MIN_GPIO && this->pin <= PI_MAX_GPIO)
        {
            gpioSetMode(this->pin, PI_INPUT);
            gpioWrite(this->pin, 0); // Set the pin low to reset it
        }
        std::cout << "Released GpioPin " << pin << "\n";
    }

    // Remove the copy and assignment operators
    GpioPin(const GpioPin &) = delete;

    // Add move constructors
    GpioPin(GpioPin &&other) noexcept
    {
        this->pin = other.pin;
        other.pin = -1;
    }
    GpioPin &operator=(const GpioPin &) = delete;

    void set_output() { gpioSetMode(this->pin, PI_OUTPUT); }
    void set_input() { gpioSetMode(this->pin, PI_INPUT); }
    void set_alert(
        gpioAlertFuncEx_t callback,
        void *callback_ctx)
    {
        gpioSetAlertFuncEx(this->pin, callback, callback_ctx);
    }

    void set_level(int level) { gpioWrite(this->pin, level); }
    void set_high() { this->set_level(1); }
    void set_low() { this->set_level(0); }

    int get_mode() { return gpioGetMode(this->pin); }
    int get_level() { return gpioRead(this->pin); }

private:
    int pin;
    int mode{PI_INPUT};
};

#endif
