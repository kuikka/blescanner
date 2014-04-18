/*
 * hci.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef HCI_H_
#define HCI_H_

#include <stdint.h>
#include <sys/socket.h>

#define HCIDEVUP	_IOW('H', 201, int)
#define HCIDEVDOWN	_IOW('H', 202, int)
#define HCIDEVRESET	_IOW('H', 203, int)
#define HCIDEVRESTAT	_IOW('H', 204, int)

#define HCIGETDEVLIST	_IOR('H', 210, int)
#define HCIGETDEVINFO	_IOR('H', 211, int)

#define BTPROTO_L2CAP	0
#define BTPROTO_HCI		1

#define SOL_HCI		0
#define SOL_L2CAP	6
#define SOL_SCO		17
#define SOL_RFCOMM	18

#define SOL_BLUETOOTH	274

#define HCI_OP_LE_READ_BUFFER_SIZE		0x2002
#define HCI_OP_LE_SET_SCAN_PARAMETERS	0x200B
#define HCI_OP_LE_SET_SCAN_ENABLE		0x200C
#define HCI_OP_LE_CREATE_CONN			0x200D
#define HCI_OP_LE_CREATE_CONN_CANCEL	0x200E
#define HCI_OP_LE_READ_WHITE_LIST_SIZE	0x200F
#define HCI_OP_LE_CLEAR_WHITE_LIST		0x2010
#define HCI_OP_LE_ADD_TO_WHITE_LIST		0x2011
#define HCI_OP_DISCONNECT				0x0006

#define HCI_MAX_EVENT_SIZE	260 // 255 + 4 + 1
#define HCI_COMMAND_PKT 1
#define HCI_EVENT_PKT	4

#define EVT_DISCONN_COMPLETE	0x05
#define EVT_CMD_STATUS			0x0F
#define EVT_CMD_COMPLETE		0x0E
#define EVT_LE_META_EVENT		0x3E


typedef struct {
	uint8_t		type;
	uint16_t	opcode;
	uint8_t		dlen;
	uint8_t		data[255];
} __attribute__ ((packed)) hci_cmd;

typedef struct {
	// Little-endian!
	uint8_t b[6];
} __attribute__((packed)) bdaddr_t;

struct sockaddr_hci {
	sa_family_t	hci_family;
	unsigned short	hci_dev;
	unsigned short  hci_channel;
};


struct hci_dev_stats {
	uint32_t err_rx;
	uint32_t err_tx;
	uint32_t cmd_tx;
	uint32_t evt_rx;
	uint32_t acl_tx;
	uint32_t acl_rx;
	uint32_t sco_tx;
	uint32_t sco_rx;
	uint32_t byte_rx;
	uint32_t byte_tx;
};

struct hci_dev_info {
	uint16_t dev_id;
	char     name[8];

	bdaddr_t bdaddr;

	uint32_t flags;
	uint8_t  type;

	uint8_t  features[8];

	uint32_t pkt_type;
	uint32_t link_policy;
	uint32_t link_mode;

	uint16_t acl_mtu;
	uint16_t acl_pkts;
	uint16_t sco_mtu;
	uint16_t sco_pkts;

	struct   hci_dev_stats stat;
};

typedef struct {
	uint8_t		evt;
	uint8_t		plen;
} __attribute__ ((packed))	hci_event_hdr;

typedef struct {
	uint8_t		ncmd;
	uint16_t	opcode;
} __attribute__ ((packed)) evt_cmd_complete;

typedef struct {
	uint8_t		status;
	uint8_t		ncmd;
	uint16_t	opcode;
} __attribute__ ((packed)) evt_cmd_status;

typedef struct {
	uint8_t		status;
	uint16_t	handle;
	uint8_t		reason;
} __attribute__ ((packed)) evt_disconnection_complete;

typedef struct {
	uint8_t		subevent;
	uint8_t		data[0];
} __attribute__ ((packed)) evt_le_meta_event;


// LE meta events
#define EVT_LE_CONNECTION_COMPLETE	0x01

typedef struct {
	uint8_t		status;
	uint16_t	handle;
	uint8_t		role;
	uint8_t		peer_bdaddr_type;
	bdaddr_t	peer_bdaddr;
	uint16_t	conn_interval;
	uint16_t	conn_latency;
	uint16_t	supervision_timeout;
	uint8_t		master_clock_accuracy;
} __attribute__ ((packed)) evt_le_connection_complete;

#define EVT_LE_ADVERTISING_REPORT	0x02



struct hci_filter {
	uint32_t type_mask;
	uint32_t event_mask[2];
	uint16_t opcode;
};
#define HCI_FILTER	2

// LE HCI commands

typedef struct {
	uint8_t		enable;
	uint8_t		filter_dup;
} __attribute__ ((packed)) le_set_scan_enable;

typedef struct {
	uint8_t		type;
	uint16_t	interval;
	uint16_t	window;
	uint8_t		own_bdaddr_type;
	uint8_t		filter;
} __attribute__ ((packed)) le_set_scan_parameters;


typedef struct {
	uint16_t	scan_interval;
	uint16_t	scan_window;
	uint8_t		initiator_filter;
	uint8_t		peer_bdaddr_type;
	bdaddr_t	peer_bdaddr;
	uint8_t		own_bdaddr_type;
	uint16_t	min_interval;
	uint16_t	max_interval;
	uint16_t	latency;
	uint16_t	supervision_timeout;
	uint16_t	min_ce_length;
	uint16_t	max_ce_length;
} __attribute__ ((packed)) le_create_connection;

typedef struct {
	uint8_t		status;
	uint16_t	data_packet_len;
	uint8_t		nr_data_packets;
} __attribute__ ((packed)) le_read_buffer_size;

typedef struct {
	uint16_t	handle;
	uint8_t		reason;
} __attribute__ ((packed)) hci_disconnect;

typedef struct {
	uint8_t		status;
	uint8_t		list_size;
} __attribute__ ((packed)) le_read_white_list_size;

typedef struct {
	uint8_t		peer_bdaddr_type;
	bdaddr_t	peer_bdaddr;
} __attribute__ ((packed)) le_add_to_white_list;
#endif /* HCI_H_ */
