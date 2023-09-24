all: spy keylog

spy: main.c cacheutils.h
	gcc main.c -Os -o spy

keylog: keylog.c
	gcc keylog.c -o keylog

clean:
	rm -f spy keylog
