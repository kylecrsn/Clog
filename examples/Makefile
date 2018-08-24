# Compilation
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGETS = $(SOURCES:.c=)

.PHONY: all clean

.all: $(TARGETS)

$(TARGETS): $(OBJECTS)
	$(MKD) $(OUT)
	for f in $(TARGETS); do $(CC) $(CL_OBJ) $(OUT)/$$f.o -o $(OUT)/$$f; done

$(OBJECTS): $(SOURCES)
	$(MKD) $(OUT)
	$(CC) -c $(CFLAGS) -I $(SRC_DIR) $< -o $(OUT)/$@

clean:
	$(RMD) $(OUT) %.o
