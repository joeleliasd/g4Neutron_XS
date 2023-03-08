#include <stdlib.h>
#include <TString.h>
#include <stdio.h>
#include <math.h>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"
#include "TFile.h"
#include "TTree.h"
#include "TSpline.h"
#include "TMath.h"

std::string tuple_list = "tuplelist.txt";
std::string material_name = "C";

const double multiplier = 1.0;
bool rw = true;

TGraph * total;
TGraph * elastic;
TGraph * inelastic;

TGraph * rw_total;
TGraph * rw_elastic;
TGraph * rw_inelastic;

TSpline3 * g4spl;
TSpline3 * elspl;
TSpline3 * scaled_elspl;

// the data for total cross section
double data_x[15] =  { 200.5, 231.4, 327.3, 374.4, 374.4, 466.9, 537.2, 593.8, 697.4, 794.7, 890.4, 998.4, 1146.4, 1626.6, 2107.7 };
double data_y[15] =  { 162.4, 166.6, 174.9, 175.6, 171.6, 177.9, 187.1, 188.4, 195.4, 201.8, 202.2, 202.6,  202.7,  195.6,  188.0 };
double data_ex[15] = {   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,    0.0,    0.0,    0.0 };
double data_ey[15] = {   1.9,   1.3,   0.8,   0.9,   4.7,   4.3,   4.2,   4.0,   3.8,   3.7,   3.8,   3.6,    3.4,    3.2,    3.3 };

// the data for "reaction" cross section, which includes nucleon knock-out, CX, etc. - evertything but elastic K-nucleus scattering
double data_reac_x[4] =  { 200.5, 231.4, 327.3, 374.4 };
double data_reac_y[4] =  { 120.4, 129.3, 141.8, 149.3 };
double data_reac_ex[4] = {   0.0,   0.0,   0.0,   0.0 };
double data_reac_ey[4] = {   2.3,   1.4,   1.5,   1.5 };

// Copied from analysis area
// below 1 GeV fit is a + b*x^0.5
const double low_a = 130.024;
const double low_b = 2.409;
// above 1 GeV fit is a + b*x^-0.5
const double high_a = 148.197;
const double high_b = 1863.265;

// how many steps to integrate the spline file numerically
const int nsteps = 100;

const double Na = 6.023*pow(10,23);
const double atomic_mass = 12.0;

const double conv_to_cm2 = 6.023*pow(10,23) * pow(10.,-27) / 12.0;

// calculate GEANT4 elastic cross section from the spline
double geantElasticXS( double Ei, double Ef )
{

  if( fabs(Ei-Ef) < 1.0e-6 ) { // interaction happens in first GEANT step, so return value
    if( Ei > 9999.9 ) return elspl->Eval( 9999.9 );
    else return elspl->Eval( Ei );
  }

  double total = 0.0;
  double step = (Ei-Ef)/(nsteps-1);
  for( int i = 0; i < nsteps; ++i ) {
    if( Ef+i*step > 50.0 && Ef+i*step < 9999.9 ) total += step * elspl->Eval( Ef + i*step );
    else if( Ef+i*step <= 50.0 ) total += step * elspl->Eval( 50.0 ); // cut it off at 50 MeV
    else total += step * elspl->Eval( 9999.9 ); // cut it off at 10 GeV
  }

  return total / (Ei-Ef);
}

// calculate DATA elastic cross section from the scaled spline
double dataElasticXS( double Ei, double Ef )
{

  if( fabs(Ei-Ef) < 1.0e-6 ) { // interaction happens in first GEANT step, so return value
    if( Ei > 9999.9 ) return scaled_elspl->Eval( 9999.9 );
    else return scaled_elspl->Eval( Ei );
  }

  double total = 0.0;
  double step = (Ei-Ef)/(nsteps-1);
  for( int i = 0; i < nsteps; ++i ) {
    if( Ef+i*step > 50.0 && Ef+i*step < 9999.9 ) total += step * scaled_elspl->Eval( Ef + i*step );
    else if( Ef+i*step <= 50.0 ) total += step * scaled_elspl->Eval( 50.0 ); // cut it off at 50 MeV
    else total += step * scaled_elspl->Eval( 9999.9 ); // cut it off at 10 GeV
  }

  return total / (Ei-Ef);
}

// calculate average value of data-driven cross section analytically from the fit
double dataXS( double Ei, double Ef )
{

  if( Ef < 0.0 ) Ef = 0.0;

  if( fabs(Ei-Ef) < 1.0e-6 ) { // interaction happens in first GEANT step, so return value
    if( Ei < 1000.0 ) return low_a + low_b*pow(Ei,0.5);
    else return high_a + high_b*pow(Ei,-0.5);
  }

  if( Ei < 1000.0 ) { // use below 1 GeV for whole thing
    return low_a + ( (2./3.) * low_b * (pow(Ei,1.5)-pow(Ef,1.5)) / (Ei-Ef) );
  } else if( Ef > 1000 ) { // use above 1 GeV for whole thing
    return high_a + ( 2 * high_b * (sqrt(Ei)-sqrt(Ef)) / (Ei-Ef) );
  } else { // starts above 1 GeV, ends below 1 GeV
    double high_part = high_a * (Ei-1000.) + ( 2 * high_b * (sqrt(Ei)-sqrt(1000.)) ); // Ef = 1000
    double low_part = low_a * (1000.-Ef) + ( (2./3.) * low_b * (pow(1000.,1.5)-pow(Ef,1.5)) ); // Ei = 1000
    return (low_part + high_part) / (Ei-Ef);
  }

  // this can't happen
  return 0.;

}

// calculate average value of geant4 cross section by brute forcing the spline
double geantXS( double Ei, double Ef )
{

  if( fabs(Ei-Ef) < 1.0e-6 ) { // interaction happens in first GEANT step, so return value
    if( Ei > 9999.9 ) return g4spl->Eval( 9999.9 );
    else return g4spl->Eval( Ei );
  }

  double total = 0.0;
  double step = (Ei-Ef)/(nsteps-1);
  for( int i = 0; i < nsteps; ++i ) {
    if( Ef+i*step > 50.0  && Ef+i*step < 9999.9 ) total += step * g4spl->Eval( Ef + i*step );
    else if( Ef+i*step <= 50.0 ) total += step * g4spl->Eval( 50.0 ); // cut it off at 50 MeV
    else total += step * g4spl->Eval( 9999.9 ); // cut it off at 10 GeV
  }

  return total / (Ei-Ef);

}


// These functions give you the weights for reweighting only the inelastic reactions and assuming the elastic MC is correct
double getCVWeightInelastic( double t, double intE, double len, double scale )
{
  double geantEl = geantElasticXS(t,intE) * conv_to_cm2;
  double dataEl = dataElasticXS(t,intE) * conv_to_cm2;
  double dataTotal = ( (dataXS(t,intE) - dataEl) * scale + dataEl ) * conv_to_cm2; // scale only the inelastic part
  double geantTotal = geantXS(t,intE) * conv_to_cm2;

  double totReac = ( 1-TMath::Exp(-1.0*len*dataTotal) ) / ( 1-TMath::Exp(-1.0*len*geantTotal) );
  double geantRatio = geantTotal / (geantTotal-geantEl); // total over inelastic
  double dataRatio = (dataTotal-dataEl) / dataTotal; // inelastic over total

  return totReac * geantRatio * dataRatio;
}

double getCVWeightElastic( double t, double intE, double len, double scale )
{
  double geantEl = geantElasticXS(t,intE) * conv_to_cm2;
  double dataEl = dataElasticXS(t,intE) * conv_to_cm2;
  double dataTotal = ( (dataXS(t,intE) - dataEl) * scale + dataEl ) * conv_to_cm2; // scale only the inelastic part
  double geantTotal = geantXS(t,intE) * conv_to_cm2;

  double totReac = ( 1-TMath::Exp(-1.0*len*dataTotal) ) / ( 1-TMath::Exp(-1.0*len*geantTotal) );
  double geantRatio = geantTotal / geantEl; // total over elastic
  double dataRatio = dataEl / dataTotal; // elastic over total

  return totReac * geantRatio * dataRatio;
}

double getCVWeightUnreacted( double t, double intE, double len, double scale )
{
  double dataEl = dataElasticXS(t,intE) * conv_to_cm2; // need this to compute scaled total XS
  double dataTotal = ( (dataXS(t,intE) - dataEl) * scale + dataEl ) * conv_to_cm2; // scale only the inelastic part
  double geantTotal = geantXS(t,intE) * conv_to_cm2;
  return TMath::Exp( -1.0*len*(dataTotal - geantTotal) );
}


void total_XS( std::string filename )
{

  double n_elastic   = 0.; // this is really just an int
  double n_inelastic = 0.;
  double nrw_elastic   = 0.;
  double nrw_inelastic = 0.;

  TFile *f = new TFile( filename.c_str(), "OLD" );
  TTree *T = (TTree *) f->Get("nTarget");

  int npart, numberEvts;
  double elasticXS, inelasticXS, thickness, density, radius, enerPrimGen;
  int pdg[100], intType[100];
  double x[100][3], p[100][4]; // filled once per particle (nPart)

  T->SetBranchAddress("npart", &npart);
  T->SetBranchAddress("pdg", pdg);
  T->SetBranchAddress("x", x);
  T->SetBranchAddress("p", p);
  T->SetBranchAddress("intType",intType);

  TTree *T2 = (TTree *) f->Get("RunInfo");
  T2->SetBranchAddress("density", &density);
  T2->SetBranchAddress("elasticXS", &elasticXS);
  T2->SetBranchAddress("inelasticXS", &inelasticXS);
  T2->SetBranchAddress("enerPrimGen", &enerPrimGen);
  T2->SetBranchAddress("numberEvts", &numberEvts);
  T2->SetBranchAddress("tickness", &thickness); // target thickness, in mm
  T2->SetBranchAddress("radius", &radius);
  T2->GetEntry(0); // the only entry

  int n_entries = T->GetEntries();

  for( int i = 0; i < n_entries; ++i ) {
    T->GetEntry(i);

    if( npart == 1 && intType[0] == 2 ) {
      n_elastic += 1.0;

      double wgt = getCVWeightElastic( enerPrimGen, 0.0, 2.0, multiplier );
      nrw_elastic += wgt;
    } else {
      n_inelastic += 1.0;

      double wgt = getCVWeightInelastic( enerPrimGen, enerPrimGen, (x[0][2]+5.0)*0.2, multiplier );
      nrw_inelastic += wgt;
    }
  }

  double convert_to_cm2_per_C_atom = atomic_mass / (density*Na*thickness*0.1);

  // convert number of events to a cross section for the default (non-reweighted)
  double total_int = n_elastic + n_inelastic;
  double surviveNum_total = numberEvts - total_int;
  double total_XS = log(numberEvts/surviveNum_total) * convert_to_cm2_per_C_atom * pow(10,27); // in mb
  double inelastic_XS = (n_inelastic/total_int)*total_XS;
  double elastic_XS = (n_elastic/total_int)*total_XS;

  int n = total->GetN();
  total->SetPoint( n, enerPrimGen, inelastic_XS+elastic_XS );
  elastic->SetPoint( n, enerPrimGen, elastic_XS );
  inelastic->SetPoint( n, enerPrimGen, inelastic_XS );

  // convert number of events to a cross section for the reweighted
  double rw_total_int = nrw_elastic + nrw_inelastic;
  // get the unreweighted number of non-interacting, and weight it by unreacted
  double rw_surviveNum_total = (numberEvts - total_int) * getCVWeightUnreacted( enerPrimGen, enerPrimGen, 2.0, multiplier );
  double rw_numberEvts = rw_total_int + rw_surviveNum_total;

  double norm_factor = numberEvts / rw_numberEvts;
  rw_numberEvts *= norm_factor;
  rw_surviveNum_total *= norm_factor;
  rw_total_int *= norm_factor;
  nrw_elastic *= norm_factor;
  nrw_inelastic *= norm_factor;

  double rw_total_XS = log(rw_numberEvts/rw_surviveNum_total) * convert_to_cm2_per_C_atom * pow(10,27); // in mb
  double rw_inelastic_XS = (nrw_inelastic/rw_total_int)*rw_total_XS;
  double rw_elastic_XS = (nrw_elastic/rw_total_int)*rw_total_XS;

  n = rw_total->GetN();
  rw_total->SetPoint( n, enerPrimGen, rw_inelastic_XS+rw_elastic_XS );
  rw_elastic->SetPoint( n, enerPrimGen, rw_elastic_XS );
  rw_inelastic->SetPoint( n, enerPrimGen, rw_inelastic_XS );

}

void kaon_xs_plot()
{

  TFile * tf = new TFile("g4Kaon.root", "OLD");
  TGraph * g4 = (TGraph*) tf->Get("total");
  TGraph * g4el = (TGraph*) tf->Get("elastic");
  TGraph * g4el_scaled = (TGraph*) tf->Get("elastic_scaled");
  g4spl = new TSpline3( "totalspline", g4 );
  elspl = new TSpline3( "elasticspline", g4el );
  scaled_elspl = new TSpline3( "scaledelasticspline", g4el_scaled );

  total = new TGraph();
  total->SetName("total");
  total->SetTitle(";Kaon KE (MeV);Total XS (mb)");
  elastic = new TGraph();
  elastic->SetName("elastic");
  elastic->SetTitle(";Kaon KE (MeV);Elastic XS (mb)");
  inelastic = new TGraph();
  inelastic->SetName("inelastic");
  inelastic->SetTitle(";Kaon KE (MeV);Inelastic XS (mb)");
  rw_total = new TGraph();
  rw_total->SetName("rw_total");
  rw_total->SetTitle(";Kaon KE (MeV);Total XS (mb)");
  rw_elastic = new TGraph();
  rw_elastic->SetName("rw_elastic");
  rw_elastic->SetTitle(";Kaon KE (MeV);Elastic XS (mb)");
  rw_inelastic = new TGraph();
  rw_inelastic->SetName("rw_inelastic");
  rw_inelastic->SetTitle(";Kaon KE (MeV);Inelastic XS (mb)");


  ifstream  tp_list(tuple_list.c_str());
  std::string tuple;
  while( tp_list >> tuple ) {
    total_XS( tuple );
  }

  TFile * fout = new TFile( "g4Kaon.root", "RECREATE" );
  fout->cd();
  elastic->Write();
  total->Write();
  inelastic->Write();

  TGraphErrors * data = new TGraphErrors( 15, data_x, data_y, data_ex, data_ey );
  data->SetName("data");
  data->SetTitle(";Kaon kinetic energy (MeV);Cross section (mb/C)");
  data->SetMarkerSize(0.7);
  data->SetMarkerColor(kMagenta+2);
  data->SetLineColor(kMagenta+2);

  TGraphErrors * data_inel = new TGraphErrors( 4, data_reac_x, data_reac_y, data_reac_ex, data_reac_ey );
  data_inel->SetName("data_inel");
  data_inel->SetTitle(";Kaon kinetic energy (MeV);Cross section (mb/C)");
  data_inel->SetMarkerSize(0.7);
  data_inel->SetMarkerColor(kGreen+2);
  data_inel->SetLineColor(kGreen+2);

  elastic->SetLineColor(kRed);
  elastic->SetMarkerColor(kRed);
  inelastic->SetLineColor(kBlue);
  inelastic->SetMarkerColor(kBlue);
  total->SetMarkerSize(0.4);
  elastic->SetMarkerSize(0.4);
  inelastic->SetMarkerSize(0.4);
  total->SetMinimum(0.0);
  total->SetMaximum(400.0);
  rw_elastic->SetLineColor(kRed);
  rw_elastic->SetMarkerColor(kRed);
  rw_inelastic->SetLineColor(kBlue);
  rw_inelastic->SetMarkerColor(kBlue);
  rw_total->SetMarkerSize(0.4);
  rw_elastic->SetMarkerSize(0.4);
  rw_inelastic->SetMarkerSize(0.4);
  rw_total->SetMinimum(0.0);
  rw_total->SetMaximum(400.0);



  TLegend leg = TLegend( 0.4, 0.6, 0.846, 0.846 );
  if( rw ) {
    leg.AddEntry( rw_total, "Corrected GEANT total", "l" );
    leg.AddEntry( rw_elastic, "Corrected GEANT elastic", "l" );
    leg.AddEntry( rw_inelastic, "Corrected GEANT inelastic", "l" );
  } else {
    leg.AddEntry( total, "G4 Total", "l" );
    leg.AddEntry( elastic, "G4 Elastic", "l" );
    leg.AddEntry( inelastic, "G4 Inelastic", "l" );
  }
  leg.AddEntry( data, "Bugg+Friedman Total", "pe" );
  leg.AddEntry( data_inel, "Friedman Reaction", "pe" );

  TCanvas * c = new TCanvas();
  if( rw ) {
    rw_total->SetTitle(";K^{+} kinetic energy;Cross section (mb)");
    rw_total->Draw("APC");
    rw_elastic->Draw("PC same");
    rw_inelastic->Draw("PC same");
  } else {
    total->Draw("APC");
    elastic->Draw("PC same");
    inelastic->Draw("PC same");
  }

  data->Draw("P same");
  data_inel->Draw("P same");

  TLatex *latex = new TLatex( 0.28, 0.83, "MINERvA Preliminary" );
  latex->SetNDC();
  latex->SetTextSize(0.03);
  latex->SetTextColor(kBlue);
  latex->SetTextFont(62);
  latex->SetTextAlign(22);
  latex->Draw();
  TLatex *latex2 = new TLatex( 0.28, 0.8, "GEANT4 9.4.2" );
  latex2->SetNDC();
  latex2->SetTextSize(0.03);
  latex2->SetTextColor(kBlack);
  latex2->SetTextFont(62);
  latex2->SetTextAlign(22);
  latex2->Draw();
    
  leg.Draw();
  c->RedrawAxis();
  c->Print("cross_section.png");
  delete c;


}


