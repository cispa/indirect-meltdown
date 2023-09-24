# LeakIDT Mitigation

This folder contains a PoC for the mitigation against our attack.


## Dependencies
The mitigation relies on [PTEditor](https://github.com/misc0110/PTEditor) to manipulate the page-table entries.
Hence, you need to install and load PTEditor as described in the official repository.

## Build
Just run `make`.

## Protect the system
To protect your system, all you need to do is execute `./idt-protector`.
