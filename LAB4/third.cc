/*
Name: UJESH MAURYA
ID: 2015UCP1338
Problem: Create a network with 2 wifi access points , create trace file and analyze.

*/

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include<iostream>


using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Third");

int main (int argc, char *argv[])
{

  uint32_t nWifi = 3 , nWifi2 = 3;
  bool tracing = true;

  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("nWifi2", "Number of wifi STA devices", nWifi2);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.Parse (argc,argv);

  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer wifiStaNodes, wifiStaNodes2;
  wifiStaNodes.Create (nWifi);
  wifiStaNodes2.Create (nWifi2);
  NodeContainer wifiApNode = p2pNodes.Get (0);
  NodeContainer wifiApNode2 = p2pNodes.Get (1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());
  phy2.SetChannel (channel.Create ());

  WifiHelper wifi, wifi2;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac, mac2;
  Ssid ssid = Ssid ("ns-3-ssid");
  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices, staDevices2;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);
  staDevices2 = wifi.Install (phy2, mac2, wifiStaNodes2);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices, apDevices2;
  apDevices = wifi.Install (phy, mac, wifiApNode);
  apDevices2 = wifi.Install (phy2, mac2, wifiApNode2);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  
  
  MobilityHelper mobility2;

  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (5.0),
                                 "MinY", DoubleValue (5.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility2.Install (wifiStaNodes2);

  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifiApNode2);
  

  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiApNode2);
  stack.Install (wifiStaNodes);
  stack.Install (wifiStaNodes2);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);
  
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterface1;
  wifiInterface1 = address.Assign (staDevices);
  address.Assign (apDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterface2;
  wifiInterface2 = address.Assign (staDevices2);
  address.Assign (apDevices2);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  
  UdpEchoClientHelper echoClient (wifiInterface1.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes2.Get (1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  if (tracing == true) {
    pointToPoint.EnablePcapAll ("third-point");
    // phy.EnablePcap ("third-access1", apDevices.Get (0));
    // phy2.EnablePcap ("third-access2", ap2Devices.Get (0));
    // csma.EnablePcap ("third-csma", csmaDevices.Get (0), true);
  }
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
