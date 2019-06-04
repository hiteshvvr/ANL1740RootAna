// Example Program for converting MIDAS format to ROOT format.
//
// T. Lindner (Jan 2016)
//
// Example is for the CAEN V792 ADC module

#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <bitset>

#include "TFile.h"
#include "TRootanaEventLoop.hxx"
#include "TTree.h"
#include "TGraph.h"
#include "TNtuple.h"

#include <fstream>
// using namespace std;

#include "TAnaManager.hxx"
#include "TH2D.h"
#include "TH1D.h"
#include "THistogramArrayBase.h"

#ifdef USE_V1740
#include "TV1740RawData.h"
#endif


#ifdef USE_V1720
#include "TV1720RawData.h"
#endif

class Analyzer : public TRootanaEventLoop
{
public:
    // An analysis manager.  Define and fill histograms in
    // analysis manager.
    TAnaManager *anaManager;

    // The tree to fill.
    TTree *fTree;
    TGraph *gr;


    // Getting raw data if required Generates large files
    int getrawdata = 1;
    std::ofstream outfile;

#ifdef USE_V1740
    int v1740ch0data[4096];
#endif

    TH1D *a =  new TH1D("QuadA", "QuadA", 1000, 00, 5000);
    Analyzer() {};

    virtual ~Analyzer() {};

    void Initialize() {}

    void BeginRun(int transition, int run, int time)
    {
        // Create a TTree
        if (getrawdata == 1)
        {
            TString csvfile = Form("v174_csv%05d.csv",run);
            outfile.open(csvfile);
        }
    }

    void EndRun(int transition, int run, int time) {

    a->SetXTitle("sample");
    a->SetYTitle("Value");
    a->Write("Pulse");



        if (getrawdata == 1)
            outfile.close();
        printf("\n");
    }

    // Main work here; create ttree events for every sequenced event in
    // Lecroy data packets.
    bool ProcessMidasEvent(TDataContainer &dataContainer)
    {
        int midasid = dataContainer.GetMidasEvent().GetSerialNumber();
        int i, numsamples, j, adc;
        uint32_t timetag;

#ifdef USE_V1740
        TV1740RawData *v1740 = dataContainer.GetEventData<TV1740RawData>("DG01");
        if(v1740)
        {

            timetag = v1740->GetTriggerTag();
            int channelmask = v1740->GetChannelMask();
            TV1740RawChannel channelData = v1740->GetChannelData(0);
            numsamples = channelData.GetNSamples();
            printf("Valueofnumsam::: %d\n",numsamples);
            outfile << midasid << "------" << "\n";
            for (j = 0; j < numsamples; j++)
            {
                outfile << timetag + j*16 << "  " << channelData.GetADCSample(j) << "\n";
                a->SetBinContent (j, channelData.GetADCSample(j));
            }
        }

        else 
            printf("No 1740 Data \n");
#endif

        return true;
    };

    // Complicated method to set correct filename when dealing with subruns.
    std::string SetFullOutputFileName(int run, std::string midasFilename) {
        char buff[128];
        Int_t in_num = 0, part = 0;
        Int_t num[2] = {0, 0};  // run and subrun values
        // get run/subrun numbers from file name
        for (int i = 0;; ++i) {
            char ch = midasFilename[i];
            if (!ch) break;
            if (ch == '/') {
                // skip numbers in the directory name
                num[0] = num[1] = in_num = part = 0;
            } else if (ch >= '0' && ch <= '9' && part < 2) {
                num[part] = num[part] * 10 + (ch - '0');
                in_num = 1;
            } else if (in_num) {
                in_num = 0;
                ++part;
            }
        }
        if (part == 2) {
            if (run != num[0]) {
                std::cerr << "File name run number (" << num[0]
                          << ") disagrees with MIDAS run (" << run << ")" << std::endl;
                exit(1);
            }
            sprintf(buff, "output_%.6d_%.4d.root", run, num[1]);
            printf("Using filename %s\n", buff);
        } else {
            sprintf(buff, "outfiles/run_root%.5d.root", run);
        }
        return std::string(buff);
    };
};

int main(int argc, char *argv[]) {
    Analyzer::CreateSingleton<Analyzer>();
    return Analyzer::Get().ExecuteLoop(argc, argv);
}


