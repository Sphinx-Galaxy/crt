# General Information

This Configurable Raditation Testsuite was designed for testing (multiple) devices under irradiation. It was made it the Qt framework and it presents different component groups in the various tabs. E.g. the first tab is all the power supplies in the test-setup, here the voltage and current can be set and the supply switched on/off either manually or automatically.

# Documentation

The user manual can be found in the 'doc' folder, it explains the manual use as well as the automated creation of configuration files.

# Configurations

In the 'configs' folder are several configurations from practical radiation test and also a few example configurations. Additionally a python script 'LBJ_to_CRT.py' can be used to generate a labjack configuration from a *.csv file derived from the Labjack_Setup.xlsx.

# Build Prerequisites

## QtCreator

As the CRT is build on the qt-framework, QtCreator 4.14.1 with Qt 5.15.2 were used.

## Labjack

To install the Labjack library download the [LJM Software](https://labjack.com/support/software/installers/ljm).

## Liblxi

The [LXI](https://de.wikipedia.org/wiki/LAN_eXtensions_for_Instrumentation) is used for the Rhode&Schwarz power supplies. To install it on a linux, follow one of the two guides below.

### Debian Linux

The lxi library is in the official paket store:

```
$ sudo apt-get install liblxi-dev
```

### Arch Linux

The lxi library for measurement devices is not officially supported on arch-linux, therefore it needs to be build manually. First install necessary packages like:

* autoconf
* autoreconf
* libtirpc
* rpcsvc-proto
* libxml2

and then get the git package and build it:

```
$ git clone https://github.com/lxi-tools/liblxi
$ cd liblxi
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
```

**IMPORTANT:** So far the lxi library seems to be not completly working on arch linux **!**
