#include <Gpio.h>
#include <Server.h>

#define LED_GPIO_PIN 12

int main(int argc, char *argv[])
{
    // Init GPIO
    Gpio::Manager gpio = Gpio::Manager();

    // Set up GPIO Pin
    gpio.addOutputPin(LED_GPIO_PIN);

    // Set up HTTP server (not HTTPS)
    Server::GpioServer server = Server::GpioServer(gpio);

    return 0;
}
