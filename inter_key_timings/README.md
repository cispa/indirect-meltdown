# Inter Keystroke Timings

## Build
Just run `make`.

## Record Keystroke Timings
To record the keystroke timings, you must run the following command:
```bash
taskset -c $CPU ./spy 32 > probe&; ./keylog > truth
python3 ./results.py truth probe
```
whereas offset is the offset of the keyboard interrupt in the IDT.
If you are unsure how you can find this or how `./spy` should be used please check the folder `idt_reader`.

## FAQ 
### What value should I set for $CPU?
You need to configure the CPU that is responsible for handling keyboard interrupts. You can identify that CPU using `idt_reader/find.sh`.

### It still does not work on my machine?
This attack only works if your keyboard is using interrupts to communicate with the OS. If this is not the case for your keyboard, it will not work. A typical example for a working keyboard would be integrated keyboards of most laptops.
