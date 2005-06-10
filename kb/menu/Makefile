TARGT := menu_ctrl
OBJ := menu_ctrl.o
RM_STUFFS := $(OBJ)
CFLAGS := -g

all: $(TARGT)

$(TARGT): $(OBJ)
	$(MAKE) -C ../KbHit/ kbhit.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ../KbHit/kbhit.o $(OBJ)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(RM_STUFFS)
	if [ -f $(TARGT) ]; \
	then \
		rm $(TARGT); \
	fi;
	$(MAKE) -C ../KbHit/ clean
