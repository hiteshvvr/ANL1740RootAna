#ifndef TV1740Waveform_h
#define TV1740Waveform_h

#include <string>
#include "THistogramArrayBase.h"

/// Class for making histograms of raw V1740 waveforms;
/// right now is only for raw or ZLE compressed data.
class TV1740Waveform : public THistogramArrayBase {
public:
  TV1740Waveform();
  virtual ~TV1740Waveform(){};

  void UpdateHistograms(TDataContainer& dataContainer);

  /// Getters/setters
  int GetNsecsPerSample() { return nanosecsPerSample; }
  void SetNanosecsPerSample(int nsecsPerSample) { this->nanosecsPerSample = nsecsPerSample; }

	// Reset the histograms; needed before we re-fill each histo.
	void Reset();

  void CreateHistograms();

	/// Take actions at begin run
	void BeginRun(int transition,int run,int time){		
		CreateHistograms();		
	}

private:
  int nanosecsPerSample;
};

#endif


