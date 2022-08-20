FROM debian:11

RUN apt-get update -qq && apt-get upgrade -y && apt-get dist-upgrade -y
RUN apt-get install build-essential cmake git liblxi-dev make pkgconf qt5-qmake qtbase5-dev qtcreator sudo wget -y

### EDIT HERE if you want to change your username and password
RUN useradd -m sphinx && echo sphinx:sphinx | chpasswd && usermod -aG sudo sphinx
###

RUN wget https://labjack.com/sites/default/files/software/labjack_ljm_software_2019_07_16_x86_64.tar.gz
RUN tar -xzf labjack_ljm_software_2019_07_16_x86_64.tar.gz
RUN labjack_ljm_software_2019_07_16_x86_64/labjack_ljm_installer.run
RUN rm -r labjack*

ENV LANG de_DE.utf8

ADD . /control
