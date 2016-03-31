#ifndef HITList_H
#define HITList_H

#include "param.h"
#include "hit.h"

class HitList
{
   public:
      HitList();
      HitList(bool* hit_layers, int hit_region, WireMap& real_wiremap, WireMap& calib_wiremap, Line& line, double sigmaR, int test_cid=-1, int test_cid_region=REGION_ALL);

      void ClearHitList();
      void AddHit(Hit& hit);
      Hit& GetHit(int idx);
      Hit& GetHit(int cid, int region);
      void PrintHitList();
      void DrawHitList(bool draw_circle=true, int marker_color=kBlack);
      int GetNumHits();

   private:
      int num_hits_;
      Hit hits_[MAX_HITLIST];
};

#endif
