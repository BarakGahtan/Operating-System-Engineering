#include <inc/assert.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/types.h>
#include <kern/pci.h>
#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/env.h>
#include <kern/pmap.h>

uint16_t e1000_mac[3];
volatile uint32_t *e1000;

__attribute__((__aligned__(16)))
struct tx_desc e1000_txd[MAXTXD];
__attribute__((__aligned__(16)))
struct rx_desc e1000_rxd[MAXRXD];

tx_packet_t tx_buf[MAXTXD];
rx_packet_t rx_buf[MAXRXD];

int
e1000_transmit(uint8_t *buf, size_t len) {
	uint32_t tail;
	if (len > MAXTXBUF) return -E_PACKET_TOO_BIG;

	tail = e1000[E1000_TDT >> 2];

	if (~e1000_txd[tail].status & E1000_TXD_STAT_DD) return -E_TX_QUEUE_FULL;

	// Copy data into kernel buffer
	memcpy(&tx_buf[tail].buf, buf, len);
	e1000_txd[tail].length = len;

	// Clear DD and set EOP
	e1000_txd[tail].status &= ~E1000_TXD_STAT_DD;
	e1000_txd[tail].cmd |= E1000_TXD_CMD_EOP;

	// Increase tail pointer
	e1000[E1000_TDT >> 2] = (tail + 1) % MAXTXD;

	return 0;
}

ssize_t
e1000_receive(uint8_t *buf, size_t len) {
	uint32_t tail, length;

	tail = (e1000[E1000_RDT >> 2] + 1) % MAXRXD;

	if (~e1000_rxd[tail].status & E1000_RXD_STAT_DD) return -E_RX_QUEUE_EMPTY;
	if ((length = e1000_rxd[tail].length) > len) return -E_BUF_TOO_SMALL;

	memcpy(buf, rx_buf[tail].buf, length);

	// Clear DD and EOP
	e1000_rxd[tail].status &= ~E1000_RXD_STAT_DD;
	e1000_rxd[tail].status &= ~E1000_RXD_STAT_EOP;

	e1000[E1000_RDT >> 2] = tail;

	return length;
}

/* LAB6 ex11 */

int e1000_interrupt_get_sent() {
  uint32_t interrupt_res = e1000[E1000_INTERRUPT_READ_REG >> 2];
  return interrupt_res & E1000_INTERRUPT_RECEIVED_BIT;
}

int e1000_interrupt_get_rec() {
  uint32_t interrupt_res = e1000[E1000_INTERRUPT_READ_REG >> 2];
  return interrupt_res & E1000_INTERRUPT_SENT_BIT;
}

void e1000_send_morning(){
  int i, j, cur_env = 0;
  for (i = 0; i < NENV; ++i) {
    j = (cur_env + i) % NENV;
    if (envs[j].send_zZ) {
      envs[j].send_zZ = false;
      envs[j].env_status = ENV_RUNNABLE;
      cprintf("Wake up sender from a good sleep\n")	;
    }
  }
}

void e1000_receive_morning(){
  int i, j, cur_env = 0;
  for (i = 0; i < NENV; ++i) {
    j = (cur_env + i) % NENV;
    if (envs[j].rec_zZ) {
      envs[j].rec_zZ = false;
      envs[j].env_status = ENV_RUNNABLE;
      cprintf("Wake up receiver from a good sleep\n")	;
    }
  }
}


int e1000_attach(struct pci_func *pcif) {
  pci_func_enable(pcif);

  e1000 = (uint32_t *) mmio_map_region((physaddr_t) pcif->reg_base[0], (size_t) pcif->reg_size[0]);
	if (e1000[E1000_STATUS >> 2] != 0x80080783) panic("e1000_attach: Bad mapping!\n");

  /* INIT TRANSMIT */
  int i;
	static_assert((uint32_t) e1000_txd % 16 == 0);
	static_assert(sizeof(e1000_txd) % 128 == 0);

	e1000[E1000_TDBAL >> 2] = PADDR(e1000_txd);
	e1000[E1000_TDBAH >> 2] = 0;
	e1000[E1000_TDLEN >> 2] = sizeof(e1000_txd);

	e1000[E1000_TDH >> 2] = 0;
	e1000[E1000_TDT >> 2] = 0;

	e1000[E1000_TCTL >> 2]  = E1000_TCTL_EN | E1000_TCTL_PSP;
	e1000[E1000_TCTL >> 2] |= 0x10 << 4;  // E1000_TCTL_CT
	e1000[E1000_TCTL >> 2] |= 0x40 << 12; // E1000_TCTL_COLD

	e1000[E1000_TIPG >> 2]  = 0xa;
	e1000[E1000_TIPG >> 2] |= 0x8 << 10;
	e1000[E1000_TIPG >> 2] |= 0xc << 20;

	memset(e1000_txd, 0, sizeof(e1000_txd));

	for (i = 0; i < MAXTXD; ++i) {
		e1000_txd[i].buffer_addr = PADDR(&tx_buf[i]);
		e1000_txd[i].cmd |= E1000_TXD_CMD_RS;
		e1000_txd[i].status |= E1000_TXD_STAT_DD;
	}
  /* END INIT TRANSMIT */


  /* TEST TRANSMIT */
  /*
  for (i = 0; i < MAXTXD + 2; i++) {
    struct PageInfo *test_page = page_alloc(ALLOC_ZERO);
    void *test_addr = (void *)page2kva(test_page);
    *(uint32_t *)test_addr = 0xbadcaffe + i;
    if (e1000_transmit(test_addr, 4) == 0) {
        cprintf("tx send a packet : index %d\n", i);
    }
  }
  */

  /* INIT RECEIVE */

	// BEFORE CHALLENGE:
	// e1000[E1000_RAL >> 2] = 0x12005452;
	// e1000[E1000_RAH >> 2] = 0x5634 | (1u << 31);

	// CHALLENGE mac LAB 6
	e1000_get_mac(e1000_mac);
	e1000[E1000_RAL >> 2] = (e1000_mac[1] << 16) | e1000_mac[0];
  e1000[E1000_RAH >> 2] = E1000_RAH_AV | e1000_mac[2];
	cprintf("MAC addr: %04x : %04x : %04x\n", e1000_mac[0], e1000_mac[1], e1000_mac[2]);

	for (i = 0; i < E1000_MAXMTA; ++i) {
		e1000[(E1000_MTA >> 2) + i] = 0;
	}

	e1000[E1000_RDBAL >> 2] = PADDR(e1000_rxd);
	e1000[E1000_RDBAH >> 2] = 0;
	e1000[E1000_RDLEN >> 2] = sizeof(e1000_rxd);

	e1000[E1000_RDH >> 2] = 0;
	e1000[E1000_RDT >> 2] = MAXRXD - 1;

	memset(e1000_rxd, 0, sizeof(e1000_rxd));

	for (i = 0; i < MAXRXD; ++i) {
		e1000_rxd[i].buffer_addr = PADDR(&rx_buf[i]);
	}

	e1000[E1000_RCTL >> 2]  = E1000_RCTL_SECRC | E1000_RCTL_LBM_NO | E1000_RCTL_SZ_2048;
	e1000[E1000_RCTL >> 2] |= E1000_RCTL_EN;
  /* END RECEIVE */

	// for EX 11 !
	e1000[E1000_INTERRUPT_MASK_REG >> 2] |= (E1000_INTERRUPT_SENT_BIT | E1000_INTERRUPT_RECEIVED_BIT);
	irq_setmask_8259A((~((uint16_t)0x800)) & irq_mask_8259A);

  return 0;

	// challenge
	char addr[64000];
	boot_map_region(kern_pgdir, addr, 64000, 0x0A0000, PTE_W | PTE_P);

	int j;
	for(j = 0; j < 64000; j++ ) {
		addr[j] = 0x0B;
	}

}

// Challenge 1

uint16_t
e1000_get_eeprom(uint16_t eeprom_addr) {
    uint32_t eerd_reg = (uint16_t) ((eeprom_addr << E1000_EERD_ADDR_OFF) | E1000_EERD_START);
    e1000[E1000_EERD >> 2] = eerd_reg;
    while ( !(e1000[E1000_EERD >> 2] & E1000_EERD_DONE) );
    uint16_t data = (uint16_t) ((e1000[E1000_EERD >> 2] >> E1000_EERD_DATA_OFF) & 0x0000FFFF);
    e1000[E1000_EERD >> 4] = 0;
    return data;
}

void e1000_get_mac(uint16_t *mac) {
		uint32_t eecd_reg = e1000[E1000_EECD >> 1];
		eecd_reg &= ~E1000_EECD_REQ;
		eecd_reg &= ~E1000_EECD_GNT;
		e1000[E1000_EECD >> 2] = eecd_reg;
    mac[0] = e1000_get_eeprom(E1000_MAC_ADDR0);
    mac[1] = e1000_get_eeprom(E1000_MAC_ADDR1);
    mac[2] = e1000_get_eeprom(E1000_MAC_ADDR2);
}


// Challenge zero copy

int e1000_transmit_direct(void *packet, size_t len) {
	uint32_t tail;
	struct PageInfo *pp;
	pte_t *pte;

	tail = e1000[E1000_TDT >> 2];

	if (~e1000_txd[tail].status & E1000_TXD_STAT_DD) return -E_TX_QUEUE_FULL;
	e1000_txd[tail].status &= ~E1000_TXD_STAT_DD;

	if (!(pp = page_lookup(curenv->env_pgdir, packet, &pte)))
		return -1;
	e1000_txd[tail].buffer_addr = page2pa(pp) + PGOFF(packet);
	e1000_txd[tail].length = len;

	e1000_txd[tail].cmd |= (E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP);

	e1000[E1000_TDT >> 2] = (tail + 1) % MAXTXD;

	return 0;
}
