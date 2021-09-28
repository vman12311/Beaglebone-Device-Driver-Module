#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>                 // Required for the GPIO functions
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/uaccess.h>	/* for copy_from_user */

#define  DEVICE_NAME "project_device"    ///< The device will appear at /dev/SpecialProj using this value
#define  CLASS_NAME  "proj_class"        ///< The device class -- this is a character device driver


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vikyat Miryala"); //shout out to my boy Derek Molloy...
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  testcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* testcharDevice = NULL; ///< The device-driver device struct pointer
static DEFINE_MUTEX(SpecialProj_mutex);
 
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
 
static struct file_operations fops =
{
   .open = dev_open,
   .write = dev_write,
   .release = dev_release,
};

static int ledOn = 1;   ///< LED on or off bits
static int ledOff = 0;

const unsigned int CQ_DEFAULT = 0;
const unsigned int *LEDtrigger[4]={53, 54, 55, 56}; 
//GPIO LEDs are hardcoded to these values from SRM ,LED:USR0 is 1_21 = 1*32+21=53 etc.
char Word[256]= {NULL};
char *L[256]= {NULL};

//static bool initialized;

char *morse_code[40] = {"",
".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",
".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",
".--","-..-","-.--","--..","-----",".----","..---","...--","....-",
".....","-....","--...","---..","----.","--..--","-.-.-.","..--.."};

char *mcodestring(int asciicode)
{
   char *mc;   // this is the mapping from the ASCII code into the mcodearray of strings.

   if (asciicode > 122)  // Past 'z'
      mc = morse_code[CQ_DEFAULT];
   else if (asciicode > 96)  // Upper Case
      mc = morse_code[asciicode - 96];
   else if (asciicode > 90)  // uncoded punctuation
      mc = morse_code[CQ_DEFAULT];
   else if (asciicode > 64)  // Lower Case 
      mc = morse_code[asciicode - 64];
   else if (asciicode == 63)  // Question Mark
      mc = morse_code[39];    // 36 + 3 
   else if (asciicode > 57)  // uncoded punctuation
      mc = morse_code[CQ_DEFAULT];
   else if (asciicode > 47)  // Numeral
      mc = morse_code[asciicode - 21];  // 27 + (asciicode - 48) 
   else if (asciicode == 46)  // Period
      mc = morse_code[38];  // 36 + 2 
   else if (asciicode == 44)  // Comma
      mc = morse_code[37];   // 36 + 1
   else
      mc = morse_code[CQ_DEFAULT];
   return mc;
}

int String_run (void)
{
int i;
for (i = 0; i < strlen(Word); i++)
{
    L[i] = mcodestring((int) Word[i]);
    printk(KERN_INFO "i=%d %s \n", i, L[i]);
    

    char *Ptr = L[i]; 
    while (*Ptr != 0)
    {
        if (*Ptr == '.')
        {
        gpio_set_value (LEDtrigger[0], ledOn);
        ssleep(1);
        gpio_set_value (LEDtrigger[0], ledOff);
        ssleep(1);
        }
        else if (*Ptr == '-')
        {
        gpio_set_value (LEDtrigger[0], ledOn);
        gpio_set_value (LEDtrigger[1], ledOn);
        gpio_set_value (LEDtrigger[2], ledOn);
        gpio_set_value (LEDtrigger[3], ledOn);

        ssleep(2);
        gpio_set_value (LEDtrigger[0], ledOff);
        gpio_set_value (LEDtrigger[1], ledOff);
        gpio_set_value (LEDtrigger[2], ledOff);
        gpio_set_value (LEDtrigger[3], ledOff);
        ssleep(1);
        }
        printk(KERN_INFO "%c\n", *Ptr);
        Ptr++;
    }
        
    }

return 0;
}

int First (void)
{
    const char * First_Message="Hello Sir";
    strcpy(Word, First_Message);
    printk(KERN_INFO "%s\n", Word);
    String_run ();
    int i;

    return 0;
}


static int dev_open(struct inode *inodep, struct file *filep)
{
    if (!mutex_trylock(&SpecialProj_mutex))
   {                                      /// Try to acquire the mutex (i.e., put the lock on/down)
                                          /// returns 1 if successful and 0 if there is contention
      printk(KERN_ALERT "SpecialProj: Device in use by another process");
      return -EBUSY;
   }
   numberOpens++;
   printk(KERN_INFO "SpecialProj: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
   copy_from_user(Word, buffer, len);   // appending received string with its length
   size_of_message = strlen(Word); // store the length of the stored message  
   printk(KERN_INFO "%s", Word);
   printk(KERN_INFO "SpecialProj: Received %zu characters from the user\n", len);
   printk(KERN_INFO "Received %d size_of_message\n", size_of_message);
   if (size_of_message < 1)
   {
       return len;
   }
   else
   {
   String_run();
   return 0;
   }
}

static int dev_release(struct inode *inodep, struct file *filep)
{    
    mutex_unlock(&SpecialProj_mutex);          /// Releases the mutex (i.e., the lock goes up)
    printk(KERN_INFO "SpecialProj: Device successfully closed\n");
    return 0;
}


static int __init gpio_init(void){
     mutex_init(&SpecialProj_mutex);
 printk(KERN_INFO "SpecialProj: Initializing SpecialProj\n");
 majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
 if (majorNumber < 0)
 {
    printk(KERN_ALERT "SpecialProj failed to register a major number\n");
    return majorNumber;
 }
 
 printk(KERN_INFO "SpecialProj: registered correctly with major number %d\n", majorNumber);

    // Register the device class
   testcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(testcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(testcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "SpecialProj: device class registered correctly\n");
 
   // Register the device driver
   testcharDevice = device_create(testcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(testcharDevice)){               // Clean up if there is an error
      class_destroy(testcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(testcharDevice);
   }
   printk(KERN_INFO "SpecialProj: device class created correctly\n"); // Made it! device was initialized
   
   printk(KERN_INFO "SpecialProj: Initializing the SpecialProj LKM\n");
   // Is the GPIO a valid GPIO number (e.g., the BBB has 4x32 but not all available)
   int i;
   for (i = 0; i < 4; i++)
   {
   if (!gpio_is_valid(LEDtrigger[i]))
        {
            printk(KERN_INFO "SpecialProj: invalid GPIO %d\n", LEDtrigger);
            return -ENODEV;
        }
   // set up the LED GPIO. It is a GPIO in output mode and will be started on ledOff
   gpio_request(LEDtrigger[i], "sysfs");          // gpioLED is hardcoded to 49, request it
   gpio_direction_output( LEDtrigger[i], ledOff);   // Set the gpio to be in output mode and on
   gpio_export(LEDtrigger[i], false);             // Causes gpio49 to appear in /sys/class/gpio
   }
   
   return 0;
}


static void __exit gpio_exit(void)
{
    mutex_destroy(&SpecialProj_mutex);                       // destroy the dynamically-allocated mutex
    device_destroy(testcharClass, MKDEV(majorNumber, 0));     // remove the device
    class_unregister(testcharClass);                          // unregister the device class
    class_destroy(testcharClass);                             // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number

    int i;
    for (i = 0; i < 4; i++)
    {
    gpio_set_value(LEDtrigger[i], ledOff);              // Turn the LED off, makes it clear the device was unloaded
    gpio_unexport(LEDtrigger[i]);                  // Unexport the LED GPIO
    gpio_free(LEDtrigger[i]);
    }// Free the LED GPIO
    printk(KERN_INFO "SpecialProj: Goodbye from the LKM!\n");
}

module_init(gpio_init);
module_exit(gpio_exit);
