
#include "ClearCore.h"
#include "EthernetTcpServer.h"
#include "parser.h"
#include "motor.h"

#define baudRate 115200
unsigned char packetReceived[1024];

// ClearCore IP address
IpAddress ip = IpAddress(192, 168, 1, 8);
// Router/switch IP address
IpAddress gateway = IpAddress(192, 168, 1, 1);
// Mask to separate network vs. host addresses (required for TCP)
IpAddress netmask = IpAddress(255, 255, 255, 0);

int main(void) {
    /* Replace with your application code below */
    bool ledState = true;
	Delay_ms(2000);
	ConnectorUsb.Mode(Connector::USB_CDC);
	ConnectorUsb.Speed(baudRate);
	ConnectorUsb.PortOpen();
	int counter = 0;
	char holder[128];
	
    while (1)
    {
		BenchBot::printf("Run Number %d\n", counter++);
		//ConnectorUsb.Send(&holder[0]);
        ConnectorLed.State(ledState);
        while (!EthernetMgr.PhyLinkActive()){
			ConnectorUsb.Send("Ethernet Unplugged");
			Delay_ms(100);
		}
		EthernetMgr.Setup();
		EthernetMgr.LocalIp(ip);
		EthernetMgr.GatewayIp(gateway);
		EthernetMgr.NetmaskIp(netmask);
		
		EthernetTcpServer server = EthernetTcpServer(8888);
		
		// Initialize a client object
		// This object will hold a connected client's information
		// allowing the server to interact with the client
		EthernetTcpClient client;
		
		// Start listening for TCP connections
		server.Begin();

		ConnectorUsb.SendLine("Server now listening for client connections...");
		
		client = server.Available();
			
		while (client.Connected()){
			if (client.BytesAvailable() > 0) {
				ConnectorIO0.State(true);
				
				Delay_ms(100);
				ConnectorUsb.Send("Read the following from the client: ");
				while (client.BytesAvailable() > 0) {
					client.Read(packetReceived, 1024);
					ConnectorUsb.Send((char *)packetReceived);
					
					for(int i=0; i<1024; i++){
						packetReceived[i]= NULL;
					}
				}
				ConnectorUsb.SendLine();
				
				client.Send(&holder[0]);
			}
			// Turn off LED if a message has not been received
			ConnectorIO0.State(false);
			if(client.ConnectionState()->state == CLOSING){
				client.Close();
			}
		}
		EthernetMgr.Refresh();
        Delay_ms(1000);
    }
}