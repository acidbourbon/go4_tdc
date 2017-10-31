

#include "Riostream.h"
#include <string>
#include <vector>
#include <map>
using namespace std;


#define cell_size 5 // mm
// ##################################################
// ##                    USAGE                     ##
// ##################################################

/*
 * 
 * */

class Hit {
public:
  int chan;
  int trig_no;
  int ref_chan;
  double t1;
  double tot;
};

class HitPair {
public:
  Hit hit_a;
  Hit hit_b;
  Bool_t got_a, got_b;
  HitPair(void);
  void fill(Hit new_hit) {
    if(new_hit.chan < 8){
      hit_a = new_hit;
      got_a = true;
    } else {
      hit_b = new_hit;
      got_b = true;
    }
  };
  Bool_t complete(void) {
    return got_a && got_b;
  };
};

HitPair::HitPair(void) {
  got_a = false;
  got_b = false;
}




Float_t get_toa_offset(TH1F* toa0) {
    
  TH1* cum_toa0 = toa0->GetCumulative();
  Int_t cum_toa0_sum = cum_toa0->GetBinContent(cum_toa0->GetEntries());
//   cout << "toa0 sum " << cum_toa0_sum <<endl;
  
  Float_t cum_toa0_up   = ((Float_t) cum_toa0_sum) *0.6;
  Float_t cum_toa0_down = ((Float_t) cum_toa0_sum) *0.1;
  
  Int_t cum_toa0_left_bin   = cum_toa0->FindFirstBinAbove(cum_toa0_down,2);
  Int_t cum_toa0_right_bin  = cum_toa0->FindFirstBinAbove(cum_toa0_up,2);
  TAxis *xaxis = cum_toa0->GetXaxis();
  Float_t cum_toa0_left  = xaxis->GetBinCenter(cum_toa0_left_bin);
  Float_t cum_toa0_right = xaxis->GetBinCenter(cum_toa0_right_bin);
  
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





void correlate_planes(TString filename){
  
  
  
  // display fit result in canvas
  gStyle->SetOptFit();

  
// ##################################################
// ##                acquire trees                 ##
// ##################################################
  
  std::vector<TString> TDC_list;
  
  
  // for the time being only put in two TDCs please!
  TDC_list.push_back("TDC_0351");
  TDC_list.push_back("TDC_0353");
  
//   TString fTdcId = "TDC_0351";
  
  TFile *f = TFile::Open(filename);
  
  TTree* data_tree[TDC_list.size()];
  
  
  TFile* correlations_out = new TFile("./correlations_out.root","RECREATE");
  
  Int_t  data_tree_entries[TDC_list.size()];
  Int_t  data_tree_index[TDC_list.size()];
  TH2F*  coinc_matrix[TDC_list.size()];
//   Bool_t data_tree_end_reached[TDC_list.size()];
 
  TH1F* coinc_distribution = new TH1F("coinc_distribution","hits in same trigger;counts",10,0-0.5,10-0.5);  
  
  // variables to which the tree entries will be read
  Hit current_hit, hit_a, hit_b, hit_c, hit_d, empty_hit;
  
  
  
  correlations_out->cd();
  
  Float_t t1_offset[2][32][32]; // two TDCs, 32 possible ref channels, 32 channels ... yes I know it's debauchery
  
  for (Int_t i = 0 ; i<2; i++){
    for (Int_t j = 0 ; j<32; j++){
      for (Int_t k = 0 ; k<32; k++){
        t1_offset[i][j][k] = 0;
      }
    }
  }
  
  
  
  // offset correction version 1 (take all hits )
 
  
  /*
  for (Int_t tdc_no = 0; tdc_no < TDC_list.size(); tdc_no++){
    for (Int_t ref_chan = 8; ref_chan <10; ref_chan++){
      for (Int_t chan = 0; chan <32; chan++){
        
        TString chan_str, ref_chan_str;
        chan_str.Form("%d",chan);
        ref_chan_str.Form("%d",ref_chan);
        
        ( (TTree*) f->Get(TDC_list[tdc_no]) )->Draw("t1>>tmp_hist(600,-100,200),","ref_chan=="+ref_chan_str+" && chan=="+chan_str,"");
        TH1F* hist = (TH1F*) correlations_out->Get("tmp_hist");
        
        t1_offset[tdc_no][ref_chan][chan] = get_toa_offset(hist); 
  //       cout << "hist offset: "<< get_toa_offset(hist) << endl;
        
      }
    }
  }
  
  */
  
  
  
  
  
  
  
  
  // a tree to store inter plane correlations
  
  
  TTree* inter_plane_correlations = new TTree("inter_plane_correlations","inter_plane_correlations");
  Int_t chan_a;
  Int_t chan_b;
  Int_t chan_c;
  Int_t chan_d;
  
  Double_t t1_a;
  Double_t t1_b;
  Double_t t1_c;
  Double_t t1_d;
  
  Double_t tot_a;
  Double_t tot_b;
  Double_t tot_c;
  Double_t tot_d;
  
//   Bool_t Ltracker_ab;
//   Bool_t Rtracker_ab;
//   Bool_t Ltracker_cd;
//   Bool_t Rtracker_cd;
  
  Double_t xpos_ab;
  Double_t xpos_cd;
  // xpos = zero is defined as the position of layer one(a) channel 0
  
/*  
  inter_plane_correlations->Branch("chan_a",&chan_a);
  inter_plane_correlations->Branch("chan_b",&chan_b);
  inter_plane_correlations->Branch("chan_c",&chan_c);
  inter_plane_correlations->Branch("chan_d",&chan_d);
  
  inter_plane_correlations->Branch("t1_a",&t1_a);
  inter_plane_correlations->Branch("t1_b",&t1_b);
  inter_plane_correlations->Branch("t1_c",&t1_c);
  inter_plane_correlations->Branch("t1_d",&t1_d);
  
  inter_plane_correlations->Branch("tot_a",&tot_a);
  inter_plane_correlations->Branch("tot_b",&tot_b);
  inter_plane_correlations->Branch("tot_c",&tot_c);
  inter_plane_correlations->Branch("tot_d",&tot_d);
  
  inter_plane_correlations->Branch("Ltracker_ab",&Ltracker_ab);
  inter_plane_correlations->Branch("Rtracker_ab",&Rtracker_ab);
  inter_plane_correlations->Branch("Ltracker_cd",&Ltracker_cd);
  inter_plane_correlations->Branch("Rtracker_cd",&Rtracker_cd);
  
  inter_plane_correlations->Branch("xpos_ab",&xpos_ab);
  inter_plane_correlations->Branch("xpos_cd",&xpos_cd); */
  
  empty_hit.chan = -1000;
  empty_hit.ref_chan = -1000;
  empty_hit.t1 = -1000;
  empty_hit.tot = -1000;
  
  TTree* inter_plane_all = new TTree("inter_plane_all","inter_plane_all");
  
  inter_plane_all->Branch("t1_a",&hit_a.t1);
  inter_plane_all->Branch("tot_a",&hit_a.tot);
  inter_plane_all->Branch("chan_a",&hit_a.chan);
  inter_plane_all->Branch("ref_chan_a",&hit_a.ref_chan);
  inter_plane_all->Branch("t1_b",&hit_b.t1);
  inter_plane_all->Branch("tot_b",&hit_b.tot);
  inter_plane_all->Branch("chan_b",&hit_b.chan);
  inter_plane_all->Branch("ref_chan_b",&hit_b.ref_chan);
  inter_plane_all->Branch("t1_c",&hit_c.t1);
  inter_plane_all->Branch("tot_c",&hit_c.tot);
  inter_plane_all->Branch("chan_c",&hit_c.chan);
  inter_plane_all->Branch("ref_chan_c",&hit_c.ref_chan);
  inter_plane_all->Branch("t1_d",&hit_d.t1);
  inter_plane_all->Branch("tot_d",&hit_d.tot);
  inter_plane_all->Branch("chan_d",&hit_d.chan);
  inter_plane_all->Branch("ref_chan_d",&hit_d.ref_chan);
  
  

//   _                                           _____         ___ 
//  | |                                         |  __ \       /   |
//  | | ___   ___  _ __     _____   _____ _ __  | |  \/ ___  / /| |
//  | |/ _ \ / _ \| '_ \   / _ \ \ / / _ \ '__| | | __ / _ \/ /_| |
//  | | (_) | (_) | |_) | | (_) \ V /  __/ |    | |_\ \ (_) \___  |
//  |_|\___/ \___/| .__/   \___/ \_/ \___|_|     \____/\___/    |_/
//                | |                                              
//                |_|                                              
//       _       _                                                 
//      | |     | |                                                
//    __| | __ _| |_ __ _                                          
//   / _` |/ _` | __/ _` |                                         
//  | (_| | (_| | || (_| |                                         
//   \__,_|\__,_|\__\__,_|                                         
//                                                                 
//                                                                 
  
  
  
  
  for (Int_t i = 0; i < TDC_list.size(); i++){
    data_tree[i] = (TTree*)f->Get(TDC_list[i]);
    data_tree_entries[i] = data_tree[i]->GetEntries();
    data_tree_index[i] = 0;
//     data_tree_end_reached[i] = false;
    cout << "TDC: " << TDC_list[i] << " entries: " << data_tree[i]->GetEntries() << endl;
    
    data_tree[i]->SetBranchAddress("trig_no", &current_hit.trig_no);
    data_tree[i]->SetBranchAddress("t1",      &current_hit.t1);
    data_tree[i]->SetBranchAddress("tot",     &current_hit.tot);
    data_tree[i]->SetBranchAddress("chan",    &current_hit.chan);
    data_tree[i]->SetBranchAddress("ref_chan",&current_hit.ref_chan);
    coinc_matrix[i] = new TH2F("coinc_matrix_"+TDC_list[i],"channels 0-7;channels 16-23",12,-2.5,9.5,10,15-0.5,24+0.5);
  }
  
 
  // loop over all possible trigger numbers
  for (Int_t trig_no = 0; trig_no < 10000 || true; trig_no++){
    
//     cout << "trig_no: " << trig_no << endl;
    Bool_t all_ends_reached = true; 
    Int_t hits_per_trigger = 0;
    // loop over all participating TDCs
    
    HitPair* current_pair[2];
    
    hit_a = empty_hit;
    hit_b = empty_hit;
    hit_c = empty_hit;
    hit_d = empty_hit;
    
    
    for (Int_t tdc_no = 0; tdc_no < TDC_list.size(); tdc_no ++){
      
//       cout << "TDC: " << TDC_list[tdc_no] << endl;
      current_pair[tdc_no] = new HitPair();
      
      // loop over tree entries
      while ( data_tree_index[tdc_no] < data_tree_entries[tdc_no] ) {
        all_ends_reached = false;
        data_tree[tdc_no]->GetEntry(data_tree_index[tdc_no]);
//         cout << "entry trig no: " << trig_no << endl;
        
        if (current_hit.trig_no == trig_no){
          if(current_hit.ref_chan == 8 || true ){ // filter only one reference channel
            hits_per_trigger++;
            
            current_hit.t1 -= t1_offset[tdc_no][current_hit.ref_chan][current_hit.chan];
            // do the interesting stuff
  //           cout << "trig_no: " << trig_no << " TDC: " << TDC_list[tdc_no] << " chan: " << chan << endl;
            current_pair[tdc_no]->fill(current_hit);
            if(current_pair[tdc_no]->complete()){
  //             cout << "pair complete! chan a: " << current_pair[tdc_no]->hit_a.chan << "chan b: "<< current_pair[tdc_no]->hit_b.chan << endl;
              coinc_matrix[tdc_no]->Fill(current_pair[tdc_no]->hit_a.chan,current_pair[tdc_no]->hit_b.chan);
            }
            
            
            if(current_hit.chan < 8 && tdc_no == 0){
              hit_a = current_hit;
            } else if (current_hit.chan >=16 && tdc_no == 0){
              hit_b = current_hit;
            } else if (current_hit.chan < 8 && tdc_no == 1){
              hit_c = current_hit;
            } else if (current_hit.chan >=16 && tdc_no == 1){
              hit_d = current_hit;
            }
            
            
          }
          
        }
        if (current_hit.trig_no <= trig_no){
          data_tree_index[tdc_no]++;
        } else {
          break;
        }
        
        
      }
      
    }
    
    
    
    /*
    // now for the inter-plane correlations:
    if( current_pair[0]->complete() && current_pair[1]->complete() ) {
//       cout << current_pair[0]->hit_a.chan << endl;
//       cout << current_pair[0]->hit_b.chan << endl;
//       cout << current_pair[1]->hit_a.chan << endl;
//       cout << current_pair[1]->hit_b.chan << endl;
//       cout << " --- " << endl;
      
      chan_a = current_pair[0]->hit_a.chan;
      chan_b = current_pair[0]->hit_b.chan;
      chan_c = current_pair[1]->hit_a.chan;
      chan_d = current_pair[1]->hit_b.chan;
      
      t1_a = current_pair[0]->hit_a.t1;
      t1_b = current_pair[0]->hit_b.t1;
      t1_c = current_pair[1]->hit_a.t1;
      t1_d = current_pair[1]->hit_b.t1;
      
      tot_a = current_pair[0]->hit_a.tot;
      tot_b = current_pair[0]->hit_b.tot;
      tot_c = current_pair[1]->hit_a.tot;
      tot_d = current_pair[1]->hit_b.tot;
      
      Ltracker_ab = false;
      Ltracker_cd = false;
      Rtracker_ab = false;
      Rtracker_cd = false;
      
      xpos_ab = -1000;
      xpos_cd = -1000;
      
      if( chan_a == 23-chan_b ){
        Ltracker_ab = true;
        xpos_ab = (chan_a + 0.25)*cell_size;
      } else if( chan_a == 23-chan_b+1 ){
        Rtracker_ab = true;
        xpos_ab = (chan_a - 0.25)*cell_size;
      }
      
      if( chan_c == chan_d - 16){
        Rtracker_cd = true;
        xpos_cd = (0.5 + chan_c - 0.25)*cell_size;
      } else if( chan_c == chan_d-17 ){
        Ltracker_cd = true;
        xpos_cd = (0.5 + chan_c + 0.25)*cell_size;
      }
      
      inter_plane_correlations->Fill();
    }
    */
    
    
    coinc_distribution->Fill(hits_per_trigger);
    
    if (hits_per_trigger) { // if you have at least one hit in one plane
      inter_plane_all->Fill();
    }
    
    if (all_ends_reached) { // all trees have been read to the end
      break;
    }
//     cout << "\n --- \n"<< endl;




    
  }
  
  

//             _ _ _               _          
//            | (_) |             | |         
//    ___ __ _| |_| |__  _ __ __ _| |_ ___    
//   / __/ _` | | | '_ \| '__/ _` | __/ _ \   
//  | (_| (_| | | | |_) | | | (_| | ||  __/   
//   \___\__,_|_|_|_.__/|_|  \__,_|\__\___|   
//                                            
//                                            
//   _   __           __  __          _       
//  | | /  |         / _|/ _|        | |      
//  | |_`| |    ___ | |_| |_ ___  ___| |_ ___ 
//  | __|| |   / _ \|  _|  _/ __|/ _ \ __/ __|
//  | |__| |_ | (_) | | | | \__ \  __/ |_\__ \
//   \__\___/  \___/|_| |_| |___/\___|\__|___/
//                                            
//                                            
  
  // calibrate t1 offsets with pair hits in overlapping cell_size
  

  for (Int_t tdc_no = 0; tdc_no < TDC_list.size(); tdc_no++){
    for (Int_t ref_chan = 8; ref_chan <10; ref_chan++){
      for (Int_t chan = 0; chan <32; chan++){
        
        TString chan_str, ref_chan_str;
        chan_str.Form("%d",chan);
        ref_chan_str.Form("%d",ref_chan);
        if ( tdc_no == 0 ) {
          if ( chan < 8 ){ 
          ( (TTree*) correlations_out->Get("inter_plane_all"))->Draw("t1_a>>tmp_hist(600,-150,150),","ref_chan_a=="+ref_chan_str+" && chan_a=="+chan_str+" && ((23-chan_b == chan_a) || (23-chan_b+1 == chan_a))","");
          } else if ( chan >= 16 ){ 
          ( (TTree*) correlations_out->Get("inter_plane_all"))->Draw("t1_b>>tmp_hist(600,-150,150),","ref_chan_b=="+ref_chan_str+" && chan_b=="+chan_str+" && ((23-chan_b == chan_a) || (23-chan_b+1 == chan_a))","");
          }
        } else if ( tdc_no == 1 ) {
          if ( chan < 8 ){ 
          ( (TTree*) correlations_out->Get("inter_plane_all"))->Draw("t1_c>>tmp_hist(600,-150,150),","ref_chan_c=="+ref_chan_str+" && chan_c=="+chan_str+" && ((chan_d - 16 == chan_c) || ( chan_d -15 == chan_c))","");
          } else if ( chan >= 16 ){ 
          ( (TTree*) correlations_out->Get("inter_plane_all"))->Draw("t1_d>>tmp_hist(600,-150,150),","ref_chan_d=="+ref_chan_str+" && chan_d=="+chan_str+" && ((chan_d - 16 == chan_c) || ( chan_d -15 == chan_c))","");
          }
        }
        TH1F* hist = (TH1F*) correlations_out->Get("tmp_hist");
        
        t1_offset[tdc_no][ref_chan][chan] = get_toa_offset(hist); 
  //       cout << "hist offset: "<< get_toa_offset(hist) << endl;
        
      }
    }
  }
  

  // draw all offsets  to a TGraph
  
  Float_t channels_x[32];
  for (Int_t i = 0 ; i<32; i++){
    channels_x[i] = i;
  }
  for (Int_t tdc_no = 0; tdc_no < TDC_list.size(); tdc_no ++){
    for (Int_t ref_chan = 8; ref_chan <10; ref_chan++){
      
      TString ref_chan_str;
      ref_chan_str.Form("%d",ref_chan);
      TGraph* tg_offset = new TGraph(32,channels_x, t1_offset[tdc_no][ref_chan]);
      tg_offset->SetName("t1_offset_"+TDC_list[tdc_no]+"_ref_chan_"+ref_chan_str);
      tg_offset->SetTitle("t1_offset_"+TDC_list[tdc_no]+"_ref_chan_"+ref_chan_str);
      tg_offset->Write();
    }
  }
  

  
  TTree* inter_plane_all_corrected = new TTree("inter_plane_all_corrected","inter_plane_all_corrected");
  
  inter_plane_all_corrected->Branch("t1_a",&hit_a.t1);
  inter_plane_all_corrected->Branch("tot_a",&hit_a.tot);
  inter_plane_all_corrected->Branch("chan_a",&hit_a.chan);
  inter_plane_all_corrected->Branch("ref_chan_a",&hit_a.ref_chan);
  inter_plane_all_corrected->Branch("t1_b",&hit_b.t1);
  inter_plane_all_corrected->Branch("tot_b",&hit_b.tot);
  inter_plane_all_corrected->Branch("chan_b",&hit_b.chan);
  inter_plane_all_corrected->Branch("ref_chan_b",&hit_b.ref_chan);
  inter_plane_all_corrected->Branch("t1_c",&hit_c.t1);
  inter_plane_all_corrected->Branch("tot_c",&hit_c.tot);
  inter_plane_all_corrected->Branch("chan_c",&hit_c.chan);
  inter_plane_all_corrected->Branch("ref_chan_c",&hit_c.ref_chan);
  inter_plane_all_corrected->Branch("t1_d",&hit_d.t1);
  inter_plane_all_corrected->Branch("tot_d",&hit_d.tot);
  inter_plane_all_corrected->Branch("chan_d",&hit_d.chan);
  inter_plane_all_corrected->Branch("ref_chan_d",&hit_d.ref_chan);
  
  inter_plane_all_corrected->Branch("xpos_ab",&xpos_ab);
  inter_plane_all_corrected->Branch("xpos_cd",&xpos_cd);
  
  inter_plane_all->SetBranchAddress("t1_a",&hit_a.t1);
  inter_plane_all->SetBranchAddress("tot_a",&hit_a.tot);
  inter_plane_all->SetBranchAddress("chan_a",&hit_a.chan);
  inter_plane_all->SetBranchAddress("ref_chan_a",&hit_a.ref_chan);
  inter_plane_all->SetBranchAddress("t1_b",&hit_b.t1);
  inter_plane_all->SetBranchAddress("tot_b",&hit_b.tot);
  inter_plane_all->SetBranchAddress("chan_b",&hit_b.chan);
  inter_plane_all->SetBranchAddress("ref_chan_b",&hit_b.ref_chan);
  inter_plane_all->SetBranchAddress("t1_c",&hit_c.t1);
  inter_plane_all->SetBranchAddress("tot_c",&hit_c.tot);
  inter_plane_all->SetBranchAddress("chan_c",&hit_c.chan);
  inter_plane_all->SetBranchAddress("ref_chan_c",&hit_c.ref_chan);
  inter_plane_all->SetBranchAddress("t1_d",&hit_d.t1);
  inter_plane_all->SetBranchAddress("tot_d",&hit_d.tot);
  inter_plane_all->SetBranchAddress("chan_d",&hit_d.chan);
  inter_plane_all->SetBranchAddress("ref_chan_d",&hit_d.ref_chan);
  
  for( Int_t event_no; event_no < inter_plane_all->GetEntries(); event_no++){
    // load
    inter_plane_all->GetEntry(event_no);
    // correct
    hit_a.t1 -= t1_offset[0][hit_a.ref_chan][hit_a.chan];
    hit_b.t1 -= t1_offset[0][hit_b.ref_chan][hit_b.chan];
    hit_c.t1 -= t1_offset[1][hit_c.ref_chan][hit_c.chan];
    hit_d.t1 -= t1_offset[1][hit_d.ref_chan][hit_d.chan];
    
    if( hit_a.chan == 23-hit_b.chan ){
//       Ltracker_ab = true;
      xpos_ab = (hit_a.chan + 0.25)*cell_size;
    } else if( hit_a.chan == 23-hit_b.chan+1 ){
//       Rtracker_ab = true;
      xpos_ab = (hit_a.chan - 0.25)*cell_size;
    }
    
    if( hit_c.chan == hit_d.chan - 16){
//       Rtracker_cd = true;
      xpos_cd = (0.5 + hit_c.chan - 0.25)*cell_size;
    } else if( hit_c.chan == hit_d.chan-17 ){
//       Ltracker_cd = true;
      xpos_cd = (0.5 + hit_c.chan + 0.25)*cell_size;
    }
    
    
    // store again
    inter_plane_all_corrected->Fill();
  }
  
  
  
 
 /*
  
  TTree *data_tree = (TTree*)f->Get(fTdcId);

      
  
  
  
  
// ##################################################
// ##                   Analysis                   ##
// ##################################################
  
  
      
  Int_t entries = data_tree->GetEntries();
      
  
  for (int i = 0; i< entries; i++){
    data_tree->GetEntry(i);
    cout << "trig_no  : "<< trig_no <<endl;
    cout << "chan     : "<< chan <<endl;
    cout << "ref chan : "<< ref_chan <<endl;
    cout << "---" <<endl;
    
  }
  */
 
//  draw stuff on the fly from the console:
//  TDC_0353->Draw("tot:t1>>abc(100,-100,100,100,0,200),","","colz")


// inter_plane_correlations->Draw("sum_t1_a:sum_t1_b>>abc(),","","")
// inter_plane_correlations->Draw("sum_t1_a:wire_aa>>abc(),","","")

// draw fish Lena
//  inter_plane_correlations->Draw("(t1_aa-t1_ab):(t1_aa+t1_ab)>>abc(),","","")
// draw fish Sandra
//  inter_plane_correlations->Draw("(t1_ba-t1_bb):(t1_ba+t1_bb)>>abc(),","","")


//draw mixed asd8/pasttrec fish_
//inter_plane_all->Draw("t1_b-t1_c:t1_b+t1_c>>inter_fish(1000,-1850,-1600,100,-200,0),","chan_b==19 && chan_c==3","colz")

// draw fish on the fly with tree comprising all hits

//   draw fish Lena
//   inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0","")

//   draw fish Sandra
//   inter_plane_all->Draw("t1_c-t1_d:t1_c+t1_d>>abc(100,-50,150,150,-150,150),","chan_c>=0 && chan_d >= 0","")

//   draw fish Lena with one hit in Sandra
//   inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (chan_c>=0 || chan_d >= 0)","")

//   draw fish Lena with two hits in Sandra
//   inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (chan_c>=0 && chan_d >= 0)","colz")

//   draw fish Lena with one hit in Sandra that is lying underneath
//   inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (chan_c == chan_a || (chan_d-16 == chan_a))","colz")
  
//   draw fish Lena with one hit in Sandra that is lying underneath
//   inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (chan_c == chan_a || (chan_d-16 == chan_a)) && (chan_c>=0 && chan_d>=0)","colz")

//   draw fish Lena -> all L-type self-trackers
// inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (23-chan_b == chan_a)","colz")

//   draw fish Lena -> all R-type self-trackers
// inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (23-chan_b+1 == chan_a)","colz")



/// commands with just complete tracks: ///

// Lena fish with all Ltrackers
// inter_plane_correlations->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","Ltracker_ab","colz")

// lena fish projection 
// inter_plane_correlations->Draw("t1_a+t1_b>>abc(),","Ltracker_ab && abs(t1_a-t1_b)<20 ","colz")

// associate Lena t1 sum with hit position in Sandra (limit to diff < 20 ns)
// inter_plane_correlations->Draw("(t1_a+t1_b):xpos_cd>>abc(),","Ltracker_ab && abs(t1_a-t1_b)<20 ","colz")




// this shows me the ange correlation

// Ltracker t1_sum vs hit angle (difference of x pos)
// inter_plane_correlations->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(),","Ltracker_ab && abs(t1_a-t1_b)<20 ","")
// Rtracker t1_sum vs hit angle (difference of x pos)
// inter_plane_correlations->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(),","Rtracker_ab && abs(t1_a-t1_b)<20 ","")

// in color
// root [58] inter_plane_correlations->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(33,-40,40,20,0,80),","Rtracker_ab && abs(t1_a-t1_b)<20 ","colz")
// Error in <TSelectorDraw::DrawSelect>: ncols*3 < ncomma ncols=2, ncomma=7
// (long long) 2174
// root [59] inter_plane_correlations->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(33,-40,40,20,0,80),","Ltracker_ab && abs(t1_a-t1_b)<20 ","colz")
// Error in <TSelectorDraw::DrawSelect>: ncols*3 < ncomma ncols=2, ncomma=7
// (long long) 2860




// fish peak for limited angle
//inter_plane_correlations->Draw("(t1_a+t1_b)>>abc(90,-10,80),","Ltracker_ab && abs(t1_a-t1_b)<20 && (abs(xpos_ab-xpos_cd)<40) ","")
//inter_plane_correlations->Draw("(t1_a+t1_b)>>abc(90,-10,80),","abs(t1_a-t1_b)<20 && (abs(xpos_ab-xpos_cd)<40) ","")


// t1 vs channel for Sandra, require coincidence in both layers
// inter_plane_all->Draw("t1_c:chan_c>>my_TH2F(32,-2,29,200,-20,180),","chan_c>=0&&(chan_d-16 == chan_c || (chan_d-15 == chan_c))","colz")


///// using inter_plane_correlations is deprecated
// better use advanced cuts now

TCut LTracker_ab = "chan_a >=0 && chan_b >= 0 && (23-chan_b   == chan_a)";
TCut RTracker_ab = "chan_a >=0 && chan_b >= 0 && (23-chan_b+1 == chan_a)";
TCut LTracker_cd = "chan_c >=0 && chan_d >= 0 && (chan_d - 17 == chan_c)";
TCut RTracker_cd = "chan_c >=0 && chan_d >= 0 && (chan_d - 16 == chan_c)";

TCut FullTrack  = (LTracker_ab || RTracker_ab) && (LTracker_cd || RTracker_cd);

inter_plane_all_corrected->Draw("t1_c-t1_d:t1_c+t1_d>>sandra_fish(500,-200,300,200,-100,100),",FullTrack && "abs(t1_c - t1_d)<200 && abs(xpos_ab-xpos_cd) < 10","colz");
inter_plane_all_corrected->Draw("t1_c+t1_d>>sandra_fish_proj(),",FullTrack && "abs(t1_c - t1_d)<10 && abs(xpos_ab-xpos_cd) < 10","");
((TH1F*) correlations_out->Get("sandra_fish_proj"))->Fit("gaus");
inter_plane_all_corrected->Draw("t1_a-t1_b:t1_a+t1_b>>lena_fish(500,-200,300,200,-100,100),",FullTrack && "abs(t1_c - t1_d)<200 && abs(xpos_ab-xpos_cd) < 10","colz");
inter_plane_all_corrected->Draw("t1_a+t1_b>>lena_fish_proj(),",FullTrack && "abs(t1_a - t1_b)<10 && abs(xpos_ab-xpos_cd) < 10","");
((TH1F*) correlations_out->Get("lena_fish_proj"))->Fit("gaus");

/*
// Sandra fish:
inter_plane_all_corrected->Draw("t1_c-t1_d:t1_c+t1_d>>abc(100,-50,150,150,-150,150),",FullTrack && "abs(t1_c - t1_d)<200","")
inter_plane_all_corrected->Draw("t1_c-t1_d:t1_c+t1_d>>abc(500,-200,300,200,-100,100),",FullTrack && "abs(t1_c - t1_d)<200","colz")



// Sandra fish center piece projection
inter_plane_all_corrected->Draw("t1_c+t1_d>>abc(),",FullTrack && "abs(t1_c - t1_d)<10","")

// limited solid angle
inter_plane_all_corrected->Draw("t1_c+t1_d>>abc(),",FullTrack && "abs(t1_c - t1_d)<10 && abs(xpos_ab-xpos_cd) < 40","")

// with Fit
inter_plane_all_corrected->Draw("t1_c+t1_d>>abc(),",FullTrack && "abs(t1_c - t1_d)<10 && abs(xpos_ab-xpos_cd) < 5",""); abc->Fit("gaus")



// Lena fish:
inter_plane_all_corrected->Draw("t1_a-t1_b:t1_a+t1_b>>abc(500,-200,300,200,-100,100),",FullTrack && "abs(t1_c - t1_d)<200","colz")
// Lena fish center piece projection
inter_plane_all_corrected->Draw("t1_a+t1_b>>abc(),",FullTrack && "abs(t1_a - t1_b)<10 && abs(xpos_ab-xpos_cd) < 10","")







// this shows me the angle correlation

// Ltracker t1_sum vs hit angle (difference of x pos)
inter_plane_correlations->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(),","Ltracker_ab && abs(t1_a-t1_b)<20 ","")
// Rtracker t1_sum vs hit angle (difference of x pos)
inter_plane_correlations->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(),","Rtracker_ab && abs(t1_a-t1_b)<20 ","")

// in color
inter_plane_all_corrected->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(33,-40,40,20,0,80),",RTracker_ab && "abs(t1_a-t1_b)<20 ","colz")
inter_plane_all_corrected->Draw("(t1_a+t1_b):(xpos_ab-xpos_cd)>>abc(33,-40,40,20,0,80),",LTracker_ab && "abs(t1_a-t1_b)<20 ","colz")

// select specific angles
inter_plane_all_corrected->Draw("(t1_a+t1_b)>>abc(),",LTracker_ab && "abs(t1_a-t1_b)<20 && (xpos_ab - xpos_cd) == 0","colz")
 continue here with plots of mean and stdev


*/

// plot angular dependence

TH1F* angle_dependence_ab_LT = new TH1F(
  "angle_dependence_ab_LT","angle dependece LTracker Lena;xpos_ab - xpos_cd (mm);t1+t2 peak (ns)",
                                     80,-50-0.5,50-0.5);  
TH1F* angle_dependence_ab_RT = new TH1F(
  "angle_dependence_ab_RT","angle dependece RTracker Lena;xpos_ab - xpos_cd (mm);t1+t2 peak (ns)",
                                     80,-50-0.5,50-0.5);  
TH1F* angle_dependence_cd_LT = new TH1F(
  "angle_dependence_cd_LT","angle dependece LTracker Sandra;xpos_ab - xpos_cd (mm);t1+t2 peak (ns)",
                                     80,-50-0.5,50-0.5);  
TH1F* angle_dependence_cd_RT = new TH1F(
  "angle_dependence_cd_RT","angle dependece RTracker Sandra;xpos_ab - xpos_cd (mm);t1+t2 peak (ns)",
                                     80,-50-0.5,50-0.5);  

for (Double_t xdiff = -50; xdiff <= 50; xdiff += 2.5){
  
  TString xdiff_str;
  xdiff_str.Form("%2.1f",xdiff);
  inter_plane_all_corrected->Draw("(t1_a+t1_b)>>ang_ab_LT(),", FullTrack && LTracker_ab && "abs(t1_a-t1_b)<10 && (xpos_ab - xpos_cd) == "+xdiff_str,"colz");
  inter_plane_all_corrected->Draw("(t1_a+t1_b)>>ang_ab_RT(),", FullTrack && RTracker_ab && "abs(t1_a-t1_b)<10 && (xpos_ab - xpos_cd) == "+xdiff_str,"colz");
  inter_plane_all_corrected->Draw("(t1_c+t1_d)>>ang_cd_LT(),", FullTrack && LTracker_cd && "abs(t1_c-t1_d)<10 && (xpos_ab - xpos_cd) == "+xdiff_str,"colz");
  inter_plane_all_corrected->Draw("(t1_c+t1_d)>>ang_cd_RT(),", FullTrack && RTracker_cd && "abs(t1_c-t1_d)<10 && (xpos_ab - xpos_cd) == "+xdiff_str,"colz");
  
  
  Int_t bin = angle_dependence_ab_LT->FindBin(xdiff);
  
  
//   TH1F* hist = (TH1F*) correlations_out->Get("abc");
//   Double_t mean = hist->GetMean();
//   Double_t stddev = hist->GetStdDev();
  
//   cout << "xdiff:  " << xdiff << endl;
//   cout << "mean:   " << mean << endl;
//   cout << "stddev: " << stddev << endl;
//   cout << "bin   : " << bin << endl;
  
  angle_dependence_ab_LT->SetBinContent(bin,( (TH1F*) correlations_out->Get("ang_ab_LT") )->GetMean() );
  angle_dependence_ab_LT->SetBinError  (bin,( (TH1F*) correlations_out->Get("ang_ab_LT") )->GetStdDev() );
  angle_dependence_ab_RT->SetBinContent(bin,( (TH1F*) correlations_out->Get("ang_ab_RT") )->GetMean() );
  angle_dependence_ab_RT->SetBinError  (bin,( (TH1F*) correlations_out->Get("ang_ab_RT") )->GetStdDev() );
  angle_dependence_cd_LT->SetBinContent(bin,( (TH1F*) correlations_out->Get("ang_cd_LT") )->GetMean() );
  angle_dependence_cd_LT->SetBinError  (bin,( (TH1F*) correlations_out->Get("ang_cd_LT") )->GetStdDev() );
  angle_dependence_cd_RT->SetBinContent(bin,( (TH1F*) correlations_out->Get("ang_cd_RT") )->GetMean() );
  angle_dependence_cd_RT->SetBinError  (bin,( (TH1F*) correlations_out->Get("ang_cd_RT") )->GetStdDev() );
  
}


// angle_dependence_LT->Draw();

}


