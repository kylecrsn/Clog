# Compilation
OBJECTS = $(CL_SRC:.c=.o)

.PHONY: all clean

all: $(OBJECTS)

%.o: %.c
	$(MKD) $(OUT)
	$(CC) $(CFLAGS) -I $(SRC_DIR) -c $< -o $@

clean:
	$(RMD) $(OUT) %.o
