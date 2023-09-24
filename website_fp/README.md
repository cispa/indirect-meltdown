# Website Fingerprinting

## Dependencies
You need to install the Python library `scikit-learn`.
To do so, execute `pip3 install scikit-learn --user`.

## Build
Just run `make`. This will automatically execute `find_core.py` to find the CPU core responsible for handling network interrupts.

## Collect Traces
To collect the interrupt traces for different websites, execute `./run.sh`. 

# Classification

Before running the classifier, set the path correctly.
To run the classifier, execute the following:
```bash
cd website_classify
python3 ./classify.py
```

## FAQ
### How can I change the set of websites that is used?
We store them as `list_15.txt` and `list.txt`. You can either change the content of these files or change the loaded file in `run.sh`.
