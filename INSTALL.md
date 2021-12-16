NexDome Beaver driver INSTALL
===================================

You must have git, CMake >= 2.4.7 and indilib in order to build this package.

1) cd to a work directory
2) $ git clone https://github.com/sifank/Beaver.git
3) $ cd Beaver
4) $ mkdir build
5) $ cd build
6) $ cmake -DCMAKE_INSTALL_PREFIX=/usr . ../
7) $ make
8) $ sudo make install

That's it - you'll have the Beaver driver listed in the Dome section
... and you can remove the "build" folder.

