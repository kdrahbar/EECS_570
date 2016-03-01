#!/bin/bash
#PBS -S /bin/bash
#PBS -N beamform
#PBS -l nodes=Typhon2:ppn=1:gpus=1
#PBS -l walltime=00:05:20
#PBS -q test

# Ultrasound Beamforming job submission script for EECS 570.

INPUT_SIZE=64

if [[ $PBS_JOBID == "" ]] ; then

  # Not running under PBS
  USE_LOCAL_MIC_NUMBER=3
  echo "Running interactive job. Use 'qsub 570_pbs_submit.sh' to submit a batch job."
  micnativeloadex $1.mic -t 300 -d $USE_LOCAL_MIC_NUMBER -a "$INPUT_SIZE" 
  micnativeloadex solution_check.mic -t 30 -d $USE_LOCAL_MIC_NUMBER -a "$INPUT_SIZE" 

else

  # Running batch job under PBS
  HOST=`cat $PBS_NODEFILE`
  MICNUM=$(cat $PBS_GPUFILE | cut -c12-)
  USERDIR=/n/typhon/home/$USER/570_p1_w1
  OUTFILE=$USERDIR/$PBS_JOBID.stdout
  echo "I'm running on: $HOST mic$MICNUM" > $OUTFILE
  # Launching job to MIC
  # timeout set to 5 minutes
  micnativeloadex $USERDIR/beamform.mic -t 300 -d $MICNUM -a "$INPUT_SIZE" >> $OUTFILE
  micnativeloadex $USERDIR/solution_check.mic -t 30 -d $MICNUM -a "$INPUT_SIZE" >> $OUTFILE

fi

