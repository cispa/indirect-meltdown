RESULTS := results/isolated_1 results/isolated_2 results/isolated_3 results/noise_1 results/noise_2 results/noise_3

all: spy keylog $(RESULTS)

spy: main.c cacheutils.h
	gcc main.c -Os -o spy

keylog: keylog.c
	gcc keylog.c -o keylog

results/%: results.py data/truth_% data/probe_%
	python $^ > "$@"

clean:
	rm -f spy keylog
