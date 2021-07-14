#ifndef DRVSPIS__H_
#define DRVSPIS__H_
#include <vector>
#include <iostream>
extern int DRVSpiSFlag; 
int DrvSpiSInit(void *handle);
int DRVSpiSRecveData(std::vector<uint8> &Data); 
#endif