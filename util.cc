#include "util.h"
#include "TMath.h"

void CalcPointsOnTangentials(
      double x2, double r1, double r2,
      double& xR1,
      double& yR1,
      double& xL1,
      double& yL1,
      double& xR2,
      double& yR2,
      double& xL2,
      double& yL2)
{
   xL1 = (r1*r1 + r1*r2)/x2;
   yL1 = TMath::Sqrt(r1*r1 - (r1*r1+r1*r2)/x2*(r1*r1+r1*r2)/x2);
   xR1 = ((x2*x2-r2*r2-r1*r2)/x2);
   yR1 = -TMath::Sqrt(r2*r2 - (-r2*r2-r1*r2)/x2*(-r2*r2-r1*r2)/x2);
   xL2 = (r1*r1 - r1*r2)/x2;
   yL2 = TMath::Sqrt(r1*r1 - (r1*r1-r1*r2)/x2*(r1*r1-r1*r2)/x2);
   xR2 = ((x2*x2-r2*r2+r1*r2)/x2);
   yR2 = TMath::Sqrt(r2*r2 - (-r2*r2+r1*r2)/x2*(-r2*r2+r1*r2)/x2);
   //if (g_debug_track>0) {
   //   printf("xR1 %lf yR1 %lf\n", xR1, yR1);
   //   printf("xL1 %lf yL1 %lf\n", xL1, yL1);
   //   printf("xR2 %lf yR2 %lf\n", xR2, yR2);
   //   printf("xL2 %lf yL2 %lf\n", xL2, yL2);
   //}
}

