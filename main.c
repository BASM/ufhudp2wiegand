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

#define VID    0x10c4
#define PID    0xea60
static libusb_device_handle *h = NULL;

void handle_error(int ret) {
  if (ret<0) { 
    perror("Failed to write to PL2303 device");
    fprintf(stderr, "Have you installed the correct udev rules?\n");
    exit(1);
  }
}
int main(int argc, char *argv[]) {

  // GPIO -> libusb
  check_handle(&h, VID, PID, NULL, NULL, NULL, -1);

  if (argc==2) {
    int a,b,num;
    num=atoi(argv[1]);
    printf("Set port: %i\n", num);
    a=num&1;
    b=num&2;

    gpio_out(h, 0, a);
    gpio_out(h, 1, b);
  }


  return 0;
}
