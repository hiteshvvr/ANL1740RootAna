#include "TV1740RawData.h"

#include <iomanip>
#include <iostream>

void TV1740RawData::HandlUncompressedData(){

  // Skip the header.
  uint32_t iPtr=4;
  uint32_t chgrMask    = GetData32()[1] & 0xFF;
  // const  uint32_t *fData = GetData32();
  // printf("%ld %ld %ld %ld \n",fData[0],fData[1],fData[2],fData[3]);

  int nActiveGroup=0;
  for(int iCh=0; iCh<8; iCh++){
    if(chgrMask & (1<<iCh))
      nActiveGroup++;
  }
  // Assume that we have readout the same number of samples for each Group and each Channel.
  // The number of 32 bit double-samples per channel is then
  // N32samples = (bank size - 4)/ ( 8 * nActiveGroup)
  int N32samples = (GetEventSize() - 4)/ (8*nActiveGroup);

  // Loop over channels
  for(int iCh=0; iCh<8; iCh++){
        
    if(!(chMask & (1<<iCh))){
      // Add empty channel data objects, to keep vector simple.
      TV1740RawChannel channel(iCh, IsZLECompressed());
      fMeasurements.push_back(channel);
      continue;
    }

    TV1740RawChannel channel(iCh, IsZLECompressed());
      
    for(int j = 0; j < N32samples; j++){
      uint32_t samp1 = (GetData32()[iPtr]&0xFFF);
      uint32_t samp2 = ((GetData32()[iPtr]>>16)&0xFFF);
      
      channel.AddADCSample(samp1);
      channel.AddADCSample(samp2);      
      iPtr++;
    }

    fMeasurements.push_back(channel);

  }  
}


TV1740RawData::TV1740RawData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  
  fGlobalHeader0 = GetData32()[0];
  fGlobalHeader1 = GetData32()[1];
  fGlobalHeader2 = GetData32()[2];
  fGlobalHeader3 = GetData32()[3];
  


  if(IsZLECompressed()){
    HandlZLECompressedData();
  }else{
    HandlUncompressedData();
  }

}

void TV1740RawData::Print(){

  std::cout << "V1740 decoder for bank " << GetName().c_str() << std::endl;
  std::cout << "Bank size: " << GetEventSize() << std::endl;
  
  if( IsZLECompressed())
    std::cout << "Data is ZLE compressed." << std::endl;
  else
    std::cout << "Data is not ZLE compressed." << std::endl;

  std::cout << "Channel Mask : " << GetChannelMask() << std::endl;
   
  std::cout << "Event counter : " << GetEventCounter() << std::endl;
  std::cout << "Trigger tag: " << GetTriggerTag() << std::endl;
  

  std::cout << "Number of channels with data: " << GetNChannels() << std::endl;
  for(int i = 0 ; i < GetNChannels(); i++){

    TV1740RawChannel channelData = GetChannelData(i);

    std::cout << "Channel: " << channelData.GetChannelNumber() << std::endl;
    
    if(IsZLECompressed()){
      std::cout << "Number of ZLE pulses: " <<  channelData.GetNZlePulses() << std::endl;
      for(int j = 0; j < channelData.GetNZlePulses(); j++){
	std::cout << "Pulse: " << j << std::endl;
	TV1740RawZlePulse pulse = channelData.GetZlePulse(j);	
	std::cout << "first sample bin: " << pulse.GetFirstBin() << std::endl;
	std::cout << "Samples ("<< pulse.GetNSamples()<<  " total): " <<std::endl;
	for(int k = 0; k < pulse.GetNSamples(); k++){
	  std::cout << pulse.GetSample(k) << ", ";
	  if(k%12 == 11) std::cout << std::endl;
	}
	std::cout << std::endl;
	
      }
    }

  }

}
