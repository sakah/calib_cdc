#ifndef HIT_H
#define HIT_H

#include "wiremap.h"

class Hit
{
   public:
      Hit();
      Hit(WireMap* real_wiremap_ptr, WireMap* calib_wiremap_ptr, int layer_number, int cell_number, double hitR, double hitZ, double sigmaR);

      void ClearHit();
      int GetLayerNumber();
      int GetCellNumber();
      double GetHitR();
      double GetHitRSmeared();
      double GetHitZ();
      double GetSigmaR();
      //TVector3 GetHitPosInChamber();
      //TVector3 GetHitPosInCell();

      void PrintHit();
      void DrawHit(bool draw_circle=true, int marker_color=kBlack, int line_style=1);
      Line& GetWireCalib();
      Line& GetWireReal();

   private:
      WireMap* real_wiremap_ptr_;
      WireMap* calib_wiremap_ptr_;
      int layer_number_;
      int cell_number_;
      double hitR_;
      double hitR_smear_;
      double hitZ_;
      double sigmaR_;
};

#endif
