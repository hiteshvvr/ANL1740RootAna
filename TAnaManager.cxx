#include "TAnaManager.hxx"
#include "TV1740RawData.h"


TAnaManager::TAnaManager(){
    fV1740Waveform = 0;
#ifdef USE_V1740
    fV1740Waveform = new TV1740Waveform();
    fV1740Waveform->DisableAutoUpdate();  // disable auto-update.  Update histo in AnaManager.
#endif
};



int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){
    if(fV1740Waveform)  fV1740Waveform->UpdateHistograms(dataContainer);
    return 1;
}

bool TAnaManager::HaveV1740Histograms(){
  if(fV1740Waveform) return true; 
  return false;
  };

TV1740Waveform* TAnaManager::GetV1740Histograms(){return fV1740Waveform;}

