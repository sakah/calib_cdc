#include "TMath.h"
#include "TMarker.h"
#include "TLine.h"
#include "line.h"

Line::Line()
{
}

void Line::MakeLine(TVector3 p1, TVector3 p2)
{
   p1_ = p1;
   p2_ = p2;
}

void Line::MakeLine(double A, double B, double C, double D, double E, double F)
{
   // Line in 3D space is described in the following
   // Ax + By + C = 0; normal to vector (A,B) and distance from origin (x,y)=(0,0) is C/sqrt(A*A+B*B)
   // Dy + Ez + F = 0; normal to vector (D,E) and distance from origin (y,z)=(0,0) is F/sqrt(D*D+E*E)
   // Here B>0 and E>0
   
   // firstly normalize
   TVector3 xy(A, B, 0);
   TVector3 yz(D, E, 0);
   TVector3 nxy = xy.Unit();
   TVector3 nyz = yz.Unit();
   A = nxy.X();
   B = nxy.Y();
   D = nyz.X();
   E = nyz.Y();

   double x1;
   double y1;
   double z1;
   double x2;
   double y2;
   double z2;
   double abs_A = TMath::Abs(A);
   double abs_E = TMath::Abs(E);

   // Ax + By + C = 0
   // Dy + Ez + F = 0
   if (abs_A>0.0001) {
      if (abs_E>0.001) {
         y1 = 0.000;
         y2 = 100.0;
         x1 = (-B*y1 - C)/A;
         x2 = (-B*y2 - C)/A;
         z1 = (-D*y1 - F)/E;
         z2 = (-D*y2 - F)/E;
      } else {
         z1 = 0.000;
         z2 = 100.0;
         y1 = (-E*z1 - F)/D;
         y2 = (-E*z2 - F)/D;
         x1 = (-B*y1 - C)/A;
         x2 = (-B*y2 - C)/A;
      }
   } else {
      x1 = 0.000;
      x2 = 100.0;
      y1 = (-A*x1-C)/B;
      y2 = (-A*x2-C)/B;
      z1 = (-D*y1 - F)/E;
      z2 = (-D*y2 - F)/E;
   }
   
   p2_.SetXYZ(x1, y1, z1);
   p1_.SetXYZ(x2, y2, z2);
}

void Line::MakeLine(double A, double C, double D, double F)
{
   double B = TMath::Sqrt(1-A*A);
   double E = TMath::Sqrt(1-D*D);
   MakeLine(A,B,C,D,E,F);
}

void Line::MoveLine(TVector3 displacement)
{
   TVector3 p1new = p1_ - displacement;
   TVector3 p2new = p2_ - displacement;
   MakeLine(p1new, p2new);
}

TVector3 Line::Trans2DPoint(TVector3& p, TVector3& pos, double ang)
{
   double px = p.X();
   double py = p.Y();
   double cx = pos.X();
   double cy = pos.Y();
   double px_new = px*TMath::Cos(ang)-py*TMath::Sin(ang) + cx;
   double py_new = px*TMath::Sin(ang)+py*TMath::Cos(ang) + cy;
   return TVector3(px_new, py_new, p.Z());
}

void Line::Trans2D(TVector3& pos, double ang)
{
   p1_ = Trans2DPoint(p1_, pos, ang);
   p2_ = Trans2DPoint(p2_, pos, ang);
}

void Line::PrintLine()
{
   p1_.Print();
   p2_.Print();
}

void Line::DrawPoint(TVector3& p, int marker_style, int marker_col, double marker_size)
{
   TMarker* m = new TMarker(p.X(), p.Y(), marker_style);
   m->SetMarkerColor(marker_col);
   m->SetMarkerSize(marker_size);
   m->Draw();
}

void Line::DrawPoints(int marker_style, int marker_col, double marker_size)
{
   DrawPoint(p1_, marker_style, marker_col, marker_size);
   DrawPoint(p2_, marker_style, marker_col, marker_size);
}

void Line::DrawPoint(double z, int marker_style, int marker_col, double marker_size)
{
   TVector3 p = GetPosAtZ(z);
   DrawPoint(p, marker_style, marker_col, marker_size);
}

void Line::DrawLine(int line_col, int line_style, double opacity)
{
   TLine *l = new TLine(p1_.X(), p1_.Y(), p2_.X(), p2_.Y());
   l->SetLineColorAlpha(line_col, opacity);
   l->SetLineStyle(line_style);
   l->Draw();
}

double Line::GetClosestPoints(Line& other, TVector3& pA, TVector3& pB)
{
	//double px1, double py1, double pz1,  // point on line1
	//double vx1, double vy1, double vz1,  // vector of line1
	//double px2, double py2, double pz2,  // point on line2
	//double vx2, double vy2, double vz2,  // vecotr of line2
	//double* x1, double* y1, double *z1,  // closest point on line1
	//double* x2, double* y2, double *z2,  // closest point on line2

   TVector3 vec1 = p2_-p1_;
   TVector3 vec2 = other.p2_-other.p1_;
   TVector3 nvec1 = vec1.Unit();
   TVector3 nvec2 = vec2.Unit();

   double px1 = p1_.X();
   double py1 = p1_.Y();
   double pz1 = p1_.Z();
   double vx1 = vec1.X();
   double vy1 = vec1.Y();
   double vz1 = vec1.Z();
   double nvx1 = nvec1.X();
   double nvy1 = nvec1.Y();
   double nvz1 = nvec1.Z();
   double px2 = other.p1_.X();
   double py2 = other.p1_.Y();
   double pz2 = other.p1_.Z();
   double vx2 = vec2.X();
   double vy2 = vec2.Y();
   double vz2 = vec2.Z();
   double nvx2 = nvec2.X();
   double nvy2 = nvec2.Y();
   double nvz2 = nvec2.Z();

	double dist = 0.0;

	// normalize vectors
	double p21x = px2 - px1;
	double p21y = py2 - py1;
	double p21z = pz2 - pz1;
	double D1 = CalcDot(p21x, p21y, p21z, nvx1, nvy1, nvz1);
	double D2 = CalcDot(p21x, p21y, p21z, nvx2, nvy2, nvz2);
	double V  = CalcDot(nvx1, nvy1, nvz1, nvx2, nvy2, nvz2);
	//fprintf(stderr,"nvx1 %lf nvy1 %lf nvz1 %lf\n",nvx1,nvy1,nvz1);
	//fprintf(stderr,"nvx2 %lf nvy2 %lf nvz2 %lf\n",nvx2,nvy2,nvz2);
	//fprintf(stderr,"D1 %lf D2 %lf V %lf\n",D1,D2,V);
	if (V==1.0 || V==-1.0) {
		fprintf(stderr,"ERROR: calc_closest_point_by_two_lines: two lines are parallel\n");
		return -1;
	}
	double t1 = (D1-V*D2)/(1.0-V*V);
	double t2 = (V*D1-D2)/(1.0-V*V);
	double ux = p21x + t2*nvx2 - t1*nvx1;
	double uy = p21y + t2*nvy2 - t1*nvy1;
	double uz = p21z + t2*nvz2 - t1*nvz1;
	dist = sqrt(ux*ux+uy*uy+uz*uz);

   if (g_debug_line) {
      fprintf(stdout,"calc_closest px1 %lf py1 %lf pz1 %lf\n",px1, py1, pz1);
      fprintf(stdout,"calc_closest vx1 %lf vy1 %lf vz1 %lf\n",vx1, vy1, vz1);
      fprintf(stdout,"calc_closest px2 %lf py2 %lf pz2 %lf\n",px2, py2, pz2);
      fprintf(stdout,"calc_closest vx2 %lf vy2 %lf vz2 %lf\n",vx2, vy2, vz2);
      fprintf(stdout,"calc_closest D1 %lf D2 %lf V %lf t1 %lf t2 %lf dist %lf\n",D1,D2,V,t1,t2,dist);
   }

   double x1 = px1 + t1*nvx1;
   double y1 = py1 + t1*nvy1;
   double z1 = pz1 + t1*nvz1;
   double x2 = px2 + t2*nvx2;
   double y2 = py2 + t2*nvy2;
   double z2 = pz2 + t2*nvz2;

   pA.SetXYZ(x1, y1, z1);
   pB.SetXYZ(x2, y2, z2);

   return dist;
}

double Line::GetDistance(Line& other)
{
   TVector3 pA;
   TVector3 pB;
   return GetClosestPoints(other, pA, pB);
}

double Line::GetDistanceSigned(Line& other)
{
   TVector3 pA;
   TVector3 pB;
   double dist = GetClosestPoints(other, pA, pB);
   double dx = pB.X() - pA.X();
   if (dx<0) return -dist;
   return dist;
}

double Line::CalcDot(double x1, double y1, double z1, double x2, double y2, double z2)
{
   return x1*x2 + y1*y2 + z1*z2;
}

TVector3 Line::GetPosAtZ(double z)
{
   double x1 = p1_.X();
   double y1 = p1_.Y();
   double z1 = p1_.Z();
   double x2 = p2_.X();
   double y2 = p2_.Y();
   double z2 = p2_.Z();
   double dx = x2-x1;
   double dy = y2-y1;
   double dz = z2-z1;
   double x = x1 + (z-z1)/(z2-z1)*(x2-x1);
   double y = y1 + (z-z1)/(z2-z1)*(y2-y1);
   return TVector3(x, y, z);
}

double Line::GetYAtX(double x)
{
   // z is not considered
   double x1 = p1_.X();
   double y1 = p1_.Y();
   double x2 = p2_.X();
   double y2 = p2_.Y();
   double dx = x2-x1;
   double dy = y2-y1;
   return dy/dx*(x-x1) + y1;
}

void Line::GetACDF(double& A, double& C, double& D, double& F)
{
   double B;
   double E;
   GetABCDEF(A,B,C,D,E,F);
}

void Line::GetABCDEF(double& A, double& B, double& C, double& D, double& E, double& F)
{
   // Ax + By + C = 0
   // Dy + Ez + F = 0
   // Here B>0 and E>0
   
   double x1 = p1_.X();
   double y1 = p1_.Y();
   double z1 = p1_.Z();
   double x2 = p2_.X();
   double y2 = p2_.Y();
   double z2 = p2_.Z();
   double dx = x2-x1;
   double dy = y2-y1;
   double dz = z2-z1;
   double abs_dx = TMath::Abs(dx);
   double abs_dy = TMath::Abs(dy);
   if (abs_dx>0.0001) {
      B = 1.0/TMath::Sqrt((dy/dx)*(dy/dx)+1);
      A = -dy/dx*B;
      C = -A*x1 - B*y1;
   } else {
      A = 1.0/TMath::Sqrt((dx/dy)*(dx/dy)+1);
      B = -dx/dy*A;
      C = -A*x1 - B*y1;
      if (B<0) {
         A -= A;
         B -= B;
         C -= C;
      }
   }
   // A -> D
   // B -> E
   // C -> F
   // x -> y
   // y -> z
   if (abs_dy>0.0001) {
      E = 1.0/TMath::Sqrt((dz/dy)*(dz/dy)+1);
      D = -dz/dy*E;
      F = -D*y1 - E*z1;
   } else {
      D = 1.0/TMath::Sqrt((dy/dz)*(dy/dz)+1);
      E = -dy/dz*D;
      F = -D*y1 - E*z1;
      if (E<0) {
         D -= D;
         E -= E;
         F -= F;
      }
   }

   if (g_debug_line) {
      printf("x1 %lf y1 %lf z1 %lf\n", x1, y1, z1);
      printf("x2 %lf y2 %lf z2 %lf\n", x2, y2, z2);
      printf("A  %lf B  %lf C  %lf D %lf E %lf F %lf\n", A,B,C,D,E,F);
   }
}

TVector3& Line::GetP1()
{
   return p1_;
}

TVector3& Line::GetP2()
{
   return p2_;
}
