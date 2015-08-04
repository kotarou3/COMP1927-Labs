DIRS = $(basename $(wildcard */))

.PHONY: all clean $(DIRS)

all: ACTION = all
all: $(DIRS)

clean: ACTION = clean
clean: $(DIRS)

$(DIRS):
	$(MAKE) $(ACTION) -C $@
