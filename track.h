#ifndef TRACK_H
#define TRACK_H

#include "param.h"
#include "hitlist.h"
#include "hit.h"

class Track
{
   public:
      Track();
      Track(HitList& hitlist);
      void SetIniLine(Line& ini_line);
      void SetFitLine(Line& fit_line);

      void PrintTrack();
      void DrawTrack();
      Line GetPossibleWireLine(Hit& hit);
      Line GetPossibleWireLineInCell(Hit& hit);
      void DrawPossibleWireLine(Hit& hit);
      void DrawPossibleWireLineInCell(Hit& hit);

      double GetChi2(Line& line);
      Line& GetIniLine();
      Line& GetFitLine();
      HitList& GetHitList();

   private:
      HitList hitlist_;
      Line ini_line_;
      Line fit_line_;

};

#endif
