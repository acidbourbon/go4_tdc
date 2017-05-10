#include <stdio.h>

#include "TTree.h"

#include "base/EventProc.h"
#include "base/Event.h"
#include "hadaq/TdcSubEvent.h"

#define CHANNELS 10
#define REFCHAN 8

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
           t1_h[i] = MakeH1(chno,chno, 2000, -200, 200, "ns");
           sprintf(chno,"Ch%02d_tot",i);
           tot_h[i] = MakeH1(chno,chno, 4000, -10, 500, "ns");
           sprintf(chno,"Ch%02d_potato",i);
           potato_h[i] = MakeH2(chno,chno,500,-200,200,500, -10, 200, "t1 (ns);tot (ns)");
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


         double num(0), ch0tm(0), ch1tm(0), ch2tm(0), ch3tm(0);
         double t1[CHANNELS];
         double t2[CHANNELS];
         bool   got_rising[CHANNELS];
         bool   got_falling[CHANNELS];
         double tot[CHANNELS];
         for (unsigned i=0; i<CHANNELS; i++) {
           got_rising[i] = false;
           got_falling[i] = false;
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
//               printf("got rising  edge, ch %d\n",(chid-1));
//               switch(chid) {
//                 case 1: ch1tm = tm; break;
//                 case 2: ch2tm = tm; break;
//                 case 3: ch3tm = tm; break;
//               }
              if(not(got_rising[chid-1])){
                got_rising[chid-1] = true;
                t1[chid-1] = tm;
              }
            }else{ // if falling edge
//               printf("got falling edge, ch %d\n",(chid-1));
              if(got_rising){
                if(not(got_falling[chid-1])){
                  got_falling[chid-1] = true;
                  t2[chid-1] = tm;
                  tot[chid-1] = t2[chid-1] - t1[chid-1];
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
            if(got_falling[i]){
              FillH1(tot_h[i],tot[i]*1e9);
              
              if(got_falling[REFCHAN]){ // t1 information only makes sense if you have 
                // a hit in the reference channel
                FillH2(potato_h[i],(t1[i]-t1[REFCHAN])*1e9,tot[i]*1e9);
                FillH1(t1_h[i],(t1[i]-t1[REFCHAN])*1e9);
              }
            }
         }
         
//          if(got_falling[0]){
//           FillH1(totCh1,tot[0]*1e9);
//          }
//          if(got_falling[1]){
//           FillH1(totCh2,tot[1]*1e9);
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

