from gpiozero import Button

button = Button(2, pull_up=True)

button.wait_for_press()
print("THE BUTTON WAS PRESSED")
