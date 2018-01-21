#include <run_manager.h>
#include <exp_setup.h>
#include <event.h>
#include <detector.h>

#include "TGraph2D.h"
#include "TFile.h"
#include "TCanvas.h"

RunManger::RunManger(bool flag )
{
  generate_fake_event = flag;

}

void RunManger::run(long int n_events)
{
  // Generating Simulated Events
  for (size_t i = 1; i <= n_events; i++)
  {
    if( n_events % i == 0)
    {
      std::cout << "Generating event #" << i << ". . ." << '\n';
    }

    Event * sample_ev = new Event(i,generate_fake_event);
    event_list.push_back(sample_ev);

  }

  // Defining OutpuT Graph
  TGraph2D * d1_detected = new TGraph2D();
  d1_detected->SetName("d1_det");
  TGraph2D * d2_detected = new TGraph2D();
  d2_detected->SetName("d2_det");
  TGraph2D * d3_detected = new TGraph2D();
  d3_detected->SetName("d3_det");

  TGraph2D * not_det = new TGraph2D();
  not_det->SetName("not_det");

  int ev_det_1 = 0;
  int ev_det_2 = 0;
  int ev_det_3 = 0;
  int ev_not_det = 0;

  int d1_c = 0;
  int d2_c = 0;
  int d3_c = 0;
  int dnot_c = 0;

  // Retrieving Detectors
  std::vector<CylindricalDetector*> det_list = setup->get_detector_list();

  CylindricalDetector * d1 = det_list[0];
  CylindricalDetector * d2 = det_list[1];
  CylindricalDetector * d3 = det_list[2];

  // Checking Detection

  bool d1_check_det,d2_check_det,d3_check_det;
  long int coincidences = 0;

  arma::vec event_momentum;

  double energy_sum;

  for (size_t i = 0; i < event_list.size(); i++)
  {

    std::vector<Photon *> photon_list = event_list[i]->get_gamma_configuration();

    d1_check_det = false;
    d2_check_det = false;
    d3_check_det = false;

    energy_sum = 0;

    for (size_t j = 0; j < photon_list.size(); j++)
    {

      arma::vec photon_momentum = photon_list[j]->get_momentum();
      double photon_energy = photon_list[j]->get_energy();

      if( j == 0 ) det_1_sp->Fill(photon_energy);
      if( j == 1 ) det_2_sp->Fill(photon_energy);
      if( j == 2 ) det_3_sp->Fill(photon_energy);

      energy_sum += photon_energy;

      if(d1->check_detection(photon_momentum))
      {
        d1_detected->SetPoint(ev_det_1, photon_momentum[0],photon_momentum[1], photon_momentum[2]);
        ev_det_1++;
        d1_c++;
        d1_check_det = true;
      }
      else if(d2->check_detection(photon_momentum))
      {
        d2_detected->SetPoint(ev_det_2, photon_momentum[0],photon_momentum[1], photon_momentum[2]);
        ev_det_2++;
        d2_c++;
        d2_check_det = true;

      }
      else if(d3->check_detection(photon_momentum))
      {
        d3_detected->SetPoint(ev_det_3, photon_momentum[0],photon_momentum[1], photon_momentum[2]);
        ev_det_3++;
        d3_c++;
        d3_check_det = true;
      }
      else
      {
        not_det->SetPoint(ev_not_det, photon_momentum[0],photon_momentum[1], photon_momentum[2]);
        ev_not_det++;
        dnot_c++;
      }

      if (d1_check_det and d2_check_det and d3_check_det ) coincidences++;
    }

    momentum_sp->Fill(energy_sum);
  }

  // Outputting result via iostream
  std::cout << "Detected photon in #1 :" << d1_c << '\n';
  std::cout << "Detected photon in #2 :" << d2_c << '\n';
  std::cout << "Detected photon in #3 :" << d3_c << '\n';
  std::cout << "Coincidences :" << coincidences << '\n';
  std::cout << "Not detected :" << dnot_c << '\n';


  // Output Result
  TFile * outfile = new TFile("risultati.root","RECREATE");

  d1_detected->Write();
  d2_detected->Write();
  d3_detected->Write();
  not_det->Write();
  det_1_sp->Write();
  det_2_sp->Write();
  det_3_sp->Write();
  momentum_sp->Write();

  outfile->Close();



  return ;
}