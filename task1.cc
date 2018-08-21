/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/random-variable-stream.h"
#include <time.h>       /* time */

#include <iostream>

using namespace std;

#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("AssignmentTemplate");

int
FlowOutput(Ptr<FlowMonitor> flowmon, FlowMonitorHelper &flowmonHelper, int runNumber, float totalSumk, int k)
{
  flowmon->CheckForLostPackets (); //check all packets have been sent or completely lost
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats (); // pull stats from flow monitor
  float sum = 0;
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
  {//iterate through collected flow stats and find the traffic from node 1 to node 0
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
    if ( (t.sourceAddress == Ipv4Address("10.1.1.1"))  && (t.destinationAddress == Ipv4Address("10.1.1.2")) ) //use the node addresses to select the flow (or flows) you want
    { 
      sum = sum + iter->second.rxBytes * 8.0 / (10 * 1024);
      //NS_LOG_UNCOND("Throughput in Kib/s over the run time: " //normal c++ output streams can be used to output this information to a file in place of the terminal
      //        << iter->second.rxBytes * 8.0 / (10 * 1024) //bits per byte/run time over bits per Kib
      //        << std::endl); //flowmonitor records a number of different statistics about each flow, however we are only interested in rxBytes
    }
  }
  

  //finding the total results for the 5 simulations
  totalSumk = totalSumk + sum;
  if (runNumber==5){
    totalSumk = totalSumk/5;
    cout << "total results after 5 simulations for distance:";
    cout << k;
    cout << ", is equal to: ";
    cout << totalSumk;
    cout << "\n";
  }
  return totalSumk;
}

int
main (int argc, char *argv[])
{
  bool verbose = false;//set it back to true if you want to debug
  bool rayleigh = false;
  int i,k,runNumber;
  float totalSumk = 0.0; //the total throughput sum of the 5 simulations for a specific distance k
  for (i=0; i<4; i++){
    if (i==0){
      cout << "\nAARF with no fading\n";
      rayleigh = false;
    }
    else if (i==1){
      cout << "\nAARF with reyleigh fading\n";
      rayleigh = true;
    }
    else if (i==2){
      cout << "\nCARA with no fading\n";  
      rayleigh = false; 
    }
    else{
      cout << "\nCARA with reyleigh fading\n";
      rayleigh = true;
    }
    
    //the loop for the distances
    for(k=5; k<=100; k=k+5){
      //initialize troughput sum holder
      totalSumk = 0.0;
      for (runNumber = 1; runNumber<6; runNumber++){
        CommandLine cmd;
        cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose); //parameter name, description, variable that will take the value read from the command line

        cmd.Parse (argc,argv);

        if (verbose)
          {
            LogComponentEnable ("PacketSink", LOG_LEVEL_INFO); //packet sink should write all actions to the command line
          }

        /* initialize random seed: */
        srand (time(NULL));
        SeedManager::SetSeed (rand());//seed number should change between runs if running multiple simulations.
        
        NodeContainer wifiStaNodes; //create AP Node and (one or more) Station node(s)
        wifiStaNodes.Create (1);
        NodeContainer wifiApNode;
        wifiApNode.Create(1);

        YansWifiChannelHelper channel; //create helpers for the channel and phy layer and set propagation configuration here.
        channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
        channel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
        
        if(rayleigh){
          channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel",
                                  "m0", DoubleValue(1.0), "m1", DoubleValue(1.0), "m2", DoubleValue(1.0)); //combining log and nakagami to have both distance and rayleigh fading (nakami with m0, m1 and m2 =1 is rayleigh)
        }

        YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
        phy.SetChannel (channel.Create ());

        WifiHelper wifi = WifiHelper::Default (); //create helper for the overall wifi setup and configure a station manager
        wifi.SetStandard(ns3::WIFI_PHY_STANDARD_80211g);

        //If loop that sets the right manager AARF or CARA depending on the flag
        if (i<3){
          wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
        }
        else{
          wifi.SetRemoteStationManager ("ns3::CaraWifiManager");
        }
        NqosWifiMacHelper mac = NqosWifiMacHelper::Default (); //create a mac helper and configure for station and AP and install

        Ssid ssid = Ssid ("example-ssid");
        mac.SetType ("ns3::StaWifiMac",
                    "Ssid", SsidValue (ssid),
                    "ActiveProbing", BooleanValue (false));

        NetDeviceContainer staDevices;
        staDevices = wifi.Install (phy, mac, wifiStaNodes);

        mac.SetType ("ns3::ApWifiMac",
                    "Ssid", SsidValue (ssid));

        NetDeviceContainer apDevices;
        apDevices = wifi.Install (phy, mac, wifiApNode);

        MobilityHelper mobility;

        mobility.SetPositionAllocator ("ns3::GridPositionAllocator", //places nodes in a grid with distance between nodes and grid width defined below, for 2 nodes can be used to easily place them at a set distance apart
                                      "MinX", DoubleValue (0.0), //first node positioned at x = 0, y= 0
                                      "MinY", DoubleValue (0.0),
                                      "DeltaX", DoubleValue (k*1.0), //nodes will be place k-meters apart in x plane
                                      "DeltaY", DoubleValue (0.0),
                                      "GridWidth", UintegerValue (3),
                                      "LayoutType", StringValue ("RowFirst"));
        mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); //nodes shouldn't move once placed
        mobility.Install (wifiApNode);
        mobility.Install (wifiStaNodes);

        InternetStackHelper stack; //install the internet stack on both nodes
        stack.Install (wifiApNode);
        stack.Install (wifiStaNodes);

        Ipv4AddressHelper address; //assign IP addresses to all nodes
        address.SetBase ("10.1.1.0", "255.255.255.0");
        address.Assign (staDevices);
        Ipv4InterfaceContainer apAddress = address.Assign (apDevices); //we need to keep AP address accessible as we need it later

        OnOffHelper onoff ("ns3::UdpSocketFactory", Address ()); //create a new on-off application to send data
        std::string dataRate = "20Mib/s"; //data rate set as a string, see documentation for accepted units
        onoff.SetConstantRate(dataRate, (uint32_t)1024); //set the onoff client application to CBR mode

        AddressValue remoteAddress (InetSocketAddress (apAddress.GetAddress (0), 8000)); //specify address and port of the AP as the destination for on-off application's packets
        onoff.SetAttribute ("Remote", remoteAddress);
        ApplicationContainer apps = onoff.Install (wifiStaNodes.Get (0));//install onoff application on stanode 0 and configure start/stop times
        Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
        apps.Start(Seconds(var->GetValue(0, 0.1)));
        apps.Stop (Seconds (10.0));

        PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress (apAddress.GetAddress (0), 8000)); //create packet sink on AP node with address and port that on-off app is sending to
        apps.Add(sink.Install(wifiApNode.Get(0)));

        Simulator::Stop (Seconds (10.0)); //define stop time of simulator

        Ptr<FlowMonitor> flowmon;
        FlowMonitorHelper flowmonHelper;
        flowmon = flowmonHelper.InstallAll();

        Simulator::Run (); //run the simulation and destroy it once done
        Simulator::Destroy ();
        totalSumk =+ FlowOutput(flowmon, flowmonHelper, runNumber, totalSumk, k);
      }
    }
  }
  return 0;
}
