

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


void correlate_planes(TString filename){
  
// ##################################################
// ##                acquire trees                 ##
// ##################################################
  
  std::vector<TString> TDC_list;
  
  TDC_list.push_back("TDC_0351");
  TDC_list.push_back("TDC_0353");
  
//   TString fTdcId = "TDC_0351";
  
  TFile *f = TFile::Open(filename);
  
  TTree* data_tree[TDC_list.size()];
  Int_t  data_tree_entries[TDC_list.size()];
  Int_t  data_tree_index[TDC_list.size()];
  TH2F*  coinc_matrix[TDC_list.size()];
//   Bool_t data_tree_end_reached[TDC_list.size()];
 
  TH1F* coinc_distribution = new TH1F("coinc_distribution","hits in same trigger;counts",10,0-0.5,10-0.5);  
  
  // variables to which the tree entries will be read
  
//   int chan;
//   int trig_no;
//   int ref_chan;
//   double t1;
//   double tot;
  
  Hit current_hit;
  
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
  for (Int_t trig_no = 0; true; trig_no++){
    
//     cout << "trig_no: " << trig_no << endl;
    Bool_t all_ends_reached = true; 
    Int_t hits_per_trigger = 0;
    // loop over all participating TDCs
    
    HitPair* current_pair[2];
    
    for (Int_t tdc_no = 0; tdc_no < TDC_list.size(); tdc_no ++){
      
//       cout << "TDC: " << TDC_list[tdc_no] << endl;
      current_pair[tdc_no] = new HitPair();
      
      // loop over tree entries
      while ( data_tree_index[tdc_no] < data_tree_entries[tdc_no] ) {
        all_ends_reached = false;
        data_tree[tdc_no]->GetEntry(data_tree_index[tdc_no]);
//         cout << "entry trig no: " << trig_no << endl;
        
        if (current_hit.trig_no == trig_no){
          hits_per_trigger++;
          // do the interesting stuff
//           cout << "trig_no: " << trig_no << " TDC: " << TDC_list[tdc_no] << " chan: " << chan << endl;
          current_pair[tdc_no]->fill(current_hit);
          if(current_pair[tdc_no]->complete()){
//             cout << "pair complete! chan a: " << current_pair[tdc_no]->hit_a.chan << "chan b: "<< current_pair[tdc_no]->hit_b.chan << endl;
            coinc_matrix[tdc_no]->Fill(current_pair[tdc_no]->hit_a.chan,current_pair[tdc_no]->hit_b.chan);
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
      cout << current_pair[0]->hit_a.chan << endl;
      cout << current_pair[0]->hit_b.chan << endl;
      cout << current_pair[1]->hit_a.chan << endl;
      cout << current_pair[1]->hit_b.chan << endl;
      cout << " --- " << endl;
    }
    
    
    coinc_distribution->Fill(hits_per_trigger);
    
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
// 
  
  
  
}


