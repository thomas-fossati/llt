## Network characterisation

```
    (|)           ((|))
.----.             / \               .---------          .--------.
| UE |--[LTE-uu]--/eNB\--[S1/S5/S8]--| SGW+PGW |--[SGi]--| server |
'----'           '-----'             '---------'         '--------'
```

LTE-uu:
  - FDD SISO, 6 RB (i.e., nominal downlink peak at 4.4Mbps)
  - (measured) baseline latency: ~3ms

S1/S5/S8:
  - data rate: 5Mbps
  - propagation latency: 0ms

SGi:
  - data rate: 10Gbps (basically unlimited)
  - propagation latency: 1ms

eNB:
  - proportional fair MAC scheduler

## Experiment one: the honest marker

Two concurrent applications on the UE, resulting in two downlink flows.

- one way real-time audio (64kbps)
- one download over TCP (greedy, non application limited)

Control and experimental group are as follows:

- control (CTL): no marking
- experimental (EXP): LLT marking applied to the real-time traffic (i.e., honest marking)
  
### Results

- Real-time flow latency (ms)

| run | mean  | min | max | stddev   |
|-----|-------|-----|-----|----------|
| CTL | 15.48 | 5   | 24  | 5.18609  |
| EXP | 4.32  | 4   | 6   | 0.509243 |

CTL->EXP (mean): -72%

- Real-time flow packet loss

| run | lost pkt |
|-----|----------|
| CTL | 0        |
| EXP | 0        |

CTL->EXP: +0%

- TCP flow throughput (Mbps)

| run | throughput |
|-----|------------|
| CTL | 3.8073840  |  (86.5% of total downlink theoretical bandwidth)
| EXP | 3.7925440  |  (86.2% of total downlink theoretical bandwidth)

CTL->EXP: -0.39%

### Summary

Increased stability of the real-time flow with minimal decrease in efficiency
of the throughput seeking flow (and therefore of the RAN as a whole).

## Experiment two: the liar

Two concurrent downloads on the UE, i.e. two TCP greedy sender (non application limited)

Control and experimental group are as follows:

- control (CTL): no marking
- experimental (EXP): TFT configured to route one of the flows (the liar) to
  port 5688 onto the low-latency bearer
  
### Results

- retransmission

| run | TCP:5687 | TCP:5688
|-----|----------|-----------
| CTL | 24       | 25
| EXP | 31       | 140

Liar ends up retransmitting a lot more (+460%) which implies a substantial
decrease in throughput:

- TCP flow throughput (Mbps)

| run | TCP:5687 | TCP:5688
|-----|----------|-----------
| CTL | 2.01904  | 1.837216
| EXP | 2.399816 | 1.332176

Liar gets -27.5% throughput (honest gets a 18.85% boost as a consequence)

### Summary

A throughput seeking flow that marks itself inconsistently is heavily penalised
due to the increase in packet loss (basically, QCI 7's delay and packet drop
budgets are not compatible with the dynamics of a buffer hungry TCP flow).
The aggregate throughput decreases, but because non-honest markers have no
incentives, this is not a real problem.
