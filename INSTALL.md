HowTo Build MPC
==========================

MPC use the GNU configure tools, which includes autoconf,
automake and libtool.  This enables high levels of portability and ease
of use. Below is an example of how one might build and install the library in
the `/usr` directory:

    ./autogen.sh
    ./configure --prefix=/usr
    make
    sudo make install
