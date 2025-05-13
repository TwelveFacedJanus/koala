CC = clang
CXX = clang++
CFLAGS = -WALL -fPIC
LDFLAGS = -lpthread -framework OpenCL

BUILD_DIR = build
PROJECT_NAME = koala_mathematica
FILE = core/math/mathematica.c

all:
	$(CC) -o $(BUILD_DIR)/$(PROJECT_NAME) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all