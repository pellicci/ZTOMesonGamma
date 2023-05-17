//---------- class declaration----------

class ZMesonGamma : public edm::EDAnalyzer {

 public:
  explicit ZMesonGamma(const edm::ParameterSet&);
  ~ZMesonGamma();

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() override;

  bool runningOnData_;
  bool verboseIdFlag_;
  const edm::InputTag packedPFCandidates_;
  const edm::InputTag prunedGenParticles_;
  const edm::InputTag slimmedPhotons_;
  const edm::InputTag slimmedElectrons_;
  const edm::InputTag slimmedMuons_; 
  const edm::InputTag slimmedJets_;
  const edm::InputTag GenInfo_;
  edm::LumiReWeighting Lumiweights_;



  edm::Service<TFileService> fs;

  void create_trees();

  // ---------- member data ----------- //
  TH1F* hEvents;
  TH1F* hPileup;

  //Counters
  int nPV;

  int nMuons10;
  int nMuons20;
  int nElectrons10;
  int nElectrons20;
  int nPhotonsChosen;
  int nPhotons20WP90;
  int nPhotons38WP80;
  int nJets30;
  int nJets25;
  int nEventsTriggered;
  int nEventsProcessed;
  int nEventsIsTwoKaons;
  int nEventsIsPhoton;
  int nEventsZMatched; 
  int nEventsZNotMatched; 
  int nEventsZMassMatched; 
  int nEventsZMassNotMatched; 
  int nEventsMesonPtNotMatched;
  int nEventsBestPairFound;
  int nEventsTrkPtFilter;
  int nEventsPairIsolationFilter;

  //bools
  bool isTwoProngTrigger;

  //debug
  bool debug;
  bool verbose;

  //TTree and TTree variables
  TTree *mytree;

  int runNumber;
  int eventNumber;


  float photonEt;
  float photonEnergy;
  float photonEta;
  float photonEtaSC;
  float photonPhi;
  float photonEtMax; //eTphmax
  float photonRegressionError;
  float photonIsoChargedHadron;
  float photonIsoNeutralHadron;
  float photonIsoPhoton;
  float photonIsoEArho;

  float firstTrkPx;
  float firstTrkPy;
  float firstTrkPz;
  float bestFirstTrkDxy;
  float bestFirstTrkDxyErr;
  float bestFirstTrkDz;
  float bestFirstTrkDzErr;
  float secondTrkPx;
  float secondTrkPy;
  float secondTrkPz;
  float bestSecondTrkDxy;
  float bestSecondTrkDxyErr;
  float bestSecondTrkDz;
  float bestSecondTrkDzErr;
  float firstTrkEnergy;
  float secondTrkEnergy;
  float firstTrkEnergyK;
  float secondTrkEnergyK;
  float firstTrkEnergyPi;
  float secondTrkEnergyPi;
  float firstTrkPt;
  float firstTrkEta;
  float firstTrkPhi;
  float firstTrkCharge;
  float secondTrkPt;
  float secondTrkEta;
  float secondTrkPhi;
  float secondTrkCharge;
  float bestPairPt;
  float bestPairEta;
  float bestPairPhi;

  float jetPhotonInvMass;
  float mesonMass;
  float ZMassFrom2KPhoton;

  float K1SumPt05;
  float K1SumPt05Ch;
  float K2SumPt05;
  float K2SumPt05Ch;
  float pairSumPt05;
  float pairSumPt05Ch;
  float isoK1;
  float isoK1Ch;
  float isoK2;
  float isoK2Ch;
  float isoPair;
  float isoPairCh;

  float metPt;
  float metpuppiPt;

  //Jet datamember  
  float jetInvMass;
  float bestJetPt;
  float bestJetEta;
  float bestJetPhi;
  int bestJetnDaughters;
  float bestJetPtMax;
  float bestJetChargedEmEnergy;
  float bestJetNeutralEmEnergy;
  float bestJetChargedHadEnergy;
  float bestJetNeutralHadEnergy;
  float bestJetChargedEmEnergyFraction;
  float bestJetNeutralEmEnergyFraction;
  float bestJetChargedHadEnergyFraction;
  float bestJetNeutralHadEnergyFraction;
  int bestJetChargedHadMultiplicity;
  float bestJetInvMass;
  float bestJetPhotonInvMass;
  float bestJetJECunc;

  //MC truth
  float PUWeight;
  float MCWeight;

  bool isPhi;
  bool isRho;

  float rho;

  //for VBF veto
  int nJets20;

  //Tokens
  edm::EDGetTokenT<std::vector<reco::GenParticle> > prunedGenParticlesToken_; 
  edm::EDGetTokenT<std::vector<pat::PackedCandidate> > packedPFCandidatesToken_; 
  edm::EDGetTokenT<std::vector<reco::Vertex> > offlineSlimmedPrimaryVerticesToken_;  
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupSummaryToken_;
  edm::EDGetTokenT<GenEventInfoProduct> GenInfoToken_;
  edm::EDGetTokenT<edm::TriggerResults> triggerBitsToken_;
  edm::EDGetTokenT<std::vector<pat::Muon> > slimmedMuonsToken_;   
  edm::EDGetTokenT<std::vector<pat::Jet> > slimmedJetsToken_;
  edm::EDGetTokenT<std::vector<pat::MET> > slimmedMETsToken_;
  edm::EDGetTokenT<std::vector<pat::MET> > slimmedMETsPuppiToken_;

  //Ele ID decisions objects
  edm::EDGetToken electronsMiniAODToken_;

  //Photon ID decisions
  edm::EDGetToken photonsMiniAODToken_;

  //rho (PU energy density)
  edm::EDGetTokenT<double> rhoToken_;

};