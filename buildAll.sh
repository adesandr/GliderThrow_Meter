#!/bin/bash
# This script build or clean all the Esp_mad project

# Print usage function
printUsage() {
    echo "Usage:"
    echo "    -b|--build \"build all project\""
    echo "    -c|--clean \"clean all project\""
}

# Delete build File if exist function
delFileIfExist() {
    if [ -e resultBuild.txt ]; then 
        rm resultBuild.txt
    fi
}

# Execute idf.py function
executeCmd() {
    echo "$1 Esp_Mad_Server and Esp_mad_Client, please wait ..."
    echo "Result is stored in resultBuild.txt"
    delFileIfExist
    dateStart=$(date +%s)
    cd ./Esp_mad_Server
    idf.py $1 $2 >> ../resultBuild.txt
    cd ../Esp_mad_Client
    idf.py $1 $2 >> ../resultBuild.txt
    cd ..
    echo >> ./resultBuild.txt
    echo "<---------------- Elapse Time for the $1 ---------------->" >> ./resultBuild.txt
    echo >> ./resultBuild.txt
    dateEnd=$(date +%s)
    elapseTime=$(( $dateEnd - $dateStart ))
    echo "$1 duration is equal to $elapseTime s" >> ./resultBuild.txt
    tail -n 4 ./resultBuild.txt
}

# Check the number of parameters
if [ $# -gt 1 ] || [ $# -eq 0 ]; then
    printUsage
    exit 1
fi

# Execute regarding the parameter
if [ $1 = "-b" ] || [ $1 = "--build" ]; then
    executeCmd "build" "size"
    grep "Total image size" ./resultBuild.txt
elif [ $1 = "-c" ] || [ "$1" = "--clean" ]; then
    executeCmd "clean"
else
    printUsage
fi