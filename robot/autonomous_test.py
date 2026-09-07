import serial
import time

from gpiozero import OutputDevice, DigitalInputDevice


# --------------------------------------------------
# MOTORER
# --------------------------------------------------

left_in1 = OutputDevice(17)
left_in2 = OutputDevice(27)

right_in1 = OutputDevice(22)
right_in2 = OutputDevice(23)


def stop():
    left_in1.off()
    left_in2.off()

    right_in1.off()
    right_in2.off()


def forward():
    left_in1.on()
    left_in2.off()

    right_in1.on()
    right_in2.off()


def backward():
    left_in1.off()
    left_in2.on()

    right_in1.off()
    right_in2.on()


def turn_right():
    # Venstre frem
    left_in1.on()
    left_in2.off()

    # Højre tilbage
    right_in1.off()
    right_in2.on()


# --------------------------------------------------
# ENCODERE
# --------------------------------------------------

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


PULSES_PER_REV = 22
WHEEL_DIAMETER_CM = 6.5

WHEEL_CIRCUMFERENCE_CM = 3.14159 * WHEEL_DIAMETER_CM
CM_PER_PULSE = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REV


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


# --------------------------------------------------
# LIDAR
# --------------------------------------------------

PORT = "/dev/serial0"
BAUD = 230400
PACKET_LEN = 47


def parse_packet(data):

    if len(data) != PACKET_LEN or data[0] != 0x54:
        return []

    start_angle = int.from_bytes(data[2:4], "little") / 100.0
    end_angle = int.from_bytes(data[4:6], "little") / 100.0

    points = []

    diff = end_angle - start_angle

    if diff < 0:
        diff += 360.0

    step = diff / 11.0 if diff > 0 else 0

    for i in range(12):

        base_idx = 6 + (i * 3)

        distance = int.from_bytes(
            data[base_idx:base_idx + 2],
            "little"
        )

        angle = (start_angle + (i * step)) % 360.0

        points.append((angle, distance))

    return points


def obstacle_in_front(points):

    for angle, distance in points:

        is_in_front = (
            0.0 <= angle <= 30.0
            or
            330.0 <= angle <= 360.0
        )

        if is_in_front and 0 < distance <= 300:

            print()
            print(
                f"FORHINDRING! "
                f"Vinkel: {angle:.1f}° | "
                f"Afstand: {distance} mm"
            )

            return True

    return False


# --------------------------------------------------
# AUTONOM TEST
# --------------------------------------------------

MAX_RUNTIME = 20


print("Autonom robot-test")
print("------------------")
print("LiDAR grænse: 30 cm")
print(f"1 encoder-puls: ca. {CM_PER_PULSE:.2f} cm")
print(f"Test stopper automatisk efter {MAX_RUNTIME} sekunder")
print()


ser = None

try:

    ser = serial.Serial(
        PORT,
        BAUD,
        timeout=0.1
    )

    stop()

    time.sleep(1)

    print("Robot kører frem...")

    forward()

    start_time = time.time()

    while time.time() - start_time < MAX_RUNTIME:

        # Vis encoder-status
        left_cm = left_count * CM_PER_PULSE
        right_cm = right_count * CM_PER_PULSE

        print(
            f"Venstre: {left_cm:.1f} cm | "
            f"Højre: {right_cm:.1f} cm",
            end="\r"
        )

        # Læs LiDAR
        header = ser.read(1)

        if header != b"\x54":
            continue

        rest_packet = ser.read(PACKET_LEN - 1)

        full_packet = header + rest_packet

        points = parse_packet(full_packet)

        if obstacle_in_front(points):

            # 1. STOP
            print("STOP")
            stop()
            time.sleep(0.5)

            # 2. BAK
            print("Bak...")
            backward()
            time.sleep(0.7)

            stop()
            time.sleep(0.2)

            # 3. DREJ
            print("Drejer til højre...")
            turn_right()
            time.sleep(0.7)

            stop()
            time.sleep(0.2)

            # 4. FORTSÆT
            print("Fortsætter frem...")
            forward()


except KeyboardInterrupt:

    print()
    print("Test afbrudt med Ctrl+C")


except Exception as e:

    print()
    print(f"FEJL: {e}")


finally:

    stop()

    if ser is not None:
        ser.close()

    print()
    print("Motorer stoppet sikkert")
    print(f"Venstre encoder: {left_count} pulser")
    print(f"Højre encoder:   {right_count} pulser")
