#ifndef __PROTOCOL645_H__
#define __PROTOCOL645_H__
 
#define DataLenMAX 256
 
typedef unsigned char INT8U;
typedef unsigned short INT16U;
 
 
typedef enum{
	DATA_OK=0,
	ERR_DATALEN,
	ERR_STARTPOS,
	ERR_CHECKSUM,
	DATA_NULL,//无标识符
}DATA;
 
typedef struct{
	INT8U addr[6];
	INT8U ctr_Code;
	INT8U DataLen;
	INT8U Data[DataLenMAX];
	INT8U CheckSum;
}DataPacketFrame;
 
 
int packet(DataPacketFrame* inBuf,INT8U* outBuf );
 
 
int UnPacket(INT8U* inBuf,INT16U len,DataPacketFrame* outBuf);

 
#endif
