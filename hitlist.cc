#include <stdio.h>
#include "line.h"
#include "hitlist.h"
#include "TVector3.h"
#include "TMarker.h"
#include "TEllipse.h"
#include "TRandom.h"

HitList::HitList()
{
   ClearHitList();
}

HitList::HitList(bool* hit_layers, int hit_region, WireMap& real_wiremap, WireMap& calib_wiremap, Line& line, double sigmaR, int test_cid, int test_cid_region)
{
   ClearHitList();

   // Make hits according to real wiremap considering wire_pos offset.
   
   int nlayers = real_wiremap.GetNumSenseLayers();
   for (int cid=0; cid<nlayers; cid++) {

      int ncells = real_wiremap.GetNumCells(cid);
      for (int icell=0; icell<ncells; icell++) {
         Line& wire_real = real_wiremap.GetWire(cid, icell);
         Line& wire_calib = calib_wiremap.GetWire(cid, icell);
         TVector3 pA; // point on track(line)
         TVector3 pB; // point on wire
         double hitR_calib = wire_calib.GetClosestPoints(line, pA, pB);
         double hitR = wire_real.GetClosestPoints(line, pA, pB);
         double hitZ = pB.Z();
         if (hitR<0.8) {
            //printf("== hitlist cid %d icell %d hitR_calib %f hitR_real %f\n", cid, icell, hitR_calib, hitR);

            Hit hit(&real_wiremap, &calib_wiremap, cid, icell, hitR, hitZ, sigmaR);
            TVector3 wire_pos_at_center = hit.GetWireReal().GetPosAtZ(0);

            //Hit hit2(&real_wiremap, cid, icell, hitR, hitZ, sigmaR);
            //TVector3 wire_pos_at_center_real = hit2.GetWire().GetPosAtZ(0);
            //printf("== calib ==\n"); wire_pos_at_center.Print();
            //printf("== real ==\n");  wire_pos_at_center_real.Print();
            //double hitR2 = wire_calib.GetClosestPoints(line, pA, pB);
            //printf("hitR(real) %f -> hitR(calib) %f\n", hitR, hitR2);

            double ypos = wire_pos_at_center.Y();

            if (hit_region==REGION_UP && ypos<0) {
               continue;
            }
            if (hit_region==REGION_DOWN && ypos>0) {
               continue;
            }
            
            if (hit_layers[cid]==false) {
               continue;
            }

            if (test_cid != -1 && test_cid == cid) {
               if (test_cid_region==REGION_UP && ypos>0) {
                  continue;
               }
               if (test_cid_region==REGION_DOWN && ypos<0) {
                  continue;
               }
            }
            AddHit(hit);
         }
      }
   }
}

void HitList::ClearHitList()
{
   num_hits_ = 0;
}

void HitList::AddHit(Hit& hit)
{
   if (num_hits_>=MAX_HITLIST) {
      fprintf(stderr, "Warning: HitList overflowed (%d)\n", MAX_HITLIST);
      return;
   }
   hits_[num_hits_] = hit;
   num_hits_++;
}

Hit& HitList::GetHit(int idx)
{
   return hits_[idx];
}

Hit& HitList::GetHit(int cid, int region)
{
   for (int ihit=0; ihit<num_hits_; ihit++) {
      int cid1 = hits_[ihit].GetLayerNumber();
      if (cid1 != cid) continue;
      double ypos = hits_[ihit].GetWireReal().GetPosAtZ(0).Y();
      if (ypos>0 && region==REGION_UP) {
         return hits_[ihit];
      } else if (ypos<0 && region==REGION_DOWN) {
         return hits_[ihit];
      }
   }
   fprintf(stderr, "Warning: no hits found at cid %d region %d\n", cid, region);
   return hits_[0];
}

void HitList::PrintHitList()
{
   for (int ihit=0; ihit<num_hits_; ihit++) {
      hits_[ihit].PrintHit();
   }
}

void HitList::DrawHitList(bool draw_circle, int marker_color)
{
   for (int ihit=0; ihit<num_hits_; ihit++) {
      hits_[ihit].DrawHit(draw_circle, marker_color);
   }
}

int HitList::GetNumHits()
{
   return num_hits_;
}
