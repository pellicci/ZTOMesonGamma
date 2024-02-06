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
mass = ROOT.RooRealVar("ZMass","ZMass",70.,200.,"GeV")#70 200
mass.setRange("full",70.,200.)


#Initialize a Landau pdf
land_mean = ROOT.RooRealVar('land_mean', 'land_mean', 103., 95.,110.) 
land_sigma = ROOT.RooRealVar('land_sigma', 'land_sigma', 13., 10., 15.) 
bkgPDF_landau = ROOT.RooLandau("landau_bkg", "bkgPDF", mass, land_mean, land_sigma)

land_add_mean = ROOT.RooRealVar('land_mean', 'land_mean', 103., 95.,110.) 
land_add_sigma = ROOT.RooRealVar('land_sigma', 'land_sigma', 13., 10., 15.) 
bkgPDF_landau_add = ROOT.RooLandau("landau_add_bkg", "bkgLandAdd", mass, land_add_mean, land_add_sigma)

#Initialize a Gaussian pdf
gaus_mean = ROOT.RooRealVar('gaus_mean', 'gaus_mean', 35., 20., 130.) 
gaus_sigma = ROOT.RooRealVar('gaus_sigma', 'gaus_sigma', 5., 0., 30.) 
bkgPDF_gaus_add = ROOT.RooGaussian("gaus_bkg", "bkgGaussAdd", mass, gaus_mean, gaus_sigma)

#bkgPDF_landau=bkgPDF_landau

#bkgPDF_landau = ROOT.RooFFTConvPdf("convolution_bkg", "Landau (X) Gaussian", mass, bkgPDF_landau, bkgPDF_gaus)
if not isPhiGammaAnalysis:
    n_1 = ROOT.RooRealVar("n1","number1 of events",20000,0,32000) 
    n_2 = ROOT.RooRealVar("n2","number2 of events",1000,0,32000)
else: 
    n_1 = ROOT.RooRealVar("n1","number1 of events",2500,0,3700) 
    n_2 = ROOT.RooRealVar("n2","number2 of events",500,0,3700)

n=ROOT.RooArgList()
n.add(n_1)
n.add(n_2) 
pdfs=ROOT.RooArgList()
pdfs.add(bkgPDF_landau_add)
pdfs.add(bkgPDF_gaus_add)
add_pdf = ROOT.RooAddPdf("convolution_bkg", "Landau (X) Gaussian", pdfs, n)

#bkgPDF_landau=add_pdf#######

parList = ROOT.RooArgSet(land_add_mean, land_add_sigma, gaus_mean, gaus_sigma)


#Input file and tree ---------------------------------------------------------------
if isPhiGammaAnalysis:
    fileInput = ROOT.TFile("histos/latest_productions/CR_Phi_BDT_SidebandsNorm.root")###########################
else :
    fileInput = ROOT.TFile("histos/latest_productions/CR_Rho_BDT_SidebandsNorm.root")###########################
fileInput.cd()
#tree = fileInput.Get("tree_output")

#prepare a rebinned TH1 for Z mass ---------------------------------------------------------------
#xLowRange  = 75.
#xHighRange = 105.
h_mZ = fileInput.Get("h_ZMass")
if isPhiGammaAnalysis:
     h_mZ.Rebin(2)
'''
if isPhiGammaAnalysis: h_mZ = ROOT.TH1F("h_mZ","h_mZ", 150, 50., 200.)
else: h_mZ =  ROOT.TH1F("h_mZ","h_mZ", 300, 50., 200.)

nentries = tree.GetEntriesFast()
print "nEntries = ",nentries

for jentry in xrange(nentries):
    ientry = tree.LoadTree( jentry )
    if ientry < 0:
        print "break"
        break
    nb = tree.GetEntry(jentry)
    if nb <= 0:
        print "nb < 0"
        continue
    
    h_mZ.Fill(tree.ZMass)

'''

#Retrieve observed_data from the tree, insert the variable also ---------------------------------------------------------------
#observed_data = ROOT.RooDataSet("observed_data","observed_data",ROOT.RooArgSet(mass),ROOT.RooFit.Import(tree))
observed_data = ROOT.RooDataHist("observed_data", "observed_data", ROOT.RooArgList(mass), h_mZ)############
nEntries = observed_data.numEntries() 
print "nEntries = ",nEntries

#recupero l'istogramma della SR
if isPhiGammaAnalysis:
    SR_input = ROOT.TFile("histos/latest_productions/SR_Phi_BDT_Data.root")############
else :
    SR_input = ROOT.TFile("histos/latest_productions/SR_Rho_BDT_Data.root")

#h_mZ_SR = SR_input.Get("tree_output")
h_mZ_SR = SR_input.Get("h_ZMass")
#True_data = ROOT.RooDataSet("True_data","True_data",ROOT.RooArgSet(mass),ROOT.RooFit.Import(h_mZ_SR))
True_data = ROOT.RooDataHist("True_data", "True_data", ROOT.RooArgList(mass), h_mZ_SR)



#Do the fit ------------------------------------------------------------------------------------------------------------------------------
#fitResult_landau = bkgPDF_landau.fitTo(observed_data,ROOT.RooFit.Save())
#fitResult_landau = bkgPDF_gaus.fitTo(observed_data,ROOT.RooFit.Save())
fitResult_landau = bkgPDF_landau.fitTo(observed_data,ROOT.RooFit.Save())
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
bkgPDF_landau.plotOn(xframe_landau,ROOT.RooFit.NormRange("full"),ROOT.RooFit.Range("full"),ROOT.RooFit.Name("bkgPDF_landau"), ROOT.RooFit.LineColor(ROOT.kBlue))
xframe_landau.SetTitle("#sqrt{s} = 13 TeV       lumi = 39.54/fb")
xframe_landau.GetXaxis().SetTitle("m_{ditrk,#gamma} [GeV]")
xframe_landau.SetMaximum(1.3*xframe_landau.GetMaximum())
bkgPDF_landau.paramOn(xframe_landau, ROOT.RooFit.Layout(0.65,0.94,0.91),ROOT.RooFit.Format("NEU",ROOT.RooFit.AutoPrecision(1))) #,ROOT.RooFit.Layout(0.65,0.90,0.90)
xframe_landau.getAttText().SetTextSize(0.02)
xframe_landau.Draw() #remember to draw the frame before the legend initialization to fill the latter correctly

#Calculate Chi square and parameters 
nParam_landau = fitResult_landau.floatParsFinal().getSize()
chi2_landau = xframe_landau.chiSquare()#Returns chi2. Remember to remove the option XErrorSize(0) from data.PlotOn
cut_chi2_landau = "{:.2f}".format(chi2_landau) #Crop the chi2 to 2 decimal digits
print "Chi square convolution = ",chi2_landau
print "n param convolution = ",nParam_landau
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
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Phi/Fit/fit_bkg.pdf")
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Phi/Fit/fit_bkg.png")
else:
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Rho/Fit/fit_bkg.pdf")
    canvas_landau.SaveAs("/eos/user/e/eferrand/ZMesonGamma/CMSSW_10_6_27/src/ZMesonGammaAnalysis/ZTOMesonGamma/plots/Rho/Fit/fit_bkg.png")

'''
# Multipdf ------------------------------------------------------------------------------------------------------------------------------
cat = ROOT.RooCategory("pdf_index","Index of Pdf which is active")
mypdfs = ROOT.RooArgList()
mypdfs.add(bkgPDF_landau)

multipdf = ROOT.RooMultiPdf("multipdf_"+CHANNEL+"_bkg","All Pdfs",cat,mypdfs)
'''
#create Workspace ------------------------------------------------------------------------------------------------------------------------------
norm   = nEntries 
norm1 = h_mZ_SR.Integral(h_mZ_SR.FindBin(70.), h_mZ_SR.FindBin(200.))#ora uso questa
print "************************************** n. events = ",nEntries, "norm = ", norm1
bkg_norm = ROOT.RooRealVar(bkgPDF_landau.GetName()+ "_norm", bkgPDF_landau.GetName()+ "_norm", norm1, 0.5*norm1, 2*norm1)#########

inputWS = ROOT.TFile("workspaces/workspace_"+CHANNEL+".root")  
inputWS.cd()
workspace = inputWS.Get("workspace_"+CHANNEL+"")
#getattr(workspace,'import')(cat)
getattr(workspace,'import')(bkgPDF_landau)
getattr(workspace,'import')(True_data)#passo i dati della signal region
getattr(workspace,'import')(bkg_norm)
print "integral BKG :",bkg_norm.Print()

fOut = ROOT.TFile("workspaces/workspace_"+CHANNEL+".root","UPDATE")
fOut.cd()
workspace.Write()
print "-------------------------------------------"
print "Final print to check the workspace update:"
workspace.Print()
fOut.Close()