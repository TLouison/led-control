# LED Control

These folders contain standalone programs that control an LED through
the GPIO pins on a Raspberry Pi. They have been tested on a Raspberry Pi 3B+
and each were designed to understand a different component of developing for
hardware.

Learnings Overview:
- GPIO are the accessible pins on the Raspberry Pi board that can be used to
  control things through software. 
- Encapsulating the GPIO pin into a class, using RAII principles, can help to
  make sure the pin is in the proper state when the program initializes and is
  left in the proper state after it ends.
- You can control GPIO through direct access (GPIO are accessible through files)
  but it is generally better to leave that very low level implementation to robust
  libraries, at least in the simple cases I've worked on here.
