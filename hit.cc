#include <stdio.h>
#include "hit.h"
#include "TVector3.h"
#include "TMarker.h"
#include "TEllipse.h"
#include "TRandom.h"

Hit::Hit()
{
   ClearHit();
}

Hit::Hit(WireMap* real_wiremap_ptr, WireMap* calib_wiremap_ptr, int layer_number, int cell_number, double hitR, double hitZ, double sigmaR)
{
   real_wiremap_ptr_ = real_wiremap_ptr;
   calib_wiremap_ptr_ = calib_wiremap_ptr;
   layer_number_ = layer_number;
   cell_number_ = cell_number;
   hitR_ = hitR;
   hitR_smear_ = gRandom->Gaus(hitR, sigmaR);
   hitZ_ = hitZ;
   sigmaR_ = sigmaR;
}

void Hit::ClearHit()
{
   layer_number_ = -1;
   cell_number_ = -1;
   hitR_ = -1e10;
   hitR_smear_ = -1e10;
   hitZ_ = -1e10;
   sigmaR_ = -1e10;
}

int Hit::GetLayerNumber()
{
   return layer_number_;
}

int Hit::GetCellNumber()
{
   return cell_number_;
}

double Hit::GetHitR()
{
   return hitR_;
}

double Hit::GetHitRSmeared()
{
   return hitR_smear_;
}

double Hit::GetHitZ()
{
   return hitZ_;
}

double Hit::GetSigmaR()
{
   return sigmaR_;
}

void Hit::PrintHit()
{
   TVector3& off = calib_wiremap_ptr_->GetOffset(layer_number_, cell_number_);
   printf("cid %2d icell %3d hitR %6.2f hitR_smear %6.2f (sigmaR %6.2f) hitZ %6.2f (xoff %f yoff %f zoff %f)\n", 
         layer_number_, cell_number_, hitR_, hitR_smear_, sigmaR_, hitZ_,
         off.X(), off.Y(), off.Z());
}

void Hit::DrawHit(bool draw_circle, int marker_color, int line_style)
{
   //TVector3 pos = calib_wiremap_ptr_->GetWire(layer_number_, cell_number_).GetPosAtZ(hitZ_);
   TVector3 pos = real_wiremap_ptr_->GetWire(layer_number_, cell_number_).GetPosAtZ(hitZ_);
   // draw wire center
   TMarker* m = new TMarker(pos.X(), pos.Y(), 20);
   m->SetMarkerSize(0.6);
   //printf("marker_color %d (kBack %d)\n", marker_color,kBlack);
   m->SetMarkerColor(marker_color);
   m->Draw();

   if (!draw_circle) {
      return;
   }

   if (layer_number_==-1) {
      return;
   }

   // draw drift circle
   double r0 = GetHitR();
   double r1 = GetHitRSmeared();
   TEllipse* e0 = new TEllipse(pos.X(), pos.Y(), r0);
   TEllipse* e1 = new TEllipse(pos.X(), pos.Y(), r1);

   e0->SetLineColor(kBlue);
   e0->SetLineStyle(line_style);
   e0->SetFillStyle(4000);
   e0->Draw();

   e1->SetLineColor(kRed);
   e1->SetLineStyle(line_style);
   e1->SetFillStyle(4000);
   e1->Draw();
}

Line& Hit::GetWireCalib()
{
   return calib_wiremap_ptr_->GetWire(layer_number_, cell_number_);
}

// for debug
Line& Hit::GetWireReal()
{
   return real_wiremap_ptr_->GetWire(layer_number_, cell_number_);
}

