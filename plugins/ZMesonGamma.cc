#define _USE_MATH_DEFINES
#include <cmath> 
#include <iostream>
using namespace std;

//ROOT includes
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include "Math/VectorUtil.h"
#include <stdlib.h>


#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"

//Vertex inclusions
#include "DataFormats/VertexReco/interface/Vertex.h" 
#include "DataFormats/BeamSpot/interface/BeamSpot.h" 
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

//Electron ID stuff
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

//Photon ID stuff
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

//Parton distribution and QCD scale variations stuff 
#include "FWCore/Framework/interface/Run.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h" //LHE reader
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h" //LHE reader

//#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h" //JEC uncertainties
//#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h" //JEC uncertainties
//#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"

//JEC uncertainties
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
//#include "CondFormats/JetMETObjects/interface/JetResolution.h"
//#include "CondFormats/JetMETObjects/interface/JetResolutionObject.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
//#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

//#include "CondFormats/DataRecord/interface/JetCorrectionsRecord.h"

//#include "CondFormats/JetMETObjects/interface/JetCorrectionsRecord.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

typedef math::XYZTLorentzVector LorentzVector;
 
#include "ZMesonGamma.h"

 
// constructors and destructor
ZMesonGamma::ZMesonGamma(const edm::ParameterSet& iConfig) : 
runningOnData_(iConfig.getParameter<bool>("runningOnData")),
verboseIdFlag_(iConfig.getParameter<bool>("phoIdVerbose"))//,
//effectiveAreas_el_( (iConfig.getParameter<edm::FileInPath>("effAreasConfigFile_el")).fullPath() ),
//effectiveAreas_ph_( (iConfig.getParameter<edm::FileInPath>("effAreasConfigFile_ph")).fullPath() )
{
  packedPFCandidatesToken_            = consumes<std::vector<pat::PackedCandidate> >(edm::InputTag("packedPFCandidates")); 
  prunedGenParticlesToken_            = consumes<std::vector<reco::GenParticle> >(edm::InputTag("prunedGenParticles"));
  offlineSlimmedPrimaryVerticesToken_ = consumes<std::vector<reco::Vertex> > (edm::InputTag("offlineSlimmedPrimaryVertices"));
  pileupSummaryToken_                 = consumes<std::vector<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo"));
  triggerBitsToken_                   = consumes<edm::TriggerResults> (edm::InputTag("TriggerResults","","HLT"));
  photonsMiniAODToken_                = consumes<std::vector<pat::Photon> > (edm::InputTag("slimmedPhotons"));
  slimmedMuonsToken_                  = consumes<std::vector<pat::Muon> >(edm::InputTag("slimmedMuons"));
  electronsMiniAODToken_              = consumes<std::vector<pat::Electron> > (edm::InputTag("slimmedElectrons"));
  slimmedJetsToken_                   = consumes<std::vector<pat::Jet> >(edm::InputTag("slimmedJets"));
  GenInfoToken_                       = consumes<GenEventInfoProduct> (edm::InputTag("generator"));
  slimmedMETsToken_                   = consumes<std::vector<pat::MET> >(edm::InputTag("slimmedMETs"));
  slimmedMETsPuppiToken_              = consumes<std::vector<pat::MET> >(edm::InputTag("slimmedMETsPuppi"));
  rhoToken_                           = consumes<double> (iConfig.getParameter <edm::InputTag>("rho"));



  hEvents = fs->make<TH1F>("hEvents", "Event counting in different steps", 8, 0., 8.);

  nEventsProcessed           = 0;
  nEventsTriggered           = 0;
  nEventsIsPhoton            = 0;
  nEventsIsTwoKaons          = 0;
  nEventsZMatched            = 0;
  nEventsZNotMatched         = 0;
  nEventsMesonPtNotMatched   = 0;
  nEventsBestPairFound       = 0;
  nEventsTrkPtFilter         = 0;
  nEventsPairIsolationFilter = 0;


  debug=false;  //DEBUG datamember 
  verbose=true; 

  hPileup   = fs->make<TH1F>("pileup", "pileup", 75,0,75);

  create_trees();
}

ZMesonGamma::~ZMesonGamma()
{
}

// ------------ method called for each event  ------------
void ZMesonGamma::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::Handle<std::vector<pat::PackedCandidate>  > PFCandidates;
  iEvent.getByToken(packedPFCandidatesToken_, PFCandidates);

  edm::Handle<std::vector<reco::GenParticle>  > genParticles;
  iEvent.getByToken(prunedGenParticlesToken_, genParticles);

  edm::Handle<edm::TriggerResults> triggerBits;
  iEvent.getByToken(triggerBitsToken_, triggerBits);  

  edm::Handle<std::vector<pat::Photon> > slimmedPhotons;
  iEvent.getByToken(photonsMiniAODToken_,slimmedPhotons);

  edm::Handle<std::vector<pat::Muon>  > slimmedMuons;
  iEvent.getByToken(slimmedMuonsToken_, slimmedMuons);

  edm::Handle<std::vector<pat::Electron> > slimmedElectrons;
  iEvent.getByToken(electronsMiniAODToken_,slimmedElectrons);

  edm::Handle<std::vector<pat::Jet > > slimmedJets;
  iEvent.getByToken(slimmedJetsToken_, slimmedJets);

  edm::Handle<std::vector<reco::Vertex > > slimmedPV;
  iEvent.getByToken(offlineSlimmedPrimaryVerticesToken_, slimmedPV);

  edm::Handle<std::vector<pat::MET > > slimmedMETs;
  iEvent.getByToken(slimmedMETsToken_, slimmedMETs);

  edm::Handle<std::vector<pat::MET > > slimmedMETsPuppi;
  iEvent.getByToken(slimmedMETsPuppiToken_, slimmedMETsPuppi);

  edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
  iSetup.get<JetCorrectionsRecord>().get("AK4PFchs",JetCorParColl); 
  JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"];


   nEventsProcessed++; //This will be saved in the output tree, giving the number of processed events
   
  //Retrieve the run number
  if(runningOnData_){
    runNumber = iEvent.id().run();
    eventNumber = iEvent.id().event();
  }


  //*************************************************************//
  //                                                             //
  //-------------------------- Vertices -------------------------//
  //                                                             //
  //*************************************************************//

  //Count the number of vertices and return if there's no vertex
  nPV = 0;
  if(slimmedPV->size()<=0){
    if(verbose) cout<<"No primary vertex found, RETURN."<<endl;
    return;
  }

  for(reco::VertexCollection::const_iterator vtx=slimmedPV->begin();vtx!=slimmedPV->end();++vtx) {
    // check that the primary vertex is not a fake one, that is the beamspot (it happens when no primary vertex is reconstructed)
    if(!vtx->isFake()) {
      nPV++;
    }
  } 


  //*************************************************************//
  //                                                             //
  //--------------------------- Pile Up -------------------------//
  //                                                             //
  //*************************************************************//

  PUWeight = -1.;
  float npT = -1.;

  if(!runningOnData_){
    edm::Handle<std::vector< PileupSummaryInfo>>  PupInfo;
    iEvent.getByToken(pileupSummaryToken_, PupInfo);

    std::vector<PileupSummaryInfo>::const_iterator PVI; 

    for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
      const int BX = PVI->getBunchCrossing();
      if(BX == 0) {
        npT  = PVI->getTrueNumInteractions();
      }
    }

    if(npT == -1) {
      std::cout << "!!!! npT = -1 !!!!" << std::endl;
      abort();
    }

  // Calculate weight using above code
      PUWeight = Lumiweights_.weight(npT);

      // Fill histogram with PU distribution
      hPileup->Fill(npT);
    }


  //*************************************************************//
  //                                                             //
  //-------------------------- MC Weight ------------------------//
  //                                                             //
  //*************************************************************//

  MCWeight = -10000000.;

  if(!runningOnData_){
    edm::Handle<GenEventInfoProduct> GenInfo;
    iEvent.getByToken(GenInfoToken_, GenInfo);
    
    float _aMCatNLOweight = GenInfo->weight();
    MCWeight = _aMCatNLOweight;

    if(MCWeight == -10000000.) {
      std::cout << "!!!! MCWeight = -10000000 !!!!" << std::endl;
      abort();
    }
  }

  //*************************************************************//
  //                                                             //
  //--------------------------- Trigger -------------------------//
  //                                                             //
  //*************************************************************//

  //Examine the trigger information, return if the trigger doesn't switch on and count the number of events where the trigger has switched on
  isTwoProngTrigger = false;

  const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
  for(unsigned int i = 0, n = triggerBits->size(); i < n; ++i){
    if(!triggerBits->accept(i)) continue;
    std::string tmp_triggername = names.triggerName(i);

    if( tmp_triggername.find("HLT_Photon35_TwoProngs35_v") != std::string::npos ){
      isTwoProngTrigger = true;
    }
  }

  if(!isTwoProngTrigger){
   if(verbose) cout<<"Event not triggered, RETURN."<<endl;
   return;
  }
  nEventsTriggered++;


  //*************************************************************//
  //                                                             //
  //------------------ Variable initialization ------------------//
  //                                                             //
  //*************************************************************//

 
   nPhotons38WP80                  = 0;
   nPhotons20WP90                  = 0;
   nPhotonsChosen                  = 0;   
    //These variables will go in the tree
   photonEt                       = 0.;
   photonEta                      = 0.;
   photonEtaSC                    = 0.;
   photonPhi                      = 0.;
   LorentzVector photonP4;
   photonEtMax                    = -1000.;

  jetPhotonInvMass                =-1.;
  mesonMass                       =-1.;

  metPt                           = 0.;
  metpuppiPt                      = 0.;

  bestJetPt                       =-1.;
  bestJetEta                      =-1.;
  bestJetPhi                      =-1.;
  bestJetnDaughters               = 0.;
  bestJetPtMax                    =-1.;
  bestJetChargedEmEnergy          = 0.;
  bestJetNeutralEmEnergy          = 0.;
  bestJetChargedHadEnergy         = 0.;
  bestJetNeutralHadEnergy         = 0.;
  bestJetChargedEmEnergyFraction  = 0.;
  bestJetNeutralEmEnergyFraction  = 0.;
  bestJetChargedHadEnergyFraction = 0.;
  bestJetNeutralHadEnergyFraction = 0.;
  bestJetChargedHadMultiplicity   = 0.;
  bestJetInvMass                  = 0.;
  bestJetPhotonInvMass            = 0.;
  bestJetJECunc                   = 0.;
  firstTrkPt                      = 0.;
  firstTrkEta                     = 0.;
  firstTrkPhi                     = 0.;
  firstTrkCharge                  = 0.;
  secondTrkCharge                 = 0.;
  secondTrkEta                    = 0.;
  secondTrkPhi                    = 0.;
  secondTrkCharge                 = 0.;
  bestPairPt                      = 0.;
  bestPairEta                     = 0.;
  bestPairPhi                     = 0.;  


  //*************************************************************//
  //                                                             //
  //----------------------------- MET ---------------------------//
  //                                                             //
  //*************************************************************//
  for(auto met = slimmedMETs->begin(); met != slimmedMETs->end(); ++met){
    metPt = met->pt();
  }

  for(auto metpuppi = slimmedMETsPuppi->begin(); metpuppi != slimmedMETsPuppi->end(); ++metpuppi){
    metpuppiPt = metpuppi->pt();
  }



  //*************************************************************//
  //                                                             //
  //---------------------------- Muons --------------------------//
  //                                                             //
  //*************************************************************//
   //Count muons for each event
  for(auto mu = slimmedMuons->begin(); mu != slimmedMuons->end(); ++mu){
    if(mu->pt() < 10. || !mu->CutBasedIdMedium || fabs(mu->eta()) > 2.4 || fabs(mu->muonBestTrack()->dxy((&slimmedPV->at(0))->position())) >= 0.2 || fabs(mu->muonBestTrack()->dz((&slimmedPV->at(0))->position())) >= 0.5) continue;
    if(!mu->PFIsoLoose) continue;
    nMuons10++;
    if(mu->pt() < 20.) continue;
    nMuons20++;
  }



  //*************************************************************//
  //                                                             //
  //-------------------------- Electrons ------------------------//
  //                                                             //
  //*************************************************************//
  //Count the number of electrons
  // Get rho value
  edm::Handle< double > rhoH;
  iEvent.getByToken(rhoToken_,rhoH);
  rho = *rhoH;

  float corrPt = 0.;
  //ph_en_sigmaUP = 0.;
  //ph_en_sigmaDW = 0.;
  //ph_en_scaleUP = 0.;
  //ph_en_scaleDW = 0.;

  for(auto el = slimmedElectrons->begin(); el != slimmedElectrons->end(); ++el){
    //Calculate electron p4, correct it with the Scale&Smearing correction and extract the pT
    LorentzVector elP4 = el->p4();// * el->userFloat("ecalTrkEnergyPostCorr")/el->energy();
    corrPt = elP4.pt();

    if(corrPt < 10. || fabs(el->eta()) > 2.5 || fabs(el->gsfTrack()->dxy((&slimmedPV->at(0))->position())) >= 0.2 || fabs(el->gsfTrack()->dz((&slimmedPV->at(0))->position())) >= 0.5) continue;

     //-------------Conditions on loose/medium MVA electron ID-------------//
    if(el->electronID("mvaEleID-Fall17-iso-V2-wp80") == 0) continue;
    nElectrons10++;
    if (corrPt < 20.) continue;
    nElectrons20++;
  }


  //*************************************************************//
  //                                                             //
  //--------------------------- Photons -------------------------//
  //                                                             //
  //*************************************************************//
  if(verbose) cout<< "PHOTONs"<<" --------------------------------"<<endl;
  
  bool cand_photon_found = false; //initialize this bool to false, return if it doesn't turn into true
  float corrEt = -1.;

  for(auto photon = slimmedPhotons->begin(); photon != slimmedPhotons->end(); ++photon){ //PHOTON FORLOOP START --------------------------------
    
    // Apply energy scale corrections, from 18Apr2023 the correction are embedded in the config file with the postReco tool
    corrEt   = photon->et(); //* photon->userFloat("ecalEnergyPostCorr") / photon->energy(); 

    if(corrEt < 20. || fabs(photon->eta()) > 2.5) continue; //loose selection to reject diphoton bkg 
    if(photon->photonID("mvaPhoID-RunIIFall17-v2-wp90") == 0) continue; //WP90
    if(!photon->passElectronVeto()) continue; 

    nPhotons20WP90++;

    if(corrEt < 35.) continue;
    if(photon->photonID("mvaPhoID-RunIIFall17-v2-wp80") == 0) continue; //WP80

    //float abseta = fabs(photon->superCluster()->eta());
    //float eA = effectiveAreas_ph_.getEffectiveArea(abseta);

    //photon_iso = (pfIso.sumChargedHadronPt + std::max( 0.0f, pfIso.sumNeutralHadronEt + pfIso.sumPhotonEt - eA*rho))/photon->et();

    //if(photon->chargedHadronIso()/corrEt > 0.3 || photon->photonIso() > 4.) continue; //|| photon->trackIso() > 6

    nPhotons38WP80++;

    // Apply energy scale corrections
    photonP4 = photon->p4();// * photon->userFloat("ecalEnergyPostCorr") / photon->energy();

    if(corrEt < photonEtMax) continue; //choose as best photon the one with highest eT
    
    photonEtMax = corrEt;
    photonIsoChargedHadron = photon->chargedHadronIso();
    photonIsoNeutralHadron = photon->neutralHadronIso();
    photonIsoPhoton        = photon->photonIso();
    //photonIsoEArho         = eA * rho;
    photonEt     = corrEt;
    //photonEnergy = photon->energy();
    photonEta    = photon->eta();
    photonEtaSC  = photon->superCluster()->eta();
    photonPhi    = photon->phi();

    photonRegressionError = photon->getCorrectedEnergyError(reco::Photon::P4type::regression2);
    if(debug) cout << "Regression2 Energy Error: " << photonRegressionError << endl;
    
    cand_photon_found = true;
    nPhotonsChosen++;

  }//PHOTON FORLOOP END -------------------------------------------------------------------------------------------------------------------------

  //Return if there are no photons chosen
  if(!cand_photon_found) {
    cout<<"No best photon found, RETURN."<<endl;
    return;
  } 

  nEventsIsPhoton++;



  //*************************************************************//
  //                                                             //
  //--------------------------- N-jets --------------------------//
  //                                                             //
  //*************************************************************//

  //int nJet=1;
  int jetIndex=-1;
  int bestJetIndex=-1; //bestJetIndex
  //int MCtruthIndex = -1;
  //float deltaR = -1;   
  int nDaughters = 0;
  //bool isBestJetFound = false; 

  //daughters forloop variable
  int firstTrkCharge; //carica primo candidato firstTrkCharge
  int secondTrkCharge; //secondTrkCharge
  float firstCandPt; // firstTrkPt
  float secondTrkPt; //secondTrkPt
  LorentzVector firstTrkP4; //quadrimpulso firstTrkP4
  LorentzVector secondTrkP4; //secondTrkP4
  LorentzVector firstTrkP4K; //quadrimpulso nell'ipotesi che sia un K firstTrkP4K
  LorentzVector secondTrkP4K; //secondTrkP4K
  LorentzVector firstTrkP4Pi; //nell'ipotesi che sia un Pi firstTrkP4Pi
  LorentzVector secondTrkP4Pi; //secondTrkP4Pi
  LorentzVector pairP4; //quadrimpulso della coppia di candidati pairP4
  LorentzVector pairP4K; //quadrimpulso della coppia nell'ipotesi siano K pairP4K
  LorentzVector PairP4Pi; //PairP4Pi
  LorentzVector bestFirstTrkP4; //bestFirstTrkP4
  LorentzVector bestSecondTrkP4; //bestSecondTrkP4
  LorentzVector bestPairP4; //bestPairP4
  float bestCoupleOfTheJetPt = 0.; //bestCoupleOfTheJetPt
  float deltaRKChosen = 0.; //deltaRChosen
  float deltaRK = 0.; //deltaR
  firstTrkEnergyK = 0.; //firstTrkEnergyK
  secondTrkEnergyK = 0.; //secondTrkEnergyK
  firstTrkEnergyPi = 0.; //firstTrkEnergyPi
  secondTrkEnergyPi = 0.;//secondTrkEnergyPi
  firstTrkPx=0.; //firstTrkPx
  firstTrkPy=0.;//firstTrkPy
  firstTrkPz=0.;//firstTrkPz
  float firstTrkDxy=-999.; //distanza sul piano xy dal vertice primario firstTrkDxy
  float firstTrkDxyErr=-999.; //firstTrkDxyErr
  float firstTrkDz=-999.; //distanza sul piano z dal vertice primario firstTrkDz
  float firstTrkDzErr=-999.; //firstTrkDzErr
  bestFirstTrkDxy=-999.; //bestFirstTrkDxy
  bestFirstTrkDz=-999.; //bestFirstTrkDz
  bestFirstTrkDxyErr=-999.; //bestFirstTrkDxyErr
  bestFirstTrkDzErr=-999.; //bestFirstCandDzErr
  secondTrkPx=0.; //secondTrkPx
  secondTrkPy=0.; //secondTrkPy
  secondTrkPz=0.;//secondTrkPz
  float secondTrkDxy=-999.; //secondTrkDxy
  float secondTrkDxyErr=-999.; //secondTrkDxyErr
  float secondTrkDz=-999.; //secondTrkDz
  float secondTrkDzErr=-999.; //secondTrkDzErr
  bestSecondTrkDxy=-999.; //bestSecondTrkDxy
  bestSecondTrkDz=-999.; //bestSecondTrkDz
  bestSecondTrkDxyErr=-999.;//bestSecondTrkDxyErr
  bestSecondTrkDzErr=-999.;//bestSecondTrkDzErr
  float firstCandEta=0.; //firstTrkEta
  float firstCandPhi=0.;//firstTrkPhi
  float secondCandEta=0.;//secondTrkEta
  float secondCandPhi=0.;//secondTrkPhi
  float PhiMass = 0.;
  float RhoMass = 0.;
  float KMass = 0.4937;
  float PiMass = 0.13957;
  bool isBestCoupleOfTheEventFound=false; //isBestCoupleOfTheEventFound
  bool isPhi = false;
  bool isRho = false;
  //std::vector<float> pt_jets_vector;   //for VBF veto
  //std::vector<LorentzVector> p4_jets_vector;   //for VBF veto
  //std::vector<float> m_jets_vector;   //for VBF veto

  //JEC uncertainties
  //JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(JetCorPar); 

  //JET LOOP
  for (auto jet = slimmedJets->begin(); jet != slimmedJets->end(); ++jet) {//JET LOOP START -------------------------------------------------------- 

    jetIndex++; 

    jetPhotonInvMass=(jet->p4()+photonP4).M(); //calculate inv mass of jet+photon
    nDaughters= jet->numberOfDaughters(); //calculate number of daughters

    //----------------------------- Pre-Filters --------------------------------------------------------//mettere tutto insieme con gli or
    float neutralHadEnergyFrac = jet->neutralHadronEnergyFraction();
    float neutralEmEnergyFrac  = jet->neutralEmEnergyFraction();
    float muonEnergyFrac       = jet->muonEnergyFraction();
    float chargedHadEnergyFrac = jet->chargedHadronEnergyFraction();
    float chargedHadMult       = jet->chargedHadronMultiplicity();
    float chargedEmEnergyFrac  = jet->chargedEmEnergyFraction();
    float pt                   = jet->pt();
    float eta                  = abs(jet->eta());

    /*if(jet->neutralHadronEnergyFraction() > 0.9) continue; //reject if neutralhadron-energy fraction is > 0.9
    if(jet->neutralEmEnergyFraction() > 0.9) continue; //reject if neutralEm-energy fraction is > 0.9, alias NO-PHOTON FILTER                              
    if(nDaughters < 2) continue; //reject if number of constituens is less then 1
    if(jet->muonEnergyFraction() > 0.8) continue; //reject if muon-energy fraction is > 0.8 
    if(jet->chargedHadronEnergyFraction() <= 0.) continue; //reject if chargedHadron-energy fraction is 0 
    if(jet->chargedHadronMultiplicity() == 0) continue; //reject if there are NOT charged hadrons 
    if(jet->chargedEmEnergyFraction() > 0.8) continue; //reject if chargedEm-energy fraction is > 0.8   
    if(jet->pt() < 20. || abs(jet->eta()) > 4.7) continue;*/

    if(neutralHadEnergyFrac > 0.9 || neutralEmEnergyFrac > 0.9 || nDaughters < 2. || muonEnergyFrac > 0.8 || chargedHadEnergyFrac < 0. || chargedHadMult == 0. || chargedEmEnergyFrac > 0.8 || pt < 20. || eta > 4.7) continue;
    //for VBF veto ----------------------
    //nJets20++;
    //eta_jets_vector.push_back(jet->eta());
    //p4_jets_vector.push_back(jet->p4());
    //pt_jets_vector.push_back(jet->pt());
    //m_jets_vector.push_back((jet->p4()).M());
    //-----------------------------------

    /*/JEC and JES uncertainties ----------------------------------------------
    jecUnc->setJetEta(jet->eta());
    jecUnc->setJetPt(jet->pt()); // here you must use the CORRECTED jet pt
    double unc = jecUnc->getUncertainty(true);
    double jetPtShifted = jet->pt() - unc; //just for JEC syst
    if (debug) cout <<"jet pT = "<<jet->pt()<<", jet pT shifted = "<<jetPtShifted<<endl;
    //------------------------------------------------------------------------*/

    if(jet->pt() < 38. || abs(jet->eta()) > 2.5) continue; //taglio in pt, deve comunque essere > del trigger (35)
    if(jetPhotonInvMass < 30.) continue; //reject jets with inv mass lower than 30 GeV (molto loose)
                           
    //-------------------------------------------------------------------------------------------------      
    
    if(verbose) cout<<"    Jet at index = "<<jetIndex<<" passed the cuts:"<<endl; 

    
    //-------------------------------------daughters forloop----------------------------

    for(int firstTrkIndex=0; firstTrkIndex < nDaughters; firstTrkIndex++){ //1ST PARTICLE LOOP STARTS

      if (verbose) cout<<"Daughter n."<<firstTrkIndex+1<<" pT = "<<slimmedJets->at(jetIndex).daughter(firstTrkIndex)->pt()<<endl; 

      firstTrkCharge = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->charge();  //take firstCand charge
      firstCandPt    = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->pt();  //take firstCand pt
      firstCandEta   = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->eta(); //take firstCand eta
      firstCandPhi   = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->phi(); //take firstCand phi
      firstTrkDxy    = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->dxy((&slimmedPV->at(0))->position()); //take firstCand dxy
      firstTrkDxyErr = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->dxyError();        
      firstTrkDz     = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->dz((&slimmedPV->at(0))->position()); //take firstCand dz
      firstTrkDzErr  = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->dzError();

      //loop only over charged daughters, mettere tutto insieme con degli ||
      if(firstTrkCharge == 0 || slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack() == NULL || abs(firstTrkDxy) >= 0.2 || abs(firstTrkDz) >= 0.5 || !(slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->quality(reco::Track::highPurity))) continue;
      /*if (slimmedJets->at(jetIndex).daughter(firstTrkIndex)->charge() == 0) continue;
      if(slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack() == NULL) continue;//se non trova una buona traccia
      if (abs(slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->dxy((&slimmedPV->at(0))->position())) >= 0.2) continue;
      if (abs(slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->dz((&slimmedPV->at(0))->position())) >= 0.5)  continue;
      if (!(slimmedJets->at(jetIndex).daughter(firstTrkIndex)->bestTrack()->quality(reco::Track::highPurity))) continue;*/         

      if(firstCandPt < 1.) continue; //firstCand filter if pT < 1 GeV

      for(int secondTrkIndex=firstTrkIndex+1; secondTrkIndex < nDaughters; secondTrkIndex++){ //2ND LOOP STARTS
      
        //loop only over charged daughters, raggruppare con gli || 
        secondTrkCharge = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->charge();
        secondTrkPt     = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->pt();
        secondCandEta   = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->eta();
        secondCandPhi   = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->phi();
        secondTrkDxy    = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dxy((&slimmedPV->at(0))->position());
        secondTrkDxyErr = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dxyError();          
        secondTrkDz     = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dz((&slimmedPV->at(0))->position());
        secondTrkDzErr  = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dzError();

        if(secondTrkCharge == 0 || slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack() == NULL || abs(secondTrkDxy) >= 0.2 || abs(secondTrkDz) >= 0.5 || !(slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->quality(reco::Track::highPurity))) continue;
        /*if (slimmedJets->at(jetIndex).daughter(secondTrkIndex)->charge() == 0) continue;
        if (slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack() == NULL) continue;
        if(verbose) cout<<"Track without bestTrack() found!"<<endl;
        //cout<<"dxy = "<<slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dxy((&slimmedPV->at(0))->position())<<endl;
        //cout<<"dz  = "<<slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dz((&slimmedPV->at(0))->position())<<endl;          
        if ((abs(slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dxy((&slimmedPV->at(0))->position()))) >= 0.2 || abs(slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->dz((&slimmedPV->at(0))->position())) >= 0.5 ) continue;
        if (!(slimmedJets->at(jetIndex).daughter(secondTrkIndex)->bestTrack()->quality(reco::Track::highPurity))) continue;*/
        
        //TRKs PT CUT --------------------------------------------------------------------------
        if(secondTrkPt < 1.) continue; //firstCand filter if pT < 1 GeV
        if(firstCandPt < 10. && secondTrkPt < 10.) continue;  //filter if both cand pT are < 10GeV

        //DITRK DELTA R CUT --------------------------------------------------------------------------         
        float deltaEta= firstCandEta - secondCandEta;

        float deltaPhi = fabs(firstCandPhi - secondCandPhi);  //phi folding, formula per il delta phi se un angolo è >Pi  
        if (deltaPhi > M_PI) deltaPhi = 2*M_PI - deltaPhi;

        deltaRK= sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
        if(deltaRK > 0.07) continue; //forse >0.05, controllare

        //OPPOSITE CHARGE - FILTER ------------------------------------------------------------
        if(firstTrkCharge * secondTrkCharge >= 0) continue; //choose only opposite charges

        //QUADRIMOMENTUM CALCULATION ------------------------------------------------------------
        firstTrkP4  = slimmedJets->at(jetIndex).daughter(firstTrkIndex)->p4(); //take quadrimomentum
        secondTrkP4 = slimmedJets->at(jetIndex).daughter(secondTrkIndex)->p4();

        firstTrkPx  = firstTrkP4.px();  //take px, py, pz of the first candidate
        firstTrkPy  = firstTrkP4.py();
        firstTrkPz  = firstTrkP4.pz();
        secondTrkPx = secondTrkP4.px(); //take px, py, pz of the second candidate
        secondTrkPy = secondTrkP4.py();
        secondTrkPz = secondTrkP4.pz();

        //PIONS OR KAONS HYPOTHESIS -----------------------------------------------------------------------------------------------------------------------------------------------            
        firstTrkEnergyK   = sqrt(firstTrkPx  * firstTrkPx  + firstTrkPy  * firstTrkPy  + firstTrkPz  * firstTrkPz  + KMass  * KMass ); //Kaon hypothesis energy recalculation
        secondTrkEnergyK  = sqrt(secondTrkPx * secondTrkPx + secondTrkPy * secondTrkPy + secondTrkPz * secondTrkPz + KMass  * KMass ); //Kaon hypothesis energy recalculation
        firstTrkEnergyPi  = sqrt(firstTrkPx  * firstTrkPx  + firstTrkPy  * firstTrkPy  + firstTrkPz  * firstTrkPz  + PiMass * PiMass); //Pion hypothesis energy recalculation
        secondTrkEnergyPi = sqrt(secondTrkPx * secondTrkPx + secondTrkPy * secondTrkPy + secondTrkPz * secondTrkPz + PiMass * PiMass); //Pion hypothesis energy recalculation          
        /*
        if (verbose) {
          cout<<"firstTrkEnergyK   = "<<firstTrkEnergyK<<endl;
          cout<<"firstTrkEnergyPi  = "<<firstTrkEnergyPi<<endl;
          cout<<"secondTrkEnergyK  = "<<secondTrkEnergyK<<endl;
          cout<<"secondTrkEnergyPi = "<<secondTrkEnergyPi<<endl;
        }
        */
        firstTrkP4K   = firstTrkP4.SetE(firstTrkEnergyK); //Kaon hypothesis quadrimomentum correction
        secondTrkP4K  = secondTrkP4.SetE(secondTrkEnergyK); //Kaon hypothesis quadrimomentum correction
        firstTrkP4Pi  = firstTrkP4.SetE(firstTrkEnergyPi); //Pion hypothesis quadrimomentum correction
        secondTrkP4Pi = secondTrkP4.SetE(secondTrkEnergyPi); //Pion hypothesis quadrimomentum correction

        pairP4K  = firstTrkP4K  + secondTrkP4K; //calculation of the couple-quadrimomentum after the correction
        PairP4Pi = firstTrkP4Pi + secondTrkP4Pi; //calculation of the couple-quadrimomentum after the correction
        
        if (verbose) {
          cout<<"KK pT = "<<pairP4K.pt()<<endl;
          cout<<"PiPi pT = "<<PairP4Pi.pt()<<endl;
        }

        //DITRK PT CUT -------------------------------------------------------------------------
        if(pairP4K.pt() < 38.) {//controllare questo taglio
          if(verbose) cout<<"couplePt cut NOT passed"<<endl;
          continue;
        }  
        
        //MESON INV MASS CUT -------------------------------------------------------------------------
        isPhi = false;
        isRho = false;

        PhiMass = (pairP4K).M(); //calculate inv mass of the Phi candidate from the couple 4-momentum 
        if (verbose) cout<<"mKK (before the meson mass selection) =  "<<PhiMass<<endl;
        if(PhiMass > 1. && PhiMass < 1.05) isPhi = true; //filter on Phi invariant mass  

        RhoMass = (PairP4Pi).M(); //calculate inv mass of the Rho candidate from the couple 4-momentum
        if (verbose) cout<<"mPiPi (before the meson mass selection) =  "<<RhoMass<<endl;
        if(RhoMass > 0.5 && RhoMass < 1.) isRho = true; //filter on Rho invariant mass   

        if (!isPhi && !isRho) continue; //continue if the pair mass doesn't match any of the two mass hypothesis

        if (isPhi && isRho){ //if both hypothesis are true, mark it as a Phi candidate (this is done because the Phi mass window is tighter, so it's more unlikely that the mass lies in the tighter window by chance)
          isPhi = true;
          isRho = false;
        }

        //update values of quadrimomenta
        if(isPhi){
          firstTrkP4  = firstTrkP4K; 
          secondTrkP4 = secondTrkP4K;
          pairP4      = pairP4K;
        }  
        if(isRho){
          firstTrkP4  = firstTrkP4Pi;
          secondTrkP4 = secondTrkP4Pi;
          pairP4      = PairP4Pi;
        }

        // ISOLATION CUT -------------------------------------------------------------------------  
        for(auto cand_iso = PFCandidates->begin(); cand_iso != PFCandidates->end(); ++cand_iso){ //ISOLATION FORLOOP START
          
          if(debug){
            cout <<endl<<"ISO CALC DETAILS ---------------------"<<endl;
            cout << "pt cand_iso = "<<cand_iso->pt()<<endl;
          }

          if(cand_iso->pt() < 0.5) continue; //do not consider tracks with pT < 500MeV

          //calculate the deltaR between the track and the first candidate ---------------------------------------
          float deltaPhi_K1 = fabs(firstTrkP4.phi()-cand_iso->phi());  //phi folding 
          if (deltaPhi_K1 > M_PI) deltaPhi_K1 = 2*M_PI - deltaPhi_K1;

          float deltaRK1 = sqrt((firstTrkP4.eta()-cand_iso->eta())*(firstTrkP4.eta()-cand_iso->eta()) + deltaPhi_K1*deltaPhi_K1);
          if (debug) cout << "deltaRK1 = "<<deltaRK1<<endl;
          if(deltaRK1 < 0.0005) continue; //remove first candidate from the sum, it means that the track (cand_iso) and the first candidate (firstTrkP4) are the same 

          //calculate the deltaR between the track and the second candidate ---------------------------------------
          float deltaPhi_K2 = fabs(secondTrkP4.phi()-cand_iso->phi());  //phi folding  
          if (deltaPhi_K2 > M_PI) deltaPhi_K2 = 2*M_PI - deltaPhi_K2;

          float deltaRK2 = sqrt((secondTrkP4.eta()-cand_iso->eta())*(secondTrkP4.eta()-cand_iso->eta()) + deltaPhi_K2*deltaPhi_K2);
          if (debug) cout << "deltaRK2 = "<<deltaRK2<<endl;
          if(deltaRK2 < 0.0005) continue; //remove second candidate from the sum

          //calculate the deltaR between the track (cand_iso) and the best pair ---------------------------------------
          float deltaPhi_Couple = fabs(pairP4.phi()-cand_iso->phi());  //phi folding  
          if (deltaPhi_Couple > M_PI) deltaPhi_Couple = 2*M_PI - deltaPhi_Couple;

          float deltaR_Couple = sqrt((pairP4.eta()-cand_iso->eta())*(pairP4.eta()-cand_iso->eta()) + deltaPhi_Couple*deltaPhi_Couple);

          //sum pT of the tracks inside a cone of deltaR = 0.3 ---------------------------------------
          if(deltaRK1 <= 0.3) K1SumPt05 += cand_iso->pt();
          if(deltaRK2 <= 0.3) K2SumPt05 += cand_iso->pt();
          if(deltaR_Couple <= 0.3) pairSumPt05 += cand_iso->pt();
          //cout<< "charge before = "<<cand_iso->charge()<<endl;

          //sum pT of the charged tracks inside a cone of deltaR = 0.3 ---------------------------------------
          if (debug) cout << "Charge = "<< cand_iso->charge() <<endl;
          if(cand_iso->charge() == 0) continue;
          // cout << "particle charge = "<<cand_iso->charge()<<endl;
          if (debug) cout << "dxy = " << fabs(cand_iso->dxy()) << " and dz = " << fabs(cand_iso->dz()) <<endl;
          if(fabs(cand_iso->dxy()) >= 0.2 || fabs(cand_iso->dz()) >= 0.5) continue; // Requesting charged particles to come from PV
          //cout<< "charge after = "<<cand_iso->charge()<<endl;
          if(deltaRK1 <= 0.3) K1SumPt05Ch += cand_iso->pt();
          if(deltaRK2 <= 0.3) K2SumPt05Ch += cand_iso->pt();
          if (debug) cout <<"deltaR_Couple = "<<deltaR_Couple<<endl;
          if(deltaR_Couple <= 0.3){
            pairSumPt05Ch += cand_iso->pt();
            if (debug) cout<<"Particle in the cone: SumPt = "<<pairSumPt05Ch<<endl;
          }
        } //ISOLATION FORLOOP END

        float isoCoupleCh = pairP4.pt()/(pairSumPt05Ch + pairP4.pt());
        if(isoCoupleCh < 0.9) {
          cout<<"No isolation cut passed."<<endl;
          continue; //reject se la coppia di tracce contribuisce per meno del 90% al pt totale del jet
        }

        //if(verbose) cout << "step 1" << endl;
        //PT MAX OF THE JET - FILTER -------------------------------------------------
        if (verbose) cout<<"Current bestCoupleOfTheEvent_Pt = "<<bestCoupleOfTheJetPt<<endl;
        
        if(pairP4.pt() <= bestCoupleOfTheJetPt) {//initially set to 0
          if(verbose) cout<<"Not passed: pT lower than the current best pair of the event"<<endl;
          continue; //choose the couple with greatest pt
        }
        //if(verbose) cout << "step 2" << endl;

        //If passed, this is the pair with the largest pT of the event so far
        bestCoupleOfTheJetPt = pairP4.pt();     
        if (verbose) cout<<"pairP4.pt() = "<<bestCoupleOfTheJetPt<<endl;

        if(verbose) cout<<"This is the best pair so far!"<<endl<<"-------------------------"<<endl;
        isBestCoupleOfTheEventFound = true;
        //if(verbose) cout << "step 3" << endl;

        //Save if best pair has been found
        bestJetIndex         = jetIndex; //note the position of the chosen jet inside the vector   
        deltaRKChosen        = deltaRK;
        bool b=false;
        if(b) cout << deltaRKChosen << endl; //da aggiungere altrimenti dà errore (variable initialized but not used)
        bestJetPhotonInvMass = jetPhotonInvMass;
        isPhi                = isPhi;
        isRho                = isRho;
        //bestJetJECunc        = unc;
        firstTrkCharge       = firstTrkCharge;
        secondTrkCharge      = secondTrkCharge;
        bestFirstTrkDxy      = firstTrkDxy;
        bestFirstTrkDz       = firstTrkDz;
        bestSecondTrkDxy     = secondTrkDxy;
        bestSecondTrkDz      = secondTrkDz;
        bestFirstTrkDxyErr   = firstTrkDxyErr;
        bestFirstTrkDzErr    = firstTrkDzErr;
        bestSecondTrkDxyErr  = secondTrkDxyErr;
        bestSecondTrkDzErr   = secondTrkDzErr;
        bestFirstTrkP4       = firstTrkP4; 
        bestSecondTrkP4      = secondTrkP4;
        bestPairP4           = pairP4;          

        if(verbose) cout << "step 4" << endl;

    } //2ND LOOP ENDS
    if(verbose) cout << "second loop ended" << endl;
  } //1ST LOOP ENDS
  if(verbose) cout << "first loop ended" << endl;

  if(jet->pt() < 25.) continue;
  nJets25++;
  if(jet->pt() < 30.) continue;
  nJets30++;

} //JET LOOP END
if(verbose) cout << "jet loop ended" << endl;
//delete jecUnc;


if(!isBestCoupleOfTheEventFound) 
{
  cout<<"No best couple detected for current event, RETURN."<<endl;
  return;
}
nEventsBestPairFound++;      
if(verbose) cout<<"Bool: nEventsBestPairFound: "<<nEventsBestPairFound<<endl;        

//DATAMEMBER SAVING
firstTrkPt                      = bestFirstTrkP4.pt();
firstTrkEta                     = bestFirstTrkP4.eta();
firstTrkPhi                     = bestFirstTrkP4.phi();
secondTrkCharge                 = bestSecondTrkP4.pt();       
secondTrkEta                    = bestSecondTrkP4.eta();
secondTrkPhi                    = bestSecondTrkP4.phi();
bestPairPt                      = bestPairP4.pt();
bestPairEta                     = bestPairP4.eta();
bestPairPhi                     = bestPairP4.phi();
bestJetInvMass                  = slimmedJets->at(bestJetIndex).mass();
bestJetPt                       = slimmedJets->at(bestJetIndex).pt();
bestJetEta                      = slimmedJets->at(bestJetIndex).eta();
bestJetPhi                      = slimmedJets->at(bestJetIndex).phi();
bestJetnDaughters               = slimmedJets->at(bestJetIndex).numberOfDaughters();
bestJetChargedEmEnergy          = slimmedJets->at(bestJetIndex).chargedEmEnergy();
bestJetNeutralEmEnergy          = slimmedJets->at(bestJetIndex).neutralEmEnergy();
bestJetChargedHadEnergy         = slimmedJets->at(bestJetIndex).chargedHadronEnergy();
bestJetNeutralHadEnergy         = slimmedJets->at(bestJetIndex).neutralHadronEnergy();
bestJetChargedEmEnergyFraction  = slimmedJets->at(bestJetIndex).chargedEmEnergyFraction();
bestJetNeutralEmEnergyFraction  = slimmedJets->at(bestJetIndex).neutralEmEnergyFraction();
bestJetChargedHadEnergyFraction = slimmedJets->at(bestJetIndex).chargedHadronEnergyFraction();
bestJetNeutralHadEnergyFraction = slimmedJets->at(bestJetIndex).neutralHadronEnergyFraction();
bestJetChargedHadMultiplicity   = slimmedJets->at(bestJetIndex).chargedHadronMultiplicity();

//MESON MASS CALCULATION
mesonMass = (bestFirstTrkP4 + bestSecondTrkP4).M();

//Z INV MASS CALCULATION
ZMassFrom2KPhoton = (bestFirstTrkP4 + bestSecondTrkP4 + photonP4).M(); //calculate inv mass of the Higgs candidate
  

//CANDIDATES SORTING
if(firstTrkPt < secondTrkCharge)  //swap-values loop, in order to fill the tree with the candidate with max pt of the couple in firstCand branches  
  {                               //and the one with min pt in secondCand branches
    float a,b,c,d,e;
    a = firstTrkPt;
    b = firstTrkEta;
    c = firstTrkPhi;
    d = firstTrkEnergy;
    e = firstTrkCharge;
    firstTrkPt     = secondTrkCharge;
    firstTrkEta    = secondTrkEta;
    firstTrkPhi    = secondTrkPhi;
    firstTrkEnergy = secondTrkEnergy;
    firstTrkCharge = secondTrkCharge;
    secondTrkCharge = a;
    secondTrkEta    = b;
    secondTrkPhi    = c;
    secondTrkEnergy = d;
    secondTrkCharge = e;
}

//CUTS ON CANDIDATES PT
if(firstTrkPt < 20. || secondTrkCharge < 5.) {
    cout<<"Final cut on candidates pT not passed, RETURN."<<endl;
    return;
}
  nEventsTrkPtFilter++;

//ISOLATION DATAMEMBER FOR TREE FILLING 
isoK1     = firstTrkPt/(K1SumPt05 + firstTrkPt);
isoK2     = secondTrkCharge/(K2SumPt05 + secondTrkCharge);
isoPair   = bestPairPt/(pairSumPt05 + bestPairPt);
isoK1Ch   = firstTrkPt/(K1SumPt05Ch + firstTrkPt);
isoK2Ch   = secondTrkCharge/(K2SumPt05Ch + secondTrkCharge);
isoPairCh = bestPairPt/(pairSumPt05Ch + bestPairPt);

//CUT ON PHI ISOLATION
if(verbose){
  cout<<endl;
  cout<<"###### ISO           = "<<isoPairCh<<endl;
  cout<<"###### isRho         = "<<isRho<<endl;
  cout<<"###### SUM pT        = "<<pairSumPt05Ch<<endl;
  cout<<"###### pT leading    = "<<firstTrkPt<<endl;
  cout<<"###### pT subleading = "<<secondTrkCharge<<endl;
  cout<<"###### MesonMass     = "<<mesonMass<<endl;
  cout<<"###### ZMass         = "<<ZMassFrom2KPhoton<<endl;
}



nEventsPairIsolationFilter++;








  
  mytree->Fill();
}

//*************************************************************//
//                                                             //
//---------------------- Create the tree ----------------------//
//                                                             //
//*************************************************************//

void ZMesonGamma::create_trees()
{
  mytree = fs->make<TTree>("mytree", "Tree containing gen&reco");
  
  mytree->Branch("nPV",&nPV);
  mytree->Branch("isTwoProngTrigger",&isTwoProngTrigger);

//Save run number info when running on data
  if(runningOnData_){
    mytree->Branch("runNumber",&runNumber);
    mytree->Branch("eventNumber",&eventNumber);
  }

  mytree->Branch("nMuons10",&nMuons10);
  mytree->Branch("nMuons20",&nMuons20);
  mytree->Branch("nElectrons10",&nElectrons10);
  mytree->Branch("nElectrons20",&nElectrons20);
  mytree->Branch("nPhotons38WP80",&nPhotons38WP80);
  mytree->Branch("nPhotons20WP90",&nPhotons20WP90);
  mytree->Branch("nPhotonsChosen",&nPhotonsChosen);
  mytree->Branch("photon_eT",&photonEt);
  mytree->Branch("photon_eta",&photonEta);
  mytree->Branch("photon_etaSC",&photonEtaSC);
  mytree->Branch("photon_phi",&photonPhi);
  mytree->Branch("photon_iso_eArho",&photonIsoEArho);
  mytree->Branch("photonRegressionError",&photonRegressionError);

  mytree->Branch("nJets30",&nJets30);
  mytree->Branch("nJets25",&nJets25);
  mytree->Branch("metPt",&metPt);
  mytree->Branch("metpuppiPt",&metpuppiPt);

  mytree->Branch("bestJet_pT",&bestJetPt);
  mytree->Branch("bestJet_eta",&bestJetEta);
  mytree->Branch("bestJet_phi",&bestJetPhi);
  mytree->Branch("bestJet_nDaughters",&bestJetnDaughters);
  mytree->Branch("bestJet_chargedEmEnergy",&bestJetChargedEmEnergy);
  mytree->Branch("bestJet_neutralEmEnergy",&bestJetNeutralEmEnergy);
  mytree->Branch("bestJet_chargedHadEnergy",&bestJetChargedHadEnergy);
  mytree->Branch("bestJet_neutralHadEnergy",&bestJetNeutralHadEnergy);
  mytree->Branch("bestJet_chargedEmEnergyFraction",&bestJetChargedEmEnergyFraction);
  mytree->Branch("bestJet_neutralEmEnergyFraction",&bestJetNeutralEmEnergyFraction);
  mytree->Branch("bestJet_chargedHadEnergyFraction",&bestJetChargedHadEnergyFraction);
  mytree->Branch("bestJet_neutralHadEnergyFraction",&bestJetNeutralHadEnergyFraction);
  mytree->Branch("bestJet_invMass",&bestJetInvMass);
  mytree->Branch("bestJet_Photon_invMass",&bestJetPhotonInvMass);
  mytree->Branch("bestJet_JECunc",&bestJetJECunc);

  mytree->Branch("firstTrkCharge",&firstTrkCharge);
  mytree->Branch("firstCandPt",&firstTrkPt);
  mytree->Branch("firstCandEta",&firstTrkEta);
  mytree->Branch("firstCandPhi",&firstTrkPhi);
  mytree->Branch("firstTrkDxy",&bestFirstTrkDxy);
  mytree->Branch("firstTrkDz",&bestFirstTrkDz);
  mytree->Branch("firstTrkDxyErr",&bestFirstTrkDxyErr);
  mytree->Branch("firstTrkDzErr",&bestFirstTrkDzErr);
  mytree->Branch("secondTrkCharge",&secondTrkCharge);
  mytree->Branch("secondTrkPt",&secondTrkCharge);
  mytree->Branch("secondCandEta",&secondTrkEta);
  mytree->Branch("secondCandPhi",&secondTrkPhi);
  mytree->Branch("secondTrkDxy",&bestSecondTrkDxy);
  mytree->Branch("secondTrkDz",&bestSecondTrkDz);
  mytree->Branch("secondTrkDxyErr",&bestSecondTrkDxyErr);
  mytree->Branch("secondTrkDzErr",&bestSecondTrkDzErr);
  mytree->Branch("bestCouplePt",&bestPairPt);
  mytree->Branch("bestCoupleEta",&bestPairEta);
  mytree->Branch("bestCouplePhi",&bestPairPhi);
  mytree->Branch("isPhi",&isPhi);
  mytree->Branch("isRho",&isRho);

  mytree->Branch("firstTrkEnergy",&firstTrkEnergy);
  mytree->Branch("secondTrkEnergy",&secondTrkEnergy);

  mytree->Branch("MesonMass",&mesonMass);
  mytree->Branch("Hmass_From2K_Photon",&ZMassFrom2KPhoton);

  mytree->Branch("pairSumPt05",&pairSumPt05);
  mytree->Branch("pairSumPt05Ch",&pairSumPt05Ch);

  mytree->Branch("iso_K1",&isoK1);
  mytree->Branch("iso_K1_ch",&isoK1Ch);
  mytree->Branch("iso_K2",&isoK2);
  mytree->Branch("iso_K2_ch",&isoK2Ch);
  mytree->Branch("iso_couple",&isoPair);
  mytree->Branch("iso_couple_ch",&isoPairCh);

  
    
}

void ZMesonGamma::beginJob()
{
  //Flag for PileUp reweighting
  if (!runningOnData_){ // PU reweighting for 2017
    Lumiweights_ = edm::LumiReWeighting("MCpileUp_2018_25ns_UltraLegacy_PoissonOOTPU.root", "MyDataPileupHistogram.root", "pileup", "pileup");
  }
}

void ZMesonGamma::endJob() 
{
  hEvents->Fill(0.5,nEventsProcessed);
  hEvents->Fill(1.5,nEventsTriggered);
  hEvents->Fill(2.5,nEventsIsPhoton);
  hEvents->Fill(3.5,nEventsBestPairFound);  
  hEvents->Fill(4.5,nEventsTrkPtFilter);  
  hEvents->Fill(5.5,nEventsPairIsolationFilter);
  hEvents->GetXaxis()->SetBinLabel(1,"processed");
  hEvents->GetXaxis()->SetBinLabel(2,"triggered");
  hEvents->GetXaxis()->SetBinLabel(3,"best photon");
  hEvents->GetXaxis()->SetBinLabel(4,"best pair");
  hEvents->GetXaxis()->SetBinLabel(5,"trks pT");
  hEvents->GetXaxis()->SetBinLabel(6,"trks iso");

}

//define this as a plug-in
DEFINE_FWK_MODULE(ZMesonGamma);