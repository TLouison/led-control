#include "gpio_pin.h"

#include <pigpio.h>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <csignal>

#define FLASH_BUTTON_GPIO_PIN 2
#define SPEED_BUTTON_GPIO_PIN 3
#define LED_GPIO_PIN 12

#define SLOW_SPEED_LED_GPIO 5
#define MED_SPEED_LED_GPIO 6
#define FAST_SPEED_LED_GPIO 13

#define LED_FLASH_TIME 1000000
#define BUTTON_POLL_RATE 10000

#define LED_FLASH_METHOD "pwm" // "pwm" or "write"

bool FLASH_ENABLED = false;
bool RUNNING = true;

int FLASH_LAST_LEVEL = 0;
int SPEED_LAST_LEVEL = 0;
int FLASH_TIME_CHANGE = 0;
int FLASH_TIME_MULT = 1;

// =============== LED METHODS ===============
int flash_time()
{
    return LED_FLASH_TIME / pow(2, FLASH_TIME_MULT);
}

// Continually loops and flashes the LED while FLASH_ENABLED is true
void flash_led(GpioPin *pin)
{
    std::cout << "INSIDE FLASH_LED" << std::endl;
    // Loop to flash the LED
    while (FLASH_ENABLED)
    {
        pin->set_high();
        usleep(flash_time());
        pin->set_low();
        usleep(flash_time());
    }
}

void toggle_led_flash(GpioPin *pin)
{
    if (FLASH_ENABLED)
    {
        std::cout << "Flash: Disabling LED..." << std::endl;
        FLASH_ENABLED = false; // Thread should terminate now that flashing disabled
    }
    else
    {
        std::cout << "Flash: Triggering LED..." << std::endl;
        FLASH_ENABLED = true;
        std::thread led_thread(flash_led, pin);
        led_thread.detach();
    }
}
// =============== END LED METHODS ===============

// =============== SPEED CHANGE METHODS ===============
struct SpeedLEDGpioPins
{
    GpioPin *slow;
    GpioPin *med;
    GpioPin *fast;
};

void clear_speed_display(SpeedLEDGpioPins pins)
{
    pins.slow->set_low();
    pins.med->set_low();
    pins.fast->set_low();
}

void update_speed_display(SpeedLEDGpioPins pins)
{
    clear_speed_display(pins);

    switch (FLASH_TIME_MULT)
    {
    case 1:
        pins.slow->set_high();
        break;
    case 2:
        pins.med->set_high();
        break;
    default:
        pins.fast->set_high();
    }
}

void change_speed(SpeedLEDGpioPins pins)
{
    switch (FLASH_TIME_MULT)
    {
    case 1:
        FLASH_TIME_MULT = 2;
        break;
    case 2:
        FLASH_TIME_MULT = 3;
        break;
    default:
        FLASH_TIME_MULT = 1;
    }
    FLASH_TIME_CHANGE++;

    update_speed_display(pins);

    std::cout << "Speed: Speed set to " << FLASH_TIME_MULT << "(" << FLASH_TIME_CHANGE << ")" << std::endl;
}
// ===============  SPEED CHANGE METHODS METHODS ===============

// =============== CALLBACKS ===============
struct FlashCallbackContext
{
    GpioPin *pin;
};
void trigger_flash_callback(int gpio, int level, uint32_t tick, void *ctx)
{
    FlashCallbackContext *context = static_cast<FlashCallbackContext *>(ctx);
    GpioPin *pin = context->pin;
    if (level == 0)
    {
        toggle_led_flash(pin);
    }
}

struct SpeedCallbackContext
{
    SpeedLEDGpioPins pins;
};
void trigger_speed_callback(int gpio, int level, uint32_t tick, void *ctx)
{
    SpeedCallbackContext *context = static_cast<SpeedCallbackContext *>(ctx);
    SpeedLEDGpioPins &pins = context->pins;
    if (level == 1)
    {
        std::thread speed_thread(change_speed, pins);
        speed_thread.detach();
    }
}
// =============== END CALLBACKS ===============

int main(int argc, char *argv[])
{
    if (gpioInitialise() < 0)
        return false;

    // Set up GPIO output pins
    std::unique_ptr<GpioPin> led = std::make_unique<GpioPin>(LED_GPIO_PIN, PI_OUTPUT);
    std::unique_ptr<GpioPin> slow_speed_led = std::make_unique<GpioPin>(SLOW_SPEED_LED_GPIO, PI_OUTPUT);
    std::unique_ptr<GpioPin> med_speed_led = std::make_unique<GpioPin>(MED_SPEED_LED_GPIO, PI_OUTPUT);
    std::unique_ptr<GpioPin> fast_speed_led = std::make_unique<GpioPin>(FAST_SPEED_LED_GPIO, PI_OUTPUT);
    SpeedLEDGpioPins speed_leds{slow_speed_led.get(), med_speed_led.get(), fast_speed_led.get()};

    std::unique_ptr<GpioPin> flash_button = std::make_unique<GpioPin>(FLASH_BUTTON_GPIO_PIN);
    std::unique_ptr<GpioPin> speed_button = std::make_unique<GpioPin>(SPEED_BUTTON_GPIO_PIN);

    // Set up display
    update_speed_display(speed_leds);

    // Checking current state of GPIO
    std::cout << "GPIO" << FLASH_BUTTON_GPIO_PIN << " is " << flash_button->get_level() << '\n';
    std::cout << "GPIO" << SPEED_BUTTON_GPIO_PIN << " is " << speed_button->get_level() << '\n';
    std::cout << "GPIO" << LED_GPIO_PIN << " is " << led->get_level() << '\n';

    // Set up callbacks to trigger the different controls
    FlashCallbackContext flash_ctx{led.get()};
    flash_button->set_alert(trigger_flash_callback, &flash_ctx);

    SpeedCallbackContext speed_ctx{speed_leds};
    speed_button->set_alert(trigger_speed_callback, &speed_ctx);

    while (RUNNING)
    {
        usleep(1000);
    }

    gpioTerminate();
}
