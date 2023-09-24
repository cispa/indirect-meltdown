all: spy

spy: main.c core_observer.h
	gcc main.c -Os -lpthread -o spy -pthread -lrt

core_observer.h:
	python3 find_core.py

clean:
	rm -f spy


