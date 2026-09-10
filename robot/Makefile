# Compiler og flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2 -Iinclude

# Navn på det færdige program
TARGET = robot_app

# Mapper
SRC_DIR = src

# Kildefiler
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/motor.cpp \
       $(SRC_DIR)/network.cpp \
       $(SRC_DIR)/lidar.cpp \
       $(SRC_DIR)/encoders.cpp \
       $(SRC_DIR)/navigation.cpp \
       $(SRC_DIR)/config.cpp

OBJS = $(SRCS:.cpp=.o)

# Standard mål
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) -lmosquitto

# Regel for at kompilere .cpp til .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Slet kompilerede filer
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

# Fortæller Make at 'all' og 'clean' er kommandoer og ikke filer
.PHONY: all clean