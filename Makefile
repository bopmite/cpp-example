CXX = clang++
CXXFLAGS = -std=c++17 -isystem $(PWD)/include
LDFLAGS = -lpthread

TARGET = main
SRC_DIR = src
BUILD_DIR = build

SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/server/http_worker.cpp \
          $(SRC_DIR)/server/controllers/test_controller.cpp

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

.PHONY: all clean
