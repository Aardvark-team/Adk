# Just type the name of the example after the bash script

./AdkC/bin/main $1.adk &&
./AdkASM/bin/adkasm $1.adkb $1.adkc &&
./AdkVM/bin/adkvm $1.adkc