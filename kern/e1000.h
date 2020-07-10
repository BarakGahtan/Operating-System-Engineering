#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

//#include <kern/e1000_hw.h>
#include <kern/pci.h>
#include <kern/picirq.h>


/* updated part */

#define E1000_VENDORID 0x8086
#define E1000_DEVICEID 0x100E

#define MAXTXD   64
#define MAXRXD   128
#define MAXTXBUF 1518
#define MAXRXBUF 2048

#define E1000_INTERRUPT_READ_REG       0xc0
#define E1000_INTERRUPT_RECEIVED_BIT   0x80
#define E1000_INTERRUPT_SENT_BIT       0x01
#define E1000_INTERRUPT_MASK_REG 	   	0x000d0

#define E1000_STATUS   0x00008  /* Device Status - RO */
#define E1000_RCTL     0x00100  /* RX Control - RW */
#define E1000_TCTL     0x00400  /* TX Control - RW */
#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */

#define E1000_RDBAL    0x02800  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    0x02804  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    0x02808  /* RX Descriptor Length - RW */
#define E1000_RDH      0x02810  /* RX Descriptor Head - RW */
#define E1000_RDT      0x02818  /* RX Descriptor Tail - RW */

#define E1000_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818  /* TX Descripotr Tail - RW */

#define E1000_MTA      0x05200  /* Multicast Table Array - RW Array */
#define E1000_RAL      0x05400  /* Receive Address Low - RW Array */
#define E1000_RAH      0x05404  /* Receive Address High - RW Array */

/* Transmit Control */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */

/* Transmit Descriptor bit definitions */
#define E1000_TXD_POPTS_TXSM 0x02       /* Insert TCP/UDP checksum */
#define E1000_TXD_CMD_EOP    0x01       /* End of Packet */
#define E1000_TXD_CMD_IFCS   0x02       /* Insert FCS (Ethernet CRC) */
#define E1000_TXD_CMD_IC     0x04       /* Insert Checksum */
#define E1000_TXD_CMD_RS     0x08       /* Report Status */
#define E1000_TXD_CMD_RPS    0x10       /* Report Packet Sent */
#define E1000_TXD_CMD_DEXT   0x20       /* Descriptor extension (0 = legacy) */
#define E1000_TXD_CMD_VLE    0x40       /* Add VLAN tag */
#define E1000_TXD_CMD_IDE    0x80       /* Enable Tidv register */
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
#define E1000_TXD_STAT_EC    0x00000002 /* Excess Collisions */
#define E1000_TXD_STAT_LC    0x00000004 /* Late Collisions */
#define E1000_TXD_STAT_TU    0x00000008 /* Transmit underrun */
#define E1000_TXD_CMD_TCP    0x01       /* TCP packet */
#define E1000_TXD_CMD_IP     0x02       /* IP packet */
#define E1000_TXD_CMD_TSE    0x04       /* TCP Seg enable */
#define E1000_TXD_STAT_TC    0x00000004 /* Tx Underrun */

/* Receive Control */
#define E1000_RCTL_RST            0x00000001    /* Software reset */
#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_SBP            0x00000004    /* store bad packet */
#define E1000_RCTL_UPE            0x00000008    /* unicast promiscuous enable */
#define E1000_RCTL_MPE            0x00000010    /* multicast promiscuous enab */
#define E1000_RCTL_LPE            0x00000020    /* long packet enable */
#define E1000_RCTL_LBM_NO         0x00000000    /* no loopback mode */
#define E1000_RCTL_LBM_MAC        0x00000040    /* MAC loopback mode */
#define E1000_RCTL_LBM_SLP        0x00000080    /* serial link loopback mode */
#define E1000_RCTL_LBM_TCVR       0x000000C0    /* tcvr loopback mode */
#define E1000_RCTL_DTYP_MASK      0x00000C00    /* Descriptor type mask */
#define E1000_RCTL_DTYP_PS        0x00000400    /* Packet Split descriptor */
#define E1000_RCTL_RDMTS_HALF     0x00000000    /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_QUAT     0x00000100    /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_EIGTH    0x00000200    /* rx desc min threshold size */
#define E1000_RCTL_MO_SHIFT       12            /* multicast offset shift */
#define E1000_RCTL_MO_0           0x00000000    /* multicast offset 11:0 */
#define E1000_RCTL_MO_1           0x00001000    /* multicast offset 12:1 */
#define E1000_RCTL_MO_2           0x00002000    /* multicast offset 13:2 */
#define E1000_RCTL_MO_3           0x00003000    /* multicast offset 15:4 */
#define E1000_RCTL_MDR            0x00004000    /* multicast desc ring 0 */
#define E1000_RCTL_BAM            0x00008000    /* broadcast enable */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 0 */
#define E1000_RCTL_SZ_2048        0x00000000    /* rx buffer size 2048 */
#define E1000_RCTL_SZ_1024        0x00010000    /* rx buffer size 1024 */
#define E1000_RCTL_SZ_512         0x00020000    /* rx buffer size 512 */
#define E1000_RCTL_SZ_256         0x00030000    /* rx buffer size 256 */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 1 */
#define E1000_RCTL_SZ_16384       0x00010000    /* rx buffer size 16384 */
#define E1000_RCTL_SZ_8192        0x00020000    /* rx buffer size 8192 */
#define E1000_RCTL_SZ_4096        0x00030000    /* rx buffer size 4096 */
#define E1000_RCTL_VFE            0x00040000    /* vlan filter enable */
#define E1000_RCTL_CFIEN          0x00080000    /* canonical form enable */
#define E1000_RCTL_CFI            0x00100000    /* canonical form indicator */
#define E1000_RCTL_DPF            0x00400000    /* discard pause frames */
#define E1000_RCTL_PMCF           0x00800000    /* pass MAC control frames */
#define E1000_RCTL_BSEX           0x02000000    /* Buffer size extension */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */
#define E1000_RCTL_FLXBUF_MASK    0x78000000    /* Flexible buffer size */
#define E1000_RCTL_FLXBUF_SHIFT   27            /* Flexible buffer shift */

/* Receive Descriptor bit definitions */
#define E1000_RXD_STAT_DD       0x01    /* Descriptor Done */
#define E1000_RXD_STAT_EOP      0x02    /* End of Packet */
#define E1000_RXD_STAT_IXSM     0x04    /* Ignore checksum */
#define E1000_RXD_STAT_VP       0x08    /* IEEE VLAN Packet */
#define E1000_RXD_STAT_UDPCS    0x10    /* UDP xsum caculated */
#define E1000_RXD_STAT_TCPCS    0x20    /* TCP xsum calculated */
#define E1000_RXD_STAT_IPCS     0x40    /* IP xsum calculated */
#define E1000_RXD_STAT_PIF      0x80    /* passed in-exact filter */
#define E1000_RXD_STAT_IPIDV    0x200   /* IP identification valid */
#define E1000_RXD_STAT_UDPV     0x400   /* Valid UDP checksum */
#define E1000_RXD_STAT_ACK      0x8000  /* ACK Packet indication */

#define E1000_MAXMTA         128

// CHALLENGE 1 LAB 6
#define E1000_RAH_AV  0x80000000        /* Receive descriptor valid */

#define E1000_MAC_BYTES     6
#define E1000_MAC_ADDR0     0x00
#define E1000_MAC_ADDR1     0x01
#define E1000_MAC_ADDR2     0x02

#define E1000_EERD_START    0x00000001
#define E1000_EERD_DONE     0x00000010
#define E1000_EERD_ADDR_OFF 8
#define E1000_EERD_DATA_OFF 16

// more
#define E1000_EECD_SK        0x00000001 /* EEPROM Clock */
#define E1000_EECD_CS        0x00000002 /* EEPROM Chip Select */
#define E1000_EECD_DI        0x00000004 /* EEPROM Data In */
#define E1000_EECD_DO        0x00000008 /* EEPROM Data Out */
#define E1000_EECD_FWE_MASK  0x00000030
#define E1000_EECD_FWE_DIS   0x00000010 /* Disable FLASH writes */
#define E1000_EECD_FWE_EN    0x00000020 /* Enable FLASH writes */
#define E1000_EECD_FWE_SHIFT 4
#define E1000_EECD_REQ       0x00000040 /* EEPROM Access Request */
#define E1000_EECD_GNT       0x00000080 /* EEPROM Access Grant */
#define E1000_EECD_PRES      0x00000100 /* EEPROM Present */
#define E1000_EECD_SIZE      0x00000200 /* EEPROM Size (0=64 word 1=256 word) */
#define E1000_EECD_TYPE      0x00002000 /* EEPROM Type (1-SPI, 0-Microwire) */
#ifndef E1000_EEPROM_GRANT_ATTEMPTS
#define E1000_EEPROM_GRANT_ATTEMPTS 1000 /* EEPROM # attempts to gain grant */
#endif
#define E1000_EECD_AUTO_RD          0x00000200  /* EEPROM Auto Read done */
#define E1000_EECD_SIZE_EX_MASK     0x00007800  /* EEprom Size */
#define E1000_EECD_SIZE_EX_SHIFT    11
#define E1000_EECD_NVADDS    0x00018000 /* NVM Address Size */
#define E1000_EECD_SELSHAD   0x00020000 /* Select Shadow RAM */
#define E1000_EECD_INITSRAM  0x00040000 /* Initialize Shadow RAM */
#define E1000_EECD_FLUPD     0x00080000 /* Update FLASH */
#define E1000_EECD_AUPDEN    0x00100000 /* Enable Autonomous FLASH update */
#define E1000_EECD_SHADV     0x00200000 /* Shadow RAM Data Valid */
#define E1000_EECD_SEC1VAL   0x00400000 /* Sector One Valid */
#define E1000_EECD_SECVAL_SHIFT      22
#define E1000_STM_OPCODE     0xDB00
#define E1000_HICR_FW_RESET  0xC0

#define E1000_SHADOW_RAM_WORDS     2048
#define E1000_ICH_NVM_SIG_WORD     0x13
#define E1000_ICH_NVM_SIG_MASK     0xC0

#define E1000_EECD     0x00010  /* EEPROM/Flash Control - RW */
#define E1000_EERD     0x00014  /* EEPROM Read - RW */



/* Structs */

struct tx_desc {
	uint64_t buffer_addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

struct rx_desc {
	uint64_t buffer_addr;
	uint16_t length;
	uint16_t csum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
} __attribute__((packed));

typedef struct {
	uint8_t buf[MAXTXBUF];
} tx_packet_t;

typedef struct {
	uint8_t buf[MAXRXBUF];
} rx_packet_t;


int e1000_attach(struct pci_func *pcif);
int e1000_transmit(uint8_t *buf, size_t len);
ssize_t e1000_receive(uint8_t *buf, size_t len);
int e1000_interrupt_get_sent();
int e1000_interrupt_get_rec();
void e1000_send_morning();
void e1000_receive_morning();

extern uint16_t e1000_mac[3];

uint16_t e1000_get_eeprom(uint16_t addr);
void e1000_get_mac(uint16_t *mac);

int e1000_transmit_direct(void *packet, size_t len);



#endif  // SOL >= 6
