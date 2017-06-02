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
  
  
  
  
  cout << "contents: "<< endl; 
  f->ls();
  
  
  TString outdir = gSystem->DirName(fname);
  
  
  std::vector<TString> list;
  
  list.push_back("meta_fish");
  list.push_back("meta_potato");
  list.push_back("coinc_matrix");
  list.push_back("meta_fish_proj");
  list.push_back("efficiency");
  list.push_back("ref_counts");
  list.push_back("Ch00_t1");
  list.push_back("Ch00_tot");
  list.push_back("Ch00_potato");
  

//   ((TH1F*)f->Get("Histograms/TDC_1483/Ch2/TDC_1483_Ch2_RisingRef"))->Draw()  
  for (Int_t i = 0; i< list.size(); i++){
    draw_and_save(f->Get("Histograms/Sec_1483/Sec_1483_"+list[i]),list[i],outdir,"colz" );
  }
  
//   new TBrowser();
}

