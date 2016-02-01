icc -qopenmp -o beamform beamform.c 
icc -o baseline baseline.c
icc -o solution_check solution_check.c
./beamform $1
./solution_check $1
./baseline $1
rm beamform

