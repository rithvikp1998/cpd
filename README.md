# cpd
All the code related to Common Print Dialog project of GSoC '17

## Intro
I am participating in the Google Summer of Code 2017 working for the OpenPrinting division of The Linux Foundation. The goal of the project is to build a Print Dialog in Qt in order to make the process of printing simple and uniform across multiple platforms and devices. The project idea can be found here: https://wiki.linuxfoundation.org/gsoc/google-summer-code-2017-openprinting-projects I am one of the two students working on the dialog itself while there are other students working on different backend APIs(CUPS, IPP, Google Cloud print etc.). 

Please note that the documentation (including License information) for this section of the project will be added during the last phase of GSoC i.e. around the first week of August, 2017 and this is NOT the official repository of the project. 

## Build From Source

Please note that for the build to work, you need to have Qt 5.8 installed. You can download and install it from Qt's website here: https://www.qt.io/download-open-source/

If you have Qt Creator installed, you can open the project from Qt Creator and run it. If you want to build the code manually, follow these instructions:

```
git clone https://github.com/rithvikp1998/cpd.git
cd cpd
mkdir build; cd build
qmake ../cpd.pro
make
cp ../test.pdf /tmp/
./cpd
```
