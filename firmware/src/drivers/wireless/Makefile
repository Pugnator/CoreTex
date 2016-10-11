DIRS:=$(wildcard */.)

all:
	for dir in $(DIRS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done

clean:
	for dir in $(DIRS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir clean; \
	done