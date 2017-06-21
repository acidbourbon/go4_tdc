#include <iostream>
#include <fstream>


#define CONSTANT_OFFSET 150

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


Float_t get_toa_offset(TH1F* toa0) {
    
  TH1* cum_toa0 = toa0->GetCumulative();
  Int_t cum_toa0_sum = cum_toa0->GetBinContent(cum_toa0->GetEntries());
//   cout << "toa0 sum " << cum_toa0_sum <<endl;
  
  Float_t cum_toa0_up   = ((Float_t) cum_toa0_sum) *0.6;
  Float_t cum_toa0_down = ((Float_t) cum_toa0_sum) *0.1;
  
  Int_t cum_toa0_left_bin   = cum_toa0->FindFirstBinAbove(cum_toa0_down,2);
  Int_t cum_toa0_right_bin  = cum_toa0->FindFirstBinAbove(cum_toa0_up,2);
  
//   for (Int_t i = 1; i<= cum_toa0->GetEntries(); i++){
//     Float_t cur_val = cum_toa0->GetBinContent(i);
//     if (cur_val < cum_toa0_down) {
//       cum_toa0_left_bin = i;
//     }
//     if (cur_val > cum_toa0_up) {
//       cum_toa0_right_bin = i;
//       break;
//     }
//   }
  TAxis *xaxis = cum_toa0->GetXaxis();
  Float_t cum_toa0_left  = xaxis->GetBinCenter(cum_toa0_left_bin);
  Float_t cum_toa0_right = xaxis->GetBinCenter(cum_toa0_right_bin);
  
//   cout << "toa0 entries  " << cum_toa0->GetEntries() <<endl;
//   cout << "toa0 up   " << cum_toa0_up <<endl;
//   cout << "toa0 down " << cum_toa0_down <<endl;
//   cout << "toa0 left_bin  " << cum_toa0_left_bin <<endl;
//   cout << "toa0 right_bin " << cum_toa0_right_bin <<endl;
//   cout << "toa0 left  " << cum_toa0_left <<endl;
//   cout << "toa0 right " << cum_toa0_right <<endl;
  
  TF1 *fa = new TF1("fa","[0] + [1]*x",0,5000);

  // This creates a function of variable x with 2 parameters. The parameters must be initialized via:
  fa->SetParameter(0,0);
  fa->SetParameter(1,1);
  
  cum_toa0->Fit(fa,"","", cum_toa0_left,cum_toa0_right);
  
  Float_t x_intersect = -fa->GetParameter(0)/fa->GetParameter(1);
  
//   cout << "x intersect " << x_intersect << endl;
  
//   draw_and_save(cum_toa0,"cum_toa0","./","");
  
  return x_intersect;

  
}




void fit_t1_offsets(TString fname){
  TFile *f = new TFile(fname);
//   f->cd("Histograms/TDC_1483");
  
  TString fpga = "1482";
  
  
  cout << "contents: "<< endl; 
  f->ls();
  
  
  TString outdir = gSystem->DirName(fname);
  
  
  std::vector<TString> list;
  
//   list.push_back("meta_fish");
//   list.push_back("meta_potato");
//   list.push_back("meta_t1_2d");
//   list.push_back("meta_tot_2d");
//   list.push_back("coinc_matrix");
//   list.push_back("meta_fish_proj");
//   list.push_back("efficiency");
//   list.push_back("ref_counts");
//   list.push_back("Ch00_t1");
//   list.push_back("Ch00_tot");
//   list.push_back("Ch00_potato");
  
  for (Int_t i = 0; i<24; i++) {
    TString a;
    a.Form("Ch%02d_t1",i);
    list.push_back(a);
  } 
  
//   list.push_back("Ch00_t1");
//   list.push_back("Ch01_t1");
//   list.push_back("Ch02_t1");
//   list.push_back("Ch03_t1");
//   list.push_back("Ch04_t1");
//   list.push_back("Ch05_t1");
//   list.push_back("Ch06_t1");
//   list.push_back("Ch07_t1");
// 
//   list.push_back("Ch16_t1");
//   list.push_back("Ch17_t1");
//   list.push_back("Ch18_t1");
//   list.push_back("Ch19_t1");
//   list.push_back("Ch20_t1");
//   list.push_back("Ch21_t1");
//   list.push_back("Ch22_t1");
//   list.push_back("Ch23_t1");

//   ((TH1F*)f->Get("Histograms/TDC_1483/Ch2/TDC_1483_Ch2_RisingRef"))->Draw()  


  ofstream myfile ("t1_offsets.txt");
  if (myfile.is_open()){
    for (Int_t i = 0; i< list.size(); i++){
  //     draw_and_save(f->Get("Histograms/Sec_"+fpga+"/Sec_"+fpga+"_"+list[i]),list[i],outdir,"colz" );
  //    TH1* cumul = ((TH1F*) f->Get("Histograms/Sec_"+fpga+"/Sec_"+fpga+"_"+list[i]))->GetCumulative();
  //     draw_and_save(cumul,list[i]+"_cumul",outdir,"colz" );
      
      Float_t offset = get_toa_offset(  (TH1F*) f->Get("Histograms/Sec_"+fpga+"/Sec_"+fpga+"_"+list[i]) )  + CONSTANT_OFFSET;
      
      cout << "\n\n offset : " << offset << "\n\n"<< endl;
      myfile << offset << endl;
    }
  }
  
//   new TBrowser();
}

