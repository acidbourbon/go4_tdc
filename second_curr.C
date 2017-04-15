#include <stdio.h>

#include "TTree.h"

#include "base/EventProc.h"
#include "base/Event.h"
#include "hadaq/TdcSubEvent.h"

class SecondProc : public base::EventProc {
   protected:

      std::string fTdcId;      //!< tdc id where channels will be selected

      double      fHits[8];    //!< 8 channel, abstract hits

      base::H1handle  hNumHits; //!< histogram with hits number

   public:
      SecondProc(const char* procname, const char* _tdcid) :
         base::EventProc(procname),
         fTdcId(_tdcid),
         hNumHits(0)
      {
         printf("Create %s for %s\n", GetName(), fTdcId.c_str());

         hNumHits = MakeH1("FineAll","Fine all", 1020, 0, 1020, "fine");

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
         for (unsigned n=0;n<8;n++) fHits[n] = 0.;

         hadaq::TdcSubEvent* sub =
               dynamic_cast<hadaq::TdcSubEvent*> (ev->GetSubEvent(fTdcId));


         if (sub==0) return false;

//         printf("%s process sub %d %s\n", GetName(), sub->Size(), fTdcId.c_str());


         double num(0), ch0tm(0);

         for (unsigned cnt=0;cnt<sub->Size();cnt++) {
            const hadaq::TdcMessageExt& ext = sub->msg(cnt);

            unsigned chid = ext.msg().getHitChannel();
            unsigned time = ext.msg().getHitTmFine();
//            if (chid==0) { ch0tm = ext.GetGlobalTime(); continue; }

            // full time
            double tm = ext.GetGlobalTime() + ch0tm;
            
            FillH1(hNumHits, time);
            
            if (time>= 600)  {
               //printf("  ch:%3d tm:%d\n", chid, time);}//tm);
               num+=1;
            }

//         FillH1(hNumHits, num);

         return true;
      }
};


void second()
{
   //hadaq::TdcProcessor::SetDefaults(700);
   new SecondProc("Second1", "TDC_1483");
}

