TARGT := kbhit
OBJ := kbhit.o
RM_STUFFS := $(OBJ)

all: $(TARGT)

$(TARGT): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(RM_STUFFS)
	if [ -f $(TARGT) ]; \
	then \
		rm $(TARGT); \
	fi;
