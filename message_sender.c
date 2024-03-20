#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "message_slot.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <message slot file path> <target message channel id> <message to pass>\n", argv[0]);
        return 1;
    }

    const char* filePath = argv[1];
    unsigned long channelId = strtoul(argv[2], NULL, 10);
    const char* message = argv[3];
    size_t messageLen = strlen(message);

    int fileDesc = open(filePath, O_WRONLY);
    if (fileDesc < 0) {
        perror("Failed to open the device file");
        return 1;
    }
    // Set the channel ID
    if (ioctl(fileDesc, IOCTL_SET_MSG_SLOT_CHANNEL, channelId) != 0) {
        perror("Failed to set the channel ID");
        close(fileDesc);
        return 1;
    }
    // Write the message
    ssize_t bytesWritten = write(fileDesc, message, messageLen);
    if (bytesWritten < 0) {
        perror("Failed to write the message");
        close(fileDesc);
        return 1;
    }
    close(fileDesc);
    return 0;  // Success
}