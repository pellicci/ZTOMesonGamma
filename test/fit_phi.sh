#!/bin/bash
python MassAnalysis/fit_signal.py Phi
python MassAnalysis/fit_bkg.py Phi
combine Datacards/datacard_Phi.txt -M AsymptoticLimits -t -1 --run expected --name Phi_bdt
