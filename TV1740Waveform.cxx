#include "TV1740Waveform.h"

#include "TV1740RawData.h"
#include "TDirectory.h"


/// Reset the histograms for this canvas
TV1740Waveform::TV1740Waveform(){

  SetNanosecsPerSample(16); //ADC clock runs at 64Mhz on the v1740 = units of 16 nsecs

  CreateHistograms();
}


void TV1740Waveform::CreateHistograms(){

  // check if we already have histogramss.
  char tname[100];
  sprintf(tname,"V1740_%i",0);

  TH1D *tmp = (TH1D*)gDirectory->Get(tname);
  if (tmp) return;

	int fWFLength = 1000; // Need a better way of detecting this...
  // int numSamples = fWFLength / nanosecsPerSample;
  int numSamples = 400;

  // Otherwise make histograms
  clear();

	for(int i = 0; i < 8; i++){ // loop over 8 channels
		
		char name[100];
		char title[100];
		sprintf(name,"V1740_%i",i);

		sprintf(title,"V1740 Waveform for channel=%i",i);	
		
		TH1D *tmp = new TH1D(name, title, numSamples, 0, fWFLength);
		tmp->SetXTitle("ns");
		tmp->SetYTitle("ADC value");
		
		push_back(tmp);
	}

}


void TV1740Waveform::UpdateHistograms(TDataContainer& dataContainer){

  int eventid = dataContainer.GetMidasData().GetEventId();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();


	//    char name[100];
	//sprintf(name,"W2%02d",iBoard);

        TV1740RawData *v1740 = dataContainer.GetEventData<TV1740RawData>("DG01");
        if(v1740)
        {
		for(int i = 0; i < 8; i++){ // loop over channels
			int index = i;
			
			// Reset the histogram...
			//        for(int ib = 0; ib < 2500; ib++)
			for(int ib = 0; ib < 400; ib++)
				GetHistogram(index)->SetBinContent(ib+1,0);
			
			
			TV1740RawChannel channelData = v1740->GetChannelData(i);
			for(int j = 0; j < channelData.GetNSamples(); j++){
				double adc = channelData.GetADCSample(j);
				GetHistogram(index)->SetBinContent(j+1,adc);
				
			}            		
    }
  }

}



void TV1740Waveform::Reset(){


	for(int i = 0; i < 8; i++){ // loop over channels
		int index =  i;

		// Reset the histogram...
		for(int ib = 0; ib < GetHistogram(index)->GetNbinsX(); ib++) {
			GetHistogram(index)->SetBinContent(ib, 0);
		}

		GetHistogram(index)->Reset();
    
  }
}
