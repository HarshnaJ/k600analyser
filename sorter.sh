#!/bin/bash


for COUNT in {1108..1109}


do
  echo "inside DO loop, with runnr = " $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/home/neveling/DATA/MID/PR236/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/home/neveling/DATA/MID/PR236/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/home/neveling/DATA/MID/PR236/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/home/neveling/DATA/MID/PR236/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/home/neveling/DATA/MID/PR236/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo "midas file name: " $FILE

  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR260/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR260/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR260/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR260/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR260/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo "and desired rootfile name : " $FILE2

  if [ -e $FILE ]
  then
      echo "entered IF loop to start analyzer"

      if [ ! -e $FILE2 ]
      then
      echo $FILE
#       ./analyzer -i /media/Elements/PR226/run000"$COUNT".mid.gz
      echo "./analyzer -i "$FILE
      ./analyzer -i $FILE
#       mv output.root /media/Elements/PR226/sorted000$COUNT.root
      mv output.root $FILE2
      fi
  fi
done
