import ROOT
import tdrstyle, CMS_lumi
import argparse

#####################################################
# This script takes the fit_Signal.py workspace and
# update it with a RooMultiPDF containing bkg models
# with their normalizations
#####################################################

#Supress the opening of many Canvas's
ROOT.gROOT.SetBatch(True)   

#bool initialization
isRhoGammaAnalysis = False
isPhiGammaAnalysis = False

#INPUT and OUTPUT #############################################################################################
#Input
p = argparse.ArgumentParser(description='Select rootfile to plot')
p.add_argument('Decay_channel_option', help='Type <<Phi>> for Phi, <<Rho>> for Rho') #flag for bkg estimation
args = p.parse_args()

if args.Decay_channel_option == "Phi":
    isPhiGammaAnalysis = True
    CHANNEL = "Phi"
    print "Z -> PhiGamma analysis"
if args.Decay_channel_option == "Rho":
    isRhoGammaAnalysis = True
    CHANNEL = "Rho"
    print "Z -> RhoGamma analysis"


#CMS-style plotting ---------------------------------------------------------------
tdrstyle.setTDRStyle()
iPeriod = 4
iPos = 11
CMS_lumi.lumiTextSize = 0.7
CMS_lumi.lumiTextOffset = 0.25
CMS_lumi.cmsTextSize = 0.8
#CMS_lumi.cmsTextOffset = 0.4
CMS_lumi.lumi_13TeV = "39.54 fb^{-1}" 

#Parameters of the PDF ---------------------------------------------------------------
mass = ROOT.RooRealVar("ZMass","ZMass",50.,200.,"GeV")
mass.setRange("full",50.,200.)

#Initialize a Chebychev pdf
a_bkg = ROOT.RooRealVar("a_bkg_cheb_"+CHANNEL,"a_bkg",0.,-3.,3.)
b_bkg = ROOT.RooRealVar("b_bkg_cheb_"+CHANNEL,"b_bkg",0.,-3.,3.)
c_bkg = ROOT.RooRealVar("c_bkg_cheb_"+CHANNEL,"c_bkg",0.,-3.,3.)
d_bkg = ROOT.RooRealVar("d_bkg_cheb_"+CHANNEL,"d_bkg",0.,-3.,3.)
e_bkg = ROOT.RooRealVar("e_bkg_cheb_"+CHANNEL,"e_bkg",0.,-3.,3.)

if CHANNEL == "Phi": 
    bkgPDF_chebychev = ROOT.RooChebychev("chebychev_preselection_bkg","bkgPDF",mass,ROOT.RooArgList(a_bkg,b_bkg,c_bkg,d_bkg,e_bkg))
else:
    bkgPDF_chebychev = ROOT.RooChebychev("chebychev_preselection_bkg","bkgPDF",mass,ROOT.RooArgList(a_bkg,b_bkg,c_bkg))
    

#Initialize a Bernstein pdf
bern_c0 = ROOT.RooRealVar('bern_c0', 'bern_c0', 0.2, 0.,1.)
bern_c1 = ROOT.RooRealVar('bern_c1', 'bern_c1', 0.1, 0.,1.)
bern_c2 = ROOT.RooRealVar('bern_c2', 'bern_c2', 0.01, 0.,2.)
bern_c3 = ROOT.RooRealVar('bern_c3', 'bern_c3', 0.01, 0.,1.)
bern_c4 = ROOT.RooRealVar('bern_c4', 'bern_c4', 0.01, 0., 1.)
bern_c5 = ROOT.RooRealVar('bern_c5', 'bern_c5', 1e-2, 0., 0.1)

bkgPDF_bernstein = ROOT.RooBernstein("bernstein_preselection_bkg", "bkgPDF", mass, ROOT.RooArgList(bern_c0,bern_c1,bern_c2,bern_c3,bern_c4,bern_c5))


#Initialize a Landau pdf
land_mean = ROOT.RooRealVar('land_mean', 'land_mean', 100., 40.,120.) 
land_sigma = ROOT.RooRealVar('land_sigma', 'land_sigma', 14., 0., 80.) 

bkgPDF_landau = ROOT.RooLandau("landau_bkg", "bkgPDF", mass, land_mean, land_sigma)
#bkgPDF_landau = bkgPDF_chebychev


#Initialize a Gaussian pdf
gaus_mean = ROOT.RooRealVar('gaus_mean', 'gaus_mean', 108., 30., 120.) 
gaus_sigma = ROOT.RooRealVar('gaus_sigma', 'gaus_sigma', 34., 0., 80.) 

bkgPDF_gaus = ROOT.RooGaussian("gaus_bkg", "bkgPDF", mass, gaus_mean, gaus_sigma)


#Input file and tree ---------------------------------------------------------------
if isPhiGammaAnalysis:
    fileInput = ROOT.TFile("histos/latest_productions/CR_Phi_BDT_Sidebands.root")
else :
    fileInput = ROOT.TFile("histos/latest_productions/CR_Rho_BDT_Sidebands.root")
fileInput.cd()
tree = fileInput.Get("tree_output")

#Retrieve observed_data from the tree, insert the variable also ---------------------------------------------------------------
observed_data = ROOT.RooDataSet("observed_data","observed_data",ROOT.RooArgSet(mass),ROOT.RooFit.Import(tree))
nEntries = observed_data.numEntries() 
print "nEntries = ",nEntries


#Do the fit ------------------------------------------------------------------------------------------------------------------------------
convolution_pdf = ROOT.RooFFTConvPdf("convolution", "Landau (X) Gaussian", mass, bkgPDF_landau, bkgPDF_gaus)
#fitResult_landau = bkgPDF_landau.fitTo(observed_data,ROOT.RooFit.Save())
#fitResult_landau = bkgPDF_gaus.fitTo(observed_data,ROOT.RooFit.Save())
fitResult_landau = convolution_pdf.fitTo(observed_data,ROOT.RooFit.Save())
#fitResult_bernstein   = bkgPDF_bernstein.fitTo(observed_data,ROOT.RooFit.Save(),ROOT.RooFit.Verbose())
#Do the F-test ------------------------------------------------------------------------------------------------------------------------------
#print "################## F-TEST"
#print "minNll = ", fitResult_bernstein.minNll()
#print "2Delta_minNll = ", 2*(31446.9134091-fitResult_bernstein.minNll()) # If 2*(NLL(N)-NLL(N+1)) > 3.85 -> N+1 is significant improvement
#print "##################"



#Plot ------------------------------------------------------------------------------------------------------------------------
canvas_landau = ROOT.TCanvas()
canvas_landau.cd()

#Landau frame
if isPhiGammaAnalysis:
    xframe_landau = mass.frame(60)
else:
    xframe_landau = mass.frame(120)

observed_data.plotOn(xframe_landau)
#bkgPDF_landau.plotOn(xframe_landau)
convolution_pdf.plotOn(xframe_landau)
xframe_landau.SetTitle("#sqrt{s} = 13 TeV       lumi = 39.54/fb")
xframe_landau.GetXaxis().SetTitle("m_{ditrk,#gamma} [GeV]")
xframe_landau.SetMaximum(1.3*xframe_landau.GetMaximum())
#bkgPDF_gaus.paramOn(xframe_landau,ROOT.RooFit.Layout(0.65,0.94,0.91),ROOT.RooFit.Format("NEU",ROOT.RooFit.AutoPrecision(1))) #,ROOT.RooFit.Layout(0.65,0.90,0.90)
#bkgPDF_landau.paramOn(xframe_landau,ROOT.RooFit.Layout(0.65,0.94,0.91),ROOT.RooFit.Format("NEU",ROOT.RooFit.AutoPrecision(1))) #,ROOT.RooFit.Layout(0.65,0.90,0.90)
convolution_pdf.paramOn(xframe_landau,ROOT.RooFit.Layout(0.65,0.94,0.91),ROOT.RooFit.Format("NEU",ROOT.RooFit.AutoPrecision(1))) #,ROOT.RooFit.Layout(0.65,0.90,0.90)
xframe_landau.getAttText().SetTextSize(0.02)
xframe_landau.Draw() #remember to draw the frame before the legend initialization to fill the latter correctly


#Calculate Chi square and parameters 
nParam_landau = fitResult_landau.floatParsFinal().getSize()
chi2_landau = xframe_landau.chiSquare()#Returns chi2. Remember to remove the option XErrorSize(0) from data.PlotOn
cut_chi2_landau = "{:.2f}".format(chi2_landau) #Crop the chi2 to 2 decimal digits
print "Chi square landau = ",chi2_landau
print "n param landau = ",nParam_landau
print ""


leg1 = ROOT.TLegend(0.5,0.52,0.72,0.90) #right positioning
leg1.SetHeader(" ")
leg1.SetNColumns(1)
leg1.SetFillColorAlpha(0,0.)
leg1.SetBorderSize(0)
leg1.SetLineColor(1)
leg1.SetLineStyle(1)
leg1.SetLineWidth(1)
leg1.SetFillStyle(1001)
leg1.AddEntry(cut_chi2_landau,"#chi^{2}/ndof = " + cut_chi2_landau + " / " + str(nParam_landau),"brNDC")

leg1.Draw()

CMS_lumi.CMS_lumi(canvas_landau, iPeriod, iPos) #Print integrated lumi and energy information

if isPhiGammaAnalysis:
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Data/Phi/fit_bkg.pdf")
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Data/Phi/fit_bkg.png")
else:
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Data/Rho/fit_bkg.pdf")
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Data/Rho/fit_bkg.png")

'''

#Chebychev frame
canvas_chebychev = ROOT.TCanvas()
canvas_chebychev.cd()

#Chebychev frame
if isPhiGammaAnalysis:
    xframe_chebychev = mass.frame(60)
else:
    xframe_chebychev = mass.frame(120)

data_blinded.plotOn(xframe_chebychev)
bkgPDF_chebychev.plotOn(xframe_chebychev,ROOT.RooFit.NormRange("LowSideband,HighSideband"),ROOT.RooFit.Range("LowSideband,HighSideband"),ROOT.RooFit.Name("bkgPDF_chebychev"),ROOT.RooFit.LineColor(ROOT.kBlue))
xframe_chebychev.SetTitle("#sqrt{s} = 13 TeV       lumi = 39.54/fb")
xframe_chebychev.GetXaxis().SetTitle("m_{ditrk,#gamma} [GeV]")
xframe_chebychev.SetMaximum(1.3*xframe_chebychev.GetMaximum())
bkgPDF_chebychev.paramOn(xframe_chebychev,ROOT.RooFit.Layout(0.45,0.94,0.91),ROOT.RooFit.Format("NEU",ROOT.RooFit.AutoPrecision(1))) #,ROOT.RooFit.Layout(0.65,0.90,0.90)
xframe_chebychev.getAttText().SetTextSize(0.02)
xframe_chebychev.Draw() #remember to draw the frame before the legend initialization to fill the latter correctly

#Calculate Chi square and parameters 
nParam_cheby = fitResult_chebychev.floatParsFinal().getSize()
chi2_cheby = xframe_chebychev.chiSquare()#Returns chi2. Remember to remove the option XErrorSize(0) from data.PlotOn
cut_chi2_cheby = "{:.2f}".format(chi2_cheby) #Crop the chi2 to 2 decimal digits
print "Chi square cheby = ",chi2_cheby
print "n param cheby = ",nParam_cheby
print ""

leg1 = ROOT.TLegend(0.5,0.52,0.72,0.90) #right positioning
leg1.SetHeader(" ")
leg1.SetNColumns(1)
leg1.SetFillColorAlpha(0,0.)
leg1.SetBorderSize(0)
leg1.SetLineColor(1)
leg1.SetLineStyle(1)
leg1.SetLineWidth(1)
leg1.SetFillStyle(1001)
leg1.AddEntry(cut_chi2_cheby,"#chi^{2}/ndof = " + cut_chi2_cheby + " / " + str(nParam_cheby),"brNDC")

leg1.Draw()

CMS_lumi.CMS_lumi(canvas_chebychev, iPeriod, iPos) #Print integrated lumi and energy information

canvas_chebychev.SaveAs("/eos/user/g/gumoret/www/latest_production/massanalysis_latest_production/fit_sidebands_GFpreselection_chebychev.pdf")
canvas_chebychev.SaveAs("/eos/user/g/gumoret/www/latest_production/massanalysis_latest_production/fit_sidebands_GFpreselection_chebychev.png")


#Bernstein frame
canvas_bernstein = ROOT.TCanvas()
canvas_bernstein.cd()

if isPhiGammaAnalysis:
    xframe_bernstein = mass.frame(60)
else:
    xframe_bernstein = mass.frame(120)

data_blinded.plotOn(xframe_bernstein)
bkgPDF_bernstein.plotOn(xframe_bernstein,ROOT.RooFit.NormRange("LowSideband,HighSideband"),ROOT.RooFit.Range("LowSideband,HighSideband"),ROOT.RooFit.Name("bkgPDF_bernstein"), ROOT.RooFit.LineColor(ROOT.kGreen))
xframe_bernstein.SetTitle("#sqrt{s} = 13 TeV       lumi = 39.54/fb")
xframe_bernstein.GetXaxis().SetTitle("m_{ditrk,#gamma} [GeV]")
xframe_bernstein.SetMaximum(1.3*xframe_bernstein.GetMaximum())
bkgPDF_bernstein.paramOn(xframe_bernstein,ROOT.RooFit.Layout(0.45,0.94,0.91),ROOT.RooFit.Format("NEU",ROOT.RooFit.AutoPrecision(1))) #,ROOT.RooFit.Layout(0.65,0.90,0.90)
xframe_bernstein.getAttText().SetTextSize(0.02)
xframe_bernstein.Draw() #remember to draw the frame before the legend initialization to fill the latter correctly

nParam_bern = fitResult_bernstein.floatParsFinal().getSize()
chi2_bern = xframe_bernstein.chiSquare()#Returns chi2. Remember to remove the option XErrorSize(0) from data.PlotOn
cut_chi2_bern = "{:.2f}".format(chi2_bern) #Crop the chi2 to 2 decimal digits
print "Chi square bern = ",chi2_bern
print "n param bern = ",nParam_bern

leg2 = ROOT.TLegend(0.5,0.42,0.72,0.80) #right positioning
leg2.SetHeader(" ")
leg2.SetNColumns(1)
leg2.SetFillColorAlpha(0,0.)
leg2.SetBorderSize(0)
leg2.SetLineColor(1)
leg2.SetLineStyle(1)
leg2.SetLineWidth(1)
leg2.SetFillStyle(1001)
leg2.AddEntry(cut_chi2_bern,"#chi^{2}/ndof = " + cut_chi2_bern + " / " + str(nParam_bern),"brNDC")

leg2.Draw()

CMS_lumi.CMS_lumi(canvas_bernstein, iPeriod, iPos) #Print integrated lumi and energy information

canvas_bernstein.SaveAs("/eos/user/g/gumoret/www/latest_production/massanalysis_latest_production/fit_sidebands_GFpreselection_bernstein.pdf")
canvas_bernstein.SaveAs("/eos/user/g/gumoret/www/latest_production/massanalysis_latest_production/fit_sidebands_GFpreselection_bernstein.png")


'''
'''
# Multipdf ------------------------------------------------------------------------------------------------------------------------------
cat = ROOT.RooCategory("pdf_index","Index of Pdf which is active")
mypdfs = ROOT.RooArgList()
mypdfs.add(bkgPDF_landau)
#mypdfs.add(bkgPDF_chebychev)
#mypdfs.add(bkgPDF_bernstein)


multipdf = ROOT.RooMultiPdf("multipdf_"+CHANNEL+"_bkg","All Pdfs",cat,mypdfs)

#create Workspace ------------------------------------------------------------------------------------------------------------------------------
norm     = nEntries #fileInput.Get("h_InvMass_TwoTrk_Photon").Integral() #get the normalization of ggH signal (area under ggH signal)
print "************************************** n. events = ",nEntries
#bkg_norm = ROOT.RooRealVar(multipdf.GetName()+ "_norm", multipdf.GetName()+ "_norm", norm,0.5*norm, 2*norm)

#workspace = ROOT.RooWorkspace("myworkspace")
#getattr(workspace,'import')(bkgPDF_chebychev)
#getattr(workspace,'import')(bkgPDF_bernstein)

inputWS = ROOT.TFile("workspaces/workspace_STAT_"+CHANNEL+"_GFpreselection_2018.root") #there's only one ws for both ggH and VBF 
inputWS.cd()
workspace = inputWS.Get("workspace_STAT_"+CHANNEL+"_GFpreselection_2018")
#getattr(workspace,'import')(bkgPDF_chebychev)
#getattr(workspace,'import')(bkgPDF_bernstein)
#getattr(workspace,'import')(bkgPDF_exponential)
getattr(workspace,'import')(cat)
getattr(workspace,'import')(multipdf)
getattr(workspace,'import')(observed_data)
getattr(workspace,'import')(bkg_norm)
print("integral BKG",bkg_norm.Print())
#workspace.Print()

#fOut = ROOT.TFile("workspaces/ws_sidebands.root","RECREATE")
fOut = ROOT.TFile("workspaces/workspace_STAT_"+CHANNEL+"_GFpreselection_2018.root","UPDATE")
fOut.cd()
workspace.Write()
#print "-------------------------------------------"
#print "Final print to check the workspace update:"
#workspace.Print()

fOut.Close()

'''
