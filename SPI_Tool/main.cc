#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h> 
#include <iostream>
#include <stdlib.h> 
#include <string>
#include <vector>
#include "ftd2xx.h"
#include "libft4222.h"
#include "DrvSPIs.h"

using namespace std;

namespace {
  vector<FT_DEVICE_LIST_INFO_NODE>FT4222DevList;
  FT_HANDLE ftHandle = (FT_HANDLE)NULL;
  const char *usageBanner = "Usage: WEI_SPIrecvImg [Img count]\r\n     Img count=0 for infinite record loop\r\n     Img count=N to record N images\r\n";
} // namespace

int InitFT4222() {
  FT_STATUS ftStatus;
  DWORD numDevs = 0;
  int i, retCode = 0,found4222 = 0;

  do {
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK) {
      printf("FT_CreateDeviceInfoList failed (error code %d)\n", (int)ftStatus);
      retCode = -10;
      break;
    }
    if (numDevs == 0) {
      printf("No devices connected.\n");
      retCode = -20;
      break;
    }

    for (DWORD iDev = 0; iDev < numDevs; ++iDev) {
      FT_DEVICE_LIST_INFO_NODE devInfo {0};

      ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
                                          devInfo.SerialNumber,
                                          devInfo.Description,
                                          &devInfo.ftHandle);

        if (FT_OK == ftStatus) {
            string desc = devInfo.Description;

            if (desc == "FT4222" || desc == "FT4222 A")
            {
                FT4222DevList.push_back(devInfo);
                found4222 ++;
                //cout<<devInfo.Description<<endl;
            }
        }
    }

    if (found4222 == 0)
        cout<<"No FT4222H detected."<<endl;
  }while(false);

  return retCode;
}

int OpenFT4222() {
  int retCode = 0;
  FT_STATUS ftStatus;
  
  do {
    //use first of FT4222
    if(FT4222DevList.size() == 0) {
      break;
    }
    ftStatus = FT_OpenEx(reinterpret_cast<PVOID>(FT4222DevList[0].LocId), 
                         FT_OPEN_BY_LOCATION, 
                         &ftHandle);
    if (ftStatus != FT_OK) {
        printf("FT_OpenEx failed (error %d)\n", (int)ftStatus);
        break;
    }
    
  }while(false);

  return retCode;  
} 

int storeImage(vector<uint8>&img, string&name ) {
  int fd;
  int nameLnegth = name.length();
  //char nameArr[nameLnegth+1];
  //strcpy(nameArr,name.c_str());
  fd = open (name.c_str(),O_CREAT | O_TRUNC | O_RDWR, 0666) ;
  if(fd == -1) {
    cout<<"unable to open file"<<endl;
    return -1;
  }    
  else {
    if(write(fd, &img[0], sizeof(uint8)*img.size()) <0) {
      cout<<"write file fail"<<endl;
    }
    close(fd);
  }
  cout<<name<<endl;
  return 0;
}
int main(int argc, char *argv[]) {
 
  uint32 fileLength = 0;
  uint8 buf;
  vector<uint8>fileData;
  string fileName, fullName;
  uint32 fileNo=0;
  long long totalFileNo = 1;
  vector<uint8> imgData;

  if(argc > 2 || argc == 1)  {
    cout<<usageBanner;  
    return -1;
  }
  else {
   totalFileNo = atoi(argv[1]);
  }
  fileName = "default";

  InitFT4222();
  OpenFT4222();
  
  DrvSpiSInit((PVOID)ftHandle);
  do {
    DRVSpiSRecveData(imgData);
    fullName=fileName+to_string(fileNo)+".dat";
    if(storeImage(imgData,fullName)!=0)
      break;
    fileNo++;

    if(totalFileNo!= 0 && fileNo>=totalFileNo)
      break;
  }while(1);
  //cout<<"recv 1 img"<<endl;
  return 0;

    
  
}
