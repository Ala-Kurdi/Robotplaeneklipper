import serial
import time

PORT = '/dev/serial0'
BAUD = 230400
PACKET_LEN = 47

def parse_packet(data):
    """Parser en 47-bytes datapakke fra LD06 LiDAR'en og udtrækker vinkler og afstande."""
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
        distance = int.from_bytes(data[base_idx:base_idx+2], 'little') # i mm
        
        angle = (start_angle + (i * step)) % 360.0
        points.append((angle, distance))

    return points

def check_obstacles():
    print("Starter LiDAR forhindrings-overvågning (FK4)...")
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print(f"Kunne ikke åbne port: {e}")
        return

    while True:
        header = ser.read(1)
        if header == b'\x54':
            rest_packet = ser.read(PACKET_LEN - 1)
            full_packet = header + rest_packet
            
            points = parse_packet(full_packet)
            
            for angle, distance in points:
                is_in_front = (0.0 <= angle <= 30.0) or (330.0 <= angle <= 360.0)
                
                if is_in_front and 0 < distance <= 300:
                    print(f"FORHINDRING REGISTRERE! Vinkel: {angle:.1f}° | Afstand: {distance} mm")

if __name__ == '__main__':
    check_obstacles()
