from gpiozero import DigitalInputDevice
from signal import pause

left_encoder = DigitalInputDevice(24, pull_up=None, active_state=True)
right_encoder = DigitalInputDevice(25, pull_up=None, active_state=True)

left_count = 0
right_count = 0

def left_pulse():
    global left_count
    left_count += 1
    print(f"Venstre: {left_count} | Højre: {right_count}")

def right_pulse():
    global right_count
    right_count += 1
    print(f"Venstre: {left_count} | Højre: {right_count}")

left_encoder.when_activated = left_pulse
right_encoder.when_activated = right_pulse

print("Begge encodere er klar")
print("Drej hjulene med hånden")
print("Ctrl+C stopper testen")

pause()
