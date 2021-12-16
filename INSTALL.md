NexDome Beaver driver INSTALL
===================================

You must have git, CMake >= 2.4.7 and indilib in order to build this package.

1) start from the 3rdparty folder (~/Projects/indi/3rdparty)
2) $ git clone https://github.com/sifank/Beaver.git
3) $ mkdir build
4) $ cd build
5) $ cmake -DCMAKE_INSTALL_PREFIX=/usr . ../
6) $ make
7) $ sudo make install

That's it - you'll have the Beaver driver listed in the Dome section
... and you can remove the "build" folder.

