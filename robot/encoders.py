from gpiozero import DigitalInputDevice

# -------------------------
# Konfiguration & konstanter
# -------------------------
LEFT_PIN = 24
RIGHT_PIN = 25

PULSES_PER_REV = 22
WHEEL_DIAMETER_CM = 6.5
WHEEL_CIRCUMFERENCE_CM = 3.14159 * WHEEL_DIAMETER_CM
CM_PER_PULSE = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REV

# Interne tællere
left_count = 0
right_count = 0

# Initialiser encoderne via gpiozero
left_encoder = DigitalInputDevice(LEFT_PIN, pull_up=None, active_state=True)
right_encoder = DigitalInputDevice(RIGHT_PIN, pull_up=None, active_state=True)

# -------------------------
# Callback funktioner (Interne)
# -------------------------
def _left_pulse():
    global left_count
    left_count += 1

def _right_pulse():
    global right_count
    right_count += 1

# Knyt pulserne til enhederne
left_encoder.when_activated = _left_pulse
right_encoder.when_activated = _right_pulse

# -------------------------
# Funktioner til main.py
# -------------------------
def get_left_distance_cm():
    """Returnerer hvor langt venstre hjul har kørt i cm."""
    return left_count * CM_PER_PULSE

def get_right_distance_cm():
    """Returnerer hvor langt højre hjul har kørt i cm."""
    return right_count * CM_PER_PULSE

def get_average_distance_cm():
    """Returnerer gennemsnittet af begge hjuls distance (god til at vide robotternes samlede kørsel)."""
    return (get_left_distance_cm() + get_right_distance_cm()) / 2.0

def reset_encoders():
    """Nulstiller tællerne (f.eks. når robotten starter på en ny strækning)."""
    global left_count, right_count
    left_count = 0
    right_count = 0