#include "MCP3008.h"
using namespace std;
/**********************************************************
 * spiOpen() :function is called by the constructor.
 * It is responsible for opening the spidev device
 * "devspi" and then setting up the spidev interface.
 * private member variables are used to configure spidev.
 * They must be set appropriately by constructor before calling
 * this function.
 * *********************************************************/
int MCP3008::spiOpen(std::string devspi){
    int statusVal = -1;
    this->spifd = open(devspi.c_str(), O_RDWR);
    if(this->spifd < 0){
        perror("could not open SPI device");
        exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_WR_MODE, &(this->mode));
    if(statusVal < 0){
        perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_RD_MODE, &(this->mode));
    if(statusVal < 0) {
      perror("Could not set SPIMode (RD)...ioctl fail");
      exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_WR_BITS_PER_WORD, &(this->bitsPerWord));
    if(statusVal < 0) {
      perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
      exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_RD_BITS_PER_WORD, &(this->bitsPerWord));
    if(statusVal < 0) {
      perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
      exit(1);
    }  
 
    statusVal = ioctl (this->spifd, SPI_IOC_WR_MAX_SPEED_HZ, &(this->speed));    
    if(statusVal < 0) {
      perror("Could not set SPI speed (WR)...ioctl fail");
      exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_RD_MAX_SPEED_HZ, &(this->speed));    
    if(statusVal < 0) {
      perror("Could not set SPI speed (RD)...ioctl fail");
      exit(1);
    }
    return statusVal;
}
 
/***********************************************************
 * spiClose(): Responsible for closing the spidev interface.
 * Called in destructor
 * *********************************************************/
 
int MCP3008::spiClose(){
    int statusVal = -1;
    statusVal = close(this->spifd);
        if(statusVal < 0) {
      perror("Could not close SPI device");
      exit(1);
    }
    return statusVal;
}
 
/********************************************************************
 * This function writes data "data" of length "length" to the spidev
 * device. Data shifted in from the spidev device is saved back into
 * "data".
 * ******************************************************************/
int MCP3008::spiWriteRead( unsigned char *data, int length){
 
  struct spi_ioc_transfer spi[length];
  int i = 0;
  int retVal = -1; 
  bzero(spi, sizeof spi); // ioctl struct must be zeroed 
 
// one spi transfer for each byte
 
  for (i = 0 ; i < length ; i++){
 
    spi[i].tx_buf        = (unsigned long)(data + i); // transmit from "data"
    spi[i].rx_buf        = (unsigned long)(data + i) ; // receive into "data"
    spi[i].len           = sizeof(*(data + i)) ;
    spi[i].delay_usecs   = 0 ;
    spi[i].speed_hz      = this->speed ;
    spi[i].bits_per_word = this->bitsPerWord ;
    spi[i].cs_change = 0;
}
 
 retVal = ioctl (this->spifd, SPI_IOC_MESSAGE(length), &spi) ;
 
 if(retVal < 0){
    perror("Problem transmitting spi data..ioctl");
    exit(1);
 }
 
return retVal;
 
}

int MCP3008::readChannel(int channel)
{
  if (channel>=0&&channel<8){
    unsigned char data[3];
    int value;
    data[0] = 1;  //  first byte transmitted -> start bit
    data[1] = 0b10000000 |( ((channel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
    data[2] = 0; // third byte transmitted....don't care
  
    spiWriteRead(data, sizeof(data));
  
    value = 0;
    value = (data[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
    value |=  (data[2] & 0xff);
    return value;
  }
  return -1;
}
 
/*************************************************
 * Default constructor. Set member variables to
 * default values and then call spiOpen()
 * ***********************************************/
 
MCP3008::MCP3008(){
    this->mode = SPI_MODE_0 ;
    this->bitsPerWord = 8;
    this->speed = 1000000;
    this->spifd = -1;
 
    this->spiOpen(std::string("/dev/spidev0.0"));
 
    }
 
/*************************************************
 * overloaded constructor. let user set member variables to
 * and then call spiOpen()
 * ***********************************************/
MCP3008::MCP3008(std::string devspi, unsigned char spiMode, unsigned int spiSpeed, unsigned char spibitsPerWord){
    this->mode = spiMode ;
    this->bitsPerWord = spibitsPerWord;
    this->speed = spiSpeed;
    this->spifd = -1;
 
    this->spiOpen(devspi);
 
}
 
/**********************************************
 * Destructor: calls spiClose()
 * ********************************************/
MCP3008::~MCP3008(){
    this->spiClose();
}