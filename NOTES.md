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
