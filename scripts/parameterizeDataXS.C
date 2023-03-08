void parameterizeDataXS()
{

  // Friedman and Bugg data combined with errors, total XS
  double data_x[15] =  { 200.5, 231.4, 327.3, 374.4, 374.4, 466.9, 537.2, 593.8, 697.4, 794.7, 890.4, 998.4, 1146.4, 1626.6, 2107.7 };
  double data_y[15] =  { 162.4, 166.6, 174.9, 175.6, 171.6, 177.9, 187.1, 188.4, 195.4, 201.8, 202.2, 202.6,  202.7,  195.6,  188.0 };
  double data_ex[15] = {   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,    0.0,    0.0,    0.0 };
  double data_ey[15] = {   1.9,   1.3,   0.8,   0.9,   4.7,   4.3,   4.2,   4.0,   3.8,   3.7,   3.8,   3.6,    3.4,    3.2,    3.3 };

  // the data for "reaction" cross section, from Friedman
  double data_reac_x[4] =  { 200.5, 231.4, 327.3, 374.4 };
  double data_reac_y[4] =  { 120.4, 129.3, 141.8, 149.3 };
  double data_reac_ex[4] = {   0.0,   0.0,   0.0,   0.0 };
  double data_reac_ey[4] = {   2.3,   1.4,   1.5,   1.5 };

  // Bogus data for elastic, which is total - reaction from Friedman
  double data_el_y[4] =  { 42.0, 37.3, 33.1, 26.3 };
  double data_el_ey[4] = {  2.3,  1.4,  1.5,  1.5 };

  // Get GEANT prediction from the root file
  TFile * tf = new TFile("g4Kaon.root", "OLD");
  TGraph * g4 = (TGraph*) tf->Get("total");
  g4->SetLineColor(kBlue);
  g4->SetMarkerColor(kBlue);
  g4->SetMarkerSize(0.5);

  TGraph * g4el = (TGraph*) tf->Get("elastic");
  g4el->SetLineColor(kGreen+2);
  g4el->SetMarkerColor(kGreen+2);
  g4el->SetMarkerSize(0.5);

  TGraphErrors * data = new TGraphErrors( 15, data_x, data_y, data_ex, data_ey );
  data->SetName("data");
  data->SetTitle(";Kaon kinetic energy (MeV);Cross section (mb/C)");
  data->SetMarkerSize(0.5);

  TGraphErrors * data_reac = new TGraphErrors( 4, data_reac_x, data_reac_y, data_reac_ex, data_reac_ey );
  data_reac->SetName("data_reac");
  data_reac->SetMarkerColor(kMagenta+3);
  data_reac->SetLineColor(kMagenta+3);
  data_reac->SetMarkerSize(0.5);

  TGraphErrors * data_el = new TGraphErrors( 4, data_reac_x, data_el_y, data_reac_ex, data_el_ey );
  data_el->SetName("data_el");
  data_el->SetMarkerColor(kGreen+4);
  data_el->SetLineColor(kGreen+4);
  data_el->SetMarkerSize(0.5);

  data->GetXaxis()->SetLimits( 0.0, 2200.0 );
  data->SetMinimum(0.0);
  data->SetMaximum(400.0);

  // make a cubic spline to the GEANT prediction
  TSpline3 * spl = new TSpline3( "totalspline", g4 );
  spl->SetLineColor(kBlue);

  TSpline3 * elspl = new TSpline3( "elasticspline", g4el );
  elspl->SetLineColor(kGreen+2);

  // Make an elastic Geant4 graph that's scaled down by a factor
  const double elastic_scale_factor = 0.45;
  TGraph * g4elScaled = new TGraph();
  g4elScaled->SetName("elastic_scaled");
  g4elScaled->SetLineColor(kGreen+4);
  g4elScaled->SetMarkerColor(kGreen+4);
  g4elScaled->SetMarkerSize(0.5);
  for( int n = 0; n < g4el->GetN(); ++n ) {
    double x, y;
    g4el->GetPoint( n, x, y );
    g4elScaled->SetPoint( n, x, y*elastic_scale_factor );
  }
  TSpline3 * scaledelspl = new TSpline3( "scaledelasticspline", g4elScaled );
  scaledelspl->SetLineColor(kGreen+4);

  TF1 * low = new TF1( "low", "[0] + [1]*pow(x,0.5)", 0.0, 1000.0 );
  TF1 * high = new TF1( "high", "[0] + [1]*pow(x,-0.5)", 1000.0, 10000.0 );
  low->SetLineColor(kRed);
  high->SetLineColor(kRed);

  data->Fit("low","QR");
  data->Fit("high","QR+");

  printf( "Below 1 GeV: %4.3f + %4.3f * x^+0.5, at 1 GeV = %3.2f\n", low->GetParameter(0), low->GetParameter(1), low->Eval(1000.0) );
  printf( "Above 1 GeV: %4.3f + %4.3f * x^-0.5, at 1 GeV = %3.2f\n", high->GetParameter(0), high->GetParameter(1), high->Eval(1000.0) );

  TLegend * leg = new TLegend( 0.2, 0.6, 0.8, 0.845 );
  leg->SetNColumns(2);
  leg->AddEntry( data, "Total XS data", "PE" );
  leg->AddEntry( data_reac, "Reaction XS data", "PE" );
  leg->AddEntry( data_el, "Total - Reaction data", "PE" );
  leg->AddEntry( low, "Fit to total XS data", "L" );
  leg->AddEntry( g4, "GEANT4 total", "P" );
  leg->AddEntry( g4el, "GEANT4 elastic", "P" );
  leg->AddEntry( spl, "Total spline", "L" );
  leg->AddEntry( elspl, "Elastic spline", "L" );
  leg->AddEntry( scaledelspl, "Scaled elastic spline", "L" );

  TCanvas * c = new TCanvas();
  data->Draw("APE");
  data_reac->Draw("PE same");
  data_el->Draw("PE same");
  g4->Draw("same P");
  g4el->Draw("same P");
  spl->Draw("same");
  elspl->Draw("same");
  g4elScaled->Draw("same");
  scaledelspl->Draw("same");
  low->Draw("same");
  high->Draw("same");
  leg->Draw();
  c->RedrawAxis();
  c->Print("parameterization.png");
  delete c;

  TFile * fout = new TFile("g4Kaon.root", "RECREATE");
  fout->cd();
  g4->Write();
  g4el->Write();
  g4elScaled->Write();

}
