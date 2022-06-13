#include<iostream>
#include "protocol645.h"
using namespace std;

int packet(DataPacketFrame* inBuf,INT8U* outBuf ){
	INT16U i=0;
    INT16U pos=0;
	INT16U j=0;
	INT8U checkSum=0;
	
 
	outBuf[i++]=0xfe;
	outBuf[i++]=0xfe;
	outBuf[i++]=0xfe;
	outBuf[i++]=0xfe;
 
	pos=i;
	//起始符
	outBuf[i++]=0x68;
	//地址域
	for(j=0;j<6;j++){
		outBuf[i++]=inBuf->addr[j];
	}
	//起始符
	outBuf[i++]=0x68;
    //控制码
	outBuf[i++]=inBuf->ctr_Code;
	//数据长度
	outBuf[i++]=inBuf->DataLen;
	//数据
	for(j=0;j< inBuf->DataLen;j++){
		outBuf[i++]=inBuf->Data[j]+0x33;
	}
	//检验和
	for(j=pos;j<i;j++){
		
		checkSum+=outBuf[j];
	}
	
	outBuf[i++]=checkSum;
 
	outBuf[i]=0x16;
 
	
	return i;
}
 
 
int UnPacket(INT8U* inBuf,INT16U len,DataPacketFrame* outBuf){
	INT16U i=0;
	INT16U pos=0;
	INT16U j=0;
	INT8U checkSum=0;
 
	for(j=0;j<len;j++){
		if(inBuf[j]==0xfe){
			i++;
		}
	}

	pos=i;
	//前导码
	while (inBuf[i] != 0x68 && i < len) { 
		i++;
		pos = i;
	}

	if (inBuf[i] != 0x68) { 
		return ERR_STARTPOS;
	}

	if (len - pos < 12) {
		return ERR_DATALEN;
	}

	i++;

	//地址域
	for(j=0;j<6;j++){
		outBuf->addr[j]=inBuf[i++];
	}
 
	i++;
	//控制码
	outBuf->ctr_Code=inBuf[i++];
	//数据长度
	outBuf->DataLen=inBuf[i++];
	
	for (j = 0; j < outBuf->DataLen; j++) {
		outBuf->Data[j] = inBuf[i++];
	}
 
	//校验和
	for(j=pos;j<i;j++){
		checkSum += inBuf[j++];
	}

	outBuf->CheckSum = inBuf[i];

	if (checkSum != inBuf[i]) {
		return ERR_CHECKSUM;
	} else {
		outBuf->CheckSum = inBuf[i];
	}
 
	return DATA_OK;
}

void print645Package(DataPacketFrame *frm)
{
	printf("地址域为：");
	for (int i = 0; i < 6; i++) {
		printf ("%02x ",  (int)outBuf.addr[i]);
	}
	printf("\n");

	printf("控制码为：%02x\n", (int)outBuf.ctr_Code);

	printf("数据长度为：%02x\n", (int)outBuf.DataLen);

	printf("数据为：");
	for (int i = 0; i < outBuf.DataLen; i++) {
		printf("%02x ",  (int)outBuf.Data[i]);
	}
	printf("\n");

	printf("cksum为：%02x\n", (int)outBuf.CheckSum);
}

//测试组帧
INT8U testPACKET() {
 
	INT16U i=0;
	/*FE FE FE FE 68 18 00 00 00 00 00 68 14 10 01 01 00 04 01 00 00 00 78 56 34 12 05 07 08 15 81 16*/
	DataPacketFrame inBuf;
	INT8U outBuf[256];
	/*INT8U outBuf[]={0};*/
	INT8U addr[6]={0x18, 0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	INT8U data[]={ 0x01 ,0x01 ,0x00 ,0x04 ,0x02 ,0x00 ,0x00 ,0x00 ,0x78 ,0x56 ,0x34 ,0x12 ,0x05 ,0x07 ,0x08 ,0x15};
 
	for(i=0;i<6;i++){
		inBuf.addr[i]=addr[i];
	}

	inBuf.ctr_Code=0x14;
 
	inBuf.DataLen=sizeof(data)/sizeof(data[0]);
 
	/*cout<<len<<endl;*/
	for(i=0;i<inBuf.DataLen;i++){
		inBuf.Data[i]=data[i];
	}
 
	int outlen = packet(&inBuf,outBuf);
 
	for(i=0;i<outlen;i++){
		cout<<hex<<(int)outBuf[i]<<" ";
	}
	return 0;
}


 
//测试解帧
INT8U testUNPACKET() {
 
	DataPacketFrame outBuf;
	INT8U inBuf[] = { 0xFE, 0xFE ,0xFE ,0xFE ,0x68 ,0x18 ,0x00 ,0x00 ,0x00, 0x00 ,0x00 ,0x68, 0x14 ,0x10 ,0x01 ,0x01 ,0x00 ,0x04 ,0x02 ,0x00 ,0x00 ,0x00 ,0x78 ,0x56 ,0x34 ,0x12 ,0x05 ,0x07 ,0x08 ,0x15 ,0x81 ,0x16 };
	INT16U i = 0;
	while (inBuf[i] != 0x68) {
		i++;
	}
	INT16U DataLenPos = i + 9;
	INT16U datalen_INT = (INT16U)inBuf[DataLenPos];

	UnPacket(inBuf, datalen_INT, &outBuf);
 
	cout << "地址域为：";
	for (int i = 0; i < 6; i++) {
		cout << hex << (int)outBuf.addr[i] << "  ";
	}

	cout << endl;
	cout << "控制码为：";
	cout << hex << (int)outBuf.ctr_Code << endl;
 
	cout << "数据长度为：";
	cout << hex << (int)outBuf.DataLen << endl;
 
	cout << "数据为：";
	for (int i = 0; i < outBuf.DataLen; i++) {
		cout << hex << (int)outBuf.Data[i] << "  ";
	}
	cout << endl;
	cout << "cksum为：";
	cout << hex << (int)outBuf.CheckSum << endl;
	return 0;
}
 
int main(){
 
	testPACKET();
	printf("\n\n");
	testUNPACKET();
 
	return 0;
}
