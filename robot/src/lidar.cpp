#include "lidar.hpp"

#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

namespace {

constexpr int PACKET_SIZE = 47;
constexpr int POINT_COUNT = 12;

// Vi filtrerer meget korte målinger væk,
// fordi robot/chassis gav faste målinger omkring 90 mm.
constexpr int MIN_DISTANCE_MM = 120;

// FK4: hindring ved højst 30 cm.
constexpr int MAX_DISTANCE_MM = 300;

// Mindst 2 målepunkter skal registrere hindringen.
// Det reducerer enkelte fejlmålinger.
constexpr int REQUIRED_HITS = 2;

// Efter fysisk test ligger området foran robotten
// omtrent i dette vinkelområde på vores LD06-montering.
constexpr double FRONT_MIN_ANGLE = 130.0;
constexpr double FRONT_MAX_ANGLE = 175.0;

struct LidarPoint {
    double angle;
    int distance;
    int confidence;
};


// ---------------------------------------------------------
// Hjælpefunktion: normaliser vinkel til 0-360 grader
// ---------------------------------------------------------
double normalize_angle(double angle)
{
    while (angle < 0.0) {
        angle += 360.0;
    }

    while (angle >= 360.0) {
        angle -= 360.0;
    }

    return angle;
}


// ---------------------------------------------------------
// Parse én komplet LD06-pakke
//
// LD06 packet:
// byte 0      = 0x54 header
// byte 1      = VerLen
// byte 2-3    = speed
// byte 4-5    = start angle
// byte 6-41   = 12 målepunkter
// byte 42-43  = end angle
// byte 44-45  = timestamp
// byte 46     = CRC
// ---------------------------------------------------------
std::vector<LidarPoint> parse_packet(
    const unsigned char* data,
    size_t len)
{
    std::vector<LidarPoint> points;

    if (len != PACKET_SIZE) {
        return points;
    }

    if (data[0] != 0x54) {
        return points;
    }

    // LD06 VerLen er normalt 0x2C.
    if (data[1] != 0x2C) {
        return points;
    }

    unsigned short start_angle_raw =
        static_cast<unsigned short>(
            data[4] |
            (static_cast<unsigned short>(data[5]) << 8)
        );

    unsigned short end_angle_raw =
        static_cast<unsigned short>(
            data[42] |
            (static_cast<unsigned short>(data[43]) << 8)
        );

    double start_angle =
        static_cast<double>(start_angle_raw) / 100.0;

    double end_angle =
        static_cast<double>(end_angle_raw) / 100.0;

    if (start_angle >= 360.0 ||
        end_angle >= 360.0) {
        return points;
    }

    double angle_diff =
        end_angle - start_angle;

    if (angle_diff < 0.0) {
        angle_diff += 360.0;
    }

    // 12 punkter giver 11 intervaller.
    double angle_step =
        angle_diff /
        static_cast<double>(POINT_COUNT - 1);

    for (int i = 0; i < POINT_COUNT; ++i) {

        int base =
            6 + (i * 3);

        int distance =
            static_cast<int>(
                data[base] |
                (static_cast<unsigned short>(
                    data[base + 1]) << 8)
            );

        int confidence =
            static_cast<int>(
                data[base + 2]
            );

        double angle =
            normalize_angle(
                start_angle +
                static_cast<double>(i) *
                angle_step
            );

        points.push_back({
            angle,
            distance,
            confidence
        });
    }

    return points;
}


// ---------------------------------------------------------
// Læs én komplet LD06-pakke fra serial
// ---------------------------------------------------------
bool read_ld06_packet(
    int fd,
    unsigned char* packet)
{
    static std::vector<unsigned char> buffer;

    unsigned char temp[256];

    ssize_t bytes_read =
        read(
            fd,
            temp,
            sizeof(temp)
        );

    if (bytes_read > 0) {

        buffer.insert(
            buffer.end(),
            temp,
            temp + bytes_read
        );
    }

    while (buffer.size() >= PACKET_SIZE) {

        // Find første header 0x54.
        auto header =
            std::find(
                buffer.begin(),
                buffer.end(),
                static_cast<unsigned char>(0x54)
            );

        // Ingen header fundet.
        if (header == buffer.end()) {
            buffer.clear();
            return false;
        }

        // Fjern støj før header.
        if (header != buffer.begin()) {

            buffer.erase(
                buffer.begin(),
                header
            );
        }

        if (buffer.size() < PACKET_SIZE) {
            return false;
        }

        // Ekstra kontrol af LD06 VerLen.
        if (buffer[1] != 0x2C) {

            buffer.erase(
                buffer.begin()
            );

            continue;
        }

        // Kopier de 47 bytes.
        std::copy(
            buffer.begin(),
            buffer.begin() + PACKET_SIZE,
            packet
        );

        // Fjern pakken fra bufferen.
        buffer.erase(
            buffer.begin(),
            buffer.begin() + PACKET_SIZE
        );

        return true;
    }

    return false;
}

} // namespace


// =========================================================
// ÅBN LIDAR
//
// Denne funktion bruges af main.cpp.
// =========================================================
int open_lidar_port()
{
    const char* port =
        "/dev/serial0";

    int fd =
        open(
            port,
            O_RDONLY |
            O_NOCTTY |
            O_NONBLOCK
        );

    if (fd < 0) {

        std::cerr
            << "[LiDAR] FEJL: kunne ikke åbne "
            << port
            << std::endl;

        return -1;
    }

    struct termios tty {};

    if (tcgetattr(fd, &tty) != 0) {

        std::cerr
            << "[LiDAR] FEJL: tcgetattr fejlede."
            << std::endl;

        close(fd);

        return -1;
    }

    // LD06 bruger 230400 baud.
    cfsetispeed(
        &tty,
        B230400
    );

    cfsetospeed(
        &tty,
        B230400
    );

    // 8N1
    tty.c_cflag &=
        ~PARENB;

    tty.c_cflag &=
        ~CSTOPB;

    tty.c_cflag &=
        ~CSIZE;

    tty.c_cflag |=
        CS8;

    tty.c_cflag &=
        ~CRTSCTS;

    tty.c_cflag |=
        CREAD | CLOCAL;

    // Raw serial mode.
    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(
            fd,
            TCSANOW,
            &tty) != 0) {

        std::cerr
            << "[LiDAR] FEJL: tcsetattr fejlede."
            << std::endl;

        close(fd);

        return -1;
    }

    tcflush(
        fd,
        TCIFLUSH
    );

    std::cout
        << "[LiDAR] LD06 åbnet på "
        << port
        << " @ 230400 baud."
        << std::endl;

    return fd;
}


// =========================================================
// KONTROLLER FOR HINDRING
//
// Returnerer:
// ""       = ingen hindring
// "center" = hindring foran robotten
//
// main.cpp kan derefter:
// STOP -> BAK -> DREJ -> FORTSÆT
// =========================================================
std::string check_obstacle_cpp(int lidar_fd)
{
    if (lidar_fd < 0) {
        return "";
    }

    unsigned char packet[PACKET_SIZE];

    int front_hits = 0;

    int packets_checked = 0;

    // Læs op til 10 allerede tilgængelige pakker.
    // Serial-porten er non-blocking.
    while (
        packets_checked < 10 &&
        read_ld06_packet(
            lidar_fd,
            packet
        )
    ) {

        packets_checked++;

        std::vector<LidarPoint> points =
            parse_packet(
                packet,
                PACKET_SIZE
            );

        for (const auto& p : points) {

            // -------------------------------------------------
            // Afstandsfilter
            //
            // < 120 mm:
            // faste ekkoer fra robot/chassis filtreres væk.
            //
            // > 300 mm:
            // uden for vores FK4 hindringsgrænse.
            // -------------------------------------------------
            if (
                p.distance < MIN_DISTANCE_MM ||
                p.distance > MAX_DISTANCE_MM
            ) {
                continue;
            }

            // Ignorer helt ugyldige målinger.
            if (p.confidence == 0) {
                continue;
            }

            // -------------------------------------------------
            // DEBUG
            //
            // Beholder vi under testen.
            // Kan fjernes senere.
            // -------------------------------------------------
            std::cout
                << "[LIDAR DEBUG] angle="
                << p.angle
                << " distance="
                << p.distance
                << " mm"
                << std::endl;

            // -------------------------------------------------
            // FRONTZONE
            //
            // Målt på vores fysiske prototype.
            //
            // Hindringer foran robotten blev observeret
            // omkring 130-175 grader.
            // -------------------------------------------------
            if (
                p.angle >= FRONT_MIN_ANGLE &&
                p.angle <= FRONT_MAX_ANGLE
            ) {

                front_hits++;
            }
        }
    }

    // Mindst to målepunkter skal bekræfte hindringen.
    if (front_hits >= REQUIRED_HITS) {

        std::cout
            << "[LiDAR] Hindring foran registreret"
            << " (" << front_hits << " hits)"
            << std::endl;

        return "center";
    }

    return "";
}
