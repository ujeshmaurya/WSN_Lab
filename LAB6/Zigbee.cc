/* 
Name: UJESH MAURYA
ID: 2015UCP1338
Problem: Create Wireless Sensor Network with 11 nodes sending hello message each at 1 interval of time
          using Zigbee (802.15.4) protocol.
 */


#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Ping6WsnExample");

int main (int argc, char **argv)
{
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
    }

  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (11);

  // Set seed for random numbers
  SeedManager::SetSeed (167);

  // Install mobility
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  Ptr<ListPositionAllocator> nodesPositionAlloc = CreateObject<ListPositionAllocator> ();
  nodesPositionAlloc->Add (Vector (0.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (10.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (20.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (30.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (40.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (50.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (60.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (70.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (80.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (90.0, 0.0, 0.0));
  nodesPositionAlloc->Add (Vector (100.0, 0.0, 0.0));
  mobility.SetPositionAllocator (nodesPositionAlloc);
  mobility.Install (nodes);

  NS_LOG_INFO ("Create channels.");
  LrWpanHelper lrWpanHelper;
  // Add and install the LrWpanNetDevice for each node
  // lrWpanHelper.EnableLogComponents();
  NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
  lrWpanHelper.AssociateToPan (devContainer, 10);

  std::cout << "Created " << devContainer.GetN() << " devices" << std::endl;
  std::cout << "There are " << nodes.GetN() << " nodes" << std::endl;

  /* Install IPv4/IPv6 stack */
  NS_LOG_INFO ("Install Internet stack.");
  InternetStackHelper internetv6;
  internetv6.SetIpv4StackInstall (false);
  internetv6.Install (nodes);

  // Install 6LowPan layer
  NS_LOG_INFO ("Install 6LoWPAN.");
  SixLowPanHelper sixlowpan;
  NetDeviceContainer six1 = sixlowpan.Install (devContainer);

  NS_LOG_INFO ("Assign addresses.");
  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer i = ipv6.Assign (six1);

  NS_LOG_INFO ("Create Applications.");

  /* Create a Ping6 application to send ICMPv6 echo request from node zero to
   * all-nodes (ff02::1).
   */
  uint32_t packetSize = 10;
  uint32_t maxPacketCount = 1;
  Time interPacketInterval = Seconds (1.);


  Ping6Helper ping6[11];
  ApplicationContainer apps[11];

  for(int j=1;j<11;j++)
  {
    ping6[j].SetLocal (i.GetAddress (j, 1));
    ping6[j].SetRemote (i.GetAddress (0, 1));
  //ping6.SetRemote (Ipv6Address::GetAllNodesMulticast ());

    ping6[j].SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    ping6[j].SetAttribute ("Interval", TimeValue (interPacketInterval));
    ping6[j].SetAttribute ("PacketSize", UintegerValue (packetSize));
    apps[j] = ping6[j].Install (nodes.Get (j));
    apps[j].Start (Seconds (2.0));
    apps[j].Stop (Seconds (10.0));
  }
    
  AsciiTraceHelper ascii;
  lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream ("ping6wsn.tr"));
  lrWpanHelper.EnablePcapAll (std::string ("ping6wsn"), true);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

