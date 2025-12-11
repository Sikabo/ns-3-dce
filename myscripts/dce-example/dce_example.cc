#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/dce-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
//#include "ns3/netanim-module.h"
//#include "ns3/constant-position-mobility-model.h"
// #include "ccnx/misc-tools.h"

#include <unistd.h>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("DCE_example");

int main (int argc, char *argv[])
{
    NodeContainer nodes;
    nodes.Create (2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));

    NetDeviceContainer devices = pointToPoint.Install (nodes);

    DceManagerHelper dceManager;
    dceManager.SetTaskManagerAttribute ("FiberManagerType", StringValue ("UcontextFiberManager"));

    InternetStackHelper stack;
    Ipv4DceRoutingHelper ipv4RoutingHelper;
    stack.SetRoutingHelper (ipv4RoutingHelper); // xxx
    stack.Install (nodes);
    dceManager.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    // setup ip routes
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    DceApplicationHelper dce;
    ApplicationContainer apps;

    // Setup a realistic stack size
    dce.SetStackSize (1 << 20);

    // Launch dce_example on node 0
    dce.SetBinary ("dce_example");
    dce.ResetArguments ();
    dce.ResetEnvironment ();
    dce.AddArgument ("10.1.1.2");
    dce.AddArgument ("12000");
    dce.AddArgument ("10");

    apps = dce.Install (nodes.Get (0));
    apps.Start (Seconds (0.7));
    apps.Stop (Seconds (10));

    // Launch dce_example on node 1
    dce.SetBinary ("dce_example");
    dce.ResetArguments ();
    dce.ResetEnvironment ();
    dce.AddArgument ("10.1.1.1");
    dce.AddArgument ("12000");
    dce.AddArgument ("50");

    apps = dce.Install (nodes.Get (1));
    apps.Start (Seconds (0.6));
    apps.Stop (Seconds (10));

    pointToPoint.EnablePcapAll ("example-dce", false); // Uncomment to enable pcap capture

    //setPos (nodes.Get (0), 1, 10, 0); // TODO: get netanim working
    //setPos (nodes.Get (1), 50,10, 0);

    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
