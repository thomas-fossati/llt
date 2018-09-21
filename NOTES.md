
TFT matches on LLT DSCP:

```
$ env NS_LOG="EpcTft:EpcTftClassifier" make run
[...]
+10.010000016s 0 EpcTftClassifier:Classify(0x1f998c8, 0x20b86d0, 200, DOWNLINK)
+10.010000016s 0 EpcTftClassifier:Classify(): [INFO ] local address: 7.0.0.2 remote address: 1.0.0.2
+10.010000016s 0 EpcTftClassifier:Classify(): [INFO ] Classifying packet: localAddr=7.0.0.2 remoteAddr=1.0.0.2 localPort=1234 remotePort=49153 tos=0x20
+10.010000016s 0 EpcTftClassifier:Classify(): [LOGIC] TFT MAP size: 2
+10.010000016s 0 EpcTftClassifier:Classify(): [LOGIC] TFT id: 2
+10.010000016s 0 EpcTftClassifier:Classify(): [LOGIC]  Ptr<EpcTft>: 0x2001920
+10.010000016s 0 EpcTft:Matches(0x2001920, DOWNLINK, 1.0.0.2, 7.0.0.2, , 49153, 1234, 20)
+10.010000016s 0 EpcTft:Matches(0x2001ba0, DOWNLINK, 1.0.0.2, 7.0.0.2, 49153, 1234, 20)
+10.010000016s 0 EpcTft:Matches(): [LOGIC] d matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] ra matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] ls matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] rps matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] rpe matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] lps matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] lpe matches
+10.010000016s 0 EpcTft:Matches(): [LOGIC] tos matches --> have match!
+10.010000016s 0 EpcTftClassifier:Classify(): [LOGIC] matches with TFT ID = 2
```

but I can't see any activation of a dedicated bearer happening as a result:
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
==> +0.000000000s -1 LteHelper:ActivateDedicatedEpsBearer(0x149a970)
==> +0.000000000s -1 LteHelper:ActivateDedicatedEpsBearer(0x149a970)
+0.000000000s -1 LteHelper:EnableDlPhyTraces()
+0.000000000s -1 LteHelper:EnableUlPhyTraces()
+0.000000000s -1 LteHelper:EnableDlMacTraces()
+0.000000000s -1 LteHelper:EnableUlMacTraces()
```

Note that the two ActivateDedicatedEpsBearer calls correspond, in fact, to one logical activation of the default bearer which happens at attachment (+0.00000000s).
