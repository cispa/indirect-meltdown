# LeakIDT PoC

## Build
Just run `make`.

## Run
Execute the spy process, i.e., the one monitoring the IDT, execute `./spy <idt-offset>`.

To check whether the exploit is working on your machine, you can run an example by running two terminals like the following:
```bash
# Terminal 1:
taskset -c $CPU0 ./spy 120

# Terminal 2:
taskset -c $CPU1 ./inject 100 5000
```
Note that `$CPU0` and `$CPU1` must be a pair of sibling hyperthreads of the same physical CPU core.

## Generating Interrupts
You have two possibilities of generating interrupts:

First, you can directly create them using either `./inject-single` or `./inject`.
The latter requires two command line arguments for the number of interrupts you want to inject and the number of CPU cycles you want to wait between injecting them, e.g., `./inject 100 5000`.
Note that the injection process and the spy need to run on sibling hyperthreads (see FAQ).

Second, you can generate interrupts by normal system activity, such as keystrokes.
The script `./find.sh` acts as a helper to identify which offset of the IDT is used to process keyboard interrupts. 

## FAQ
### Why does the spy process not detect anything?
There could be multiple reasons: First, if your CPU is not affected by Meltdown, our attack does no work. You can check whether your system is affected by executing: `cat /sys/devices/system/cpu/vulnerabilities/meltdown`.
Second, the spy only detects interrupts on the core it is running on, hence you should pin it to a specific core using `taskset`.

### How can I pin the process to a specific core?
The `taskset` utility allows this. For example, to execute the spy on core 3, you can execute `tasket -c 3 ./spy`.

### How can I execute two processes on sibling hyperthreads?
First, you need to figure out which pairs of hyperthreads exist on your system.
You can find that information by executing `lscpu -e`.
An example output would be the following:
```
# lscpu -e
CPU NODE SOCKET CORE L1d:L1i:L2:L3 ONLINE    MAXMHZ   MINMHZ       MHZ
  0    0      0    0 0:0:0:0          yes 4700.0000 400.0000 1104.7870
  1    0      0    1 1:1:1:0          yes 4700.0000 400.0000 1086.7960
  2    0      0    2 2:2:2:0          yes 4700.0000 400.0000 1299.3500
  3    0      0    3 3:3:3:0          yes 4700.0000 400.0000 1090.3840
  4    0      0    0 0:0:0:0          yes 4700.0000 400.0000 1239.9270
  5    0      0    1 1:1:1:0          yes 4700.0000 400.0000 1265.5720
  6    0      0    2 2:2:2:0          yes 4700.0000 400.0000 1281.8730
  7    0      0    3 3:3:3:0          yes 4700.0000 400.0000 1067.1121
```
Two CPUs that share the same core are considered a sibling hyperthreads, i.e., core 3 and core 7 in the above example.
