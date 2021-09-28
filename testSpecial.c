#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

int main()
{
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   fd = open("/dev/project_device", O_RDWR);             // Open the device with read/write access   
   if (fd < 0)
   {
      perror("Failed to open the device...");
      return errno;
   }
   
   
   printf("Type in a short string to send to the kernel module:\n");
   scanf("%[^\n]%*c", stringToSend);                // Read in a string: %[^\n]%*c reads w/ spaces
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); // Writes string to character special files
   if (ret < 0)
   {
      perror("Failed to write the message to the device.");
      return errno;
   }
   
/**
   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: [%s]\n", receive);
   
   
   printf("1 for Caps, 2 for Lower, 3 for Upper\n");
   scanf("%[^\n]%*c", secondToSend);                // Read in a string: %[^\n]%*c reads w/ spaces
   
   if (strcmp(secondToSend, "1") == 0)
    {
           ret = write(fd, "Next_Is_Option2231", 50); // Writes string to character special files
            if (ret < 0)
            {
                perror("Failed to write the message to the device.");
                return errno;
            }
    }
   else if (strcmp(secondToSend, "2") == 0)
    {
           ret = write(fd, "Next_Is_Option2232", 50); // Writes string to character special files
            if (ret < 0)
            {
                perror("Failed to write the message to the device.");
                return errno;
            }
    }    
   else if (strcmp(secondToSend, "3") == 0)
    {
           ret = write(fd, "Next_Is_Option2231", 50); // Writes string to character special files
            if (ret < 0)
            {
                perror("Failed to write the message to the device.");
                return errno;
            }
    }
    else
    {
        perror("Did not input correct option");
        return errno;
    }
       
   printf("Sending option to Kernel\n");
   printf("%s", secondToSend);
   ret = write(fd, secondToSend, strlen(secondToSend)); // Writes string to character special files
   if (ret < 0)
   {
      perror("Failed to write the message to the device.");
      return errno;
   }
   
   ret = write(fd, secondToSend, strlen(secondToSend)); // Writes string to character special files
   if (ret < 0)
   {
      perror("Failed to write the message to the device.");
      return errno;
   }
 
   
   
   
   
   printf("Press ENTER to read back from the device...\n");
   getchar();
 
   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");
**/

   return 0;
}
