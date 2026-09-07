from gpiozero import DigitalInputDevice
from signal import pause

PULSES_PER_REV = 22
WHEEL_DIAMETER_CM = 6.5
WHEEL_CIRCUMFERENCE_CM = 3.14159 * WHEEL_DIAMETER_CM
CM_PER_PULSE = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REV

left_encoder = DigitalInputDevice(24, pull_up=None, active_state=True)
right_encoder = DigitalInputDevice(25, pull_up=None, active_state=True)

left_count = 0
right_count = 0

def left_pulse():
    global left_count
    left_count += 1

    distance = left_count * CM_PER_PULSE

    print(
        f"Venstre: {left_count} pulser = {distance:.1f} cm | "
        f"Højre: {right_count} pulser = {right_count * CM_PER_PULSE:.1f} cm"
    )

def right_pulse():
    global right_count
    right_count += 1

    distance = right_count * CM_PER_PULSE

    print(
        f"Venstre: {left_count} pulser = {left_count * CM_PER_PULSE:.1f} cm | "
        f"Højre: {right_count} pulser = {distance:.1f} cm"
    )

left_encoder.when_activated = left_pulse
right_encoder.when_activated = right_pulse

print("Encoder afstandstest klar")
print(f"1 puls = ca. {CM_PER_PULSE:.2f} cm")
print("Drej hjulene med hånden")
print("Ctrl+C stopper testen")

pause()
