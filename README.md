
## Resources

* Website: https://purkproject.com
* Docs: https://docs.purkproject.com
* Explorer: https://explorer.purkproject.com
* API: https://api.purkproject.com
* Donate: https://donate.purkproject.com
* Github: https://github.com/purkproject/purk
* Discord: https://discord.gg/cqmSrfz

## Introduction

Purk is an open source, ASIC resistant, secure and untraceable cryptocurrency for donations, tips and instant private payments with minimal transaction fees. Based on CryptoNote technology, Purk helps balance CPU and GPU mining speeds, which encourages fair and equal distribution whilst providing extremely fast block verification times.

## Purk Button

The Purk Button is accessible from the GUI wallet. Users can generate an embed code to accept Purk as a donation on their website or application. Visit the "Purk Button" page in the GUI wallet to get started. The Purk Button is compatible with Chrome, Firefox, Opera and soon Microsoft Edge and Internet Explorer [[1]](https://developer.microsoft.com/en-us/microsoft-edge/platform/issues/11963735/). Safari is not supported [[2]](https://bugs.webkit.org/show_bug.cgi?id=171934).

## Purk Button API

You can use the Purk Button API to build advanced websites and applications that accept donations and micropayments. Whether you want to monetize website content, create fundraising platforms or raise money for charitable causes; the Purk Button API has everything you need to integrate Purk with your website and retrieve detailed statistics about payments and donations being made by your users.

## About this Project

This is the core implementation of Purk. It is open source and completely free to use without restrictions. There are no restrictions on anyone creating an alternative implementation of Purk that uses the protocol and network in a compatible manner.

Anyone is able to contribute to Purk. If you have a bug fix or code change, feel free to submit it as a pull request. In cases where the change is relatively small or does not affect other parts of the codebase it may be merged in immediately by any one of the lead developers. If the change is particularly large or complex, it it will be discussed at length either well in advance of the pull request being submitted, or even directly on the pull request.

## Features

* Purk Button
* Purk Donations API
* Wallet Address Aliases
* Untraceable payments
* Unlinkable Transactions
* Analysis Resistant
* Adaptive Limits
* Self Funding

## Specifications

* Algorithm: Wild Keccak
* Supply: 179,424,673 PURK
* Block Retarget: 90 Seconds
* Emission: 21
* Default P2P Port: 10110 
* Default RPC Port: 10120 
* Premine: 0%
* Development Bounty: 0-1% (Controlled by votes)

## Deployment

### Linux

Dependencies: GCC 4.7.3 or later, CMake 2.8.6 or later, and Boost 1.53 (but don't use 1.54) or later. You may download them from:

* http://gcc.gnu.org/
* http://www.cmake.org/
* http://www.boost.org/

Alternatively, you can follow the command line tutorial below. Tested on Ubuntu 14.04.5 x64. 

**Get Required Libraries**

```
sudo apt-get update
sudo apt-get install build-essential g++ python-dev autotools-dev libicu-dev build-essential libbz2-dev cmake git
```

**Install GCC 4.8**

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
sudo apt install g++-4.8
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
```

**Install Boost**

```
mkdir tmp
cd tmp
wget -O boost_1_53_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.53.0/boost_1_53_0.tar.gz/download
tar xzvf boost_1_53_0.tar.gz
cd boost_1_53_0/
./bootstrap.sh --with-libraries=system,filesystem,serialization,program_options,date_time,thread,regex,atomic,iostreams,log,locale,wave --prefix=/usr/local
sudo ./b2 install
sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf'
sudo ldconfig
```

**Git Clone**

`git clone https://github.com/purkproject/purk.git`

**Compile Purk**

```
cd purk
make
```

###  MacOS X

Dependencies: GCC 4.7.3 or later, CMake 2.8.6 or later, and Boost 1.53 (but don't use 1.54) or later. You may download them from:

* http://gcc.gnu.org/
* http://www.cmake.org/
* http://www.boost.org/

Alternatively, it may be possible to install them using a package manager.

More detailed instructions for OS X (assume you’re using MacPorts (they’re, however, pretty self-explanatory and homebrew users shouldn't have troubles following it too):

* Install latest Xcode and command line tools (these are in fact MacPorts prerequisites)
* Install GCC 4.8 and toolchain selector tool: `sudo port install gcc48 gcc_select`
* Set GCC 4.8 as an active compiler toolchain: `sudo port select --set gcc mp-gcc48`
* Install latest Boost, CMake: `sudo port install boost cmake`

To build, change to a directory where this file is located, and run `make`. The resulting executables can be found in `build/release/src`.

**Advanced options**:

Parallel build: run `make -j<number of threads>` instead of just `make`.

Debug build: run `make build-debug`.

Test suite: run `make test-release` to run tests in addition to building. Running `make test-debug` will do the same to the debug version.

Building with Clang: it may be possible to use Clang instead of GCC, but this may not work everywhere. To build, run `export CC=clang CXX=clang++` before running `make`.

### Windows

Dependencies: MSVC 2012 or later, CMake 2.8.6 or later, and Boost 1.53 (but don't use 1.54) or later. You may download them from:

* http://www.microsoft.com/
* http://www.cmake.org/
* http://www.boost.org/

To build, change to a directory where this file is located, and run this commands: 
```bash
 mkdir build
 cd build
 cmake -G "Visual Studio 11 Win64" ..
```

And then do Build.
Good luck!

## Donate

* Donate: https://donate.purkproject.com
* PURK: @ionictong
* BTC: 16irMH3VZEKxZ4h3ZZq2NhGUfNskG6u61o
