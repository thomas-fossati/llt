# simple

## network characterisation

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

## experiment description

Two concurrent applications on the UE:
- one way real-time audio (64kbps)
- one download over TCP (greedy, non application limited)
resulting in two downlink flows.

- control (CTL): no marking
- experimental (EXP): LLT marking applied to the real-time traffic (honest marking)
  
## results

### real-time flow latency (ms)

| run | mean  | min | max | stddev   |
|-----|-------------|-----|----------|
| CTL | 15.48 | 5   | 24  | 5.18609  |
| EXP | 4.32  | 4   | 6   | 0.509243 |

CTL->EXP (mean): -72%

### real-time flow packet loss

| run | lost pkt |
|-----|----------|
| CTL | 0        |
| EXP | 0        |

CTL->EXP: +0%

### TCP flow throughput (Mbps)

| run | throughput |
|-----|------------|
| CTL | 3.8073840  |  (86.5% of total downlink theoretical bandwidth)
| EXP | 3.7925440  |  (86.2% of total downlink theoretical bandwidth)

-0.389768933209783
CTL->EXP: -0.39%
