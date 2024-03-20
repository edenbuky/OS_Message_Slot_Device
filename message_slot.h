#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

// The major device number.
// We don't rely on dynamic registration
// any more. We want ioctls to know this
// number at compile time.
#define MAJOR_NUM 235

// Set the message of the device driver
#define IOCTL_SET_MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)

#define DEVICE_RANGE_NAME "message_slot"
#define BUF_LEN 128
#define DEVICE_FILE_NAME "msg_slot"
#define SUCCESS 0
#define MAX_CHANNELS 1048576  // 2^20

// Structure to represent a channel in a message slot
typedef struct Channel {
  int id;
  char message[BUF_LEN];
  int message_len;
  struct Channel *next;
} Channel;

// Structure to represent a message slot, which contains multiple channels
typedef struct MessageSlot {
  struct Channel *channels;
  int len;
} MessageSlot;

#endif
