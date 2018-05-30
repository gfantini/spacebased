#!/bin/bash

echo "This script will look up for the MC (g4cuore) files."
echo "===================================================="

PATH_LIST_MC="/nfs/cuore1/data/simulation/CUORE/BkgModel/fit_reference/ListMC.txt"
PATH_HAYSTACK="/nfs/cuore1/data/simulation/CUORE/"
echo "[INFO] PATH_LIST_MC = ${PATH_LIST_MC}"
echo "[INFO] PATH_HAYSTACK = ${PATH_HAYSTACK}"
echo "[INFO] For each file listed, will run find -L <HAYSTACK> -name <LINE>"
echo "****************************************************"
echo " "

COUNTER=1;
awk '{ print $1; }' ${PATH_LIST_MC} | while read in
do 
    echo "N${COUNTER} ${in}"
    find -L ${PATH_HAYSTACK} -name ${in}
    echo "-----------------------------------------------------"
    COUNTER=$((COUNTER+1))
done