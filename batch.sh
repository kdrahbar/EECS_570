icc -opt-prefetch-distance=20 -fp-model fast=2 -fimf-precision:low -parallel -no-prec-div -ansi-alias -O3 -no-prec-div -openmp -o $1.mic -mmic $1.c 
icc -o solution_check.mic -mmic solution_check.c
./570_pbs_submit.sh $1
if [ $# -eq 1 ]
  then
    rm $1.mic
fi
