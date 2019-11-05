#!/bin/sh
cd qrngdownload
./qrngdownload qdata 1 USERNAME PASSWORD && od qdata -i --address-radix=n > qdata.dec && rm qdata
# replace USERNAME and PASSWORD in above line with QRNG service credentials, see https://qrng.physik.hu-berlin.de/register/
cut -c2-12 qdata.dec > qdata.formatted
cut -c14-24 qdata.dec >> qdata.formatted 
cut -c26-36 qdata.dec >> qdata.formatted && rm qdata.dec
line=1                                 ##Starting line to read from QRNG data
NUMb=`expr $line + 1`                 
NUMc=`expr $line + 2`
NUMd=`expr $line + 3`                                        ##begin filtering out numbers from data
NUMA=`sed -n \
    -e "${line}p" \
    -e 's/^[ \t]*//'  qdata.formatted`
NUMB=`sed -n \
    -e "${NUMb}p" \
    -e 's/^[ \t]*//'  qdata.formatted`
NUMC=`sed -n \
    -e "${NUMc}p" \
    -e 's/^[ \t]*//'  qdata.formatted`
NUMD=`sed -n \
    -e "${NUMd}p" \
    -e 's/^[ \t]*//'  qdata.formatted`
NUMS=$NUMA","$NUMB","$NUMC","$NUMD"X"  ##this is the string sent to Arduino
#begin arudino comms
echo -n "Arduino location: "
read ard
#while true; do                        ##commented out lines allow the script to loop
stty -F $ard cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts -hupcl
tail -f $ard &
exec 3<> $ard
echo $NUMS >&3
echo "sent '"$NUMS"' to Arduino"
exec 3>&-
#    line=`expr $line + 3`
#    NUMb=`expr $line + 1`
#    NUMc=`expr $line + 2`
#    NUMA=`sed -n \
#    -e "${line}p" \
#    -e 's/^[ \t]*//'  qdata.formatted`
#    NUMB=`sed -n \
#    -e "${NUMb}p" \
#    -e 's/^[ \t]*//'  qdata.formatted`
#    NUMC=`sed -n \
#    -e "${NUMc}p" \
#    -e 's/^[ \t]*//'  qdata.formatted`
#    NUMS=$NUMA","$NUMB","$NUMC"X" 
#    sleep 10s
#done
