#include "openwsn.h"
#include "udpprint.h"
#include "openqueue.h"
#include "openserial.h"

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void udpprint_init() {
   //Udpprint is for communication from host to mobile only
}

//This method will be triggered when the mobile receives a packet from host
//We want to send over serial the 4 bytes of data we received on the UDP packet
void udpprint_receive(OpenQueueEntry_t* msg) {

   //No handshake required
   //We don't want to print on serial of DAG root - OpenVisualizer WILL fail
   if(!idmanager_getIsDAGroot()){
	  openserial_printData((uint8_t*)(msg->payload),msg->length);
   }
   openqueue_freePacketBuffer(msg);
}

void udpprint_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openserial_printError(
      COMPONENT_UDPPRINT,
      ERR_UNEXPECTED_SENDDONE,
      (errorparameter_t)0,
      (errorparameter_t)0
   );
   openqueue_freePacketBuffer(msg);
}

bool udpprint_debugPrint() {
   return FALSE;
}

//=========================== private =========================================
