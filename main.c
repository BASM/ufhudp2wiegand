#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libusb.h>

#include <inttypes.h>

static libusb_device_handle *h = NULL;
static int debug = 1;

#define WGPULSETIME 2
#define WGPAUSETIME 20
static int sendone(int num) {

  if (num==0) {
    gpio_out(h, 0, 0);
    usleep(WGPULSETIME);
    gpio_out(h, 0, 1);
  } else {
    gpio_out(h, 2, 0);
    usleep(WGPULSETIME);
    gpio_out(h, 2, 1);
  }
  //if (debug) printf("1");
  return 0;
}

static int sendzero(int num) {
  if (num==0) {
    gpio_out(h, 1, 0);
    usleep(WGPULSETIME);
    gpio_out(h, 1, 1);
  } else {
    gpio_out(h, 3, 0);
    usleep(WGPULSETIME);
    gpio_out(h, 3, 1);
  }
  //if (debug) printf("0");
  return 0;
}

static int senduint32_t(int num, uint32_t cmdle) {
  int i;
  int tmp;
  int odd=1,even=0;
  //uint32_t cmd=htobe32(cmdle);
  uint32_t cmd=cmdle;

  for (tmp=0,i=0; i<16; i++)
    if ((cmd>>i)&1) tmp++;
  if ((tmp%2)==0) odd=0;

  for (tmp=0,i=16; i<32; i++)
    if ((cmd>>i)&1) tmp++;
  if ((tmp%2)==0) even=1;

  if (even)  sendzero(num);
  else       sendone(num);
  usleep(WGPAUSETIME);

  if (debug) printf(" ");
  for (i=0; i<32; i++) {
    int b = (cmd>>(31-i))&1;
    if (b) sendone(num);
    else   sendzero(num);
    usleep(WGPAUSETIME);
    if ((i%8)==7) if (debug) printf(" ");
  }

  if (odd) sendzero(num);
  else     sendone(num);
  usleep(WGPAUSETIME);
  return 0;
}


int UhrWgSend(int num, uint32_t wgcmd) {
  printf("Send to WG: %i: %i\n", num, wgcmd);
  senduint32_t(num, wgcmd);
  usleep(10000);
  return 0;
}

#define VID    0x10c4
#define PID    0xea60

void handle_error(int ret) {
  if (ret<0) { 
    perror("Failed to write to PL2303 device");
    fprintf(stderr, "Have you installed the correct udev rules?\n");
    exit(1);
  }
}
typedef struct {
  uint32_t key;
  char     devname[32];
} udpmsg;

static int msgarrvd(udpmsg *msg) {
  int bar1=0,bar2=0;
  uint32_t key;
  char *devname=msg->devname;
  key=le32toh(msg->key);
  printf("Msg from: %s, key: %i (0x%x)\n", msg->devname,key,key);

  if (strstr(devname, "barr1")!=NULL) bar1=1;
  if (strstr(devname, "barr2")!=NULL) bar2=1;
  if (strstr(devname, "dev")!=NULL) bar2=bar1=1;

  if (bar1==1) UhrWgSend(0, key);
  if (bar2==1) UhrWgSend(1, key);

  return 1;
}

#define BUFLEN 2048
int main(int argc, char *argv[]) {
  int                s,status,recv_len;
  char               msg[2048];
  struct sockaddr_in si_me, si_other;
  int                slen = sizeof(si_other);

  // GPIO -> libusb
  check_handle(&h, VID, PID, NULL, NULL, NULL, -1);

  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s==-1) {
    perror("Error create socket");
    return 1;
  }
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(12000);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to port
  status = bind(s , (struct sockaddr*)&si_me, sizeof(si_me));
  if (status==-1) {
    perror("Error bind socket");
    close(s);
    return 1;
  }

  while (1) {
    printf("Waiting for data...");
    fflush(stdout);

    recv_len = recvfrom(s, msg, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
    if (recv_len>0) {
      msgarrvd((udpmsg *)msg);
    }
  }
  close(s);
  return 0;
}
