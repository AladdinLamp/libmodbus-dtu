/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <time.h>

#include <modbus.h>

#define G_MSEC_PER_SEC 1000

//------------------ 
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
//---

int packet(DataPacketFrame* inBuf,INT8U* outBuf ){
	INT16U i=0;
    INT16U pos=0;
	INT16U j=0;
	INT8U checkSum=0;
	
 
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
		printf(" %02x[j = %d] ", outBuf[j], j);
		
		checkSum+=outBuf[j];
	}
	
	outBuf[i++]=checkSum;
 
	outBuf[i]=0x16;
 
	
	return i + 1;
}
 
 
int UnPacket(INT8U* inBuf,INT16U len,DataPacketFrame* outBuf){
	INT16U i=0;
	INT16U pos=0;
	INT16U j=0;
	INT8U checkSum=0;
 
	for(j=0;j<len;j++){
		if(inBuf[j]==0xfe){
			i++;
		} else { 
			break;
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
		outBuf->Data[j] = inBuf[i++] - 0x33;
	}
 
	//校验和
	for(j=pos; j < i; j++){
		checkSum += inBuf[j];
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
		printf ("%02x ",  (int)frm->addr[i]);
	}
	printf("\n");

	printf("控制码为：%02x\n", (int)frm->ctr_Code);

	printf("数据长度为：%02x\n", (int)frm->DataLen);

	printf("数据为：");
	for (int i = 0; i < frm->DataLen; i++) {
		printf("%02x ",  (int)frm->Data[i]);
	}
	printf("\n");

	printf("cksum为：%02x\n", (int)frm->CheckSum);
}


static uint32_t gettime_ms(void)
{
    struct timeval tv;
#if !defined(_MSC_VER)
    gettimeofday(&tv, NULL);
    return (uint32_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else
    return GetTickCount();
#endif
}

enum {
    TCP,
    RTU
};

int test_dh(int argc, char **argv);


/* Tests based on PI-MBUS-300 documentation */
int main(int argc, char *argv[])
{

	return test_dh(argc, argv);


    uint8_t *tab_bit;
    uint16_t *tab_reg;
    modbus_t *ctx;
    int i;
    int nb_points;
    double elapsed;
    uint32_t start;
    uint32_t end;
    uint32_t bytes;
    uint32_t rate;
    int rc;
    int n_loop;
    int use_backend;

    if (argc > 1) {
        if (strcmp(argv[1], "tcp") == 0) {
            use_backend = TCP;
            n_loop = 100000;
        } else if (strcmp(argv[1], "rtu") == 0) {
            use_backend = RTU;
            n_loop = 100;
        } else {
            printf("Usage:\n  %s [tcp|rtu] - Modbus client to measure data bandwith\n\n", argv[0]);
            exit(1);
        }
    } else {
        /* By default */
        use_backend = TCP;
        n_loop = 100000;
    }
    n_loop = 1;

    if (use_backend == TCP) {
        ctx = modbus_new_tcp("127.0.0.1", 1502);
    } else {
        ctx = modbus_new_rtu("/dev/ttyXRUSB3", 9600, 'E', 8, 1);
        //ctx = modbus_new_rtu("/dev/ttyXRUSB2", 9600, 'N', 8, 1);
        //ctx = modbus_new_rtu("/dev/ttyXRUSB1", 9600, 'N', 8, 1);
        modbus_set_slave(ctx, 1);
    }
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    //modbus_set_debug(ctx, 1);

    /* Allocate and initialize the memory to store the status */
    tab_bit = (uint8_t *) malloc(MODBUS_MAX_READ_BITS * sizeof(uint8_t));
    memset(tab_bit, 0, MODBUS_MAX_READ_BITS * sizeof(uint8_t));

    /* Allocate and initialize the memory to store the registers */
    tab_reg = (uint16_t *) malloc(MODBUS_MAX_READ_REGISTERS * sizeof(uint16_t));
    memset(tab_reg, 0, MODBUS_MAX_READ_REGISTERS * sizeof(uint16_t));


    printf("READ REGISTERS\n\n");

    nb_points = 1;
    start = gettime_ms();
    for (i=0; i<n_loop; i++) {
        //rc = modbus_read_registers(ctx, 0, nb_points, tab_reg);
        //rc = modbus_read_registers(ctx, 65016, nb_points, tab_reg);
        rc = modbus_read_func_registers(ctx, 0x46, 65016, nb_points, tab_reg);
        //rc = modbus_read_func_registers(ctx, 3, 20166, nb_points, tab_reg);
        if (rc == -1) {
            fprintf(stderr, "read %s\n", modbus_strerror(errno));
            return -1;
        }
    }
    end = gettime_ms();
    elapsed = end - start;

    printf("%X%X\n", (tab_reg[0] & 0xFF00) >> 8, tab_reg[0] & 0x00FF);

    for (int i = 0; i < nb_points; i++) { 
	    printf ("i = %d | reg = %d \n", i, (int16_t)tab_reg[i]);
    }

    rate = (n_loop * nb_points) * G_MSEC_PER_SEC / (end - start);
    printf("Transfert rate in points/seconds:\n");
    printf("* %d registers/s\n", rate);
    printf("\n");

    //modbus_write_bit(ctx, 30001, 1); 
    //modbus_write_bit(ctx, 30003, 1); 
    //modbus_write_bit(ctx, 30005, 1); 
    //modbus_write_bit(ctx, 30002, 0); 
    //modbus_write_bit(ctx, 30004, 0); 
    //modbus_write_bit(ctx, 30006, 0); 
    //modbus_write_bit

    bytes = n_loop * nb_points * sizeof(uint16_t);
    rate = bytes / 1024 * G_MSEC_PER_SEC / (end - start);
    printf("Values:\n");
    printf("* %d x %d values\n", n_loop, nb_points);
    printf("* %.3f ms for %d bytes\n", elapsed, bytes);
    printf("* %d KiB/s\n", rate);
    printf("\n");

    /* TCP:Query and reponse header and values */
    bytes = 12 + 9 + (nb_points * sizeof(uint16_t));
    printf("Values and TCP Modbus overhead:\n");
    printf("* %d x %d bytes\n", n_loop, bytes);
    bytes = n_loop * bytes;
    rate = bytes / 1024 * G_MSEC_PER_SEC / (end - start);
    printf("* %.3f ms for %d bytes\n", elapsed, bytes);
    printf("* %d KiB/s\n", rate);
    printf("\n\n");
    free(tab_reg);




    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}

int combine(unsigned char *resp1, int len1,  unsigned char *resp2, int len2) 
{ 
	int i = 0; 

	for (; i < len2; i++) { 
		resp1[len1++] = resp2[i];
	}

	for (i = 0; i < len1; i++) { 
		printf("%02x ", resp1[i]);
	}
	printf("\n");

	return len1;
}

int test_dh(int argc, char **argv) 
{
	unsigned char resp[512];

    modbus_t *ctx;
	DataPacketFrame frm;
	
	ctx = modbus_new_rtu("/dev/ttyS3", 1200, 'E', 8, 1);

	modbus_set_slave(ctx, 1);
    modbus_set_debug(ctx, 1);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }


    memset(&frm, 0, sizeof(frm));
    for (int i = 0; i < 6;i++) { 
	    frm.addr[i] = 0x99;
    }
    frm.ctr_Code = 0x08;
    frm.DataLen = 6;

    time_t now = time(NULL);

    int  regnum = 6;
    struct tm *localtm = localtime(&now);

    frm.Data[0] = localtm->tm_sec;
    frm.Data[1] = localtm->tm_min;
    frm.Data[2] = localtm->tm_hour;
    frm.Data[3] = localtm->tm_mday;
    frm.Data[4] = localtm->tm_mon + 1;
    frm.Data[5] = (localtm->tm_year + 1900)%100;

    unsigned char outBuf[1024];

       int outlen = packet(&frm,outBuf);
        
       for(int i=0;i<outlen;i++){
	       printf("%02x ", (int)outBuf[i]);
       } 
       printf("\n");

	int req_list[] = {0xfe,0xfe,0x68,0x98,0x54,0x26,0x00,0x00,0x00,0x68,0x01,0x02,0x43,0xf3,0x19,0x16};
	//int req_list[]={0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x08,0x06,0x33,0x33,0x33,0x36,0x3B,0x47,0xC5,0x16};

	unsigned char req[124];

	int i = 0;
	for (i = 0; i < sizeof(req_list)/sizeof(int); i++) { 
		req[i] = req_list[i];
	}
	int rc;

	unsigned char cksum = 0;
	for (int j = 2; j < sizeof(req_list)/sizeof(int) - 2; j++) { 
		cksum += req[j];
	}
	req[sizeof(req_list)/sizeof(int) - 2 ] = cksum;

	rc = serial_send_raw_request(ctx, outBuf, outlen);
	if (rc < 0) { 
        fprintf(stderr, "serial_send_raw_request failed: %s\n",
                modbus_strerror(errno));
		return -1;

	}

	usleep(900000);

	memset(&frm, 0, sizeof(frm));
	/*
	for (int i = 0; i < 6;i++) { 
		frm.addr[i] = 0x99;
	}
	*/
	frm.addr[0] = 0x98;
	frm.addr[1] = 0x54;
	frm.addr[2] = 0x26;
	frm.addr[3] = 0x00;
	frm.addr[4] = 0x00;
	frm.addr[5] = 0x00;
	frm.ctr_Code = 0x01;
	frm.DataLen = 2;


	if (argc == 3) { 
		frm.Data[0] = strtol(argv[2], NULL, 16);
		frm.Data[1] = strtol(argv[1], NULL, 16);
	} else { 
		frm.Data[0] = 0x10;
		frm.Data[1] = 0xCF;
	}

       outlen = packet(&frm,outBuf);
        
       for(int i=0;i<outlen;i++){
	       printf("%02x ", (int)outBuf[i]);
       } 
       printf("\n");


	//rc = serial_send_raw_request(ctx, req, i);
	rc = serial_send_raw_request(ctx, outBuf, outlen);
	if (rc < 0) { 
        fprintf(stderr, "serial_send_raw_request failed: %s\n",
                modbus_strerror(errno));
		return -1;

	}

	//usleep(900000);

	rc = serial_receive_msg(ctx, resp, sizeof(resp));
	if (rc < 0) { 
        fprintf(stderr, "serial_receive_msg failed: %s\n",
                modbus_strerror(errno));
		return -1;
	}

	unsigned char resp1[56];
	resp1[0] = 0;
	int rc2 = serial_receive_msg(ctx, resp1, sizeof(resp1));
	if (rc2 > 0) { 
		rc = combine(resp, rc,  resp1, rc2);
	}

	int ret = UnPacket(resp, rc, &frm);
	if (ret != DATA_OK) { 
	}

	print645Package(&frm);


    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
