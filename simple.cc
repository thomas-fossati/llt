#include "ns3/applications-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/epc-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/lte-helper.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/point-to-point-helper.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("LLTSimple");

/**
 * Instantiates one UE and one application server.
 * Attach the UE to LTE using a bearer with configurable QCI.
 * Attach the application server to the SGi-LAN.
 * Make traffic flow from the application server to the UE.
 */
int main(int argc, char* argv[]) {
  CommandLine cmd;
  cmd.Parse(argc, argv);

  ConfigStore config;
  config.ConfigureDefaults();
  // Slightly weird, but we need to parse again so that we can override default
  // values from the command line
  cmd.Parse(argc, argv);

  // This will instantiate some common objects (e.g., the Channel object) and
  // provide the methods to add eNBs and UEs and configure them.
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

  // Create EPC entities (PGW & friends) and a point-to-point network topology
  // Also tell the LTE helper that the EPC will be used
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
  lteHelper->SetEpcHelper(epcHelper);

  // Create Node object for the eNodeB
  // (NOTE that the Node instances at this point still don’t have an LTE
  // protocol stack installed; they’re just empty nodes.)
  NodeContainer eNB;
  eNB.Create(1);

  // Create Node object for the UE
  NodeContainer UE;
  UE.Create(1);

  // Configure the Mobility model for all the nodes.  The above will place all
  // nodes at the coordinates (0,0,0).  Refer to the documentation of the ns-3
  // mobility model for how to set a different position or configure node
  // movement.
  MobilityHelper mobility;

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(eNB);

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(UE);

  // Install an LTE protocol stack on the eNB
  NetDeviceContainer eNBDevice;
  eNBDevice = lteHelper->InstallEnbDevice(eNB);

  // Install an LTE protocol stack on the UE
  NetDeviceContainer UEDevice;
  UEDevice = lteHelper->InstallUeDevice(UE);

  // Install the IP protocol stack on the UE
  InternetStackHelper IPStack;
  IPStack.Install(UE);

  Ipv4InterfaceContainer UEIpIface =
      epcHelper->AssignUeIpv4Address(NetDeviceContainer(UEDevice.Get(0)));
  // Set the default gateway for the UE
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> ueStaticRouting =
      ipv4RoutingHelper.GetStaticRouting(UE.Get(0)->GetObject<Ipv4>());
  ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

  // Attach the UE to an eNB. This will configure the UE according to the eNB
  // settings, and create an RRC connection between them.
  // A side-effect of this call is to activate the default bearer.
  lteHelper->Attach(UEDevice, eNBDevice.Get(0));

  // Add a dedicated low-latency bearer for applications marking
  // their traffic with the LLT PHB codepoint for low-latency traffic
  // https://tools.ietf.org/html/draft-you-tsvwg-latency-loss-tradeoff-00#section-4.4
  Ptr<EpcTft> tft = Create<EpcTft>();
  EpcTft::PacketFilter pf;
  // pf.typeOfService = 0x14;  // 000101 (00)
  // XXX Since it looks problematic setting the ToS bits on the UdpClient
  // application, we set the filter on the port.
  pf.localPortStart = 1234;
  pf.localPortEnd = 1234;
  tft->Add(pf);
  lteHelper->ActivateDedicatedEpsBearer(
      UEDevice, EpsBearer(EpsBearer::NGBR_VOICE_VIDEO_GAMING), tft);

  // Create an application server in the SGi-LAN
  Ptr<Node> PGW = epcHelper->GetPgwNode();

  NodeContainer appServer;
  appServer.Create(1);

  IPStack.Install(appServer);

  // Create the SGiLAN as a point-to-point topology between the PGW and the
  // application server
  // - capacity: 100Gb/s
  // - MTU: 1500 bytes
  // - propagation delay: 0.01s
  PointToPointHelper SGiLAN;
  SGiLAN.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
  SGiLAN.SetDeviceAttribute("Mtu", UintegerValue(1500));
  SGiLAN.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));

  NetDeviceContainer SGiLANDevices = SGiLAN.Install(PGW, appServer.Get(0));

  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer SGiLANIpIfaces = ipv4h.Assign(SGiLANDevices);
  // interface 0 is localhost, 1 is the point-to-point device
  // Ipv4Address appServerAddr = SGiLANIpIfaces.GetAddress(1);

  Ptr<Ipv4StaticRouting> appServerStaticRouting =
      ipv4RoutingHelper.GetStaticRouting(appServer.Get(0)->GetObject<Ipv4>());
  appServerStaticRouting->AddNetworkRouteTo(
      epcHelper->GetUeDefaultGatewayAddress(), Ipv4Mask("255.255.0.0"), 1);

  // Application simulating downlink communication with an UdpClient
  // application on the appServer and a PacketSink on the UE (use UDP port 1234,
  // which will activate the dedicated bearer)
  uint16_t dlPort = 1234;

  PacketSinkHelper packetSinkHelper(
      "ns3::UdpSocketFactory",
      InetSocketAddress(Ipv4Address::GetAny(), dlPort));
  ApplicationContainer recvApp = packetSinkHelper.Install(UE.Get(0));
  recvApp.Start(Seconds(0.01));

  UdpClientHelper client(UEIpIface.GetAddress(0), dlPort);
  // simulate downstream real-time audio, specifically:
  // 64kbps PCM audio packetized in 20ms increments
  // IP/UDP/RTP/PCM 20+8+12+160=200
  // - required BW: 200 / 0.02 bytes/second = 80kbps
  // - packet size: 160 PCM + 12 RTP
  // - packet rate: 20ms (50 pps)
  client.SetAttribute("PacketSize", UintegerValue(172));
  client.SetAttribute("Interval", TimeValue(MilliSeconds(20)));
  // Send for 10s at most
  client.SetAttribute("MaxPackets", UintegerValue(500));
  ApplicationContainer sendApp = client.Install(appServer.Get(0));
  sendApp.Start(Seconds(0.01));

  // Dump PHY, MAC, RLC and PDCP level KPIs
  // lteHelper->EnablePhyTraces();
  // lteHelper->EnableMacTraces();
  // lteHelper->EnableRlcTraces();
  // lteHelper->EnablePdcpTraces();
  lteHelper->EnableTraces();

  SGiLAN.EnablePcapAll("llt");
  AsciiTraceHelper ascii;
  SGiLAN.EnableAsciiAll(ascii.CreateFileStream("llt.tr"));

  // Set the stop time.
  // This is needed otherwise the simulation will last forever, because (among
  // others) the start-of-subframe event is scheduled repeatedly, and the ns-3
  // simulator scheduler will hence never run out of events.
  Simulator::Stop(Seconds(10));

  // Run the simulation
  Simulator::Run();

  // Cleanup and exit:
  Simulator::Destroy();

  return 0;
}
