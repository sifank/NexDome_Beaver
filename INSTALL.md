NexDome Beaver driver INSTALL
===================================

You must have git, CMake >= 2.4.7 and indilib in order to build this package.

In a working directory of your choosing:
1) $ git clone https://github.com/sifank/Beaver.git
2) $ cd Beaver
3) $ mkdir build
4) $ cd build
5) $ cmake -DCMAKE_INSTALL_PREFIX=/usr . ../
6) $ make
7) $ sudo make install

Potential Issue
====================================
Since this will build 'outside' of the indi-3rdparty structure, you might get
the error: *** No rule to make target '/usr/lib/libindidriver.so'
As long as you have indilib installed, it's on your system, just not under /usr/lib.
To fix:
1) $ locate libindidriver.so
    1) If you don't have locate, it can be installed with: $ sudo apt install locate
    2) Then update it's db: $ sudo updatedb
2) $ cd /usr/lib; ls -ld libindidriver*   # make sure what this link points to does not 
                                          # exist (eg libindidriver.so.1), eg, only one libindidriver
3) $ sudo rm /usr/lib/libindidriver.so    # remove empty link
4) $ sudo ln -s /usr/lib/x86_(replace from locate above)/libindidriver.so libindidriver.so
   (example on my Raspberry Pi):  sudo ln -s /usr/lib/x86_64-linux-gnu/libindidriver.so libindidriver.so)

That's it - you'll have the Beaver driver listed in the Dome section
... and you can remove the "build" folder.

