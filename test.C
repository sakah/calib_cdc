bool g_hit_layers[20] = {true};
int g_hit_region = REGION_ALL;
void set_hit_layers_all()
{
   g_hit_region = REGION_ALL;
   for (int cid=0; cid<20; cid++) {
      g_hit_layers[cid] = true;
   }
}
void set_hit_layers_up_cdc()
{
   g_hit_region = REGION_UP;
   for (int cid=0; cid<20; cid++) {
      g_hit_layers[cid] = true;
   }
}
void set_hit_layers_down_cdc()
{
   g_hit_region = REGION_DOWN;
   for (int cid=0; cid<20; cid++) {
      g_hit_layers[cid] = true;
   }
}
void set_hit_layers_PT4()
{
   g_hit_region = REGION_UP;
   for (int cid=0; cid<19; cid++) {
      g_hit_layers[cid] = true;
   }
   g_hit_layers[0] = false; // inner guard layers
   for (int cid=9; cid<19; cid++) {
      g_hit_layers[cid] = false;
   }
}

TGraphErrors* make_graph_error(int n, double*x, double*y, double*ex, double*ey, char* title, double ymin, double ymax, int color, int width)
{
   TGraphErrors* gr = new TGraphErrors(n, x, y, ex, ey);
   gr->SetTitle(title);
   gr->SetMinimum(ymin);
   gr->SetMaximum(ymax);
   gr->SetMarkerColor(color);
   gr->SetLineColor(color);
   gr->SetLineWidth(width);
   gr->GetXaxis()->SetTitleSize(0.07);
   gr->GetXaxis()->SetTitleOffset(0.6);
   gr->GetYaxis()->SetTitleSize(0.08);
   gr->GetYaxis()->SetTitleOffset(0.3);
   return gr;
}

WireMap* g_calib_wiremap_ptr=NULL;
WireMap* g_real_wiremap_ptr=NULL;
WireMap& get_calib_wiremap()
{
   if (g_calib_wiremap_ptr==NULL) {
      g_calib_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", 0, 0, 0, RAND_FIX);
   }
   return *g_calib_wiremap_ptr;
}
WireMap& get_real_wiremap()
{
   if (g_real_wiremap_ptr==NULL) {
      //double xoff_um = 100.0; 
      //double yoff_um = 100.0; 
      //double xoff_um = 1000.0;  // 1mm
      //double yoff_um = 1000.0;  // 1mm
      double xoff_um = 1000.0;  // 1mm
      double yoff_um = 1000.0;  // 1mm
      double zoff_um = 0.0;
      int uniform_or_fix = RAND_FIX;
      //int uniform_or_fix = RAND_UNIFORM;
      g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);
   }
   return *g_real_wiremap_ptr;
}

void drawall(int real_or_calib, double arg_z, int color)
{
   //TCanvas *c1 = new TCanvas(cname);
   //c1->DrawFrame(-100, -100, 100, 100);

   WireMap* wiremap_ptr = NULL;
   if (real_or_calib==0) {
      get_real_wiremap();
      wiremap_ptr = g_real_wiremap_ptr;
   } else {
      get_calib_wiremap();
      wiremap_ptr = g_calib_wiremap_ptr;
   }

   int nlayers = wiremap_ptr->GetNumSenseLayers();
   for (int cid=0; cid<nlayers; cid++) {

      if (g_hit_layers[cid]==false) {
         continue;
      }

      int ncells = wiremap_ptr->GetNumCells(cid);
      double z = arg_z;
      if (arg_z<-999) {
         z = wiremap_ptr->GetZHV(cid);
      } else if (z>999) {
         z = wiremap_ptr->GetZRO(cid);
      }
      //printf("--> cid %d z %f\n", cid, z);
      for (int icell=0; icell<ncells; icell++) {
         TVector3 p = wiremap_ptr->GetWire(cid, icell).GetPosAtZ(z);
         if (icell==0) printf("p: %f %f %f (z %f)\n", p.X(), p.Y(), p.Z(), z);
         wiremap_ptr->GetWire(cid,icell).DrawPoint(z, 20, color, 0.5);
      }
   }
}

void drawone(double arg_z, int icell)
{
   WireMap& wiremap = get_calib_wiremap();

   int nlayers = wiremap.GetNumSenseLayers();
   for (int cid=0; cid<nlayers; cid++) {
      double z = arg_z;
      if (arg_z<-999) {
         z = wiremap.GetZHV(cid);
      } else if (z>999) {
         z = wiremap.GetZRO(cid);
      }
      wiremap.GetWire(cid,icell).DrawPoint(z, 20, kRed, 0.5);
   }
}

void hitlist(double p1x, double p1y, double p1z, double p2x, double p2y, double p2z)
{
   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();

   double sigmaR = 0.02;
   Line line;
   line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));
   HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, line, sigmaR);
   hitlist.PrintHitList();

   //drawall(p1z);
   TCanvas *c1 = new TCanvas("c1");
   c1->DrawFrame(-100, -100, 100, 100);
   hitlist.DrawHitList();
   line.DrawLine(kMagenta);
}

void fit_calib(int test_cid, int test_cid_region, double p1x, double p1y, double p1z, double p2x, double p2y, double p2z)
{
   double sigmaR = 0.02;
   Line ini_line;
   ini_line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));

   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();
   HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR);
   HitList hitlist_for_track(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR, test_cid, test_cid_region);

   printf("---  hits ----\n");
   hitlist.PrintHitList();
   printf("---  hits for track ----\n");
   hitlist_for_track.PrintHitList();

   Track track(hitlist_for_track);
   track.SetIniLine(ini_line);

   TrackFitter fitter(track);
   fitter.DoFit();
   fitter.PrintFitResults();
   Line& fit_line = fitter.GetFitLine();
   track.SetFitLine(fit_line);

   //TCanvas *c1 = new TCanvas(cname);
   //c1->DrawFrame(-100, -100, 100, 100);
   //c1->DrawFrame(-2, 50, 2, 70);
   track.DrawTrack();

   HitList hitlist_calib(g_hit_layers, g_hit_region, calib_wiremap, calib_wiremap, ini_line, sigmaR);
   Hit& test_hit_calib = hitlist_calib.GetHit(test_cid, test_cid_region);
   int test_icell = test_hit_calib.GetCellNumber();
   // double hitR = test_hit.GetHitRSmeared();
   double hitX = test_hit_calib.GetWireCalib().GetDistanceSigned(ini_line);
   double fitX = test_hit_calib.GetWireCalib().GetDistanceSigned(fit_line);
   double resX = fitX-hitX;
   printf("p1: %lf %lf %lf p2: %lf %lf %lf hitX %f fitX %f resX %f\n", p1x, p1y, p1z, p2x, p2y, p2z, hitX, fitX, resX);
   test_hit_calib.DrawHit(2);
}

void fit_real(int test_cid, int test_cid_region, double p1x, double p1y, double p1z, double p2x, double p2y, double p2z)
{
   double sigmaR = 0.02;
   Line ini_line;
   ini_line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));

   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();
   HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, real_wiremap, ini_line, sigmaR);
   HitList hitlist_for_track(g_hit_layers, g_hit_region, real_wiremap, real_wiremap, ini_line, sigmaR, test_cid, test_cid_region);

   printf("---  hits ----\n");
   hitlist.PrintHitList();
   printf("---  hits for track ----\n");
   hitlist_for_track.PrintHitList();

   Track track(hitlist_for_track);
   track.SetIniLine(ini_line);

   TrackFitter fitter(track);
   fitter.DoFit();
   fitter.PrintFitResults();
   Line& fit_line = fitter.GetFitLine();
   track.SetFitLine(fit_line);

   //TCanvas *c1 = new TCanvas(cname);
   //c1->DrawFrame(-100, -100, 100, 100);
   //c1->DrawFrame(-2, 50, 2, 70);
   track.DrawTrack();
}

void fits(int test_cid, int test_cid_region, double xmin, double xmax, double ymin, double ymax)
{
   double p1x = gRandom->Uniform(-15, 15);
   double p1y = 100.0;
   double p1z = gRandom->Uniform(0., 0.);
   double p2x = gRandom->Uniform(-15, 15);
   double p2y = -100.0;
   double p2z = gRandom->Uniform(0., 0.);

   double sigmaR = 0.02;
   Line ini_line;
   ini_line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));

   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();
   HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR);
   HitList hitlist_for_track(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR, test_cid, test_cid_region);

   Hit& test_hit = hitlist.GetHit(test_cid, test_cid_region);
   int test_icell = test_hit.GetCellNumber();

   printf("---  test_hit ----\n");
   test_hit.PrintHit();
   printf("---  hits ----\n");
   hitlist.PrintHitList();
   printf("---  hits for track ----\n");
   hitlist_for_track.PrintHitList();

   Track track(hitlist_for_track);
   track.SetIniLine(ini_line);

   TrackFitter fitter(track, false);
   fitter.DoFit();
   fitter.PrintFitResults();
   Line& fit_line = fitter.GetFitLine();
   track.SetFitLine(fit_line);

   TCanvas* c1_fits = new TCanvas("c1", "", 500, 500);
   c1_fits->DrawFrame(xmin,ymin,xmax,ymax);
   track.DrawTrack();
   test_hit.DrawHit(2);
   track.DrawPossibleWireLine(test_hit);
}

void draw_possible_wire_lines(int test_cid, int test_cid_region, int num_tracks)
{
   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();

   TCanvas* c1_fits = new TCanvas("c1", "", 500, 500);
   c1_fits->DrawFrame(-0.8, -0.8, 0.8, 0.8);

   for (int itrack=0; itrack<num_tracks; itrack++) {
      double p1x = gRandom->Uniform(-15, 15);
      double p1y = 100.0;
      double p1z = gRandom->Uniform(0., 0.);
      double p2x = gRandom->Uniform(-15, 15);
      double p2y = -100.0;
      double p2z = gRandom->Uniform(0., 0.);

      double sigmaR = 0.02;
      Line ini_line;
      ini_line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));

      HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR);
      HitList hitlist_for_track(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR, test_cid, test_cid_region);

      Hit& test_hit = hitlist.GetHit(test_cid, test_cid_region);
      int test_icell = test_hit.GetCellNumber();

      //printf("---  test_hit ----\n");
      //test_hit.PrintHit();
      //printf("---  hits ----\n");
      //hitlist.PrintHitList();
      //printf("---  hits for track ----\n");
      //hitlist_for_track.PrintHitList();

      Track track(hitlist_for_track);
      track.SetIniLine(ini_line);

      TrackFitter fitter(track, false);
      fitter.DoFit();
      //fitter.PrintFitResults();
      Line& fit_line = fitter.GetFitLine();
      track.SetFitLine(fit_line);

      //track.DrawTrack();
      //test_hit.DrawHit(2);
      //track.DrawPossibleWireLine(test_hit);
      
      track.DrawPossibleWireLineInCell(test_hit);
   }
}

void fill_content(TH2F* h2, Line& possible_line_in_cell)
{
   int num_xbins = h2->GetXaxis()->GetNbins();
   for (int ixbin=0; ixbin<num_xbins; ixbin++) {
      double x_um = h2->GetXaxis()->GetBinCenter(ixbin);
      double x_cm = x_um*1e-4;
      double y_cm = possible_line_in_cell.GetYAtX(x_cm);
      double y_um = y_cm*1e+4;
      //printf("ixbin %d x %f (um) y %f (um)\n", ixbin, x_um, y_um);
      h2->Fill(x_um, y_um);
   }
}

TH2F* get_possible_wire_pos(int test_cid, int test_cid_region, int num_tracks)
{
   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();

   // seach the region, 100umx100um around original wire center
   double width = 100; // +/- 50um
   int num_bins = 1000.0;
   double bin_width = width/num_bins;
   TH2F* h2 = new TH2F("h2","", 
         num_bins, -width/2.0 - bin_width/2.0, width/2.0 - bin_width/2.0, 
         num_bins, -width/2.0 - bin_width/2.0, width/2.0 - bin_width/2.0);

   h2->SetMarkerStyle(20);

   for (int itrack=0; itrack<num_tracks; itrack++) {
      double p1x = gRandom->Uniform(-15, 15);
      double p1y = 100.0;
      double p1z = gRandom->Uniform(0., 0.);
      double p2x = gRandom->Uniform(-15, 15);
      double p2y = -100.0;
      double p2z = gRandom->Uniform(0., 0.);

      double sigmaR = 0.02;
      Line ini_line;
      ini_line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));

      HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR);
      HitList hitlist_for_track(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR, test_cid, test_cid_region);

      Hit& test_hit = hitlist.GetHit(test_cid, test_cid_region);
      int test_icell = test_hit.GetCellNumber();

      Track track(hitlist_for_track);
      track.SetIniLine(ini_line);

      TrackFitter fitter(track, false);
      fitter.DoFit();
      //fitter.PrintFitResults();
      Line& fit_line = fitter.GetFitLine();
      track.SetFitLine(fit_line);

      Line line = track.GetPossibleWireLineInCell(test_hit);

      fill_content(h2, line);
   }

   return h2;
}

TCanvas* draw_canvas(char* cname, double xmin=-2, double xmax=2, double ymin=50, double ymax=70, int set_grid=0)
{
   TCanvas *c1 = new TCanvas(cname);
   //c1->DrawFrame(-100, -100, 100, 100);
   //c1->DrawFrame(-2, 50, 2, 70);
   c1->DrawFrame(xmin,ymin,xmax,ymax);
   c1->SetGrid(set_grid);
   return c1;
}


TH1F* get_residue(int test_cid, int test_cid_region, double sigmaR, int num_tracks, 
      int fix_or_uniform,
      int nbin, double xmin, double xmax,
      double arg_p1x, double arg_p1y, double arg_p1z,
      double arg_p2x, double arg_p2y, double arg_p2z,
      bool verbose=true)
{
   TH1F* h1 = new TH1F("h1", ";hitX-fitX (um);", nbin, xmin, xmax);

   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();

   for (int itrack=0; itrack<num_tracks; itrack++) {
      double p1x;
      double p1y;
      double p1z;
      double p2x;
      double p2y;
      double p2z;
      if (fix_or_uniform==RAND_FIX) {
         p1x = arg_p1x;
         p1y = arg_p1y;
         p1z = arg_p1z;
         p2x = arg_p2x;
         p2y = arg_p2y;
         p2z = arg_p2z;
      } else if (fix_or_uniform==RAND_UNIFORM) {
         p1x = gRandom->Uniform(-1.6, 1.6);
         p1y = 100.0;
         p1z = gRandom->Uniform(0., 0.);
         p2x = gRandom->Uniform(-1.6, 1.6);
         p2y = -100.0;
         p2z = gRandom->Uniform(0., 0.);
      }

      Line ini_line;
      ini_line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));

      HitList hitlist_for_track(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, ini_line, sigmaR, test_cid, test_cid_region);
      if (verbose) {
         printf("--- hitlist ---\n");
         hitlist_for_track.PrintHitList();
      }

      Track track(hitlist_for_track);
      if (verbose) {
         printf("--- track.GetChi2(ini_line) ---\n");
         track.GetChi2(ini_line);
      }

      track.SetIniLine(ini_line);
      if (verbose) {
         track.PrintTrack();
      }

      TrackFitter fitter(track, false);
      fitter.DoFit();
      Line& fit_line = fitter.GetFitLine();
      track.SetFitLine(fit_line);

      if (verbose) {
         printf("--- test_hit ---\nn");
      }

      HitList hitlist_calib(g_hit_layers, g_hit_region, calib_wiremap, calib_wiremap, ini_line, sigmaR);
      Hit& test_hit_calib = hitlist_calib.GetHit(test_cid, test_cid_region);
      int test_icell = test_hit_calib.GetCellNumber();
      // double hitR = test_hit.GetHitRSmeared();
      double hitX = test_hit_calib.GetWireCalib().GetDistanceSigned(ini_line);
      double fitX = test_hit_calib.GetWireCalib().GetDistanceSigned(fit_line);
      double resX = fitX-hitX;

      if (verbose) {
         printf("p1: %lf %lf %lf p2: %lf %lf %lf hitX %f fitX %f resX %f\n", p1x, p1y, p1z, p2x, p2y, p2z, hitX, fitX, resX);
      }

      if (verbose) {
         printf("--fit_result--\n");
         fitter.PrintFitResults();
         printf("--test_hit--\n");
         test_hit_calib.PrintHit();
         printf("--initial params--\n");
         printf("p1: %6.2f %6.2f %6.2f p2: %6.2f %6.2f %6.2f\n", p1x, p1y, p1z, p2x, p2y, p2z);
         printf("--fit_line--\n");
         fit_line.PrintLine();
      }

      h1->Fill(-resX*1e4); // cm ->um
   }
   return h1;
}

void fit1()
{
   double xoff_um = 0;
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_FIX;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);
   draw_canvas("fit1");
   fit_real (4, REGION_UP, -0.405225, 100.000000, 0.000000, 0.487323, -100.000000, 0.000000);
   fit_calib(4, REGION_UP, -0.405225, 100.000000, 0.000000, 0.487323, -100.000000, 0.000000);
}

void fit2()
{
   double xoff_um = 1000;
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_FIX;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);

   draw_canvas("fit2");
   fit_real (4, REGION_UP, -0.405225, 100.000000, 0.000000, 0.487323, -100.000000, 0.000000);
   fit_calib(4, REGION_UP, -0.405225, 100.000000, 0.000000, 0.487323, -100.000000, 0.000000);
}

void res2()
{
   double xoff_um = 1000;
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_FIX;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);

   double sigmaR = 0.02;
   int num_tracks = 1;
   bool verbose = true;
   double xmin = -2000;
   double xmax = +2000;
   int nbin = (xmax-xmin)/10.0;

   TH1F* h1 = get_residue(4, REGION_UP, sigmaR, num_tracks, uniform_or_fix, 
         nbin, xmin,xmax,
         -0.405225, 100.000000, 0.000000, 0.487323, -100.000000, 0.000000, verbose);
}

void fit3()
{
   double xoff_um = 1000;
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_FIX;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);

   draw_canvas("fit3");
   fit_real (4, REGION_UP, +0.405225, 100.000000, 0.000000, -0.487323, -100.000000, 0.000000);
   fit_calib(4, REGION_UP, +0.405225, 100.000000, 0.000000, -0.487323, -100.000000, 0.000000);
}

void res_test_fix(int test_cid, double sigmaR=0.02, double xoff_um=1000, int num_tracks=10000)
{
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_FIX;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);

   bool verbose = true;
   double xmin;
   double xmax;
   if (xoff_um>0) {
      xmin = -1.5*xoff_um;
      xmax = 1.5*xoff_um;
   } else {
      xmin = 1.5*xoff_um;
      xmax = -1.5*xoff_um;
   }
   xmin = -2000;
   xmax = +2000;
   int nbin = (xmax-xmin)/10.0;
   TH1F* h1 = get_residue(test_cid, REGION_UP, sigmaR, num_tracks, RAND_UNIFORM, nbin, xmin,xmax, 0,0,0,0,0,0,false);
   h1->Draw();
}

void res_test_uniform(int test_cid, double sigmaR=0.02, double xoff_um=1000, int num_tracks=10000)
{
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_UNIFORM;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);

   bool verbose = true;
   double xmin;
   double xmax;
   if (xoff_um>0) {
      xmin = -1.5*xoff_um;
      xmax = 1.5*xoff_um;
   } else {
      xmin = 1.5*xoff_um;
      xmax = -1.5*xoff_um;
   }
   xmin = -2000;
   xmax = +2000;
   int nbin = (xmax-xmin)/10.0;
   TH1F* h1 = get_residue(test_cid, REGION_UP, sigmaR, num_tracks, RAND_UNIFORM, nbin,xmin, xmax, 0,0,0,0,0,0,false);
   h1->Draw();
}

void residue_vs_layer(const char* cname, double xoff_um, double sigmaR, int num_tracks, double ymin_mean, double ymax_mean, double ymin_sigma, double ymax_sigma, char* pdf_name=NULL)
{
   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_UNIFORM;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);

   int gr_num=0;
   double gr_x[100];
   double gr_y_mean[100];
   double gr_y_sigma[100];
   double gr_x_err[100];
   double gr_y_mean_err[100];
   double gr_y_sigma_err[100];
   for (int test_cid=0; test_cid<19; test_cid++) {
      if (g_hit_layers[test_cid]==false) {
         continue;
      }
      bool verbose = false;
      double xmin = -2000;
      double xmax = +2000;
      int nbin = (xmax-xmin)/10.0;
      TH1F *h1 = get_residue(test_cid, REGION_UP, sigmaR, num_tracks, RAND_UNIFORM, nbin, xmin,xmax, 0,0,0,0,0,0,verbose);
      gr_x[gr_num] = test_cid;
      h1->Fit("gaus");
      TF1* f1 = h1->GetFunction("gaus");
      gr_y_mean[gr_num] = f1->GetParameter(1); // um
      gr_y_sigma[gr_num] = f1->GetParameter(2); // um
      gr_x_err[gr_num] = 0.0;
      gr_y_mean_err[gr_num] = f1->GetParError(1); //um
      gr_y_sigma_err[gr_num] = f1->GetParError(2); //um
      printf("gr: num %d x %f y_mean %f\n", gr_num, gr_x[gr_num], gr_y_mean[gr_num]);
      gr_num++;
   }

   TCanvas* c1 = new TCanvas(cname);
   c1->Divide(1,2);
   c1->cd(1)->SetGrid();
   c1->cd(2)->SetGrid();
   char* title_mean  = Form("Mean of residue @ xoff %3.0f (um), sigmaR %3.0f (um), %d tracks; Layer Number; Mean (um)", xoff_um, sigmaR*1e4, num_tracks);
   char* title_sigma = Form("Sigma of residue @ xoff %3.0f (um), sigmaR %3.0f (um), %d tracks; Layer Number; Sigma (um)", xoff_um, sigmaR*1e4, num_tracks);

   TGraphErrors* gr_mean = make_graph_error(gr_num, gr_x, gr_y_mean, gr_x_err, gr_y_mean_err, title_mean, ymin_mean, ymax_mean, kRed, 2);
   TGraphErrors* gr_sigma = make_graph_error(gr_num, gr_x, gr_y_sigma, gr_x_err, gr_y_sigma_err, title_sigma, ymin_sigma, ymax_sigma, kBlue, 2);

   c1->cd(1); gr_mean->Draw("apl");
   c1->cd(2); gr_sigma->Draw("apl");
   if (pdf_name) {
      c1->Print(pdf_name);
   }
}

void draw_all_cdc_residue_vs_layer(const char* cname, double xoff_um, double sigmaR, int num_tracks, char* pdf_name)
{
   set_hit_layers_all();

   double ymin_mean = -50;
   double ymax_mean = +50;
   double ymin_sigma = 0;
   double ymax_sigma = 100;

   residue_vs_layer(cname, xoff_um, sigmaR, num_tracks, ymin_mean, ymax_mean, ymin_sigma, ymax_sigma, pdf_name);
}

void draw_up_cdc_residue_vs_layer(const char* cname, double xoff_um, double sigmaR, int num_tracks, char* pdf_name)
{
   set_hit_layers_up_cdc();

   double ymin_mean = -50;
   double ymax_mean = +50;
   double ymin_sigma = 0;
   double ymax_sigma = 100;

   residue_vs_layer(cname, xoff_um, sigmaR, num_tracks, ymin_mean, ymax_mean, ymin_sigma, ymax_sigma, pdf_name);
}

void check_offset(int marker_color=kBlack, double xoff_um=1000, double xmin=-0.5, double xmax=0.5, double ymin=45, double ymax=85)
{
   set_hit_layers_all();

   double yoff_um = 0;
   double zoff_um = 0;
   int uniform_or_fix = RAND_UNIFORM;
   g_real_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", xoff_um, yoff_um, zoff_um, uniform_or_fix);
   g_calib_wiremap_ptr = new WireMap("B1T_square_1.5m_ver_2015_12_23.txt", 0, 0, 0, RAND_FIX);

   WireMap& calib_wiremap = get_calib_wiremap();
   WireMap& real_wiremap = get_real_wiremap();

   double p1x = 0.5;
   double p1y = 100;
   double p1z = 0;
   double p2x = 0.5;
   double p2y = -100;
   double p2z = 0;

   double sigmaR = 0.02;
   Line line;
   line.MakeLine(TVector3(p1x, p1y, p1z), TVector3(p2x, p2y, p2z));
   HitList hitlist(g_hit_layers, g_hit_region, real_wiremap, calib_wiremap, line, sigmaR);
   hitlist.PrintHitList();

   //drawall(p1z);
   //TCanvas *c1 = new TCanvas("c1");
   //c1->SetGrid();
   //c1->DrawFrame(xmin, ymin, xmax, ymax);
   hitlist.DrawHitList(false, marker_color);
   //line.DrawLine(kMagenta);
}

void draw_offset_all()
{
   TCanvas* c1 = draw_canvas("c1", -100, 100, -100, 100, 1);
   check_offset(kBlack, 0);
   check_offset(kRed, 50);
   check_offset(kBlue, 100);
   check_offset(kMagenta, 150);
   check_offset(kGreen, 200);

   // inner/outer wall
   TEllipse* e1 = new TEllipse(0, 0, 50);
   e1->SetFillStyle(4001);
   e1->SetLineColor(kBlue);
   e1->Draw();

   TEllipse* e2 = new TEllipse(0, 0, 85);
   e2->SetFillStyle(4001);
   e2->SetLineColor(kBlue);
   e2->Draw();

   c1->Print("offset_all.pdf");
}
void draw_offset_up1()
{
   TCanvas* c1 = draw_canvas("c1", -0.02, 0.02, 50, 85, 1);
   check_offset(kBlack, 0);
   check_offset(kRed, 50);
   check_offset(kBlue, 100);
   check_offset(kMagenta, 150);
   check_offset(kGreen, 200);
   c1->Print("offset_up1.pdf");
}

void draw_offset_up2()
{
   TCanvas* c1 = draw_canvas("c1", -0.02+0.83, 0.02+0.83, 45, 90, 1);
   //TCanvas* c1 = draw_canvas("c1", -10, 10, 50, 90, 1);
   check_offset(kBlack, 0);
   check_offset(kRed, 50);
   check_offset(kBlue, 100);
   check_offset(kMagenta, 150);
   check_offset(kGreen, 200);
   c1->Print("offset_up2.pdf");
}

void draw_offset_down1()
{
   TCanvas* c1 = draw_canvas("c1", -0.02, 0.02, -85, -50, 1);
   check_offset(kBlack, 0);
   check_offset(kRed, 50);
   check_offset(kBlue, 100);
   check_offset(kMagenta, 150);
   check_offset(kGreen, 200);
   c1->Print("offset_down1.pdf");
}

void draw_offset_down2()
{
   TCanvas* c1 = draw_canvas("c1", -0.02+0.83, 0.02+0.83, -85, -50, 1);
   check_offset(kBlack, 0);
   check_offset(kRed, 50);
   check_offset(kBlue, 100);
   check_offset(kMagenta, 150);
   check_offset(kGreen, 200);
   c1->Print("offset_down2.pdf");
}

void draw_all_cdc()
{
   draw_all_cdc_residue_vs_layer("all",   0, 0.01, 1000, "all_xoff_0um_sigmaR_0.01_ntracks_1000.pdf");
   draw_all_cdc_residue_vs_layer("all",  50, 0.01, 1000, "all_xoff_50um_sigmaR_0.01_ntracks_1000.pdf");
   draw_all_cdc_residue_vs_layer("all", 100, 0.01, 1000, "all_xoff_100um_sigmaR_0.01_ntracks_1000.pdf");
   draw_all_cdc_residue_vs_layer("all", 150, 0.01, 1000, "all_xoff_150um_sigmaR_0.01_ntracks_1000.pdf");
   draw_all_cdc_residue_vs_layer("all", 200, 0.01, 1000, "all_xoff_200um_sigmaR_0.01_ntracks_1000.pdf");
}

void draw_up_cdc()
{
   draw_up_cdc_residue_vs_layer("up",    0, 0.01, 1000, "up_xoff_0um_sigmaR_0.01_ntracks_1000.pdf");
   draw_up_cdc_residue_vs_layer("up",   50, 0.01, 1000, "up_xoff_50um_sigmaR_0.01_ntracks_1000.pdf");
   draw_up_cdc_residue_vs_layer("up",  100, 0.01, 1000, "up_xoff_100um_sigmaR_0.01_ntracks_1000.pdf");
   draw_up_cdc_residue_vs_layer("up",  150, 0.01, 1000, "up_xoff_150um_sigmaR_0.01_ntracks_1000.pdf");
   draw_up_cdc_residue_vs_layer("up",  200, 0.01, 1000, "up_xoff_200um_sigmaR_0.01_ntracks_1000.pdf");
}
