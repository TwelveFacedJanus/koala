CC=clang
CFLAGS=-I. -I.. -I../math -I/opt/homebrew/opt/glfw/include -I/opt/homebrew/opt/glew/include
LDFLAGS=-L/opt/homebrew/opt/glfw/lib -lglfw -L/opt/homebrew/opt/glew/lib -lglew -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

SRCS=model_rotating_test.c ../types.c ../math/mathemetica.c ../../external/ant_logger/src/ant_logger.c ../async.c
OBJS=$(SRCS:.c=.o)
TARGET=main_graphics

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o 