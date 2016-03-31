#include "TrackFitter.h"
#include "TMath.h"

static TrackFitter *TrackFitter_obj;

TrackFitter::TrackFitter(Track track, bool verbose)
{
   track_ = track;

   fit_num_params_ = 4;
   minuit_ = new TFitter(fit_num_params_);
   TrackFitter_obj = this;

   if (!verbose) {
      Int_t ierr=0;
      gMinuit->SetPrintLevel(-1); 
   //   gMinuit->mnexcm("SET NOWarnings",0,0,ierr);
   }

   minuit_->SetFCN(TrackFitter::MinuitFunction);
}

void TrackFitter::DoFit()
{
   Line& ini_line = track_.GetIniLine();
   ini_line.GetACDF(ini_pars_[0], ini_pars_[1], ini_pars_[2], ini_pars_[3]);

   double A_ini = ini_pars_[0];
   double C_ini = ini_pars_[1];
   double D_ini = ini_pars_[2];
   double F_ini = ini_pars_[3];
   double A_step = 1.0;
   double C_step = 1.0;
   double D_step = 1.0;
   double F_step = 1.0;
   minuit_->SetParameter(0,"A",A_ini,A_step,-1,1);
   minuit_->SetParameter(1,"C",C_ini,C_step, 0,0);
   minuit_->SetParameter(2,"D",D_ini,D_step,-1,1);
   minuit_->SetParameter(3,"F",F_ini,F_step, 0,0);

   //double arglist[10];
   //arglist[0] = 0;
   //minuit_->ExecuteCommand("SET PRINT",arglist,1);
   minuit_->ExecuteCommand("SIMPLEX",0,0);
   minuit_->ExecuteCommand("MIGRAD",0,0);

   // get result
   for (int i=0; i<fit_num_params_; i++) {
      fit_min_pars_[i] = minuit_->GetParameter(i);
      fit_err_pars_[i] = minuit_->GetParError(i);
   }
   minuit_->GetStats(fit_chi2_,fit_edm_,fit_errdef_,fit_nvpar_,fit_nparx_);

   fit_line_.MakeLine(
         fit_min_pars_[0],
         fit_min_pars_[1],
         fit_min_pars_[2],
         fit_min_pars_[3]);
   //printf("=== fit_chi2 %f\n", fit_chi2_);
#if 0
   double a,b,c,d,e,f;
   fit_line_.GetABCDEF(a,b,c,d,e,f);
   printf("A) %f %f %f %f %f %f\n", a,b,c,d,e,f);
   double A = fit_min_pars_[0];
   double B = TMath::Sqrt(1-A*A);
   double C = fit_min_pars_[1];
   double D = fit_min_pars_[2];
   double E = TMath::Sqrt(1-D*D);
   double F = fit_min_pars_[3];
   fit_line_.MakeLine(A,B,C,D,E,F);
   printf("B) %f %f %f %f %f %f\n", A,B,C,D,E,F);
   printf("make line after fitting\n");
   fit_line_.PrintLine();
#endif

#if 0
   // extend line from cid=1 to cid=7 or 8 for drawing purpose
   int cidA=1;
   int cidB=7;
   if (hits_[8].UseByFit()) cidB=8;

   Hit& hitA = hits_[cidA];
   Hit& hitB = hits_[cidB];
   Line& wireA = wiremap.GetWire(cidA, hitA.GetCellNumber());
   Line& wireB = wiremap.GetWire(cidB, hitB.GetCellNumber());
   TVector3 pAA;
   TVector3 pBA;
   TVector3 pAB;
   TVector3 pBB;
   fit_line_.GetClosestPoints(wireA, pAA, pBA);
   fit_line_.GetClosestPoints(wireB, pAB, pBB);
   Line newline;
   newline.MakeLine(pAA, pAB);
   //printf("pA1 "); pA1.Print();
   //printf("pA7 "); pA7.Print();
   //double dx1 = ini_line_.GetP2().X()- ini_line_.GetP1().X();
   //double dy1 = ini_line_.GetP2().Y()- ini_line_.GetP1().Y();
   //double dz1 = ini_line_.GetP2().Z()- ini_line_.GetP1().Z();
   //double dx2 = fit_line_.GetP2().X()- fit_line_.GetP1().X();
   //double dy2 = fit_line_.GetP2().Y()- fit_line_.GetP1().Y();
   //double dz2 = fit_line_.GetP2().Z()- fit_line_.GetP1().Z();
   //printf("dx1 %f dy1 %f dz1 %f dy1/dx1 %f dy1/dx1 %f\n", dx1, dy1, dz1, dy1/dx1, dy1/dz1);
   //printf("dx2 %f dy2 %f dz1 %f dy2/dx2 %f dy1/dx1 %f\n", dx2, dy2, dz2, dy2/dx2, dy2/dz2);
   fit_line_ = newline;

   //printf("comapre with iniline and fitline\n");
   //ini_line_.PrintLine();
   //fit_line_.PrintLine();

   // apply hits
   SetHitZWithLine(wiremap, fit_line_);
#endif
}

void TrackFitter::GetIniParams(double* ini_pars)
{
   for (int i=0; i<fit_num_params_; i++) {
      ini_pars[i] = ini_pars_[i];
   }
}

void TrackFitter::GetFitParams(double* fit_pars)
{
   for (int i=0; i<fit_num_params_; i++) {
      fit_pars[i] = fit_min_pars_[i];
   }
}

double TrackFitter::GetChi2()
{
   return fit_chi2_;
}

double TrackFitter::GetRedChi2()
{
   return fit_chi2_/fit_num_params_;
}

int TrackFitter::GetNumFitParams()
{
   return fit_num_params_;
}

int TrackFitter::GetNumHits()
{
   return track_.GetHitList().GetNumHits();
}

int TrackFitter::GetNDF()
{
   return GetNumHits() - GetNumFitParams();
}

void TrackFitter::PrintFitResults()
{
   char ch = 'A';
   for (int i=0; i<fit_num_params_; i++) {
      if (i==0) ch = 'A';
      if (i==1) ch = 'C';
      if (i==2) ch = 'D';
      if (i==3) ch = 'F';
      if (i==4) ch = 'T';
      printf("[%c] ini_par %8.2f => fit_par %8.2f +/- %8.2f\n", ch, ini_pars_[i], fit_min_pars_[i], fit_err_pars_[i]);
   }
   printf("fit_chi2_ / ndf  = %f (%f / %d) nhits %d nparam %d\n", fit_chi2_/GetNDF(), fit_chi2_, GetNDF(), GetNumHits(), GetNumFitParams());
}

void TrackFitter::MinuitFunction(int& nDim, double* gout, double& result, double par[], int flg)
{
   // Line in 3D space is described in the following
   // Ax + By + C = 0; normal to vector (A,B) and distance from origin (x,y)=(0,0) is C/sqrt(A*A+B*B)
   // Dy + Ez + F = 0; normal to vector (D,E) and distance from origin (y,z)=(0,0) is F/sqrt(D*D+E*E)
   // vector (A,B) and (D,E) is normalized
   
   double A = par[0];
   double C = par[1];
   double D = par[2];
   double F = par[3];
   Line line;
   line.MakeLine(A,C,D,F);
   result = TrackFitter_obj->track_.GetChi2(line);
}

Track& TrackFitter::GetTrack()
{
   return track_;
}

Line& TrackFitter::GetFitLine()
{
   return fit_line_;
}

