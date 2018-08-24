.PHONY: all clean

all: $(CL_OBJ)

$(SRC_DIR)/$(OUT)/%.o: %.c
	$(MKD) $(OUT)
	$(CC) $(CFLAGS) -I $(SRC_DIR) -c $< -o $@

clean:
	$(RMD) $(OUT) %.o
