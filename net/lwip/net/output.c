#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver

	int r, permitions, num_try;
	envid_t from_environment;

	while (true) {
		r = ipc_recv(&from_environment, &nsipcbuf, &permitions);

		if ((permitions & (PTE_P | PTE_W | PTE_U)) != (PTE_P | PTE_W | PTE_U)) {
			continue;
		}
		if (r != NSREQ_OUTPUT || from_environment != ns_envid) {
			continue;
		}

		num_try = 0;
		//while ((r = sys_transmit_packet((uint8_t *) nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len)) < 0) {
		while ((r = sys_e1000_transmit_direct((uint8_t *) nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len)) < 0) {
			if (r == -E_PACKET_TOO_BIG) cprintf("output: Packet is too big\n");
			num_try += 1;
			if (num_try > 10) {
				cprintf("output: Couldnt send packet\n");
				break;
			}
			sys_yield();
		}
	}
}
