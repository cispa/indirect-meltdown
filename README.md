# Indirect Meltdown

This repository contains the proof-of-concept implementation for the paper ["Indirect Meltdown: Building Novel Side-Channel Attacks from Transient Execution Attacks" (ESORICS 2023)](https://publications.cispa.saarland/4011/1/masc_esorics23.pdf).

## LeakIDT PoC
The folder `idt_reader` contains a PoC for LeakIDT.
The attacker code monitors the interrupt-descriptor table for a given offset and prints a message when it occurs.


## LeakIDT Mitigation PoC
The folder `mitigation` contain the PoC for our proposed mitigation.

## Case Studies

### Website Fingerprinting
The folder `website_fp` contains the code we used to fingerprint websites based on leaked network interrupts.

### Observing Keystroke Timings
The folder `inter_key_timings` contains code that gathers the timings when you time.

## Contact
If there are questions regarding this tool, please send an email to `daniel.weber (AT) cispa` or message `@weber_daniel` on Twitter.

## Research Paper
You can find the paper [here](https://publications.cispa.saarland/4011/1/masc_esorics23.pdf).
You can cite our work with the following BibTeX entry:
```latex
@inproceedings{Weber2023Indirect,
 author={Weber, Daniel and Thomas, Fabian and Gerlach, Lukas and Zhang, Ruiyi and Schwarz, Michael},
 booktitle = {ESORICS},
 title={Indirect Meltdown: Building Novel Side-Channel Attacks from Transient Execution Attacks},
 year = {2023}
}
```

## Disclaimer
We are providing this code as-is. 
You are responsible for protecting yourself, your property and data, and others from any risks caused by this code. 
This code may cause unexpected and undesirable behavior to occur on your machine. 
