I can see the activation of the dedicated bearer with QCI 7:
```
$ env NS_LOG="LteHelper" make run
[...]
+0.000000000s -1 LteHelper:DoComponentCarrierConfigure(0x149a970, 18100, 100, 25, 25)
+0.000000000s -1 LteHelper:InstallSingleEnbDevice(): [DEBUG] 0x149a970component carrier map size 1
+0.000000000s -1 LteHelper:InstallSingleEnbDevice(): [LOGIC] set the propagation model frequencies
+0.000000000s -1 LteHelper:InstallSingleEnbDevice(): [LOGIC] DL freq: 2.12e+09
+0.000000000s -1 LteHelper:InstallSingleEnbDevice(): [LOGIC] UL freq: 1.93e+09
+0.000000000s -1 LteHelper:InstallSingleEnbDevice(): [INFO ] adding this eNB to the EPC
+0.000000000s -1 LteHelper:InstallUeDevice(0x149a970)
+0.000000000s -1 LteHelper:InstallSingleUeDevice(0x149a970)
+0.000000000s -1 LteHelper:DoComponentCarrierConfigure(0x149a970, 18100, 100, 25, 25)
+0.000000000s -1 LteHelper:Attach(0x149a970)
+0.000000000s -1 LteHelper:Attach(0x149a970)
+0.000000000s -1 LteHelper:ActivateDedicatedEpsBearer(0x149a970)
+0.000000000s -1 LteHelper:ActivateDedicatedEpsBearer(0x149a970)
+0.000000000s -1 LteHelper:ActivateDedicatedEpsBearer(): [INFO ] dedicated EPS bearer (2) with QCI 7 activated
+0.000000000s -1 LteHelper:EnableDlPhyTraces()
+0.000000000s -1 LteHelper:EnableUlPhyTraces()
+0.000000000s -1 LteHelper:EnableDlMacTraces()
+0.000000000s -1 LteHelper:EnableUlMacTraces()
```
Somewhat surprisingly, this happens at attachment.  For some reason I was expecting it to be triggered by the TFT match.


I also observe TFT matches on LLT DSCP, which is good:

```
$ env NS_LOG="EpcTft:EpcTftClassifier" make run
[...]
+9.990000016s 0 EpcSgwPgwApplication:RecvFromTunDevice(0x2527200, 03-06-00:00:00:00:00:01, 03-06-ff:ff:ff:ff:ff:ff, 0x266db20, 200)
+9.990000016s 0 EpcSgwPgwApplication:RecvFromTunDevice(): [LOGIC] packet addressed to UE 7.0.0.2
+9.990000016s 0 EpcSgwPgwApplication:Classify(0x25358b0, 0x266db20)
+9.990000016s 0 EpcTftClassifier:Classify(0x25358b8, 0x266db20, 200, DOWNLINK)
+9.990000016s 0 EpcTftClassifier:Classify(): [INFO ] local address: 7.0.0.2 remote address: 1.0.0.2
+9.990000016s 0 EpcTftClassifier:Classify(): [INFO ] Classifying packet: localAddr=7.0.0.2 remoteAddr=1.0.0.2 localPort=1234 remotePort=49153 tos=0x20
+9.990000016s 0 EpcTftClassifier:Classify(): [LOGIC] TFT MAP size: 2
+9.990000016s 0 EpcTftClassifier:Classify(): [LOGIC] TFT id: 2
+9.990000016s 0 EpcTftClassifier:Classify(): [LOGIC]  Ptr<EpcTft>: 0x259d9e0
+9.990000016s 0 EpcTft:Matches(0x259d9e0, DOWNLINK, 1.0.0.2, 7.0.0.2, , 49153, 1234, 20)
+9.990000016s 0 EpcTft:Matches(0x259dc60, DOWNLINK, 1.0.0.2, 7.0.0.2, 49153, 1234, 20)

+9.990000016s 0 EpcTft:Matches(): [LOGIC] ls matches
+9.990000016s 0 EpcTft:Matches(): [LOGIC] rps matches
+9.990000016s 0 EpcTft:Matches(): [LOGIC] rpe matches
+9.990000016s 0 EpcTft:Matches(): [LOGIC] lps matches
+9.990000016s 0 EpcTft:Matches(): [LOGIC] lpe matches
+9.990000016s 0 EpcTft:Matches(): [LOGIC] tos matches --> have match!
+9.990000016s 0 EpcTftClassifier:Classify(): [LOGIC] matches with TFT ID = 2
+9.990000016s 0 EpcSgwPgwApplication:SendToS1uSocket(0x2527200, 0x266db20, 10.0.0.5, 2) <== TEID:2
```

This means the downlink traffic originating from my UDP sender is carried over the dedicated low-delay bearer rather than the default one.

# what is the simplest possible experiment?

- using the same topology
- one constant bit-rate sender filling downlink at the requested percentage (e.g., 75..95)
- one real-time sender that can be configured to mark or not
  - the receiver side of this sender is instrumented to measure delay, jitter and packet loss

# on FDD bandwidth

| system bandwidth (MHz) | num usable RBs | SISO DL peak (Mbps) | MIMO 2x2 DL peak (Mbps) | 16 QAM UL peak (Mbps) | 64 QAM UL peak (Mbps) |
|------------------------|----------------|---------------------|-------------------------|-----------------------|-----------------------|
| 1.4                    | 6              | 4.4                 | 8.8                     | 3                     | 4.4                   |
| 3                      | 15             | 11.1                | 22.1                    | 7.5                   | 11.1                  |
| 5                      | 25             | 18.3                | 36.7                    | 12.6                  | 18.3                  |
| 10                     | 50             | 36.7                | 75                      | 25.5                  | 36.7                  |
| 15                     | 75             | 55.1                | 110                     | 37.9                  | 55.1                  |
| 20                     | 100            | 75                  | 150                     | 51                    | 75                    |


- inspect {up,down}link RBs:
```
../../waf --run "llt-simple --PrintAttributes=ns3::LteEnbNetDevice" | grep -A1 '[DU]lBandwidth'
```

- inspect SISO/MIMO Tx/MIMO 2x2:
```
../../waf --run "llt-simple --PrintAttributes=ns3::LteEnbRrc" | grep -A1 DefaultTransmissionMode
```

# real-time A/V characterisation

https://dsp.stackexchange.com/questions/29493/what-is-typical-rtp-packet-size-for-h-264-video-transmission
```
H.264 also implements B-Frames. This type of frame fills in information between
P-Frames. This means that the B-Frame will need to be held until the next
P-frame arrives before the information can be used. B-Frames are not used in
all modes of H.264. The encoder decides what type of frame is best suited.
There are typically more P-frames than I-frames. Lab analysis has shown
TelePresence I-frames to generally be 64K wide (50 packets @ 1316 bytes), while
P- frames average 8K wide (9 packets at 900 bytes).
```
