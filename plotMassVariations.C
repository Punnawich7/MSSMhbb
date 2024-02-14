void plotMassVariations() {

  TH1::SetDefaultSumw2();
  std::string syst = "JER";
  //Signal region map
  std::multimap<int, int> signalregion = {
        {125, 1},
        {130, 1},
        {140, 1},
        {160, 1},
        {180, 1},
        {200, 1},
        {200, 2},
        {250, 2},
        {300, 2},
        {350, 2},
        {400, 2},
        {400, 3},
        {450, 3},
        {500, 3},
        {600, 3},
        {700, 3}
  };
    
  std::map<int, float> xmin = {
        {1, 120},
        {2, 180},
        {3, 240}
    };
    
  std::map<int, float> xmax = {
        {1, 300},
        {2, 460},
        {3, 800}
    };

  // Loop over mass points
  for (auto iter = signalregion.begin(); iter != signalregion.end(); ++iter) {
    int mass = iter->first;
    int fitrange = iter->second;

    // Create canvas for plotting
    TCanvas* canvas = new TCanvas("canvas", "Signal Mass Variations", 800, 800);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();             
    pad1->cd();               
    
    // Open the root files for the three variations
    TString rootFileNominal = TString::Format("input_doubleCB_SL/signal_m%d_SR%d.root", mass, fitrange);
    TString rootFileUp = TString::Format("input_doubleCB_SL/signal_m%d_SR%d_%sup.root", mass, fitrange,  syst.c_str());
    TString rootFileDown = TString::Format("input_doubleCB_SL/signal_m%d_SR%d_%sdown.root", mass, fitrange,  syst.c_str());

    TFile* fileNominal = TFile::Open(rootFileNominal);
    TFile* fileUp = TFile::Open(rootFileUp);
    TFile* fileDown = TFile::Open(rootFileDown);

    if (!fileNominal || !fileUp || !fileDown) {
      std::cerr << "Error: Could not open root files for mass " << mass << std::endl;
      continue;
    }

    // Get the RooWorkspace objects from the root files
    RooWorkspace* wsNominal = (RooWorkspace*)fileNominal->Get("w");
    RooWorkspace* wsUp = (RooWorkspace*)fileUp->Get("w");
    RooWorkspace* wsDown = (RooWorkspace*)fileDown->Get("w");

    if (!wsNominal || !wsUp || !wsDown) {
      std::cerr << "Error: Could not retrieve RooWorkspace for mass " << mass << std::endl;
      continue;
    }

    // Get the RooDataHist objects
    RooDataHist* dataNominal = (RooDataHist*)wsNominal->data("sig");
    RooDataHist* dataUp = (RooDataHist*)wsUp->data("sig");
    RooDataHist* dataDown = (RooDataHist*)wsDown->data("sig");

    // Get the RooAbsPdf objects
    RooAbsPdf* pdfNominal = wsNominal->pdf("signal_dcb");
    RooAbsPdf* pdfUp = wsUp->pdf("signal_dcb");
    RooAbsPdf* pdfDown = wsDown->pdf("signal_dcb");

    // Plot the nominal data and pdf in black
    RooRealVar mbb("mbb", "mbb", xmin[fitrange], xmax[fitrange]); 
    RooPlot * frame1 = mbb.frame(RooFit::Title(" "));

    dataNominal->plotOn(frame1, RooFit::Name("dataNominal_plot"), RooFit::MarkerColor(kBlack), RooFit::LineColor(kBlack), RooFit::MarkerStyle(20), RooFit::LineWidth(2));
    pdfNominal->plotOn(frame1, RooFit::LineColor(kBlack), RooFit::LineWidth(2));

    // Plot the up variation data and pdf in blue
    dataUp->plotOn(frame1, RooFit::Name("dataUp_plot"),RooFit::MarkerColor(kBlue), RooFit::LineColor(kBlue), RooFit::MarkerStyle(21), RooFit::LineWidth(2));
    pdfUp->plotOn(frame1, RooFit::LineColor(kBlue), RooFit::LineWidth(2));

    // Plot the down variation data and pdf in red
    dataDown->plotOn(frame1, RooFit::Name("dataDown_plot"), RooFit::MarkerColor(kRed), RooFit::LineColor(kRed), RooFit::MarkerStyle(22), RooFit::LineWidth(2));
    pdfDown->plotOn(frame1, RooFit::LineColor(kRed), RooFit::LineWidth(2));

    frame1->GetXaxis()->SetLabelOffset(999);

    frame1->Draw();

    //some cosmetics
    TLatex latex;
    latex.SetTextFont(43);
    latex.SetTextSize(20);
    latex.SetTextAlign(11);
    latex.DrawLatexNDC(gPad->GetLeftMargin(), 1.02 - canvas->GetTopMargin(), "CMS Simulation      Work in progress");
    latex.DrawLatexNDC(gPad->GetLeftMargin() + 0.7, 1.02 - canvas->GetTopMargin(), "(13 TeV)");

    TLegend *  mbb_legend;
    // if (mass <= 300 || mass == 350 ) {
    //   mbb_legend = new TLegend(0.6,0.7,0.8,.85);
    // }
    // else {
    //   mbb_legend = new TLegend(0.15,0.7,0.35,0.85);
    // }
    if (mass <= (xmin[fitrange] + xmax[fitrange])/2.) {
      mbb_legend = new TLegend(0.65,0.7,0.85,.85);
    }
    else {
      mbb_legend = new TLegend(0.20,0.7,0.40,0.85);
    }
    mbb_legend -> SetBorderSize(0);
    mbb_legend -> AddEntry((TObject*)0, Form("m_{A/H} = %d GeV,", mass), "");
    mbb_legend -> AddEntry(frame1->findObject("dataNominal_plot"),"central","lep");
    mbb_legend -> AddEntry(frame1->findObject("dataUp_plot"),Form("%s +1#sigma variation", syst.c_str()),"lep");
    mbb_legend -> AddEntry(frame1->findObject("dataDown_plot"),Form("%s -1#sigma variation", syst.c_str()),"lep");
    mbb_legend -> Draw();

    // canvas->cd(2);
    canvas->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.3);
    pad2->SetGridy();   // horizontal grid
    pad2->Draw();
    pad2->cd();       // pad2 becomes the current pad

    double ratiomin, ratiomax;
    if (mass == 140) {
      ratiomin = 0.0;
      ratiomax = 2.3;
    }
    else {
      ratiomin = 0.5;
      ratiomax = 1.5;
    }

    int n_bins = dataNominal->numEntries();

    TH1F* ratio_nominal = (TH1F*)pdfNominal->createHistogram("histNominal", mbb, RooFit::Binning(n_bins));
    TH1F* ratio_up = (TH1F*)pdfUp->createHistogram("histUp", mbb, RooFit::Binning(n_bins));
    TH1F* ratio_down = (TH1F*)pdfDown->createHistogram("histDown", mbb, RooFit::Binning(n_bins));

    ratio_up->Divide(ratio_nominal);
    ratio_down->Divide(ratio_nominal);

    double binCenter_up[n_bins], binContent_up[n_bins];
    double binCenter_down[n_bins], binContent_down[n_bins];

    for (int i = 0; i < n_bins; ++i) {
      binCenter_up[i] = ratio_up->GetBinCenter(i+1);
      binContent_up[i] = ratio_up->GetBinContent(i+1);

      binCenter_down[i] = ratio_down->GetBinCenter(i+1);
      binContent_down[i] = ratio_down->GetBinContent(i+1);
    }

    TGraph* g_ratio_up = new TGraph(n_bins, binCenter_up, binContent_up);
    TGraph* g_ratio_down = new TGraph(n_bins, binCenter_down, binContent_down);

    g_ratio_up -> SetTitle(" ");
    g_ratio_up -> SetLineColor(kBlue);
    g_ratio_up -> SetLineWidth(2);
    // g_ratio_up -> GetYaxis() -> SetRangeUser(ratio_up->GetMinimum() - ratio_up->GetMinimum()*0.05, ratio_up->GetMaximum() + ratio_up->GetMaximum()*0.05);
    g_ratio_up -> GetYaxis() -> SetRangeUser(ratiomin, ratiomax);
    g_ratio_up -> GetXaxis() -> SetRangeUser(xmin[fitrange], xmax[fitrange]);
  
    g_ratio_up ->GetYaxis()->SetTitle("Down(Up)/Central");
    g_ratio_up ->GetYaxis()->SetTitleSize(20);
    g_ratio_up ->GetYaxis()->SetTitleFont(43);
    g_ratio_up ->GetYaxis()->SetTitleOffset(1.5);
    g_ratio_up ->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    g_ratio_up ->GetYaxis()->SetLabelSize(15);

  // X axis ratio plot settings
    g_ratio_up ->GetXaxis()->SetTitle("M_{12}");
    g_ratio_up ->GetXaxis()->SetLabelOffset(0);
    g_ratio_up ->GetXaxis()->SetTitleSize(20);
    g_ratio_up ->GetXaxis()->SetTitleFont(43);
    g_ratio_up ->GetXaxis()->SetTitleOffset(3.5);
    g_ratio_up ->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    g_ratio_up ->GetXaxis()->SetLabelSize(15);
    
    g_ratio_up -> Draw("AL");

    g_ratio_down -> SetTitle(" ");
    g_ratio_down -> SetLineColor(kRed);
    g_ratio_down -> SetLineWidth(2);
    // g_ratio_up -> GetYaxis() -> SetRangeUser(ratio_up->GetMinimum() - ratio_up->GetMinimum()*0.05, ratio_up->GetMaximum() + ratio_up->GetMaximum()*0.05);
    g_ratio_down -> GetYaxis() -> SetRangeUser(ratiomin, ratiomax);
    g_ratio_down -> GetXaxis() -> SetRangeUser(xmin[fitrange], xmax[fitrange]);
    
    g_ratio_down ->GetYaxis()->SetTitle("Down(Up)/Central");
    g_ratio_down ->GetYaxis()->SetTitleSize(20);
    g_ratio_down ->GetYaxis()->SetTitleFont(43);
    g_ratio_down ->GetYaxis()->SetTitleOffset(1.5);
    g_ratio_down ->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    g_ratio_down ->GetYaxis()->SetLabelSize(15);

  // X axis ratio plot settings
    g_ratio_down ->GetXaxis()->SetTitle("M_{12}");
    g_ratio_down ->GetXaxis()->SetLabelOffset(0);
    g_ratio_down ->GetXaxis()->SetTitleSize(20);
    g_ratio_down ->GetXaxis()->SetTitleFont(43);
    g_ratio_down ->GetXaxis()->SetTitleOffset(3.5);
    g_ratio_down ->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    g_ratio_down ->GetXaxis()->SetLabelSize(15);

    g_ratio_down -> Draw("L same");
    
    // Save the plot in pdf and png formats
    TString outputFileName = TString::Format("plotVariations/%s_m%d_SR%d.pdf", syst.c_str(), mass, fitrange);
    canvas->SaveAs(outputFileName);
    outputFileName.ReplaceAll(".pdf", ".png");
    canvas->SaveAs(outputFileName);

    // Close the root files
    fileNominal->Close();
    fileUp->Close();
    fileDown->Close();
    delete fileNominal;
    delete fileUp;
    delete fileDown;

    // Delete the canvas
    delete canvas;
  }
}
