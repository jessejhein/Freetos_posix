TARGT := kbhit
OBJ := kbhit.o
RM_STUFFS := $(OBJ)

CC := gcc
CFLAGS := -Wall -O2

all: $(TARGT)

$(TARGT): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

%.o : %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm $(RM_STUFFS)
	if [ -f $(TARGT) ]; \
	then \
		rm $(TARGT); \
	fi;
