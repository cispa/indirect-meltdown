#!/bin/bash

echo "Starting in 1 second, press keys!"
sleep 1
echo "Let's go"

for c in $(seq 1 `nproc`);
do
	for irq in $(seq 32 250);
	do
		echo $c:$irq
		timeout 0.5 taskset -c $c ./spy $irq | grep -c "Hit 2"
	done
done
