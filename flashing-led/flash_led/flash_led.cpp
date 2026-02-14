#include <pigpio.h>
#include <unistd.h>

#define LED_GPIO_PIN 17
#define LED_FLASH_TIME 1000000

// Turns the LED on for 1 second, then back off
void flash_led() {
    gpioWrite(LED_GPIO_PIN, 1);
    usleep(LED_FLASH_TIME);
    gpioWrite(LED_GPIO_PIN, 0);
}

bool init_gpio() {
    if (gpioInitialise() < 0) return false;

    gpioSetMode(LED_GPIO_PIN, PI_OUTPUT);
    return true;
}

int main(int argc, char *argv[]) {
    bool init_success = init_gpio();

    if (!init_success)
        return 1;

    for (int i = 0; i < 5; ++i) {
        flash_led();
        usleep(LED_FLASH_TIME); // sleep after each flash to determine wait until next flash
    }

    gpioTerminate();
    return 0;
}

