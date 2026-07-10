#include "can_structure.h"
#define TBS 2
//C1CMR
#define TR 0
#define RRB 2
#define STB 5
//C1GSR
#define RBS 0
#define TBS 2
#define TCS 3
#define TS 5
//C1TF1/C1RFS
#define RXERR 16
#define TXERR 24
//C1TF1/C1RFS
#define DLC 16
#define RTR 30
//#define FOSC 12000000
//#define CCLK FOSC*5
#define PCLK 6000000

#define BIT_RATE 125000
#define QUANTA 15
#define BRP (PCLK/(BIT_RATE*QUANTA))
#define SAMPLE_POINT (0.7*QUANTA)
#define TSEG1 ((int)(SAMPLE_POINT-1))
#define TSEG2 (QUANTA-(1+TSEG1))
#define SJW ((TSEG2>=5)?4:(TSEG2-1))

#define TSEG1_LVAL (TSEG1-1)
#define TSEG2_LVAL (TSEG2-1)
#define BRP_VAL (BRP-1)
#define SJW_LVAL (SJW-1)
#define SAM 0
#define BTR_LVAL ((SAM<<23)|(TSEG2_LVAL<<20)|(TSEG1_LVAL<<16)|(SJW_LVAL<<14)|(BRP_VAL))
//AFMR
#define ACCBP 1
void Init_CAN1(void);
void rx_main_node(main_node *RXF);
void tx_main_node(main_node TXF);
