#ifndef TRACK_FITTER_H
#define TRACK_FITTER_H

#include "TMinuit.h"
#include "TFitter.h"
#include "track.h"

class TrackFitter
{
   public:
      TrackFitter(Track track, bool verbose=true);
      void   DoFit();
      void   GetIniParams(double* ini_params);
      void   GetFitParams(double* fit_params);
      double GetChi2();
      double GetRedChi2();
      int GetNumHits();
      int GetNumFitParams();
      int GetNDF();
      void PrintFitResults();
      Track& GetTrack();
      Line& GetFitLine();


   private:
      static void MinuitFunction(int& nDim, double* gout, double& result, double par[], int flg);
      TFitter *minuit_;
      Track track_;

      double ini_pars_[5];

      Line fit_line_;
      int fit_num_params_;
      double fit_min_pars_[5];
      double fit_err_pars_[5];
      double fit_chi2_;
      double fit_edm_;
      double fit_errdef_;
      int fit_nvpar_;
      int fit_nparx_;
};

#endif
