#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "message_slot.h"  // Ensure this points to the correct header file path

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <message slot file path> <target message channel id>\n", argv[0]);
        return 1;
    }

    const char* filePath = argv[1];
    unsigned long channelId = strtoul(argv[2], NULL, 10);
    char message[BUF_LEN];  // Use the BUF_LEN from message_slot.h

    int fileDesc = open(filePath, O_RDONLY);
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

    // Read the message
    ssize_t bytesRead = read(fileDesc, message, sizeof(message));
    if (bytesRead < 0) {
        perror("Failed to read the message");
        close(fileDesc);
        return 1;
    }

    // Ensure string is null-terminated to safely print
    message[bytesRead < BUF_LEN ? bytesRead : BUF_LEN - 1] = '\0';

    printf("%s\n", message);  // Print the message

    close(fileDesc);
    return 0;  // Success
}
