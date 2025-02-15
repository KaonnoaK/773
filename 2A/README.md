1. make
2. taskset -c 0 ./sender
3. taskset -c 1 ./receiver

alternative way to compile 
gcc utils.c sender.c -o sender
gcc utils.c receiver.c -o receiver 
taskset -c 0 ./sender
taskset -c 1 ./receiver

#Calibration

we used the code calibration code that was given in task 1. 

# CS773 Assignment 2A

# Task 2A

## Team Members
Bhavani Onkar | 24M1094
Mohith sai babu kota | 23M1169
Satvikkumar Patel | 23M1117

## Task 2A
### Approach

1. syncing the sender and receiver before they start their respective job
2. this syncing involves a jitter - a window that bounds the sender and receiver's interaction , this bound defines the tolerance band for synchronisation
3. every bit the sender is sending, is sent over an agreed interval of time. Single bit is sent multiple times within that interval
4. The receiver listens during this interval ( given the jitter ) , and consider the most likely occuring case ie either miss or hit

sending bit - the sender flushes the cacheline if bit is 1 otherwise nothing
receiving bit - if its miss then interpreted as 1 otherwise 0.

the cacheline - an agreeed virtual address by both parties that maps to the first set of the cache. 

### Results
- **Accuracy:** [40-70 %]
- **Bandwidth:** [~50kbps]

You can also mention accuracy vs bandwidths that you observed while tuning.


## Plagiarism Checklist
Your answers should be written in a new line below each bullet.

1. Have you strictly adhered to the submission guidelines? - No

2. Have you received or provided any assistance for this assignment beyond public discussions with peers/TAs? - No

3. If you answered "yes" to the previous question, please specify what part of code you've taken and from whom you've taken it.

