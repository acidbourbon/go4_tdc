
#include <iostream>
#include <fstream>




#define walk_correction_slope -0.0901558 
#define walk_correction_mean_tot 150

#define Par_0        -726.76
#define Par_1      -0.727522 
#define Par_2     0.00296743 
#define Par_3   -4.00484e-06 

// #define exp_offset -7.87862e+02
// #define exp_slope -2.69912e-02 
// #define exp_const 5.18843e+00
#define exp_offset -7.87862e+02
#define exp_slope -1.69710e-02
#define exp_const  4.24679e+00 


#define peak_hood 30

void draw_and_save(TObject *hist,TString name,TString outdir,TString draw_options) {
  
//   if(DRAW_PNG) {
    TCanvas *c = new TCanvas("c_"+name,"c_"+name,200,10,1024,786);
    c->cd();
    hist->Draw(draw_options);
    c->Print(outdir+"/"+name+".png");
//   }
//   c->Print(outdir+name+".pdf");
//     c->Print(outdir+name+".svg");
}

TString from_env(TString env_var,TString default_val){
  if(gSystem->Getenv(env_var)){
    return gSystem->Getenv(env_var);
  } 
  return default_val;
}


void HistXCut(TH1F *hist, Float_t min, Float_t max) {
  
    //  cutting ghots signals with long drift times t1 > 90 ns
    Int_t binNumber = hist->GetNbinsX() ;
    for(Int_t ibin =  hist->FindBin(max) ; ibin < binNumber ; ibin++){
      hist->SetBinContent(ibin,0);    
      }
    for(Int_t ibin = 1 ; ibin < hist->FindBin(min)   ; ibin++){
      hist->SetBinContent(ibin,0);    
      }
      
    hist->SetEntries((Int_t) hist->Integral());
}

Float_t cut_around_maximum(TH1F *hist, Float_t below, Float_t above) {
  
  Int_t binmax = hist->GetMaximumBin();
  Double_t x = hist->GetXaxis()->GetBinCenter(binmax);
  HistXCut(hist,x-below,x+above);
  
  return x;
  
}



void draw_things(TString fname){
  TFile *f = new TFile(fname);
//   f->cd("Histograms/TDC_0353");
  
  TString TDC=from_env("TDC","1482");
  
  
  cout << "contents: "<< endl; 
  f->ls();
  gStyle->SetOptFit();
  
  TString outdir = gSystem->DirName(fname);
  
  
  std::vector<TString> list;
  
  //list.push_back("meta_fish");
//   list.push_back("meta_potato");
//   list.push_back("meta_t1_2d");
//   list.push_back("meta_tot_2d");
  //list.push_back("coinc_matrix");
//   list.push_back("meta_fish_proj");
  list.push_back("efficiency");
//   list.push_back("ref_counts");
//   list.push_back("Ch03_t1");
//   list.push_back("Ch03_tot");
//   list.push_back("Ch03_potato");
//   list.push_back("Ch04_t1");
//   list.push_back("Ch04_tot");
//   list.push_back("Ch04_potato");
//   list.push_back("Ch05_t1");
//   list.push_back("Ch05_tot");
//   list.push_back("Ch05_potato");
//   list.push_back("Ch06_t1");
//   list.push_back("Ch06_tot");
//   list.push_back("Ch06_potato");
//   list.push_back("Ch07_t1");
  list.push_back("Ch08_t1");
//   list.push_back("Ch07_tot");
//   list.push_back("Ch07_potato");
//   list.push_back("Ch00_potato");
  list.push_back("Ch07_t1");
  list.push_back("Ch07_tot");
  list.push_back("Ch07_mhit_t1");
//   list.push_back("Ch07_mhit_tot");
  list.push_back("Ch07_potato");
  list.push_back("Ch17_potato");
  list.push_back("Ch09_t1");
  list.push_back("Ch09_mhit_t1");
/*  
  list.push_back("Ch00_t1");
  list.push_back("Ch01_t1");
  list.push_back("Ch02_t1");
  list.push_back("Ch03_t1");
  list.push_back("Ch04_t1");
  list.push_back("Ch05_t1");
  list.push_back("Ch06_t1");
  list.push_back("Ch07_t1");
  

  list.push_back("Ch16_t1");
  list.push_back("Ch17_t1");
  list.push_back("Ch18_t1");
  list.push_back("Ch19_t1");
  list.push_back("Ch20_t1");
  list.push_back("Ch21_t1");
  list.push_back("Ch22_t1");
  list.push_back("Ch23_t1"); */

//   ((TH1F*)f->Get("Histograms/TDC_0353/Ch2/TDC_0353_Ch2_RisingRef"))->Draw()  
  for (Int_t i = 0; i< list.size(); i++){
    draw_and_save(f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_"+list[i]),list[i],outdir,"colz" );
//     TH1* cumul = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_"+list[i]))->GetCumulative();
//     draw_and_save(cumul,list[i]+"_cumul",outdir,"colz" );
  }
  
  
  
  
  
  TCanvas *c = new TCanvas("c_fit","c_fit",200,10,1024,786);
  
//   TH1F* t1_clone = (TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch07_t1")->Clone();
  TH1F* t1_clone = (TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch17_t1")->Clone();
//   TH1F* t1_clone = (TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_fish_proj_17_vs_07")->Clone();
  Double_t max = cut_around_maximum(t1_clone,peak_hood,peak_hood);
  t1_clone->Draw();
  t1_clone->Fit("gaus","WW q","",max - peak_hood,max + peak_hood);
  float fit_const =  t1_clone->GetFunction("gaus")->GetParameter(0);	
  float fit_mean  =  t1_clone->GetFunction("gaus")->GetParameter(1);
  float fit_sigma =  t1_clone->GetFunction("gaus")->GetParameter(2);	
//   cout << " fit params: " << fit_const << fit_mean << fit_sigma << endl;
  
  double par[6];
  
  par[0] = fit_const * 1./5;
  par[1] = fit_mean + 2;
  par[2] = fit_sigma * 2;
  par[3] = fit_const * 4./5;
  par[4] = fit_mean;
  par[5] = fit_sigma;
  t1_clone->GetXaxis()->SetRangeUser(max-peak_hood,max+peak_hood);
  TF1 *fit	= new TF1 ("fit","gaus(0)+gaus(3)",max-peak_hood,max+peak_hood);
  fit->SetParameters(par);
  t1_clone->Fit(fit);
  
  par[0] = fit->GetParameter(0);
  par[1] = fit->GetParameter(1);
  par[2] = fit->GetParameter(2);
  par[3] = fit->GetParameter(3);
  par[4] = fit->GetParameter(4);
  par[5] = fit->GetParameter(5);
  
  TF1 *back = new TF1("back","gaus",max-30,max+30);
  back->SetLineColor(3);
  back->SetParameter(0,par[0]);
  back->SetParameter(1,par[1]);
  back->SetParameter(2,par[2]);
  back->Draw("same");
  
  cout << "ratio of function areas (main/backg peak): " << (par[3]*par[5])/(par[0]*par[2]) << endl;
  
  cout << "t1 peak entries: " << t1_clone->Integral() << endl;// /((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch08_t1"))->GetEntries() << endl;
  cout << "efficiency: (via counts)" << t1_clone->Integral() /((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch08_t1"))->Integral() << endl;
  cout << "Fit integral: " << fit->Integral( max-peak_hood,max+peak_hood ) << endl;
  cout << "efficiency: (via fit)" << fit->Integral(max-peak_hood,max+peak_hood) / ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch08_t1"))->Integral() << endl;
//   fit->DrawIntegral();
  
  
  if(0){
  TCanvas *c = new TCanvas("c_walk","c_walk",200,10,1024,786);
  TF1 *walk_exp	= new TF1 ("walk_exp","expo(0)+pol0(2)",50,300);
  walk_exp->SetParameter(0,exp_const);
  walk_exp->SetParameter(1,exp_slope);
  walk_exp->SetParameter(2,exp_offset);
  walk_exp->Draw();
  
  TF1 *walk_cubic	= new TF1 ("walk_cubic","pol3(0)",50,300);
  walk_cubic->SetParameter(0,Par_0);
  walk_cubic->SetParameter(1,Par_1);
  walk_cubic->SetParameter(2,Par_2);
  walk_cubic->SetParameter(3,Par_3);
  walk_cubic->SetLineColor(3);
  walk_cubic->Draw("same");
  
  }
  
  
  /*
  TH2F* my_potato = (TH2F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch07_potato")->Clone();
  my_potato->GetXaxis()->SetRangeUser(-800,-760);
  my_potato->GetYaxis()->SetRangeUser(100,250);
//   TFile *fout = new TFile("draw_things_temp_out.root","RECREATE");
  my_potato->Draw("colz");
//   my_potato->ProfileX();
//   my_potato->QuantilesX();
  TF1 *fa = new TF1("fa","gaus");
  TObjArray* arr = new TObjArray();
  my_potato->FitSlicesX();
//   my_potato->FitSlicesY();
  
  */
  /*
  TH1F* meta_fish_proj = (TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_"+"meta_fish_proj");
  //draw_and_save(meta_fish_proj,"meta_fish_proj",outdir,"colz" );
  
  
  gStyle->SetOptFit();
  
  meta_fish_proj->Fit("gaus");
  TF1 *fit = meta_fish_proj->GetFunction("gaus");
  Double_t chi2 = fit->GetChisquare();
  Double_t p2 = fit->GetParameter(2);
  Double_t e2 = fit->GetParError(2);
  cout << "Gauss fit sigma: " << p2 << " +- " << e2 << "ns" << endl;
 
  
  TH1F* efficiency = (TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_"+"efficiency");
  draw_and_save(efficiency,"efficiency",outdir,"colz text" );
  Float_t avg_efficiency = 0;
  
  // pasttrec average
  
  avg_efficiency += efficiency->GetBinContent(1+4);
  avg_efficiency += efficiency->GetBinContent(1+5);
//     avg_efficiency += efficiency->GetBinContent(1+6); // broken channel
  avg_efficiency += efficiency->GetBinContent(1+7);
  
  avg_efficiency += efficiency->GetBinContent(1+19);
  avg_efficiency += efficiency->GetBinContent(1+20);
  
  avg_efficiency /= 5;
  
  
  
//   // asd8 average
//   avg_efficiency += efficiency->GetBinContent(1+1);
//   avg_efficiency += efficiency->GetBinContent(1+2); 
//   avg_efficiency += efficiency->GetBinContent(1+3);
//   
//   avg_efficiency += efficiency->GetBinContent(1+16);
//   avg_efficiency += efficiency->GetBinContent(1+17);
//   avg_efficiency += efficiency->GetBinContent(1+18);
//   
//   avg_efficiency /= 6;
  
  cout << "avg_efficiency:  " << avg_efficiency << endl;
  
  
  ofstream myfile (outdir + "/" + "results.txt");
  if (myfile.is_open()){
    myfile << "Gauss fit sigma: " << p2 << " +- " << e2 << "ns" << endl;
    myfile << "avg_efficiency:  " << avg_efficiency << endl;
  }
  
  //draw_and_save(meta_fish_proj,"meta_fish_proj_fit",outdir,"colz" );
  
//   new TBrowser();

*/
}

