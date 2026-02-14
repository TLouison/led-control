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

#define BUTTON_PRESS_METHOD "events" // "polling" or "events"
#define LED_FLASH_METHOD "pwm"		 // "pwm" or "write"

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
void flash_led()
{
	// Loop to flash the LED
	while (FLASH_ENABLED)
	{
		{
			// If we are using PWM, flash using 100% range and 0% range
			if (strcmp(LED_FLASH_METHOD, "pwm"))
			{
				gpioPWM(LED_GPIO_PIN, 2000);
				usleep(flash_time());
				gpioPWM(LED_GPIO_PIN, 0);
			}
			else
			{
				// Otherwise, trigger with gpioWrite to turn on/off
				gpioWrite(LED_GPIO_PIN, 1);
				usleep(flash_time());
				gpioWrite(LED_GPIO_PIN, 0);
			}

			usleep(flash_time()); // sleep after each flash to determine wait until next flash
		}
	}
}

void toggle_led_flash()
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
		std::thread led_thread(flash_led);
		led_thread.detach();
	}
}
// =============== END LED METHODS ===============

// =============== SPEED CHANGE METHODS ===============
void clear_speed_display()
{
	gpioWrite(SLOW_SPEED_LED_GPIO, 0);
	gpioWrite(MED_SPEED_LED_GPIO, 0);
	gpioWrite(FAST_SPEED_LED_GPIO, 0);
}

void update_speed_display()
{
	clear_speed_display();

	switch (FLASH_TIME_MULT)
	{
	case 1:
		gpioWrite(SLOW_SPEED_LED_GPIO, 1);
		break;
	case 2:
		gpioWrite(MED_SPEED_LED_GPIO, 1);
		break;
	default:
		gpioWrite(FAST_SPEED_LED_GPIO, 1);
	}
}

void change_speed()
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

	update_speed_display();

	std::cout << "Speed: Speed set to " << FLASH_TIME_MULT << "(" << FLASH_TIME_CHANGE << ")" << std::endl;
}
// ===============  SPEED CHANGE METHODS METHODS ===============

// =============== CALLBACKS ===============
void trigger_flash_callback(int gpio, int level, uint32_t tick)
{
	if (level == 0)
	{
		toggle_led_flash();
	}
}

void trigger_speed_callback(int gpio, int level, uint32_t tick)
{
	if (level == 1)
	{
		std::thread speed_thread(change_speed);
		speed_thread.detach();
	}
}

void trigger_user_input_callback(int command)
{
	switch (command)
	{
	case 1:
		toggle_led_flash();
		break;
	case 2:
		change_speed();
		break;
	case 9:
		RUNNING = false;
		break;
	default:
		std::cout << "Command not recognized." << std::endl;
	}
}
// =============== END CALLBACKS ===============

void get_user_input()
{
	while (RUNNING)
	{
		std::cout << "Enter a command number (1 - On/Off, 2 - Speed, 9 - Exit): ";
		int u_input{};
		std::cin >> u_input;
		trigger_user_input_callback(u_input);
	}
}

void handle_exit(int sig)
{
	RUNNING = false;
	std::cin.ignore();
	gpioTerminate();
	std::exit(sig);
}

// =============== GPIO HANDLERS ===============
void reset_leds()
{
	// Turn off LEDs if they were left on
	gpioWrite(LED_GPIO_PIN, 0);
	gpioWrite(SLOW_SPEED_LED_GPIO, 0);
	gpioWrite(MED_SPEED_LED_GPIO, 0);
	gpioWrite(FAST_SPEED_LED_GPIO, 0);

	update_speed_display();
}

bool init_gpio()
{
	if (gpioInitialise() < 0)
		return false;

	// Set up modes
	gpioSetMode(FLASH_BUTTON_GPIO_PIN, PI_INPUT);
	gpioSetMode(SPEED_BUTTON_GPIO_PIN, PI_INPUT);
	gpioSetMode(LED_GPIO_PIN, PI_OUTPUT);

	// Make sure display is correct at the start
	reset_leds();

	// Set up PWM for LED
	gpioSetPWMfrequency(LED_GPIO_PIN, 500);
	gpioSetPWMrange(LED_GPIO_PIN, 2000);

	return true;
}
// =============== END GPIO HANDLERS ===============

int main(int argc, char *argv[])
{
	bool init_success = init_gpio();
	if (!init_success)
		return 1;

	// Checking current state of GPIO
	std::cout << "GPIO" << FLASH_BUTTON_GPIO_PIN << " is " << gpioRead(FLASH_BUTTON_GPIO_PIN) << '\n';
	std::cout << "GPIO" << SPEED_BUTTON_GPIO_PIN << " is " << gpioRead(SPEED_BUTTON_GPIO_PIN) << '\n';
	std::cout << "GPIO" << LED_GPIO_PIN << " is " << gpioRead(LED_GPIO_PIN) << '\n';

	// Set up signal handling to allow users to exit input loop
	signal(SIGINT, handle_exit);
	signal(SIGTERM, handle_exit);

	// If we are doing events, use alert func to trigger the callbacks
	if (strcmp(BUTTON_PRESS_METHOD, "events") == 0)
	{
		gpioSetAlertFunc(FLASH_BUTTON_GPIO_PIN, trigger_flash_callback);
		gpioSetAlertFunc(SPEED_BUTTON_GPIO_PIN, trigger_speed_callback);
	}

	// Spawn a thread to take user input
	std::thread user_input(get_user_input);
	user_input.detach();

	while (RUNNING)
	{
		if (strcmp(BUTTON_PRESS_METHOD, "polling") == 0)
		{
			// If we are doing polling, check the value for each button after each sleep
			int level = gpioRead(FLASH_BUTTON_GPIO_PIN);
			if (FLASH_LAST_LEVEL != level)
			{
				std::cout << "Poll found new level on button. Toggling LED." << std::endl;
				FLASH_LAST_LEVEL = level;
				flash_led();
			}

			int speed_level = gpioRead(SPEED_BUTTON_GPIO_PIN);
			if (SPEED_LAST_LEVEL != speed_level)
			{
				std::cout << "Poll found new level on speed button. Changing speed." << std::endl;
				SPEED_LAST_LEVEL = speed_level;
				change_speed();
			}
		}

		usleep(1000);
	}

	handle_exit(0);
}
