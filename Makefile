CFLAGS = -Wall -Werror -g
LDFLAGS = -lSDL2 -lSDL2_ttf -lm -lGL

TARGET = anonymity

SRCS = auxdata.c examples.c main.c render.c vector.c entity.c graphics.c network.c stabilizing_algo.c events.c history_tree.c queue.c terminating_algo.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean