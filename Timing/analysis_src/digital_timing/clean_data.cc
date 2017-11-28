/*
*
* Reading TTree from CAEN Digitizer
* Channel aligning
*/

#include "opt_par.h"

struct acqEventPSD_t {

	ULong64_t	timetag;
	UInt_t		baseline;
	UShort_t	qshort;
	UShort_t	qlong;
	UShort_t	pur;
	UShort_t	samples[4096];
};

class cleanEv : public TObject
{
	public:

		long int timetag;
		short qshort;
		short qlong;
		short samples[4096];


		ClassDef(cleanEv, 1)

};

ClassImp(cleanEv)

void clean_data(string input_file_name, string outfile_name)
{

	// Retrieving File
  TFile * input_file = new TFile(input_file_name.c_str());

	TTree * daq_tree;

  // Retrieving TTree from TFile

  daq_tree = (TTree*) input_file->Get("acq_tree_0");

  // Linking

  acqEventPSD_t event_ch_A, event_ch_B;

  TBranch * branch_ch_A, * branch_ch_B;
  branch_ch_A = daq_tree->GetBranch(Form("acq_ch%i",0));
  branch_ch_A->SetAddress(&event_ch_A.timetag);
  branch_ch_B = daq_tree->GetBranch(Form("acq_ch%i",1));
  branch_ch_B->SetAddress(&event_ch_B.timetag);

  // Align channels

  long int nEventsA = (long int) branch_ch_A->GetEntries();
  long int nEventsB = (long int) branch_ch_B->GetEntries();

	// Declaring Saving ttree structure

	cleanEv * event_ch_A_clean = new  cleanEv;
	cleanEv * event_ch_B_clean = new  cleanEv;

	TFile * outfile = new TFile(outfile_name.c_str(),"RECREATE");

	TTree * out_tree = new TTree("acq_tree_0", "Dati Allineati");

	out_tree->Branch("acq_ch0",&event_ch_A_clean);
	out_tree->Branch("acq_ch1",&event_ch_B_clean);

	// aligning channels

	int counter_A = 0;
	int skipA = 0;
	int counter_B = 0;
	int skipB = 0;
	int coincident_events = 0;

	int id_A = 0;
	int id_B = 0;

	long int time_difference = 0;


	while(counter_A < nEventsA and counter_B < nEventsB)
	{

	    // Display Progress

	    if (counter_A % (nEventsA/100) == 0 or counter_B % (nEventsB/100) == 0)
	    {
	        system("clear");
	        std::cout << "Loading Event " << counter_A << endl;
	        std::cout << "-----------------------\n";
	        std::cout << "Load "
	                  <<  (int)((counter_A*100.0)/nEventsA)
	                  << "% in channel A"
	                  << std::endl;
	        std::cout << "Load "
	                  <<  (int)((counter_B*100.0)/nEventsB)
	                  << "% in channel B"
	                  << std::endl;
	    }

	    // Events filling

	    branch_ch_A->GetEntry(counter_A);
	    branch_ch_B->GetEntry(counter_B);

	    time_difference = event_ch_A.timetag-event_ch_B.timetag;

	    if(TMath::Abs(time_difference) > 100) //  <--- scegli la finestra di allinamento in sample 1sample=1ns
	    {
	        if( time_difference > 0){ counter_B++; skipB++; } else { counter_A++; skipA++; }
	    }
	    else
	    {

				event_ch_A_clean->timetag = event_ch_A.timetag;
				event_ch_A_clean->qlong = event_ch_A.qlong;

				event_ch_B_clean->timetag = event_ch_B.timetag;
				event_ch_B_clean->qlong = event_ch_B.qlong;


				for (size_t i = 0; i < 480; i++)
				{
					event_ch_A_clean->samples[i] = event_ch_A.samples[i];
					event_ch_B_clean->samples[i] = event_ch_B.samples[i];

				}
				for (size_t i = 480; i < 4096; i++)
				{
					event_ch_A_clean->samples[i] = 0;
					event_ch_B_clean->samples[i] = 0;
				}

				out_tree->Fill();

	      // updating counters

				coincident_events++;
	      id_A++;
	      id_B++;
	      counter_A++;
	      counter_B++;

    }
	}

	out_tree->Write();
	outfile->Close();

  return ;
}
