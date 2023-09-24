all: main.c cacheutils.h inject-single inject
	gcc main.c -Os -o spy
	
inject: inject.c
	gcc inject.c -o inject

inject-single: inject-single.c
	gcc inject-single.c -o inject-single

clean:
	rm -f spy inject inject-single
	
