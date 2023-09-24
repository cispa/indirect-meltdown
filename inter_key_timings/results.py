#!/usr/bin/env python3
import sys
import statistics

with open(sys.argv[1], "r") as truth:
    with open(sys.argv[2], "r") as probe:
        timings_truth=[int(x) for x in truth.read().strip().split("\n")]
        timings_probe=[int(x) for x in probe.read().strip().split("\n")]

        # matching
        m={}
        for tp in timings_probe:
            # search for nearest timestamp in ground truth data and match
            lowest_diff_t=0
            lowest_diff=10000000000000
            for t in timings_truth:
                diff=abs(tp-t)
                if diff<lowest_diff:
                    lowest_diff=diff
                    lowest_diff_t=t

            m[lowest_diff_t]=m.get(lowest_diff_t, [])+[tp]

        false_pos=0
        true_pos=0
        false_neg=0

        delays=[]
        for true in timings_truth:
            if true not in m:
                # no match found but expected two
                false_neg+=2
            else:
                matched_timestamps=m[true]
                assert(len(matched_timestamps)>0)

                # search nearest element in matched timestamps
                lowest_diff=1000000000000000
                lowest_diff_t=0
                for t in matched_timestamps:
                    diff=abs(true-t)
                    if diff<lowest_diff:
                        lowest_diff=diff
                        lowest_diff_t=t

                # check if we really detected the key down
                if lowest_diff<30*1000:
                    delays+=[lowest_diff_t-true]

                    if len(matched_timestamps)<2:
                        # we are missing interrupts, we are expecting 2
                        false_neg+=2-len(matched_timestamps)
                        true_pos+=len(matched_timestamps)
                    elif len(matched_timestamps)==2:
                        true_pos+=2
                    else:
                        # we have more interrupts but expecting 2
                        true_pos+=2
                        false_pos+=len(matched_timestamps)-2
                else:
                    # we don't have key up data so just assume these are false positives
                    false_pos+=len(matched_timestamps)
                    # we are expecting 2
                    false_neg+=2

        recall = true_pos / (true_pos + false_neg)
        precision = true_pos / (true_pos + false_pos)
        f_score = 2 * ((precision * recall) / (precision + recall))

        print("recall:   ", recall)
        print("precision:", precision)
        print("f-score:  ", f_score)

        print()
        print("delay:")
        print("mean:   ", statistics.mean(delays))
        print("median: ", statistics.median(delays))
        print("std dev:", statistics.stdev(delays))
        print("worst:  ", max([(abs(x), x) for x in delays]))
