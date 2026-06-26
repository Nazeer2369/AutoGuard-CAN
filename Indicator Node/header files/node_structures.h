typedef struct node
{
    //can id
    unsigned int can_id;
	int tx_node_id;
    ///data bytes
    int data1,data2;
    
    //bit fields
    struct bit
    {
        //dlc bit(length of the data)0-8 bytes
        //bit   
        //1 --->0,1   
        //2 --->01,10,11
        //3 --->001,010,011,100,101,110,111
        //4 --->0001,0010,0011,0100,0101,0110,0111,1000
        unsigned dlc:4;
        //rtr bit
        /*
        Remote Transmission Request
            Determines:
            0:Data frame or
            1:Remote frame
        */
        unsigned rtr:1;
    }node_bit_field;
}main_node;
void Init_CAN1(void) ;
