from gpiozero import OutputDevice, DigitalInputDevice
from time import sleep

# -------------------------
# Motor GPIO
# -------------------------

left_in1 = OutputDevice(17)
left_in2 = OutputDevice(27)

right_in1 = OutputDevice(22)
right_in2 = OutputDevice(23)

# -------------------------
# Encoder GPIO
# -------------------------

left_encoder = DigitalInputDevice(
    24,
    pull_up=None,
    active_state=True
)

right_encoder = DigitalInputDevice(
    25,
    pull_up=None,
    active_state=True
)

# -------------------------
# Hjul / encoder data
# -------------------------

PULSES_PER_REV = 22
WHEEL_DIAMETER_CM = 6.5

WHEEL_CIRCUMFERENCE_CM = 3.14159 * WHEEL_DIAMETER_CM
CM_PER_PULSE = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REV

TARGET_DISTANCE_CM = 20

target_pulses = round(TARGET_DISTANCE_CM / CM_PER_PULSE)

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


def stop():
    left_in1.off()
    left_in2.off()

    right_in1.off()
    right_in2.off()


def forward():
    # Venstre motor
    left_in1.on()
    left_in2.off()

    # Højre motor
    right_in1.on()
    right_in2.off()


print("Kør-afstand test")
print(f"Mål: {TARGET_DISTANCE_CM} cm")
print(f"1 puls: ca. {CM_PER_PULSE:.2f} cm")
print(f"Stop efter ca. {target_pulses} pulser på begge hjul")
print()

try:
    stop()
    sleep(1)

    print("Motorer starter...")
    forward()

    while True:

        left_cm = left_count * CM_PER_PULSE
        right_cm = right_count * CM_PER_PULSE

        print(
            f"Venstre: {left_count} ({left_cm:.1f} cm) | "
            f"Højre: {right_count} ({right_cm:.1f} cm)",
            end="\r"
        )

        # Stop venstre motor når venstre hjul når målet
        if left_count >= target_pulses:
            left_in1.off()
            left_in2.off()

        # Stop højre motor når højre hjul når målet
        if right_count >= target_pulses:
            right_in1.off()
            right_in2.off()

        # Når begge er nået frem, afslut testen
        if left_count >= target_pulses and right_count >= target_pulses:
            break

        sleep(0.02)

    stop()

    print()
    print("Mål nået - motorer STOPPET")
    print(f"Venstre afstand: {left_count * CM_PER_PULSE:.1f} cm")
    print(f"Højre afstand:   {right_count * CM_PER_PULSE:.1f} cm")

except KeyboardInterrupt:
    print()
    print("Test afbrudt")

finally:
    stop()
    print("Motorer stoppet sikkert")
