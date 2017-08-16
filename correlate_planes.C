

#include "Riostream.h"
#include <string>
#include <vector>
#include <map>

#define RISING 1
#define FALLING 0
// ##################################################
// ##                    USAGE                     ##
// ##################################################

/*
 * 
 * */

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
//   Bool_t data_tree_end_reached[TDC_list.size()];
 
  
  
  // variables to which the tree entries will be read
  
  int entry_chan;
  int entry_trig_no;
  int entry_ref_chan;
  double entry_t1;
  double entry_tot;
  
  for (Int_t i = 0; i < TDC_list.size(); i++){
    data_tree[i] = (TTree*)f->Get(TDC_list[i]);
    data_tree_entries[i] = data_tree[i]->GetEntries();
    data_tree_index[i] = 0;
//     data_tree_end_reached[i] = false;
    cout << "TDC: " << TDC_list[i] << " entries: " << data_tree[i]->GetEntries() << endl;
    
    data_tree[i]->SetBranchAddress("trig_no",&entry_trig_no);
    data_tree[i]->SetBranchAddress("t1",&entry_t1);
    data_tree[i]->SetBranchAddress("tot",&entry_tot);
    data_tree[i]->SetBranchAddress("chan",&entry_chan);
    data_tree[i]->SetBranchAddress("ref_chan",&entry_ref_chan);
  }
  
 
  // loop over all possible trigger numbers
  for (Int_t trig_no = 0; true; trig_no++){
    
//     cout << "trig_no: " << trig_no << endl;
    Bool_t all_ends_reached = true; 
    // loop over all participating TDCs
    for (Int_t tdc_no = 0; tdc_no < TDC_list.size(); tdc_no ++){
      
//       cout << "TDC: " << TDC_list[tdc_no] << endl;
      
      // loop over tree entries
      while ( data_tree_index[tdc_no] < data_tree_entries[tdc_no] ) {
        all_ends_reached = false;
        data_tree[tdc_no]->GetEntry(data_tree_index[tdc_no]);
//         cout << "entry trig no: " << entry_trig_no << endl;
        
        if (entry_trig_no == trig_no){
          
          // do the interesting stuff
//           cout << "trig_no: " << trig_no << " TDC: " << TDC_list[tdc_no] << " chan: " << entry_chan << endl;
          
          
        }
        if (entry_trig_no <= trig_no){
          data_tree_index[tdc_no]++;
        } else {
          break;
        }
        
        
      }
      
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
    cout << "chan     : "<< entry_chan <<endl;
    cout << "ref chan : "<< entry_ref_chan <<endl;
    cout << "---" <<endl;
    
  }
  */
  
  
  
}


