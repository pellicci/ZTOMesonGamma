#!/bin/bash
python generate_histos.py phi 1 preselection unblind rootfiles/latest_productions/ZMesonGamma_Data.root histos/latest_productions/CR_Phi_preselection_Sidebands.root
python generate_histos.py phi 0 preselection blind rootfiles/latest_productions/ZMesonGamma_Data.root histos/latest_productions/SR_Phi_preselection_Data.root
python generate_histos.py phi 0 preselection unblind rootfiles/latest_productions/ZMesonGammaPhi_Signal.root histos/latest_productions/SR_Phi_preselection_Signal.root
python normalize_for_BkgEstimation.py preselection phi
python plot_histos_bkgEstimation.py 0.0001 0 1 histos/latest_productions/SR_Phi_preselection_Data.root histos/latest_productions/SR_Phi_preselection_Signal.root histos/latest_productions/CR_Phi_preselection_SidebandsNorm.root
