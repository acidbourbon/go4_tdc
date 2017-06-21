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




void draw_things(TString fname){
  TFile *f = new TFile(fname);
//   f->cd("Histograms/TDC_1483");
  
  TString fpga = "1482";
  
  
  cout << "contents: "<< endl; 
  f->ls();
  
  
  TString outdir = gSystem->DirName(fname);
  
  
  std::vector<TString> list;
  
  list.push_back("meta_fish");
  list.push_back("meta_potato");
  list.push_back("meta_t1_2d");
  list.push_back("meta_tot_2d");
  list.push_back("coinc_matrix");
  list.push_back("meta_fish_proj");
  list.push_back("efficiency");
  list.push_back("ref_counts");
//   list.push_back("Ch00_t1");
//   list.push_back("Ch00_tot");
//   list.push_back("Ch00_potato");
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

//   ((TH1F*)f->Get("Histograms/TDC_1483/Ch2/TDC_1483_Ch2_RisingRef"))->Draw()  
  for (Int_t i = 0; i< list.size(); i++){
    draw_and_save(f->Get("Histograms/Sec_"+fpga+"/Sec_"+fpga+"_"+list[i]),list[i],outdir,"colz" );
//     TH1* cumul = ((TH1F*) f->Get("Histograms/Sec_"+fpga+"/Sec_"+fpga+"_"+list[i]))->GetCumulative();
//     draw_and_save(cumul,list[i]+"_cumul",outdir,"colz" );
  }
  
//   new TBrowser();
}

