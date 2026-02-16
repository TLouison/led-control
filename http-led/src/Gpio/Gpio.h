#ifndef GPIO_PIN_H
#define GPIO_PIN_H

#include <pigpio.h>
#include <functional>
#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <format>
#include <memory>

namespace Gpio
{
    typedef std::string GpioPinName_t;

    class Pin
    {
    public:
        Pin(int pin, int mode)
        {
            this->pin = pin;
            this->mode = mode;
            gpioSetMode(pin, mode);
            std::cout << "Created GPIO Pin " << pin << " in mode " << mode << "\n";
        }
        ~Pin()
        {
            if (this->pin >= PI_MIN_GPIO && this->pin <= PI_MAX_GPIO)
            {
                gpioSetMode(this->pin, PI_INPUT);
                gpioWrite(this->pin, 0); // Set the pin low to reset it
            }
            std::cout << "Released GpioPin " << pin << "\n";
        }

        // Remove the copy and assignment operators
        Pin(const Pin &) = delete;

        // Add move constructors
        Pin(Pin &&other) noexcept
        {
            this->pin = other.pin;
            other.pin = -1;
        }
        Pin &operator=(const Pin &) = delete;

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

    class Manager
    {
    public:
        Manager()
        {
            if (gpioInitialise() < 0)
                throw;

            this->pins = std::map<std::string, std::unique_ptr<Pin>>{};
            this->is_ready = true;
        }

        ~Manager()
        {
            // Clear the map to free the pins before terminating GPIO
            this->pins.clear();

            // Terminate the GPIO connections when done.
            gpioTerminate();
        }

        // Remove the copy and assignment operators
        Manager(const Manager &) = delete;
        Manager &operator=(const Manager &) = delete;

        bool keyExists(GpioPinName_t name)
        {
            if (this->pins.find(name) == this->pins.end())
            {
                return false;
            }
            return true;
        }

        Pin *getPinWithName(GpioPinName_t name)
        {
            if (!this->keyExists(name))
            {
                throw std::invalid_argument(std::format("Pin with name {} does not exist.", name));
            }

            return this->pins[name].get();
        }

        void
        addPin(GpioPinName_t name, int pin, int mode)
        {
            if (this->keyExists(name))
            {
                throw std::invalid_argument(std::format("Pin with name {} already exists.", name));
            }
            this->pins.insert({name, std::make_unique<Pin>(pin, mode)});
        }

        void addInputPin(GpioPinName_t name, int pin)
        {
            this->addPin(name, pin, PI_INPUT);
        }

        void addOutputPin(GpioPinName_t name, int pin)
        {
            this->addPin(name, pin, PI_OUTPUT);
        }

    private:
        bool is_ready = false;
        std::map<GpioPinName_t, std::unique_ptr<Pin>> pins;
    };
};

#endif
