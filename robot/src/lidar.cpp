#include "../include/lidar.hpp"
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <cmath>

int open_lidar_port() {
    int fd = open("/dev/serial0", O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        std::cerr << "Kunne ikke åbne LiDAR-port." << std::endl;
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, B230400);
    cfsetispeed(&tty, B230400);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

struct LidarPoint {
    double angle;
    int distance;
};

std::vector<LidarPoint> parse_packet(const unsigned char* data, size_t len) {
    std::vector<LidarPoint> points;
    if (len != 47 || data[0] != 0x54) return points;

    unsigned short start_angle_raw = data[2] | (data[3] << 8);
    unsigned short end_angle_raw = data[4] | (data[5] << 8);

    double start_angle = start_angle_raw / 100.0;
    double end_angle = end_angle_raw / 100.0;

    double diff = end_angle - start_angle;
    if (diff < 0) diff += 360.0;

    double step = (diff > 0) ? (diff / 11.0) : 0.0;

    for (int i = 0; i < 12; ++i) {
        int base_idx = 6 + (i * 3);
        int distance = data[base_idx] | (data[base_idx + 1] << 8);
        double angle = fmod(start_angle + (i * step), 360.0);
        points.push_back({angle, distance});
    }

    return points;
}

std::string check_obstacle_cpp(int fd) {
    if (fd < 0) return "";

    unsigned char header[1];
    int n = read(fd, header, 1);
    if (n > 0 && header[0] == 0x54) {
        unsigned char rest[46];
        int total_read = 0;
        while (total_read < 46) {
            int r = read(fd, rest + total_read, 46 - total_read);
            if (r <= 0) break;
            total_read += r;
        }

        if (total_read == 46) {
            unsigned char full_packet[47];
            full_packet[0] = 0x54;
            memcpy(full_packet + 1, rest, 46);

            auto points = parse_packet(full_packet, 47);

            bool left_blocked = false;
            bool right_blocked = false;
            bool center_blocked = false;

            for (const auto& p : points) {
                if (p.distance > 0 && p.distance <= 300) {
                    if (p.angle >= 315.0 && p.angle <= 360.0) {
                        left_blocked = true;
                    } else if (p.angle >= 0.0 && p.angle <= 45.0) {
                        right_blocked = true;
                    } else if (p.angle >= 350.0 || p.angle <= 10.0) {
                        center_blocked = true;
                    }
                }
            }

            if (center_blocked) return "center";
            if (left_blocked) return "left";
            if (right_blocked) return "right";
        }
    }
    return "";
}