#!/bin/bash
# File : test.sh
# Author : Adam Švenk
# Date : 2022-04-10
# Version : 1.0

#Počet čísiel
numbers=8;

#Preloženie zdrojového súboru oems.c
mpic++ --prefix /usr/local/share/OpenMPI -o oems oems.c

#Vytvorenie súboru s 8 náhodnými vstupnými číslami
dd if=/dev/random bs=1 count=$numbers of=numbers status=none

#Sputenie preloženého programu
mpirun --prefix /usr/local/share/OpenMPI -oversubscribe -np 19 oems

#Odstránenie vytvoreného súboru
rm -f oems numbers
