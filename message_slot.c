#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/types.h>
#include <linux/slab.h>
#include <linux/kernel.h>   
#include <linux/module.h>   
#include <linux/fs.h>       
#include <linux/uaccess.h>  
#include <linux/string.h>

MODULE_LICENSE("GPL");

#include "message_slot.h"

static MessageSlot slots[257]; 

//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode, struct file*  file ){
  unsigned minor = iminor(inode);
  if (!slots[minor].channels) {
    slots[minor].channels = kmalloc(sizeof(Channel), GFP_KERNEL);
    if (!slots[minor].channels) {
      return -ENOMEM;
    }
    slots[minor].len = 0;
    slots[minor].channels->id = -1; // Indicates that no channel is currently selected
    slots[minor].channels->message_len = 0;
    slots[minor].channels->next = NULL;
  }
  file->private_data = (void*)-1; // Indicates no channel selected
  return SUCCESS;
}

static int device_release( struct inode* inode, struct file*  file)
{ 
  return SUCCESS;
}

static ssize_t device_read(struct file* file,char __user* buffer, size_t length, loff_t* offset)
{ 
  int minor;
  unsigned long channel_id;
  Channel *currChannel;
  
  minor = iminor(file_inode(file));
  if ((unsigned long)file->private_data == -1) {
    return -EINVAL; // No valid channel selected
  }
  channel_id = (unsigned long)file->private_data;
  currChannel = slots[minor].channels;

  while (currChannel != NULL && currChannel->id != channel_id) {
    currChannel = currChannel->next;
  }
  if (currChannel == NULL){
    // No channel is currently selected
    return -EINVAL; 
  }
  if (currChannel->message_len == 0){
    return -EWOULDBLOCK; // No message in the channel
  }
  if (length < currChannel->message_len) {
    return -ENOSPC; // Buffer too small
  }
  if (copy_to_user(buffer, currChannel->message, currChannel->message_len) != 0) {
    return -EFAULT;
  }
  return currChannel->message_len;
}

static ssize_t device_write( struct file* file, const char __user* buffer, size_t length, loff_t* offset)
{
  int minor;
  unsigned long channel_id;
  Channel *currChannel;
  // Check if the length of the message is within the valid range
  if (length == 0 || length > BUF_LEN) {
    return -EMSGSIZE; // Return error if message size is not valid
  }
  minor = iminor(file_inode(file));
  if ((unsigned long)file->private_data == -1) {
    return -EINVAL; // No valid channel selected
  }
  channel_id = (unsigned long)file->private_data;
  currChannel = slots[minor].channels;

  while (currChannel != NULL && currChannel->id != channel_id) {
    currChannel = currChannel->next;
  }

  if (currChannel == NULL) {
    // No channel is currently selected
    return -EINVAL;
  }

  if (copy_from_user(currChannel->message, buffer, length) != 0) {
    return -EFAULT; // Return error if copy_from_user fails
  }
  currChannel->message_len = length;

  return length;
}

static long device_ioctl(struct file* file, unsigned int   ioctl_command_id, unsigned long  ioctl_param)
{  
  int minor;
  Channel *currChannel, *prevChannel;
  if (ioctl_command_id == IOCTL_SET_MSG_SLOT_CHANNEL) {
    if (ioctl_param == 0) {
      // Invalid channel ID, return an error
      return -EINVAL;
    }
    
    minor = iminor(file_inode(file));
    currChannel = slots[minor].channels;

    if (slots[minor].len +1 >= MAX_CHANNELS) {
    return -ENOSPC; // no more than 2^20 message channels will be used
    }

    prevChannel = NULL;
    while (currChannel != NULL && currChannel->id != ioctl_param) {
      prevChannel = currChannel;
      currChannel = currChannel->next;
    }
    if (currChannel == NULL) {
      currChannel = kmalloc(sizeof(Channel), GFP_KERNEL);
      slots[minor].len++;
      if (!currChannel) {
        // Memory allocation failed
        return -ENOMEM;
      }

      currChannel->id = ioctl_param;
      currChannel->message_len = 0;
      currChannel->next = NULL;

      if (prevChannel != NULL) {
        prevChannel->next = currChannel;
      } else {
        slots[minor].channels = currChannel;
      }
    }

    file->private_data = (void*)ioctl_param;
    return SUCCESS;
  }
  return -EINVAL;
}
//==================== DEVICE SETUP =============================
// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops = {
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

static int __init message_slot_init(void) {
    int ret_val, i;
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME, &Fops);
    if (ret_val < 0) {
      printk(KERN_ERR "%s failed registering the character device for %d\n", DEVICE_RANGE_NAME, MAJOR_NUM);
      return ret_val;
    }
    for (i = 0; i < 257; i++){
        slots[i].channels = NULL;
    }
    printk(KERN_INFO "Message slot device registered, it is assigned major number %d\n", MAJOR_NUM);
    return SUCCESS; // Non-zero return means that the module couldn't be loaded.
}

static void __exit message_slot_cleanup(void) {
  int i;
  Channel *channels, *temp;
  for (i = 0; i < 257; i++){
    channels = slots[i].channels;
    while (channels != NULL){
      temp = channels;
      channels = channels->next;
      kfree(temp);
    }
  }
  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}
module_init(message_slot_init);
module_exit(message_slot_cleanup);
//========================= END OF FILE =========================