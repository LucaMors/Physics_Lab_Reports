#ifndef RUN_MANAGER_H
#define RUN_MANAGER_H

#include <vector>
#include <event.h>
#include <exp_setup.h>

#include "TH1F.h"

// Up to now work only with our experimental setup due to the lack of
// time for a general implementation

class RunManger
{
  private:

    std::vector<Event*> event_list;
    bool generate_fake_event; // Debugging Purpouse
    ExperimentalSetup * setup = new ExperimentalSetup();

    TH1F * det_1_sp = new TH1F("d1_en_sp","Energy Spectrum D#1",511,0,511);
    TH1F * det_2_sp = new TH1F("d2_en_sp","Energy Spectrum D#2",511,0,511);
    TH1F * det_3_sp = new TH1F("d3_en_sp","Energy Spectrum D#3",511,0,511);
    TH1F * coincidences_sp_1 = new TH1F("coincidences_sp_1","Triple Coincidences #1",511,0,511);
    TH1F * coincidences_sp_2 = new TH1F("coincidences_sp_2","Triple Coincidences #2",511,0,511);
    TH1F * coincidences_sp_3 = new TH1F("coincidences_sp_3","Triple Coincidences #3",511,0,511);

  public:

    RunManger(bool );
    void run(long int );

};

#endif
