#ifndef TV1740RawData_hxx_seen
#define TV1740RawData_hxx_seen

#include <vector>

#include "TGenericData.hxx"


/// Class to store information from a single V1740 channel.
/// Class will store either the full ADC waveform (if not compressed)
/// or a vector of TV1740RawZlePulse (if compressed).
class TV1740RawChannel {
  
 public:

  /// constructor
 TV1740RawChannel(int channel): 
  fChannelNumber(channel){
      
  }


  int GetChannelNumber() const {return fChannelNumber;};

  
  /// Get the ADC sample for a particular bin (for uncompressed data).
  int GetNSamples() const{return fWaveform.size();};

  /// Get the ADC sample for a particular bin (for uncompressed data).
  int GetADCSample(int i) const{
    if(i >= 0 && i < (int)fWaveform.size())
      return fWaveform[i];

    // otherwise, return error value.
    return -1;
  }

  /// Returns true for objects with no ADC samples or ZLE pulses.
  int IsEmpty() const {
    if(fWaveform.size()==0) return true;
    return false;
  }

  /// Add an ADC sample
  /// Warning: this method just adds an ADC sample to the back 
  /// of the vector.  Must add in order and must not add any sample twice.
  void AddADCSample(uint32_t sample){ fWaveform.push_back(sample);};
  

 private:

  /// Channel number
  int fChannelNumber;
  std::vector<uint32_t> fWaveform;

};


/// Class to store data from CAEN V1740, 64.5MHz ADC.
///
/// For Full info on CAEN V1740 module look in CAEN website. 
///
/// This class encapsulates the data from a single board (in a single MIDAS bank).
/// This decoder is for the raw waveform data and not for the DPP data.
class TV1740RawData: public TGenericData {

public:

  /// Constructor
  TV1740RawData(int bklen, int bktype, const char* name, void *pdata);

  /// Get the number of 32-bit words in bank.
  uint32_t GetEventSize() const {return (fGlobalHeader0 & 0xfffffff);};

  /// Get the channel Group mask; ie, the set of channels for which we 
  /// have data for this event.
  uint32_t GetChannelMask() const {return (fGlobalHeader1 & 0xff);};

  /// Is the V1740 data ZLE compressed?
  bool IsZLECompressed() const {return ((fGlobalHeader1 >> 24) & 0x1);};

  /// Get event counter
  uint32_t GetEventCounter() const {return ((fGlobalHeader2) & 0xffffff);};

  /// Get trigger tag
  uint32_t GetTriggerTag() const {return ((fGlobalHeader3) & 0xffffffff);};


  /// Get Number of channels in this bank.
  int GetNChannels() const {return fMeasurements.size();}
  
  /// Get Channel Data
  TV1740RawChannel GetChannelData(int i) {
    if(i >= 0 && i < (int)fMeasurements.size())
      return fMeasurements[i];

    return TV1740RawChannel(0);
  }
  

  void Print();


private:

  /// Helper method to handle uncompressed data.
  void HandlUncompressedData();

  /// The overall global headers
  uint32_t fGlobalHeader0;
  uint32_t fGlobalHeader1;
  uint32_t fGlobalHeader2;
  uint32_t fGlobalHeader3;
  

  /// Vector of V1740 measurements
  std::vector<TV1740RawChannel> fMeasurements;

};

#endif
