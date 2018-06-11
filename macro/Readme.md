# Macro to process space based coincidences

## Produce super-reduced ntuples for analysing coincidences
PlotCoincidencesEnergyVsEnergy.C(inputFileName,outputExtraLabel)
This will read the space based coincidences from /nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/ and output a ROOT file with the histograms of
- TotalEnergy - Energy : Energy plots for all the multiplicities, all energies, and with other different selections.
- Sel1: sum energy 2615
- Sel2: energy 2615
And also a TTree with the basic informations needed to produce whatever plot you need.

## Produce Radius distributions for 2615sum and 2615acc events
PlotInteractiveEvE.C
Will read the reduced ntuple file and output a root file with histograms.
Actually would be nice if it appended those histograms to the file .root it reads

## [INTERACTIVE] Produce Efficiency plots for signal and bkg, and Purity : Efficiency curve
ComputeOptimalCut.C
