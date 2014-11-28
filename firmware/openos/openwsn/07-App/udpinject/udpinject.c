#include "openwsn.h"
#include "udpinject.h"
#include "openudp.h"
#include "openqueue.h"
#include "openserial.h"
#include "packetfunctions.h"

//=========================== variables =======================================
uint8_t host_addr_128b[]  = {0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
int handshake = 1;

//=========================== prototypes ======================================

//=========================== public ==========================================

void udpinject_init() {
   //Udpinject is for communication from mobile to host only
}

//This method will be triggered when the mobile receives a U serial frame
void udpinject_trigger() {

   OpenQueueEntry_t* pkt;
   uint8_t number_bytes_from_input_buffer;
   uint8_t input_buffer[255];
   int i;

   number_bytes_from_input_buffer = openserial_getInputBuffer(&(input_buffer[0]),sizeof(input_buffer));

   //Prepare packet
   pkt = openqueue_getFreePacketBuffer(COMPONENT_UDPINJECT);
   if (pkt==NULL) {
      openserial_printError(COMPONENT_UDPINJECT,ERR_NO_FREE_PACKET_BUFFER,
                            (errorparameter_t)0,
                            (errorparameter_t)0);
      return;
   }
   pkt->creator                     = COMPONENT_UDPINJECT;
   pkt->owner                       = COMPONENT_UDPINJECT;
   pkt->l4_protocol                 = IANA_UDP;
   pkt->l4_sourcePortORicmpv6Type   = WKP_UDP_INJECT;
   pkt->l4_destination_port         = WKP_UDP_INJECT;
   pkt->l3_destinationAdd.type      = ADDR_128B;
   //Address to host
   memcpy(&(pkt->l3_destinationAdd.addr_128b[0]),&host_addr_128b,16);
   packetfunctions_reserveHeaderSize(pkt,number_bytes_from_input_buffer);
   for(i=0; i<number_bytes_from_input_buffer; i++)
	   ((uint8_t*)pkt->payload)[i]  = input_buffer[i];
   //Send packet
   if ((openudp_send(pkt))==E_FAIL) {
      openqueue_freePacketBuffer(pkt);
   }
}

void udpinject_sendDone(OpenQueueEntry_t* msg, owerror_t error) {

	uint8_t msg_error[2] = {'S','D'};

	msg->owner = COMPONENT_UDPINJECT;
   if (msg->creator!=COMPONENT_UDPINJECT) {
      openserial_printError(COMPONENT_UDPINJECT,ERR_UNEXPECTED_SENDDONE,
                            (errorparameter_t)0,
                            (errorparameter_t)0);
   }
   if(error == E_SUCCESS)
   	msg_error[1] = 'S';
   else
   	msg_error[1] = 'F';
   openserial_printData(msg_error,2);

   openqueue_freePacketBuffer(msg);
}

//This method will be triggered when the mobile receives a handshake packet from host
void udpinject_receive(OpenQueueEntry_t* msg) {

   OpenQueueEntry_t* pkt;

   //We are ONLY expecting ONE handshake message here
   //if (handshake){
      pkt = openqueue_getFreePacketBuffer(COMPONENT_UDPINJECT);
      if (pkt==NULL) {
    	  openserial_printError(COMPONENT_UDPINJECT,ERR_NO_FREE_PACKET_BUFFER,
    			  (errorparameter_t)0,
    			  (errorparameter_t)0);
    	  return;
      }
      pkt->creator                     = COMPONENT_UDPINJECT;
      pkt->owner                       = COMPONENT_UDPINJECT;
      pkt->l4_protocol                 = IANA_UDP;
      pkt->l4_sourcePortORicmpv6Type   = WKP_UDP_INJECT;
      pkt->l4_destination_port         = WKP_UDP_INJECT;
      pkt->l3_destinationAdd.type      = ADDR_128B;
      //Address to host
      memcpy(&(pkt->l3_destinationAdd.addr_128b[0]),&host_addr_128b,16);
      packetfunctions_reserveHeaderSize(pkt,2);
      ((uint8_t*)pkt->payload)[0]      = 'O';
      ((uint8_t*)pkt->payload)[1]      = 'K';
      //Send packet
      if ((openudp_send(pkt))==E_FAIL) {
    	  openqueue_freePacketBuffer(pkt);
      }
      handshake = 0;
   //}

   openqueue_freePacketBuffer(msg);
}

bool udpinject_debugPrint() {
   return FALSE;
}
//=========================== private =========================================
