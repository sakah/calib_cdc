#include "track.h"
#include "TMath.h"

Track::Track()
{
} 

Track::Track(HitList& hitlist)
{
   hitlist_ = hitlist;
}

void Track::SetIniLine(Line& ini_line)
{
   ini_line_ = ini_line;
}

void Track::SetFitLine(Line& fit_line)
{
   fit_line_ = fit_line;
}

void Track::PrintTrack()
{
   hitlist_.PrintHitList();
   ini_line_.PrintLine();
   fit_line_.PrintLine();
}

void Track::DrawTrack()
{
   hitlist_.DrawHitList();
   ini_line_.DrawLine(kMagenta, 2);
   fit_line_.DrawLine(kRed);
}

Line Track::GetPossibleWireLine(Hit& hit)
{
   Line line = fit_line_;
   TVector3 pA; // track
   TVector3 pB; // wire
   Line& wire = hit.GetWireCalib();
   double fitR = fit_line_.GetClosestPoints(wire, pA, pB);
   TVector3 pWire = wire.GetPosAtZ(pA.Z());
   TVector3 relpos = pA - pWire; // wrt wire center

   line.MoveLine(relpos); // possible line = track - fitX
   return line;
}

Line Track::GetPossibleWireLineInCell(Hit& hit)
{
   Line line = fit_line_;
   TVector3 pA; // track
   TVector3 pB; // wire
   Line& wire = hit.GetWireCalib();
   double fitR = fit_line_.GetClosestPoints(wire, pA, pB);
   TVector3 pWire = wire.GetPosAtZ(pA.Z());
   TVector3 relpos = pA - pWire; // wrt wire center

   line.MoveLine(relpos); // possible line = track - fitX
   line.MoveLine(pWire); // so that origin of wire is center
   return line;
}

void Track::DrawPossibleWireLine(Hit& hit)
{
   Line line = GetPossibleWireLine(hit);
   line.DrawLine(kBlue, 1, 0.35);
}

void Track::DrawPossibleWireLineInCell(Hit& hit)
{
   Line line = GetPossibleWireLineInCell(hit);
   line.DrawLine(kBlue, 1, 0.35);
}

double Track::GetChi2(Line& line)
{
   double chi2 = 0.0;
   double chi2_real = 0.0;
   int num_hits = hitlist_.GetNumHits();
   //printf("=== debug chi2\n");
   for (int ihit=0; ihit<num_hits; ihit++) {
      Hit& hit = hitlist_.GetHit(ihit);
      double hitR = hit.GetHitRSmeared(); 
      //double hitR = hit.GetHitR(); // for debug
      double fitR = line.GetDistance(hit.GetWireCalib());
      double fitR_real = line.GetDistance(hit.GetWireReal());
      double sigmaR = hit.GetSigmaR();
      double dx = (fitR-hitR)/sigmaR;
      double dx_real = (fitR_real-hitR)/sigmaR;
      chi2 += dx*dx;
      chi2_real += dx_real*dx_real;
      int cid = hit.GetLayerNumber();
      int icell = hit.GetCellNumber();
      //printf("cid %d icell %d hitR %f fitR_real %f fitR %f chi2 %f chi2_real %f dx %f dx_real %f\n", 
      //      cid, icell, hitR, fitR_real, fitR, chi2, chi2_real, dx, dx_real);
   }
   return chi2;
}

Line& Track::GetIniLine()
{
   return ini_line_;
}

Line& Track::GetFitLine()
{
   return fit_line_;
}

HitList& Track::GetHitList()
{
   return hitlist_;
}
