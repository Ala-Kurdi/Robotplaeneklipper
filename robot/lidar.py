import serial

PORT = '/dev/serial0'
BAUD = 230400
PACKET_LEN = 47

def open_lidar():
    """Åbner den serielle forbindelse til LiDAR'en."""
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        return ser
    except Exception as e:
        print(f"Kunne ikke åbne LiDAR-port: {e}")
        return None

def parse_packet(data):
    """Parser en 47-bytes datapakke og udtrækker vinkler og afstande."""
    if len(data) != PACKET_LEN or data[0] != 0x54:
        return []

    start_angle = int.from_bytes(data[2:4], 'little') / 100.0
    end_angle = int.from_bytes(data[4:6], 'little') / 100.0

    points = []
    diff = end_angle - start_angle
    if diff < 0:
        diff += 360.0  

    step = diff / 11.0 if diff > 0 else 0

    for i in range(12):
        base_idx = 6 + (i * 3)
        distance = int.from_bytes(data[base_idx:base_idx+2], 'little')
        angle = (start_angle + (i * step)) % 360.0
        points.append((angle, distance))

    return points

def check_obstacle(ser):
    """
    Læser LiDAR og returnerer en retning for forhindringen:
    'left', 'right', 'center' eller None hvis der er fri bane.
    """
    if not ser:
        return None

    header = ser.read(1)
    if header == b'\x54':
        rest_packet = ser.read(PACKET_LEN - 1)
        full_packet = header + rest_packet
        
        points = parse_packet(full_packet)
        
        left_blocked = False
        right_blocked = False
        center_blocked = False

        for angle, distance in points:
            if 0 < distance <= 300: # Forhindring inden for 30 cm
                if 315.0 <= angle <= 360.0:
                    left_blocked = True
                elif 0.0 <= angle <= 45.0:
                    right_blocked = True
                elif 350.0 <= angle <= 10.0:
                    center_blocked = True
                
        if center_blocked:
            return "center" # Blokeret i midten, bak og drej tilfældigt
        elif left_blocked:
            return "left"   # Forhindring til venstre -> drej mod højre!
        elif right_blocked:
            return "right"  # Forhindring til højre -> drej mod venstre!
            
    return None