
gcc utils.c sender.c -o sender

gcc utils.c receiver.c -o receiver 

taskset -c 0 ./sender

taskset -c 1 ./receiver

#Calibration

we tried to run fill the cache to various capacities in a seemingly isolated environment to measure the number of accesses in a given time.

# CS773 Assignment 2B

# Task 2B

## Team Members
Bhavani Onkar | 24M1094

Mohith sai babu kota | 23M1169

Satvikkumar Patel | 23M1117

## Task 2B
### Approach

1. syncing the sender and receiver before they start their respective job
2. this syncing involves a jitter - a window that bounds the sender and receiver's interaction , this bound defines the tolerance band for synchronisation
3. given within a window , the number of accesses is measured by the reciever and it judges based on the accesses to decode it as 0 or 1 



### Results
- **Accuracy:** [0 %]
- **Bandwidth:** [none]

You can also mention accuracy vs bandwidths that you observed while tuning.


## Plagiarism Checklist
Your answers should be written in a new line below each bullet.

1. Have you strictly adhered to the submission guidelines? - No

2. Have you received or provided any assistance for this assignment beyond public discussions with peers/TAs? - No

3. If you answered "yes" to the previous question, please specify what part of code you've taken and from whom you've taken it.

