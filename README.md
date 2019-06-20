# ledger-app-vsystems
V Systems wallet application for Ledger Nano S &amp; Blue

## Development environment setup
Developping applications for Ledger Nano S &amp; Blue requires two compilers :

* A standard ARM gcc to build the non-secure (STM32) firmware and link the secure (ST31) applications
* A customized ARM clang with ROPI support to build the secure (ST31) applications

To setup the development environment in easy way, we use docker to setup. For example, your workspace is **~/workspace/ledger**, just run:

```
docker run -t -v ~/workspace/ledger:/home/ledger -i sheldonys/ledger-devenv:latest
```

For detail environment setup, please refer: https://github.com/LedgerHQ/blue-devenv/

## Build
In docker instance, we clone **ledger-app-vsystems** from github and build it. For example, we build it with Nano SDK:

```
cd /home/ledger
git clone https://github.com/virtualeconomy/ledger-app-vsystems.git
cd ledger-app-vsystems
make BOLOS_ENV=/opt/ledger-blue/ BOLOS_SDK=/home/nanos-secure-sdk
```

## Install to your Ledger device
Open other terminal and make sure these following libraries are installed.

```
sudo apt-get install libudev-dev libusb-1.0-0-dev python-dev virtualenv zlib1g-dev
```

To avoid hid conflict issue, we'd better setup and run `ledgerblue` in python virtual environment:

```
cd ~/workspace/ledger/ledger-app-vsystems
virtualenv venv
source venv/bin/activate
pip install --upgrade setuptools
pip install triangle
pip install --upgrade cython
pip install ledgerblue
```

Now install app to your ledger. Connect ledger to PC. Enter PIN to unlock screen and be on the main menu screen. And then

```
python -m ledgerblue.loadApp --appFlags 0x40 --path "44'/360'" --curve secp256k1 --curve ed25519 --tlv --targetId 0x31100004 --delete --fileName bin/app.hex --appName "VSYS" --appVersion 1.0.0 --icon 0100000000ffffff00ffffffffffffffffc7e38fe31ff71fff3ffe7ffe7ffefffeffffffffffffffff --dataSize `cat debug/app.map |grep _nvram_data_size | tr -s ' ' | cut -f2 -d' '` 
```