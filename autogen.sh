#!/bin/sh
# As seen on
#   Autotools A Practitioner's Guide to GNU Autoconf, 
#     Automake, and Libtool by John Calcote
#
# autoreconf runs all the autotool configuration tools in the right order
# and will avoid regenerating files. 
# 
autoreconf --install --make # install missing files
# automake --add-missing --copy >/dev/null 2>&1 # add install-sh 