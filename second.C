#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGo4AnalysisObjectManager.h"


#include "base/EventProc.h"
#include "base/Event.h"
#include "hadaq/TdcSubEvent.h"

#define CHANNELS 32
#define FISHES   16
#define REFCHAN_A 8
#define REFCHAN_B 11

#define HODO_VETO_CHAN 9
#define HODO_VETO_L -717
// #define HODO_VETO_R (-730 + ref_channel_offset)
#define HODO_VETO_R -710 
#define enable_HODO_VETO 0


#define TAKE_FIRST_HIT 1 // has to be 1 for enable_HODO_VETO


#define VERBOSE 0
//

// #define t1_L -300
// #define t1_R 300
// #define tot_L -10
// #define tot_R 500

// Muentz-Torte
//#define t1_L -300 // GSI Dlab
//#define t1_R 300 // GSI Dlab
//#define t1_L -300 // HZDR
//#define t1_R 300 // HZDR
#define t1_L -1500 // juelich diamond
#define t1_R 500 // juelich diamond
//#define tot_L -10
//#define tot_R 500
#define tot_L -10 // HZDR
#define tot_R 1000 // HZDR

#define ref_channel_offset -75 //ns fine measured ref channel relative to coarse measured cts trigger channel

// in the first iteration, scanning through data in the coincidence window, rejecting hits (fuzzy edges)

//#define spike_rejection 90 //ns for ASD8 0x72 (25000) with LASER
// #define spike_rejection 90 //ns for PASTTREC pt10 // for t1 calibration
//#define spike_rejection 60 //ns for PASTTREC pt10 
// #define spike_rejection 45 //ns for PASTTREC with all the nice filters
// #define spike_rejection 90 //ns for PASTTREC pt15
//#define spike_rejection 100 //ns for PASTTREC pt20
// #define spike_rejection 30 //ns for ASD8 0xA9
// #define spike_rejection 60 //ns for ASD8 0x72
// #define spike_rejection 75 //ns for ASD8 0x52
//#define spike_rejection 47 //ns for ASD8 thr 37000 with LASER
// #define spike_rejection 90 //ns for PASTTREC pt20 with LASER
//#define spike_rejection 90 //ns for PASTTREC pt20 with Fe55
#define spike_rejection 60


#define individual_spike_rejection 1

//#define ref_spike_rejection 100


//#define t1_accept_L (-250 + ref_channel_offset) //ns // GSI Dlab
//#define t1_accept_L (-1000000 + ref_channel_offset) //ns // HZDR fe55
//#define t1_accept_L (-400 + ref_channel_offset) //ns // HZDR 
//#define t1_accept_L (-150 + ref_channel_offset) //ns // Muentz-Torte
// #define t1_accept_L (-1500 + ref_channel_offset) //ns // juelich diamond
#define t1_accept_L (-900 + ref_channel_offset) //ns // juelich diamond
//#define t1_accept_R (100 + ref_channel_offset)//ns // GSI Dlab
//#define t1_accept_R (1000000 + ref_channel_offset)//ns // HZDR fe55
//#define t1_accept_R (300 + ref_channel_offset)//ns // HZDR
#define t1_accept_R (300 + ref_channel_offset)//ns // juelich diamond
// #define t1_accept_R (-130 + ref_channel_offset)//ns // Muentz-Torte
// #define t1_accept_R (-90 + ref_channel_offset)//ns // ASD8 with thr 0x52

#define fish_proj_cut 20



// real cuts on selected data

#define max_tot 10000 // Muentz-Torte
// #define t1_cut_L -850
// #define t1_cut_R -700
#define t1_cut_L -1500 // juelich diamond
#define t1_cut_R 500 // juelich diamond


// #define coincidence_rejection 7
#define accept_hits_per_layer 2

#define enable_coincidence_rejection 0

#define enable_single_hits       0
#define enable_one_hit_per_layer 1
#define enable_two_to_one_hits   0


class MHPulse {
public:
  double t1;
  double t2;
  double tot;
};

TFile *tree_out;
std::map<std::string,int> trig_no;
std::map<std::string,TTree*> data_tree;



Bool_t file_exists(TString fname){
  
  fstream src_file(fname.Data());
  return src_file.good();
}

TString from_env(TString env_var,TString default_val){
  if(gSystem->Getenv(env_var)){
    return gSystem->Getenv(env_var);
  } 
  return default_val;
}



class SecondProc : public base::EventProc {
   protected:

      std::string fTdcId;      //!< tdc id where channels will be selected

      double      fHits[8];    //!< 8 channel, abstract hits

      base::H1handle  hNumHits; //!< histogram with hits number
//       base::H2handle  h2D;
      
//       base::H1handle  totCh1; //!< histogram with hits number
//       base::H1handle  totCh2; //!< histogram with hits number
      
      
      base::H1handle  tot_h[CHANNELS]; 
      base::H1handle  tot_untrig_h[CHANNELS]; 
      base::H1handle  t1_h[CHANNELS]; 
      base::H1handle  t1_mhit_h[CHANNELS]; 
      base::H1handle  t2_mhit_h[CHANNELS]; 
      base::H1handle  potato_h[CHANNELS];
      base::H1handle  meta_potato_h;
      base::H1handle  meta_t1_h;
      base::H1handle  meta_tot_h;
      base::H1handle  meta_tot_2d;
      base::H1handle  meta_t1_2d;
      base::H1handle  coinc_matrix;
      base::H1handle  meta_fish;
      base::H1handle  meta_fish_proj;
      base::H1handle  fishes[FISHES];
      base::H1handle  fish_proj[FISHES];
      base::H1handle  efficiency_h;
      base::H1handle  ref_counts_h;
      base::H1handle  dut_counts_h;
      
      
      int entry_chan;
      int entry_ref_chan;
      double entry_t1;
      double entry_tot;
      
      
      
      
      
//       int evt_no;
      
   public:
      SecondProc(const char* procname, const char* _tdcid) :
         base::EventProc(procname),
         fTdcId(_tdcid),
         hNumHits(0)
      {
         printf("Create %s for %s\n", GetName(), fTdcId.c_str());

         hNumHits = MakeH1("FineAll","Fine all", 1020, 0, 1020, "fine");
//          h2D = MakeH2("h2d","title", 100, -20, 20, 100, -20,20,"x;y");
//          totCh1 = MakeH1("totCh1","totCh1", 20000, -200, 200, "ns");
//          totCh2 = MakeH1("totCh2","totCh2", 20000, -200, 200, "ns");
         
       trig_no[fTdcId] = 0;  
       data_tree[fTdcId] = new TTree((TString) fTdcId,"data recorded");
       data_tree[fTdcId]->Branch("trig_no",&trig_no[fTdcId]);
       data_tree[fTdcId]->Branch("t1",&entry_t1);
       data_tree[fTdcId]->Branch("tot",&entry_tot);
       data_tree[fTdcId]->Branch("chan",&entry_chan);
       data_tree[fTdcId]->Branch("ref_chan",&entry_ref_chan);
       
         
        for( unsigned i=0; i<CHANNELS; i++ ) {
          char chno[16];
          sprintf(chno,"Ch%02d_t1",i);
          t1_h[i] = MakeH1(chno,chno, 2000, t1_L, t1_R, "ns");
          sprintf(chno,"Ch%02d_mhit_t1",i);
          t1_mhit_h[i] = MakeH1(chno,chno, 2000, t1_L, t1_R, "ns");
          sprintf(chno,"Ch%02d_mhit_t2",i);
          t2_mhit_h[i] = MakeH1(chno,chno, 2000, t1_L, t1_R, "ns");
          sprintf(chno,"Ch%02d_tot",i);
          tot_h[i] = MakeH1(chno,chno, 4000, tot_L, tot_R, "ns");
          sprintf(chno,"Ch%02d_tot_untrig",i);
          tot_untrig_h[i] = MakeH1(chno,chno, 4000, tot_L, tot_R, "ns");
          sprintf(chno,"Ch%02d_potato",i);
          potato_h[i] = MakeH2(chno,chno,500,t1_L,t1_R,500, tot_L, tot_R, "t1 (ns);tot (ns)");
        }
        
        meta_t1_h = MakeH1("meta_t1","meta_t1", 2000, t1_L, t1_R, "ns");
        meta_t1_2d = MakeH2("meta_t1_2d","meta_t1_2d", 200, t1_L, t1_R,CHANNELS,-0.5,CHANNELS-0.5, "ns;channel#");
        meta_tot_h = MakeH1("meta_tot","meta_tot", 4000, tot_L, tot_R, "ns");
        meta_tot_2d = MakeH2("meta_tot_2d","meta_tot_2d", 200, tot_L, tot_R,CHANNELS,-0.5,CHANNELS-0.5, "ns;channel#");
        meta_potato_h = MakeH2("meta_potato","meta_potato",500,t1_L,t1_R,500, tot_L, tot_R, "t1 (ns);tot (ns)");
        
//         evt_no = 0;
        
        ref_counts_h = MakeH1("ref_counts","ref_counts", CHANNELS, -0.5, CHANNELS-0.5, "channel #");
        dut_counts_h = MakeH1("dut_counts","dut_counts", CHANNELS, -0.5, CHANNELS-0.5, "channel #");
        efficiency_h = MakeH1("efficiency","efficiency", CHANNELS, -0.5, CHANNELS-0.5, "channel #;kind:F");
        
//         TGo4AnalysisObjectManager* taom = new TGo4AnalysisObjectManager;
        
//         efficiency_h = taom->MakeTH1("F","Histograms","efficiency", CHANNELS, -0.5, CHANNELS -0.5, "Efficiency", "channel", "efficiency");
// //         efficiency_h = new TH1F("efficiency","channel # ",CHANNELS, -0.5, CHANNELS-0.5);
// //         AddHistogram(efficiency_h, "", true);
         
        coinc_matrix = MakeH2("coinc_matrix","coinc_matrix",12,-2.5,9.5,10,15-0.5,24+0.5, "channels 0-7;channels 16-23");
        meta_fish = MakeH2("meta_fish","meta_fish",250,-300,200,200,-100,100, "T_A+T_B;T_B-T_A");
        
        meta_fish_proj = MakeH1("meta_fish_proj","meta_fish_proj",250,-300,200, "T_A+T_B;counts");
        
        
        // fish loop for Lena
        for( unsigned i=0; i<24; i++ ) {
          for( unsigned j=0; j<24; j++ ) {
//             if((i==(j-16)) || (i==(j-16 + 1)) || (i==(j-16 -1))  ) { //if is on diagonal of coinc matrix or one below the diagonal -- cells are overlapping
            
            unsigned fish_index = 0; // for diagonal elements
            // diagonal elements of coincidence matrix
            if ( fTdcId == "TDC_0353" ) {
            if        ( i==4 && j==19){
              fish_index = 0;
            } else if ( i==5 && j==18){
              fish_index = 1;
            } else if ( i==6 && j==17){
              fish_index = 2;
            } else if ( i==7 && j==16){
              fish_index = 3;
            } else if ( i==8 && j==19){
              fish_index = 4;
            } else if ( i==9 && j==18){
              fish_index = 5;
            } else if ( i==10 && j==17){
              fish_index = 6;
            } else if ( i==11 && j==16){
              fish_index = 7;
            // next-to-diagonal elements of coincidence matrix
            } else if ( i==3 && j==19){
              fish_index = 8;
            } else if ( i==4 && j==18){
              fish_index = 9;
            } else if ( i==5 && j==17){
              fish_index = 10;
            } else if ( i==6 && j==16){
              fish_index = 11;
            } else if ( i==7 && j==15){
              fish_index = 12;
            } else if ( i==8 && j==14){
              fish_index = 13;
            } else if ( i==9 && j==13){
              fish_index = 14;
            } else {
              continue;
            }
            } else if ( fTdcId == "TDC_0351" ) {
            if        ( i==4 && j==19){
              fish_index = 0;
            } else if ( i==5 && j==18){
              fish_index = 1;
            } else if ( i==6 && j==17){
              fish_index = 2;
            } else if ( i==7 && j==16){
              fish_index = 3;
            } else if ( i==8 && j==19){
              fish_index = 4;
            } else if ( i==9 && j==18){
              fish_index = 5;
            } else if ( i==10 && j==17){
              fish_index = 6;
            } else if ( i==11 && j==16){
              fish_index = 7;
            // next-to-diagonal elements of coincidence matrix
            } else if ( i==5 && j==19){
              fish_index = 8;
            } else if ( i==6 && j==18){
              fish_index = 9;
            } else if ( i==7 && j==17){
              fish_index = 10;
            } else if ( i==8 && j==20){
              fish_index = 11;
            } else if ( i==9 && j==19){
              fish_index = 12;
            } else if ( i==10 && j==18){
              fish_index = 13;
            } else if ( i==11 && j==17){
              fish_index = 14;
            } else {
              continue;
            }
            } else {
              continue;
            }
            
            char chno[64];
            sprintf(chno,"fish_%02d_vs_%02d",j,i);
            fishes[fish_index]    = MakeH2(chno,chno,250,-1950,-1450,200,-100,100, "T_A+T_B;T_B-T_A");
            sprintf(chno,"fish_proj_%02d_vs_%02d",j,i);
            fish_proj[fish_index] = MakeH1(chno,chno,250,-1950,-1450, "T_A+T_B;counts");
//             }
          }
        }
         
         // enable storing already in constructor
         SetStoreEnabled();
      }
      
      virtual void UserPostLoop(void) {
        cout << "--- User Post Loop " << fTdcId << endl;
        tree_out->cd();
        data_tree[fTdcId]->Write();
      }


      virtual void CreateBranch(TTree* t)
      {
         // only called when tree is created in first.C
         // one can ignore
         t->Branch(GetName(), fHits, "hits[8]/D");
      }

      virtual bool Process(base::Event* ev)
      {
//          printf("### DEBUG ###\n");
         for (unsigned n=0;n<8;n++) fHits[n] = 0.;

         hadaq::TdcSubEvent* sub =
               dynamic_cast<hadaq::TdcSubEvent*> (ev->GetSubEvent(fTdcId));
         
         if(VERBOSE) cout<< "tdc: " << fTdcId << " evt no: " << trig_no[fTdcId] << endl;
         if (sub==0) return false;

//         printf("%s process sub %d %s\n", GetName(), sub->Size(), fTdcId.c_str());

         static int overlaps[32][2] = {
           //contains geometrical overlaps. index 1 is the channel (cell) number, the value pairs stored here
           // are the channels of the cells that it overlaps with.
           //-1 means not connected 
           // this is also a list of DUTs 
           {-1,16} , //0
           {16,17} ,
           {17,18} ,
           {18,19} ,
           {19,20} ,
           {20,21} ,
           {21,22} ,
           {22,23} , //7
           {-1,-1} , //8
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} , //15
           {0,1} , //16
           {1,2} ,
           {2,3} ,
           {3,4} ,
           {4,5} ,
           {5,6} ,
           {6,7} ,
           {7,-1} , //23
           {-1,-1} , //24
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} ,
           {-1,-1} //31
        };
        
        static float channel_spike_rejection[32] = {
          spike_rejection, //0
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection, //5
          spike_rejection,
          spike_rejection,
          spike_rejection,
          10, // hodoscope
          spike_rejection, //10
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection, //15
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection, //20
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection, //25
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection,
          spike_rejection, //30
          spike_rejection
        };
        
        static float effective_spike_rejection = from_env("spike_rejection", TString::Itoa(spike_rejection,10) ).Atof();
        
        static float t1_offsets[32];
        
        static int ref_counts[32];
        static int dut_counts[32];
        static bool is_dut[32];
        
        
        // this is only run once --
        static bool is_initialized=false;
        if(not(is_initialized)){
          for( int i = 0; i<31; i++){
            ref_counts[i] = 0;
            dut_counts[i] = 0;
            t1_offsets[i] = 0;
            is_dut[i] = false;
            if((overlaps[i][0] >= 0) && (overlaps[i][1] >= 0)){
              is_dut[i] = true;
            }
          }
          
          if( file_exists("t1_offsets.txt") ){
            cout << "found t1 offset correction file!" << endl;
            
            ifstream in;
            in.open("t1_offsets.txt");
            Int_t nlines = 0;
            string line;
            while (1) {
              in >> line;
              if (!in.good()) break;
              Float_t offset;
              if(sscanf(line.c_str(),"%f",&offset)){
                t1_offsets[nlines] = offset;
                nlines++;
              }
            }
          }
          
          
          is_initialized = true;
        }
        // ------

         double num(0), ch0tm(0), ch1tm(0), ch2tm(0), ch3tm(0);
         double t1_candidate[CHANNELS];
         double t2_candidate[CHANNELS];
         double t1[CHANNELS];
         double t2[CHANNELS];
         bool   got_rising[CHANNELS];
         bool   got_falling[CHANNELS];
         bool   got_real_hit[CHANNELS];
         double tot[CHANNELS];
         for (unsigned i=0; i<CHANNELS; i++) {
           got_rising[i] = false;
           got_falling[i] = false;
           got_real_hit[i] = false;
         }
         
         std::vector<MHPulse> MultiHitMem[CHANNELS];
         
         for (Int_t i = 0; i< CHANNELS; i++){
           MultiHitMem[i] = std::vector<MHPulse>(0);
         }
         
         
         bool got_HODO_okay = false;
         bool got_HODO_veto = false;
         

         for (unsigned cnt=0;cnt<sub->Size();cnt++) {
            const hadaq::TdcMessageExt& ext = sub->msg(cnt);

            unsigned chid = ext.msg().getHitChannel();
//             unsigned time = ext.msg().getHitTmFine();
//           bool rising   = not(ext.msg().isHitRisingEdge()); // use this line for falling edge first/negative pulses
           bool rising   = ext.msg().isHitRisingEdge(); // use this line for rising edge first/positive pulses
//            if((chid-1) == 8) {
//		    // positive polarity only for trigger input
//		    rising   = ext.msg().isHitRisingEdge(); // use this line for rising edge first/positive pulses
//	    }
//             printf("message, ch %d\n",(chid-1));
            
            if (chid==0) { ch0tm = ext.GetGlobalTime(); continue; }

            // full time
            double tm = ext.GetGlobalTime() + ch0tm;
            if((chid-1) >= CHANNELS) {continue;} // channel out of range of analysis
            if(rising){
              
//               if(chid-1 == 8){
//               printf("got rising  edge, ch %d\n, relative time: %f\n",(chid-1), (tm-ch0tm)*1e9);
//            }
//               switch(chid) {
//                 case 1: ch1tm = tm; break;
//                 case 2: ch2tm = tm; break;
//                 case 3: ch3tm = tm; break;
//               }
              
//               if(not(got_rising[chid-1])){
              
//                 if( (chid-1) == HODO_VETO_CHAN ){
//                   if( (((tm - ch0tm)*1e9) > HODO_VETO_L) && (((tm - ch0tm)*1e9) < HODO_VETO_R ) ) {
//                     got_HODO_okay = true;
//                   } else {
//                     got_HODO_veto = true;
//                   }
//                 }
                
//                 if( !(TAKE_FIRST_HIT && got_real_hit[chid-1]) ){ // block subsequent hits if TAKE_FIRST_HIT setting is active
                  if(( ((tm - ch0tm)*1e9) > t1_accept_L) && (((tm - ch0tm)*1e9) < t1_accept_R )  || (chid-1) == REFCHAN_A || (chid-1) == REFCHAN_B) { // this condition sets another coincidence window, except for REFCHAN_A
                    got_rising[chid-1] = true;
                    got_falling[chid-1] = false;
                    t1_candidate[chid-1] = tm;
                  }
//                 }
//               }
            }else{ // if falling edge
//               printf("got falling edge, ch %d\n",(chid-1));
              if(got_rising[chid-1]){
                if(not(got_falling[chid-1])){
                  got_falling[chid-1] = true;
                  t2_candidate[chid-1] = tm;
                  Double_t candidate_tot_ns = (t2_candidate[chid-1] - t1_candidate[chid-1])*1e9;
                  
//                   if( (candidate_tot_ns > spike_rejection) ){
                  if( (individual_spike_rejection == 0) && (candidate_tot_ns > effective_spike_rejection)  || (individual_spike_rejection == 1) && (candidate_tot_ns > channel_spike_rejection[chid -1])  || (chid-1) == REFCHAN_A || (chid-1) == REFCHAN_B ){
//                   if( (candidate_tot_ns > spike_rejection) &&  ((t2_candidate[chid-1] - t1_candidate[chid-1])*1e9 < max_tot )    ){
                    // hit is long enough not to be rejected
//                     t1[chid-1] = t1_candidate[chid-1];
//                     t2[chid-1] = t2_candidate[chid-1];
//                     tot[chid-1] = t2[chid-1] - t1[chid-1];
                    got_real_hit[chid-1] = true;
                    
                    // fill untriggered tot histogram
                    FillH1(tot_untrig_h[chid-1],(t2_candidate[chid-1]-t1_candidate[chid-1])*1e9);
                    
                    MHPulse this_pulse;
                    this_pulse.t1=t1_candidate[chid-1];
                    this_pulse.t2=t2_candidate[chid-1];
                    this_pulse.tot=t2_candidate[chid-1]-t1_candidate[chid-1];
                    MultiHitMem[chid-1].push_back(this_pulse);
//                     cout << "current tot " << (t2_candidate[chid-1]- t1_candidate[chid-1])*1e9 << endl;
//                     cout << "fill MultiHitMem tot " << MultiHitMem[chid-1][MultiHitMem[chid-1].size()-1].tot*1e9 << endl;
                  }
//                   printf("got hit, ch %d, tot = %f ns\n",(chid-1), tot[chid-1]*1e9);
                }
              }
            }
            
//             FillH1(hNumHits, time);
            
//             if (time>= 600)  {
//                //printf("  ch:%3d tm:%d\n", chid, time);}//tm);
//                num+=1;
//             }
         }
//          FillH2(h2D, (ch2tm-ch1tm)*1e9, (ch3tm-ch2tm)*1e9);
         
         
         
         
//          // reject events where too many channels fire at once
//          unsigned hits_at_once = 0;
//          for( unsigned i=0; i<CHANNELS; i++ ) {
//             if(got_real_hit[i]){
//               hits_at_once++;
//             }
//          }
//          if( hits_at_once > coincidence_rejection) {
//           for( unsigned i=0; i<CHANNELS; i++ ) {
//             got_real_hit[i] = false;
//           }
//          }

//
//
//                Select either first or last hit from trigger window
//      
//
        double t1_ref = 0; 
        if(got_real_hit[REFCHAN_A] || got_real_hit[REFCHAN_B] ){ 
          // a hit in the reference channel
          t1_ref = MultiHitMem[REFCHAN_A][0].t1;
          entry_ref_chan = REFCHAN_A;
          if (got_real_hit[REFCHAN_B]){
            t1_ref = MultiHitMem[REFCHAN_A][0].t1;
            entry_ref_chan = REFCHAN_B;
          }
        } else {
          return true; // if there is no signal in the ref chan to the trigger, don't continue!
        }
        
//         cout << "ref chan: " << entry_ref_chan << " t1: " << t1_ref*1e9 << endl;

        for ( Int_t i = 0; i<CHANNELS; i++){
//           for (Int_t j = 0; j < MultiHitMem[i].size(); j++){
//             cout << "chan "<< i<< " tot: " << (MultiHitMem[i][j].t2 - MultiHitMem[i][j].t1 )*1e9 << endl;
//             cout << "chan "<< i<< " t1 : " << (MultiHitMem[i][j].t1-t1_ref)*1e9 << endl;
//           }
          for (Int_t j = 0; j < MultiHitMem[i].size(); j++){
            t1[i] = MultiHitMem[i][j].t1;
            t2[i] = MultiHitMem[i][j].t2;
            tot[i] = MultiHitMem[i][j].tot;
            FillH1(t1_mhit_h[i],(t1[i]-t1_ref)*1e9);
            FillH1(t2_mhit_h[i],(t2[i]-t1_ref)*1e9);
           
            
            if(TAKE_FIRST_HIT) { // overwrite again with the value of the first hit
              t1[i] = MultiHitMem[i][0].t1;
              t2[i] = MultiHitMem[i][0].t2;
              tot[i] = MultiHitMem[i][0].tot;
            }
              
            
          }
          
        }

//                     t1[chid-1] = t1_candidate[chid-1];
//                     t2[chid-1] = t2_candidate[chid-1];
//                     tot[chid-1] = t2[chid-1] - t1[chid-1];
        
        
         unsigned hits_layer_a = 0;
         unsigned hits_layer_b = 0;
         
         int hit_channels_layer_a[8];
         int hit_channels_layer_b[8];
         
         for( unsigned i=0; i<8; i++ ) {
            if(got_real_hit[i]){
              hit_channels_layer_a[hits_layer_a] = i;
              hits_layer_a++;
            }
         }
         for( unsigned i=16; i<24; i++ ) {
            if(got_real_hit[i]){
              hit_channels_layer_b[hits_layer_b] = i;
              hits_layer_b++;
            }
         }
        
//          if( hits_layer_a > accept_hits_per_layer || hits_layer_b > accept_hits_per_layer) {
//           for( unsigned i=0; i<CHANNELS; i++ ) {
//             got_real_hit[i] = false;
//           }
//          }
         
         bool keep_event = false;
         
         if( enable_two_to_one_hits ) {
          if( hits_layer_a == 2 && hits_layer_b == 1 ){
            if( hit_channels_layer_a[0] == hit_channels_layer_a[1] -1){ // cells are next to each other in layer
              if( hit_channels_layer_a[0] == hit_channels_layer_b[0]-16 ) { // overlap is correct
                keep_event = true;
              }
            }
          }
          
          if( hits_layer_a == 1 && hits_layer_b == 2 ){
            if( hit_channels_layer_b[0] == hit_channels_layer_b[1] -1){ // cells are next to each other in layer
              if( hit_channels_layer_a[0] == hit_channels_layer_b[0]-15 ) { // overlap is correct
                keep_event = true;
              }
            }
          }
         }
         
         if(enable_one_hit_per_layer && hits_layer_a == 1 && hits_layer_b == 1 ){
//            if( hit_channels_layer_a[0] == hit_channels_layer_b[0]-16 || hit_channels_layer_a[0] == hit_channels_layer_b[0]-15 ){
            keep_event = true;
//            }
         }
         
         
         if( enable_single_hits ) {
          if( hits_layer_a == 0 && hits_layer_b == 1 ){
            keep_event = true;
          }
          if( hits_layer_a == 1 && hits_layer_b == 0 ){
            keep_event = true;
          }
         }
         
         if( enable_coincidence_rejection == 0 ) {
           keep_event = true;
	 }

	 
       if ( enable_HODO_VETO ){
	     if( got_real_hit[HODO_VETO_CHAN] ){
           keep_event = false;
           if ((t1[HODO_VETO_CHAN] - t1[REFCHAN_A])*1e9 > -716 && (t1[HODO_VETO_CHAN] - t1[REFCHAN_A])*1e9 < -710) {
            keep_event = true;
           }
         } else {
           keep_event = false;
         }
       }
	 
	 
         if( keep_event == false ) {
          for( unsigned i=0; i<CHANNELS; i++ ) {
            got_real_hit[i] = false;
          }
         }
         
         
         for( unsigned i=0; i<CHANNELS; i++ ) {
            if(got_real_hit[i]){
              
//               if(got_real_hit[REFCHAN_A] || got_real_hit[REFCHAN_B] ){ // t1 information only makes sense if you have 
//                 // a hit in the reference channel
//                 double t1_ref = t1[REFCHAN_A];
//                 entry_ref_chan = REFCHAN_A;
//                 if (got_real_hit[REFCHAN_B]){
//                   t1_ref = t1[REFCHAN_B];
//                   entry_ref_chan = REFCHAN_B;
//                 }
                double t1_vs_ref = (t1[i]-t1_ref)*1e9 ;
                if( ( (t1_vs_ref > t1_cut_L) && (t1_vs_ref < t1_cut_R) && (tot[i]*1e9 < max_tot) ) || i == entry_ref_chan)  {
                  
                  // fill histograms
                  FillH1(tot_h[i],tot[i]*1e9);
                  FillH2(potato_h[i],t1_vs_ref - t1_offsets[i],tot[i]*1e9);
                  FillH1(t1_h[i],t1_vs_ref - t1_offsets[i]);
                  
                  if( (i != REFCHAN_A) && (i != REFCHAN_B) ) {
                    FillH2(meta_potato_h,t1_vs_ref - t1_offsets[i],tot[i]*1e9);
                    FillH1(meta_tot_h,tot[i]*1e9);
                    FillH1(meta_t1_h,t1_vs_ref - t1_offsets[i]);
                    FillH2(meta_tot_2d,tot[i]*1e9,i);
                    FillH2(meta_t1_2d,t1_vs_ref - t1_offsets[i],i);
                    entry_chan = i;
                    entry_t1 = t1_vs_ref - t1_offsets[i];
                    entry_tot = tot[i]*1e9;
                    data_tree[fTdcId]->Fill();
                  }
                  
                  // efficiency estimation ... this cell, cell #i, is a reference detector
                  ref_counts[i]++; // count up reference counts
//                   FillH1(counts_h,i-0.5);
                  if(is_dut[i]){ // is there a cell pair forming a DUT (detector under test) overlapping with the reference?
                    
                    int dut_a = overlaps[i][0];
                    int dut_b = overlaps[i][1];
                    
                    if(got_real_hit[ dut_a ]){ // is there a DUT hit in the first cell overlapping with the reference?
                      double t1_dut_a_vs_ref = (t1[dut_a]-t1_ref)*1e9 ; // is in coincidence window?
                      if( (t1_dut_a_vs_ref > t1_cut_L) && (t1_dut_a_vs_ref < t1_cut_R))  {
                        dut_counts[i]++; // count up dut counts
                      }
                    } else if(got_real_hit[ dut_b ]){  // is there a DUT hit in the second cell overlapping with the reference?
                      double t1_dut_b_vs_ref = (t1[dut_b]-t1_ref)*1e9 ; // is in coincidence window?
                      if( (t1_dut_b_vs_ref > t1_cut_L) && (t1_dut_b_vs_ref < t1_cut_R))  {
                        dut_counts[i]++; // count up dut counts
                      }
                    }
                    
                    
                  }
                }
//               }
            }
         }
         
         
           
        // fill the coincidence coinc_matrix
          for( unsigned i=0; i<24; i++ ) {
              if(got_real_hit[i]){
                  double t1_vs_ref_a = (t1[i]-t1_ref)*1e9 - t1_offsets[i] ;
                  if( (t1_vs_ref_a > t1_cut_L) && (t1_vs_ref_a < t1_cut_R))  {
                  
                    for( unsigned j=0; j<24; j++ ) {
                        if(got_real_hit[j]){
                            double t1_vs_ref_b = (t1[j]-t1_ref)*1e9 - t1_offsets[j];
                            if( (t1_vs_ref_b > t1_cut_L) && (t1_vs_ref_b < t1_cut_R))  {
                              FillH2(coinc_matrix,i,j);
                              
                              unsigned fish_index = 0; // for diagonal elements
            if ( fTdcId == "TDC_0353" ) {
            if        ( i==4 && j==19){
              fish_index = 0;
            } else if ( i==5 && j==18){
              fish_index = 1;
            } else if ( i==6 && j==17){
              fish_index = 2;
            } else if ( i==7 && j==16){
              fish_index = 3;
            } else if ( i==8 && j==19){
              fish_index = 4;
            } else if ( i==9 && j==18){
              fish_index = 5;
            } else if ( i==10 && j==17){
              fish_index = 6;
            } else if ( i==11 && j==16){
              fish_index = 7;
            // next-to-diagonal elements of coincidence matrix
            } else if ( i==3 && j==19){
              fish_index = 8;
            } else if ( i==4 && j==18){
              fish_index = 9;
            } else if ( i==5 && j==17){
              fish_index = 10;
            } else if ( i==6 && j==16){
              fish_index = 11;
            } else if ( i==7 && j==15){
              fish_index = 12;
            } else if ( i==8 && j==14){
              fish_index = 13;
            } else if ( i==9 && j==13){
              fish_index = 14;
            } else {
              continue;
            }
            } else if ( fTdcId == "TDC_0351" ) {
            if        ( i==4 && j==19){
              fish_index = 0;
            } else if ( i==5 && j==18){
              fish_index = 1;
            } else if ( i==6 && j==17){
              fish_index = 2;
            } else if ( i==7 && j==16){
              fish_index = 3;
            } else if ( i==8 && j==19){
              fish_index = 4;
            } else if ( i==9 && j==18){
              fish_index = 5;
            } else if ( i==10 && j==17){
              fish_index = 6;
            } else if ( i==11 && j==16){
              fish_index = 7;
            // next-to-diagonal elements of coincidence matrix
            } else if ( i==5 && j==19){
              fish_index = 8;
            } else if ( i==6 && j==18){
              fish_index = 9;
            } else if ( i==7 && j==17){
              fish_index = 10;
            } else if ( i==8 && j==20){
              fish_index = 11;
            } else if ( i==9 && j==19){
              fish_index = 12;
            } else if ( i==10 && j==18){
              fish_index = 13;
            } else if ( i==11 && j==17){
              fish_index = 14;
            } else {
              continue;
            }
            } else {
              continue;
            }
                              
                              FillH2(meta_fish,(t1_vs_ref_a + t1_vs_ref_b),(t1_vs_ref_b - t1_vs_ref_a));
                              FillH2(fishes[fish_index],(t1_vs_ref_a + t1_vs_ref_b),(t1_vs_ref_b - t1_vs_ref_a));
                              if( abs(t1_vs_ref_b - t1_vs_ref_a) < fish_proj_cut  ){ // cut on drift time difference
                                FillH1(fish_proj[fish_index],(t1_vs_ref_a + t1_vs_ref_b));
                                FillH1(meta_fish_proj,(t1_vs_ref_a + t1_vs_ref_b));
                              }
                              
                            }
                        }
                    }
                    
                  }
              }
          }
        
        /*
        for (int i = 0 ; i<CHANNELS; i++) {
          ((TH1F*) ref_counts_h)->SetBinContent(i+1,ref_counts[i]);
          ((TH1F*) dut_counts_h)->SetBinContent(i+1,dut_counts[i]);
          if( is_dut[i] && (ref_counts[i] > 0)){
            ((TH1F*) efficiency_h)->SetBinContent(i+1,((float) dut_counts[i])/((float) ref_counts[i]));
          }
        }*/
        
        for (int i = 0 ; i<CHANNELS; i++) {
            ((TH1F*) efficiency_h)->SetBinContent(i+1,((float) (((TH1F*) t1_h[i])->GetEntries()) )/((float) (((TH1F*) t1_h[REFCHAN_A])->GetEntries())));
        }
        
        
        
         
//          if(got_real_hit[0]){
//           FillH1(totCh1,tot[0]*1e9);
//          }
//          if(got_real_hit[1]){
//           FillH1(totCh2,tot[1]*1e9);ref_counts[i]
//          }

//         FillH1(hNumHits, num);


//          draw_and_save((TH2F*) meta_fish, "meta_fish","./","colz");

//    tree_out->cd();
//    data_tree[fTdcId]->Write();
         trig_no[fTdcId]++;
         
         return true;
      }
      
      
};


void second()
{
   //hadaq::TdcProcessor::SetDefaults(700);
   tree_out = new TFile("./tree_out.root","RECREATE");
   new SecondProc("Sec_0351", "TDC_0351");
   new SecondProc("Sec_0353", "TDC_0353");
//   new SecondProc("Sec_0351", "TDC_0351");
   cout << "end of second()" << endl;
}

