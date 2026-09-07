from gpiozero import DigitalInputDevice
from signal import pause

encoder = DigitalInputDevice(24, pull_up=None, active_state=True)

pulse_count = 0

def count_pulse():
    global pulse_count
    pulse_count += 1
    print(f"Puls: {pulse_count}")

encoder.when_activated = count_pulse

print("Encoder test klar")
print("Drej hjulet med hånden...")
print("Tryk Ctrl+C for at stoppe")

pause()
