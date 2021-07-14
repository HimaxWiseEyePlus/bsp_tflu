#include <iostream>
#include <string>
#include <vector>
#include "ftd2xx.h"
#include "libft4222.h"

#define DRV_HANDLE_CHECK(condition) (condition) ? (void)0 : abort()
#define SPI_SYNC_POS_V2     0x0
#define SPI_SYNC_SIZE_V2	0x02

#define SPI_DATA_TYPE_POS_V2 SPI_SYNC_POS_V2+SPI_SYNC_SIZE_V2
#define SPI_DATA_TYPE_SIZE_V2 0x1

#define SPI_LENGTH_POS_V2   SPI_DATA_TYPE_POS_V2+SPI_DATA_TYPE_SIZE_V2
#define SPI_LENGTH_SIZE_V2  0x04

#define SPI_HEADER_V2   SPI_LENGTH_POS_V2+SPI_LENGTH_SIZE_V2
#define SPI_SYNC_CMD1 0xC0
#define SPI_SYNC_CMD2 0x5A

using namespace std;

int DRVSpiSFlag = 0;

namespace {
  FT_HANDLE ftHandle = (FT_HANDLE)NULL;
  union sizeUnion {
    uint32 sizeInt;
    struct
    {
      uint8 b1;
      uint8 b2;
      uint8 b3;
      uint8 b4; // or number them in reverse order
    } myBytes;
  };
} // namespace

int DRVSpiSRecveData(vector<uint8> &Data) {

  uint16 sizeTransferred = 0;
  uint16 rxSize, raw_data_sz = 0;
  vector<uint8>recvBuf;
  vector<uint8>dataBuf;

  //sizeShortUnion write_size;
  sizeUnion data_size_v2;
  bool header_flag = false;
  int retCode = -1;

  DRV_HANDLE_CHECK(ftHandle != NULL); 

  DRVSpiSFlag = 1;
  recvBuf.clear();
  dataBuf.clear();
  while (DRVSpiSFlag) {
    if (DRVSpiSFlag && (FT4222_SPISlave_GetRxStatus(ftHandle, &rxSize) == FT4222_OK)) {

      if (rxSize == 0)
        continue;

      recvBuf.resize(rxSize);

      if (FT4222_SPISlave_Read(ftHandle, &recvBuf[0], rxSize, &sizeTransferred) == FT4222_OK) {
        if (sizeTransferred != rxSize) {
          //printf("read data size is not equal\n");
        }
        else {
          // append data to dataBuf
          dataBuf.insert(dataBuf.end(), recvBuf.begin(), recvBuf.end());
        }
      }
      else {
        break;
      }

      if(dataBuf.size()>=SPI_HEADER_V2 && header_flag == false) {
	    //check if any sync byte inside
	    while(DRVSpiSFlag && dataBuf.size() >= 1) {
	      if(dataBuf[SPI_SYNC_POS_V2] != SPI_SYNC_CMD1)
		    dataBuf.erase(dataBuf.begin());
		  else if(dataBuf.size() ==1)
		    break;
		  else if(dataBuf.size() >=2 && dataBuf[SPI_SYNC_POS_V2+1] != SPI_SYNC_CMD2)
		    dataBuf.erase(dataBuf.begin(),dataBuf.begin()+2);
		  else {
		    if(dataBuf.size() >= SPI_HEADER_V2) {
		      //data type can be found at dataBuf[SPI_DATA_TYPE_POS_V2];
			  data_size_v2.myBytes.b1 = dataBuf[SPI_LENGTH_POS_V2];
			  data_size_v2.myBytes.b2 = dataBuf[SPI_LENGTH_POS_V2+1];
			  data_size_v2.myBytes.b3 = dataBuf[SPI_LENGTH_POS_V2+2];
			  data_size_v2.myBytes.b4 = dataBuf[SPI_LENGTH_POS_V2+3];
			  header_flag = true;
			  dataBuf.erase(dataBuf.begin(), dataBuf.begin() + SPI_HEADER_V2);
              cout<<"image size:"<<data_size_v2.sizeInt<<endl;
		    }
            break;
		  }
	    }

	  } // end of if

	  if( header_flag == true && (dataBuf.size() >= data_size_v2.sizeInt)) {
	    header_flag = false;
        Data.clear();
        Data.insert(Data.begin(),dataBuf.begin(),dataBuf.begin()+data_size_v2.sizeInt);
	    
	    dataBuf.erase(dataBuf.begin(), dataBuf.begin() + data_size_v2.sizeInt);
        DRVSpiSFlag = 0;
        retCode = 0;
	  }

    } // end of FT4222_SPISlave_GetRxStatus
    
  } // end of while 

  return retCode;
}

int DrvSpiSInit(void *handle) {
  ftHandle = (FT_HANDLE)handle;
  int retCode = -1;

  FT4222_ClockRate clk = SYS_CLK_80;

  DRV_HANDLE_CHECK((FT_HANDLE)handle != NULL); 
  
  do{
    if(FT4222_OK != FT4222_SetClock((FT_HANDLE)handle, clk))
      break;
    if(FT4222_OK != FT4222_SPISlave_InitEx(handle, SPI_SLAVE_NO_PROTOCOL))
      break;
    if(FT4222_OK != FT4222_SPI_SetDrivingStrength((FT_HANDLE)handle, DS_4MA, DS_4MA, DS_4MA))
    break;  
    if(FT4222_OK != FT_SetUSBParameters((FT_HANDLE)handle, 4 * 1024, 0))
    break;  

    retCode = 0; 
    DRVSpiSFlag = 1;
  }while(false);  

  if(retCode != 0)
    cout<<"DrvSpiSInit FAIL"<<endl;    
  
  return retCode;
}