import serial

PORT = '/dev/serial0'
BAUD = 230400

def read_lidar():
    print(f"Forsøger at forbinde til LiDAR på {PORT} ({BAUD} baud)...")
    
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        print("Forbindelse oprettet! Lytter efter data (tryk Ctrl+C for at stoppe)...\n")
    except Exception as e:
        print(f"Fejl ved åbning af seriel port: {e}")
        return

    while True:
        header = ser.read(1)
        if header == b'\x54':
            
            packet = ser.read(46)
            
            if len(packet) == 46:
                start_angle_raw = int.from_bytes(packet[2:4], byteorder='little')
                end_angle_raw = int.from_bytes(packet[4:6], byteorder='little')
                
                start_angle = start_angle_raw / 100.0
                end_angle = end_angle_raw / 100.0

                print(f"Modtaget LiDAR-pakke -> Vinkelinterval: fra {start_angle:.1f}° til {end_angle:.1f}°")

if __name__ == '__main__':
    read_lidar()