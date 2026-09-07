from gpiozero import DigitalInputDevice
from signal import pause

left_encoder = DigitalInputDevice(24, pull_up=None, active_state=True)
right_encoder = DigitalInputDevice(25, pull_up=None, active_state=True)

left_count = 0
right_count = 0

def left_pulse():
    global left_count
    left_count += 1

def right_pulse():
    global right_count
    right_count += 1

left_encoder.when_activated = left_pulse
right_encoder.when_activated = right_pulse

print("Encoder kalibrering")
print("Drej ET hjul præcis én hel omgang.")
print("Tryk Ctrl+C bagefter.")

try:
    pause()
except KeyboardInterrupt:
    print()
    print(f"Venstre pulser: {left_count}")
    print(f"Højre pulser:   {right_count}")
