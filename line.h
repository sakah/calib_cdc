#ifndef LINE_H
#define LINE_H

#include "param.h"
#include "TVector3.h"

class Line
{
   public:
      Line();
      void MakeLine(TVector3 p1, TVector3 p2);
      void MakeLine(double A, double B, double C, double D, double E, double F);
      void MakeLine(double A, double C, double D, double F);
      void MoveLine(TVector3 displacement);
      void Trans2D(TVector3& pos, double ang);
      void DrawPoints(int marker_style, int marker_col, double marker_size);
      void DrawPoint(double z, int marker_style, int marker_col, double marker_size);
      void DrawLine(int line_col, int line_style=1, double opacity=1);
      void PrintLine();
      double GetClosestPoints(Line& other, TVector3& pA, TVector3& pB);
      double GetDistance(Line& other);
      double GetDistanceSigned(Line& other);
      TVector3 GetPosAtZ(double z);
      double GetYAtX(double x);
      void GetABCDEF(double& A, double&B, double& C, double&D, double& E, double& F);
      void GetACDF(double& A, double& C, double& D, double& F);
      TVector3& GetP1();
      TVector3& GetP2();

   private:
      TVector3 p1_;
      TVector3 p2_;
      TVector3 Trans2DPoint(TVector3& p, TVector3& pos, double ang);
      void DrawPoint(TVector3& p, int marker_style, int marker_col, double marker_size);
      double CalcDot(double x1, double y1, double z1, double x2, double y2, double z2);
};

#endif
