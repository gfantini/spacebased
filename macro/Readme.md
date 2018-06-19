# Macro to process space based coincidences

## [MANDATORY] Produce super-reduced ntuples for analysing coincidences
PlotCoincidencesEnergyVsEnergy.C(inputFileName,outputExtraLabel)
This will read the space based coincidences from /nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/ and output a ROOT file with the histograms of
- TotalEnergy - Energy : Energy plots for all the multiplicities, all energies, and with other different selections.
- Sel1: sum energy 2615
- Sel2: energy 2615
### OUTPUT
TTree "outTree" with the basic informations needed to produce whatever plot you need.

## [MANDATORY] Produce Radius distributions for 2615sum and 2615acc events
### REQUIRES:   PlotCoincidencesEnergyVsEnergy
PlotInteractiveEvE.C
Will read the reduced ntuple file and output a root file with histograms.
Actually would be nice if it appended those histograms to the file .root it reads
### OUTPUT
TH1D hSumMX histogram of radii of all multiplicities
TH1D hSumM2 histogram of radii of M2

## [MANDATORY] Produce Matrix of corrections for the accidentals / true coincidences from MC
### REQUIRES:   PlotCoincidencesEnergyVsEnergy output.root and MC file of calibration *_g4cuore.root 
PlotEnergySpectrumCalibration.C
Computes a matrix of corrections with bin 1keV * 1keV
Right now only M2 is supported.
# OUTPUT
Append to .root of data a TH2D with the corrections from the MC

##[MANDATORY] [INTERACTIVE] Produce Efficiency plots for signal and bkg, and Purity : Efficiency curve
### REQUIRES: PlotEnergySpectrumCalibration.C (correction matrix)
ComputeOptimalCut.C
Produces different plots of efficiencies and score function S/sqrt(S+B) as a function of R to select the best radius

## PlotTotalEnergy
This macro is supposed to plot the total energy spectra taken from 2 different R processing in order to see the improvements on the thallium line 