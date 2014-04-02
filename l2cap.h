/*
 * l2cap.h
 *
 *  Created on: Mar 29, 2014
 *      Author: kuikka
 */

#ifndef L2CAP_H_
#define L2CAP_H_

#include <stdint.h>

typedef struct {
	uint16_t	len;
	uint16_t	cid;
} __attribute__ ((packed)) l2cap_hdr;

struct sockaddr_l2 {
	sa_family_t	l2_family;
	unsigned short	l2_psm;
	bdaddr_t	l2_bdaddr;
	unsigned short	l2_cid;

#define BDADDR_LE_PUBLIC       0x01
#define BDADDR_LE_RANDOM       0x02
	uint8_t		l2_bdaddr_type;
};

/* L2CAP socket options */
#define L2CAP_OPTIONS	0x01
struct l2cap_options {
	uint16_t	omtu;
	uint16_t	imtu;
	uint16_t	flush_to;
	uint8_t		mode;
	uint8_t		fcs;
	uint8_t		max_tx;
	uint16_t	txwin_size;
};

#define L2CAP_CONNINFO	0x02
struct l2cap_conninfo {
	uint16_t	hci_handle;
	uint8_t		dev_class[3];
};




#endif /* L2CAP_H_ */
