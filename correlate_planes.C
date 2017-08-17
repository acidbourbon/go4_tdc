

#include "Riostream.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

#define RISING 1
#define FALLING 0
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
  
  
  
  
  // look-up tables for the detectors, truth tables identify self trackers of two types
  
  
  //  ####
  //  ####
  //    ####
  //    ####
  // (upper cell is left of lower overlapping cell)
  Bool_t is_stracker_L[2][32][32];
  
  
  //    ####
  //    ####
  //  ####
  //  ####
  // (upper cell is right of lower overlapping cell)
  Bool_t is_stracker_R[2][32][32];
  
  
  for(int i = 0 ; i<2; i++){
    for(int j = 0 ; j<32; j++){
      for(int k = 0 ; k<32; k++){
        is_stracker_L[i][j][k] = false;
        is_stracker_R[i][j][k] = false;
      }
    }
  }
  
    
  // for Lena -> TDC_0351 -> tdc_no 0
  
  is_stracker_L[0][00][23]=true;
  is_stracker_L[0][01][22]=true;
  is_stracker_L[0][02][21]=true;
  is_stracker_L[0][03][20]=true;
  is_stracker_L[0][04][19]=true;
  is_stracker_L[0][05][18]=true;
  is_stracker_L[0][06][17]=true;
  is_stracker_L[0][07][16]=true;
  
  is_stracker_R[0][01][23]=true;
  is_stracker_R[0][02][22]=true;
  is_stracker_R[0][03][21]=true;
  is_stracker_R[0][04][20]=true;
  is_stracker_R[0][05][19]=true;
  is_stracker_R[0][06][18]=true;
  is_stracker_R[0][07][17]=true;
  
  
  
  // for Sandra -> TDC_0353 -> tdc_no 1
  
  is_stracker_L[1][00][17]=true;
  is_stracker_L[1][01][18]=true;
  is_stracker_L[1][02][19]=true;
  is_stracker_L[1][03][20]=true;
  is_stracker_L[1][04][21]=true;
  is_stracker_L[1][05][22]=true;
  is_stracker_L[1][06][23]=true;
  
  is_stracker_R[1][00][16]=true;
  is_stracker_R[1][01][17]=true;
  is_stracker_R[1][02][18]=true;
  is_stracker_R[1][03][19]=true;
  is_stracker_R[1][04][20]=true;
  is_stracker_R[1][05][21]=true;
  is_stracker_R[1][06][22]=true;
  is_stracker_R[1][07][23]=true;
  
  correlations_out->cd();
  
  Float_t t1_offset[2][32][32]; // two TDCs, 32 possible ref channels, 32 channels ... yes I know it's debauchery
  
  
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
  
  
  
  empty_hit.chan = -1;
  empty_hit.ref_chan = -1;
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
            // do the interesting stuff
  //           cout << "trig_no: " << trig_no << " TDC: " << TDC_list[tdc_no] << " chan: " << chan << endl;
            current_pair[tdc_no]->fill(current_hit);
            if(current_pair[tdc_no]->complete()){
  //             cout << "pair complete! chan a: " << current_pair[tdc_no]->hit_a.chan << "chan b: "<< current_pair[tdc_no]->hit_b.chan << endl;
              coinc_matrix[tdc_no]->Fill(current_pair[tdc_no]->hit_a.chan,current_pair[tdc_no]->hit_b.chan);
            }
            
            current_hit.t1 -= t1_offset[tdc_no][current_hit.ref_chan][current_hit.chan];
            
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
      
      inter_plane_correlations->Fill();
    }
    
    
    coinc_distribution->Fill(hits_per_trigger);
    
    if (hits_per_trigger) { // if you have at least one hit in one plane
      inter_plane_all->Fill();
    }
    
    if (all_ends_reached) { // all trees have been read to the end
      break;
    }
//     cout << "\n --- \n"<< endl;




    
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

// draw fish on the fly with new cool tree

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

//   draw fish Lena -> all L-type self-trackers
// inter_plane_all->Draw("t1_a-t1_b:t1_a+t1_b>>abc(100,-50,150,150,-150,150),","chan_a>=0 && chan_b >= 0 && (23-chan_b == chan_a) && (chan_c>=0 || chan_d >= 0)","colz")

}


