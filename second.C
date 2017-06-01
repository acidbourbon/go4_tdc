#include <stdio.h>

#include "TTree.h"
#include "TH1.h"
#include "TGo4AnalysisObjectManager.h"


#include "base/EventProc.h"
#include "base/Event.h"
#include "hadaq/TdcSubEvent.h"

#define CHANNELS 32
#define FISHES   32
#define REFCHAN 8

#define t1_L -300
#define t1_R 300
#define tot_L -10
#define tot_R 200

#define ref_channel_offset -75 //ns

#define spike_rejection 60 //ns
#define t1_accept_L -1000 //+ ref_channel_offset //ns
#define t1_accept_R 1000 //+ ref_channel_offset//ns

#define fish_proj_cut 20


class SecondProc : public base::EventProc {
   protected:

      std::string fTdcId;      //!< tdc id where channels will be selected

      double      fHits[8];    //!< 8 channel, abstract hits

      base::H1handle  hNumHits; //!< histogram with hits number
//       base::H2handle  h2D;
      
//       base::H1handle  totCh1; //!< histogram with hits number
//       base::H1handle  totCh2; //!< histogram with hits number
      
      
      base::H1handle  tot_h[CHANNELS]; 
      base::H1handle  t1_h[CHANNELS]; 
      base::H1handle  potato_h[CHANNELS];
      base::H1handle  meta_potato_h;
      base::H1handle  meta_t1_h;
      base::H1handle  meta_tot_h;
      base::H1handle  coinc_matrix;
      base::H1handle  meta_fish;
      base::H1handle  meta_fish_proj;
      base::H1handle  fishes[FISHES];
      base::H1handle  fish_proj[FISHES];
      base::H1handle  efficiency_h;
      base::H1handle  ref_counts_h;
      base::H1handle  dut_counts_h;
      
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
         
         
         
        for( unsigned i=0; i<CHANNELS; i++ ) {
          char chno[16];
          sprintf(chno,"Ch%02d_t1",i);
          t1_h[i] = MakeH1(chno,chno, 2000, t1_L, t1_R, "ns");
          sprintf(chno,"Ch%02d_tot",i);
          tot_h[i] = MakeH1(chno,chno, 4000, tot_L, tot_R, "ns");
          sprintf(chno,"Ch%02d_potato",i);
          potato_h[i] = MakeH2(chno,chno,500,t1_L,t1_R,500, tot_L, tot_R, "t1 (ns);tot (ns)");
        }
        
        meta_t1_h = MakeH1("meta_t1","meta_t1", 2000, t1_L, t1_R, "ns");
        meta_tot_h = MakeH1("meta_tot","meta_tot", 4000, tot_L, tot_R, "ns");
        meta_potato_h = MakeH2("meta_potato","meta_potato",500,t1_L,t1_R,500, tot_L, tot_R, "t1 (ns);tot (ns)");
        
        ref_counts_h = MakeH1("ref_counts","ref_counts", CHANNELS, -0.5, CHANNELS-0.5, "channel #");
        dut_counts_h = MakeH1("dut_counts","dut_counts", CHANNELS, -0.5, CHANNELS-0.5, "channel #");
        efficiency_h = MakeH1("efficiency","efficiency", CHANNELS, -0.5, CHANNELS-0.5, "channel #;kind:F");
        
//         TGo4AnalysisObjectManager* taom = new TGo4AnalysisObjectManager;
        
//         efficiency_h = taom->MakeTH1("F","Histograms","efficiency", CHANNELS, -0.5, CHANNELS -0.5, "Efficiency", "channel", "efficiency");
// //         efficiency_h = new TH1F("efficiency","channel # ",CHANNELS, -0.5, CHANNELS-0.5);
// //         AddHistogram(efficiency_h, "", true);
         
        coinc_matrix = MakeH2("coinc_matrix","coinc_matrix",8,-0.5,7.5,8,16-0.5,23+0.5, "channels 0-7;channels 16-23");
        meta_fish = MakeH2("meta_fish","meta_fish",250,-400,100,200,-100,100, "T_A+T_B;T_B-T_A");
        
        meta_fish_proj = MakeH1("meta_fish_proj","meta_fish_proj",250,-400,100, "T_A+T_B;counts");
        
//         for (unsigned i=0; i<FISHES; i++ ) {
//         }
        for( unsigned i=0; i<8; i++ ) {
          for( unsigned j=16; j<24; j++ ) {
            if((i==(j-16)) || (i==(j-16 + 1)) || (i==(j-16 -1))  ) { //if is on diagonal of coinc matrix or one below the diagonal -- cells are overlapping
              unsigned fish_index = i; // for diagonal elements
              if( i==(j-16 + 1) ) { // next to diagonal elements
                fish_index = i + 8;
              }
              if( i==(j-16 - 1) ) { // next to diagonal elements, other side
                fish_index = i + 16;
              }
              char chno[64];
              sprintf(chno,"fish_%02d_vs_%02d",j,i);
              fishes[fish_index]    = MakeH2(chno,chno,250,-400,100,200,-100,100, "T_A+T_B;T_B-T_A");
              sprintf(chno,"fish_proj_%02d_vs_%02d",j,i);
              fish_proj[fish_index] = MakeH1(chno,chno,250,-400,100, "T_A+T_B;counts");
            }
          }
        }
         
         // enable storing already in constructor
         SetStoreEnabled();
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
        
        static int ref_counts[32];
        static int dut_counts[32];
        static bool is_dut[32];
        
        
        // this is only run once --
        static bool is_initialized=false;
        if(not(is_initialized)){
          for( int i = 0; i<31; i++){
            ref_counts[i] = 0;
            dut_counts[i] = 0;
            is_dut[i] = false;
            if((overlaps[i][0] >= 0) && (overlaps[i][1] >= 0)){
              is_dut[i] = true;
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
                
              
//                 if(( ((tm - ch0tm)*1e9) > t1_accept_L) && (((tm - ch0tm)*1e9) < t1_accept_R )) {
//                 if (false) {
                  got_rising[chid-1] = true;
                  got_falling[chid-1] = false;
                  t1_candidate[chid-1] = tm;
//                 }
//               }
            }else{ // if falling edge
//               printf("got falling edge, ch %d\n",(chid-1));
              if(got_rising[chid-1]){
                if(not(got_falling[chid-1])){
                  got_falling[chid-1] = true;
                  t2_candidate[chid-1] = tm;
                  
                  if( (t2_candidate[chid-1] - t1_candidate[chid-1])*1e9 > spike_rejection ){
                    // hit is long enough not to be rejected
                    t1[chid-1] = t1_candidate[chid-1];
                    t2[chid-1] = t2_candidate[chid-1];
                    tot[chid-1] = t2[chid-1] - t1[chid-1];
                    got_real_hit[chid-1] = true;
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
         
         
         
         
         for( unsigned i=0; i<CHANNELS; i++ ) {
            if(got_real_hit[i]){
              
              if(got_real_hit[REFCHAN]){ // t1 information only makes sense if you have 
                // a hit in the reference channel
                double t1_vs_ref = (t1[i]-t1[REFCHAN])*1e9 ;
                if( (t1_vs_ref > t1_L) && (t1_vs_ref < t1_R))  {
                  
                  // fill histograms
                  FillH1(tot_h[i],tot[i]*1e9);
                  FillH2(potato_h[i],(t1[i]-t1[REFCHAN])*1e9,tot[i]*1e9);
                  FillH1(t1_h[i],(t1[i]-t1[REFCHAN])*1e9);
                  
                  if( i != REFCHAN ) {
                    FillH2(meta_potato_h,(t1[i]-t1[REFCHAN])*1e9,tot[i]*1e9);
                    FillH1(meta_tot_h,tot[i]*1e9);
                    FillH1(meta_t1_h,(t1[i]-t1[REFCHAN])*1e9);
                  }
                  
                  // efficiency estimation ... this cell, cell #i, is a reference detector
                  ref_counts[i]++; // count up reference counts
//                   FillH1(counts_h,i-0.5);
                  if(is_dut[i]){ // is there a cell pair forming a DUT (detector under test) overlapping with the reference?
                    
                    int dut_a = overlaps[i][0];
                    int dut_b = overlaps[i][1];
                    
                    if(got_real_hit[ dut_a ]){ // is there a DUT hit in the first cell overlapping with the reference?
                      double t1_dut_a_vs_ref = (t1[dut_a]-t1[REFCHAN])*1e9 ; // is in coincidence window?
                      if( (t1_dut_a_vs_ref > t1_L) && (t1_dut_a_vs_ref < t1_R))  {
                        dut_counts[i]++; // count up dut counts
                      }
                    } else if(got_real_hit[ dut_b ]){  // is there a DUT hit in the second cell overlapping with the reference?
                      double t1_dut_b_vs_ref = (t1[dut_b]-t1[REFCHAN])*1e9 ; // is in coincidence window?
                      if( (t1_dut_b_vs_ref > t1_L) && (t1_dut_b_vs_ref < t1_R))  {
                        dut_counts[i]++; // count up dut counts
                      }
                    }
                    
                    
                  }
                }
              }
            }
         }
         
         
        // fill the coincidence coinc_matrix
        if(got_real_hit[REFCHAN]){ 
           
          for( unsigned i=0; i<8; i++ ) {
              if(got_real_hit[i]){
                  double t1_vs_ref_a = (t1[i]-t1[REFCHAN])*1e9 ;
                  if( (t1_vs_ref_a > t1_L) && (t1_vs_ref_a < t1_R))  {
                  
                    for( unsigned j=16; j<24; j++ ) {
                        if(got_real_hit[j]){
                            double t1_vs_ref_b = (t1[j]-t1[REFCHAN])*1e9 ;
                            if( (t1_vs_ref_b > t1_L) && (t1_vs_ref_b < t1_R))  {
                              FillH2(coinc_matrix,i,j);
                              
                              if((i==(j-16)) || (i==(j-16 + 1)) || (i==(j-16 - 1))) { //if is on diagonal of coinc matrix or one below the diagonal -- cells are overlapping
                                FillH2(meta_fish,(t1_vs_ref_a + t1_vs_ref_b),(t1_vs_ref_b - t1_vs_ref_a));
                                unsigned fish_index = i; // for diagonal elements
                                if( i==(j-16 + 1) ) { // next to diagonal elements
                                  fish_index = i + 8;
                                }
                                if( i==(j-16 - 1) ) { // next to diagonal elements, other side
                                  fish_index = i + 16;
                                }
                                FillH2(fishes[fish_index],(t1_vs_ref_a + t1_vs_ref_b),(t1_vs_ref_b - t1_vs_ref_a));
                                if( abs(t1_vs_ref_b - t1_vs_ref_a) < fish_proj_cut){ // cut on drift time difference
                                  FillH1(fish_proj[fish_index],(t1_vs_ref_a + t1_vs_ref_b));
                                  FillH1(meta_fish_proj,(t1_vs_ref_a + t1_vs_ref_b));
                                }
                              }
                              
                            }
                        }
                    }
                    
                  }
              }
          }
        }
        
        
        for (int i = 0 ; i<CHANNELS; i++) {
          ((TH1F*) ref_counts_h)->SetBinContent(i+1,ref_counts[i]);
          ((TH1F*) dut_counts_h)->SetBinContent(i+1,dut_counts[i]);
          if( is_dut[i] && (ref_counts[i] > 0)){
            ((TH1F*) efficiency_h)->SetBinContent(i+1,((float) dut_counts[i])/((float) ref_counts[i]));
          }
        }
        
        
        
         
//          if(got_real_hit[0]){
//           FillH1(totCh1,tot[0]*1e9);
//          }
//          if(got_real_hit[1]){
//           FillH1(totCh2,tot[1]*1e9);ref_counts[i]
//          }

//         FillH1(hNumHits, num);


         return true;
      }
};


void second()
{
   //hadaq::TdcProcessor::SetDefaults(700);
//    new SecondProc("Sec_1482", "TDC_1482");
   new SecondProc("Sec_1483", "TDC_1483");
}

