#include <stdio.h>

// #include "TTree.h"

#include "base/EventProc.h"
#include "base/Event.h"
#include "hadaq/TdcSubEvent.h"
#include "hadaq/HldProcessor.h"
#include "hadaq/TrbProcessor.h"
#include "base/ProcMgr.h"         

class SecondProc : public base::EventProc {
protected:
  
  std::string fTdcId;      //!< tdc id where channels will be selected
  double      fHits[33][2];    //!< 33 channel, abstract hits, two dges
  
  unsigned eventnumber;

//  base::H1handle  hNumHits; //!< histogram with hits number
  /*base::H1handle  hDif1; //!< histogram with hits number
  base::H1handle  hDif2; //!< histogram with hits number
  base::H1handle  hDif3; //!< histogram with hits number
  base::H1handle  hDif4; //!< histogram with hits number
  base::H1handle  hDif5; //!< histogram with hits number
  base::H1handle  hDif6; //!< histogram with hits number
  base::H1handle  hDif7; //!< histogram with hits number
  base::H1handle  hTot1; //!< histogram with hits number
  base::H1handle  hTot2; //!< histogram with hits number
  base::H1handle  hTot3; //!< histogram with hits number
  base::H1handle  hTot4; //!< histogram with hits number
  base::H1handle  hTot5; //!< histogram with hits number
  base::H1handle  hTot6; //!< histogram with hits number
  base::H2handle  hUser; //!< user-defined 2D histogram
*/
  base::H1handle  hDiff_Cal;
  base::H1handle  hTest;
public:
  SecondProc(const char* procname, const char* _tdcid) :
    base::EventProc(procname),
    fTdcId(_tdcid)//,
//    hUser(0)
  {
    printf("Create %s for %s\n", GetName(), fTdcId.c_str());

   // hNumHits = MakeH1("NumHits","Num hits", 100, 0, 100, "number");

    eventnumber = 0;

    hDiff_Cal = MakeH1("Diff_Cal","Diff_Cal",1800,-170,170, "ns");

    hTest = MakeH1("TEst","TEST",1800,-170,170, "chnl");
    // enable storing already in constructor
    // SetStoreEnabled();
  }

  virtual void CreateBranch(TTree* t)
  {
    // only called when tree is created in first.C
    // one can ignore
    // t->Branch(GetName(), fHits, "hits[8]/D");
  }

  virtual bool Process(base::Event* ev)
  {
    for (unsigned n=0;n<33;n++) {
      fHits[n][0] = 0.;
      fHits[n][1] = 0.;
    }

    eventnumber++;
    hadaq::TdcSubEventFloat* sub =
      dynamic_cast<hadaq::TdcSubEventFloat*> (ev->GetSubEvent(fTdcId));

    //printf("%s process sub %p %s\n", GetName(), sub, fTdcId.c_str());

    // when return false, event processing is cancelled
    if (sub==0) return true;

    hadaq::HldProcessor* hld = dynamic_cast<hadaq::HldProcessor*> (base::ProcMgr::instance()->FindProc("HLD"));
    //hadaq::TrbProcessor* trb = hld ? hld->FindTRB(0x1000) : 0;
    hadaq::TdcProcessor* tdc = hld ? hld->FindTDC(0x1000) : 0;
    //if (hld) printf("HLD event header %u\n", hld->GetLastEventHdr().GetSeqNr());
    //if (trb) printf("TRB subevent header trig:%x decoding:%x\n", trb->GetLastSubeventHdr().GetTrigNr(), trb->GetLastSubeventHdr().GetDecoding());
    //if (tdc) printf("TDC header/trailer messages raw %x %x\n", tdc->GetLastTdcHeader().getData(), tdc->GetLastTdcTrailer().getData());
//if (tdc)  printf("Right TRB %d\n", tdc->fCurrentTemp);

//    FillH1(hTest, tdc->fHits.GetBin(1));
    //if ()

    double num(0);
    for (unsigned cnt=0;cnt<sub->Size();cnt++) {
      const hadaq::MessageFloat& msg = sub->msg(cnt);

      unsigned chid = msg.getCh();
      unsigned edge = msg.getEdge(); // 0 - rising, 1 - falling
      // if (chid==0) { ch0tm = ext.GetGlobalTime(); continue; }

      // time relative ch0
      double tm = msg.stamp;

      // use only first hit in each channel
      if ((chid<33) && (fHits[chid][edge]==0.)) fHits[chid][edge] = tm;

      printf("  ch:%3d tm:%f, edge:%d\n", chid, tm, edge);
      num+=1;
    }

   //printf("tot(%d): %f\n", eventnumber, fHits[0][0]);
    //printf("led(%d): %f %f\n", eventnumber, (fHits[9][0] - fHits[11][0]), (fHits[9][1] - fHits[11][1]));

//    FillH1(hNumHits, num);

    /*
      FillH1(hDif1, (fHits[9][1] - fHits[9][0]) );
      FillH1(hDif2, (fHits[9][0] - fHits[11][0]) );
      FillH2(hUser, (fHits[9][1] - fHits[9][0]) , (fHits[9][0] - fHits[11][0]) );
    */

    //printf("tot: %f\n", (fHits[6][1] - fHits[6][0]));
    /*double tot2;
    double tot4;
    double tot6;

    tot2 = fHits[2][1]  - fHits[2][0];
    tot4 = fHits[4][1]  - fHits[4][0];    
    tot6 = fHits[6][1]  - fHits[6][0];    

    
    if (tot2 > 2 && tot2 < 20) 
      FillH1(hTot1, tot2);

    if (tot4 > 1 && tot4 < 18) 
      FillH1(hTot2, tot4);

    if (tot6 > 35 && tot6 < 55) 
      FillH1(hTot3, tot6);
    

    if( fHits[6][0]!=0 && fHits[2][0]!=0 && fHits[4][0]!=0) {
      FillH1(hTot4, tot2);
      FillH1(hTot5, tot4);
      FillH1(hTot6, tot6);
    }
    

    
    if (fHits[4][0]!=0 && fHits[2][0]!=0  )
      FillH1(hDif1, (fHits[4][0]  - fHits[2][0]));
    
    if (fHits[6][0]!=0 && fHits[4][0]!=0  )
      FillH1(hDif2, (fHits[6][0]  - fHits[4][0]));

    if (fHits[6][0]!=0 && fHits[2][0]!=0 )
      FillH1(hDif3, (fHits[6][0]  - fHits[2][0]));

    if (fHits[4][0]!=0 && fHits[2][0]!=0 && fHits[6][0]!=0 )
      FillH1(hDif4, (fHits[4][0]  - fHits[2][0]));
    
    if (fHits[6][0]!=0 && fHits[4][0]!=0 && fHits[2][0]!=0 )
      FillH1(hDif5, (fHits[6][0]  - fHits[4][0]));

    if (fHits[6][0]!=0 && fHits[2][0]!=0 && fHits[4][0]!=0)
      FillH1(hDif6, (fHits[6][0]  - fHits[2][0]));
*/
   
/*    if (fHits[6][0]!=0 && fHits[2][0]!=0 && fHits[4][0]!=0 &&


        tot2 > 8.0 && tot2 < 10.0 &&
	tot4 > 8 && tot4 < 14 &&
	tot6 > 45.0 && tot6 < 51.0 
*/
        /*
        tot2 > 7. && tot2 < 12. &&
	tot4 > 8. && tot4 < 13.0 &&
	tot6 > 46.0 && tot6 < 50.0 
        */

        
        

//	)
//      FillH1(hDif7, (fHits[6][0]  + fHits[4][0])/2 - fHits[2][0]  );


    

    // cuts no longer needed - one see only normal triggers here
    /*	 if(
	 ((fHits[6][1] - fHits[6][0]) > 38)  && ((fHits[6][1] - fHits[6][0])<48) &&
	 ((fHits[4][1] - fHits[4][0]) > 35)  && ((fHits[4][1] - fHits[4][0])<42) &&
	 ((fHits[2][1] - fHits[2][0]) > 35)  && ((fHits[2][1] - fHits[2][0])<42)
	 ) {
	 //FillH1(hDif1, (fHits[6][0] + fHits[4][0])/2 - fHits[2][0] );
	 FillH1(hDif1, (fHits[6][0]  - fHits[2][0] ));
	 //FillH1(hDif2, (fHits[1][1] - fHits[2][1]) );
	 }
    */

  FillH1(hDiff_Cal, fHits[1][0]);
    return true;
  }
};


void second()
{
  //new SecondProc("A", "TDC_1133");
  //new SecondProc("A", "TDC_1580");
  new SecondProc("A", "TDC_1000");
}
