# Study for an Aeolian Gate
This repository archives code and documentation for "Study for an Aeolian Gate", an artwork I made in 2013.

The "study" obtains random numbers from [a quantum random number generator operated by Humboldt
University of Berlin](https://qrng.physik.hu-berlin.de/). These numbers are taken as input by an Arduino sketch, which uses them to determine activation 
duration and speed of a small industrial cooling fan.

## Files
- `random.ino` Arduino sketch
- `random.sh`
- `/qrngdownload` This directory contains source and executable files for `qrngdownload`, a copy of those [provided by Humboldt
University of Berlin for use with their quantum random number generator service](https://qrng.physik.hu-berlin.de/download).

## Hardware description
This piece was conceived as a study for an as-yet unrealized sculpture or architectural intervention. Therefore, the
 hardware implementation used the minimum possible components to establish technical details and core conceptual
 motivations. 
 
 The key components are as follows:
 - Linux instance to run `random.sh` (and qrngdownload)
 - Arduino UNO loaded with `random.ino`, connected to Linux instance via USB to load random data
 - Sparkfun Ardumoto (Arduino-compatible shield using a L298 H-bridge)
 - 92 mm x 92 mm industrial cooling fan, 12VDC 0.18A (Mcmaster-carr part 1939K35)
 - 12 V power supply
 
 ## Disclaimer
 This code and associated information is supplied here for archival/informational purposes, and carries no expressed
 or implied warranty or guarantee. Please thoroughly test in a safe environment before using this code in any device.
