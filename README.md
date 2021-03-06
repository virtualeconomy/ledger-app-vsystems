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
Quit docker instance or open other terminal. Make sure these following libraries are installed.

```
sudo apt-get install libudev-dev libusb-1.0-0-dev python-dev virtualenv zlib1g-dev
```

Go to your workspace and make sure you have access right to the workspace:

```
cd ~/workspace/ledger/ledger-app-vsystems
sudo chown -R <user>:<user> .
```

To avoid hid conflict issue, we'd better setup and run `ledgerblue` in python virtual environment:

```
virtualenv venv
source venv/bin/activate
pip install --upgrade setuptools
pip install triangle
pip install --upgrade cython
pip install ledgerblue
```

On Linux you need to create a set of udev rules to allow device access. To grant access permision, you need run:

```
sudo groupadd plugdev
sudo gpasswd -a <user> plugdev
wget -q -O - https://raw.githubusercontent.com/LedgerHQ/udev-rules/master/add_udev_rules.sh | sudo bash
```

Note: If connection issue is still happend, please refer [here](https://support.ledger.com/hc/en-us/articles/115005165269-What-if-Ledger-Wallet-is-not-recognized-on-Linux-) to solve.

Now install app to your ledger. Connect ledger to PC. Enter PIN to unlock screen and be on the main menu screen. And then

```
python -m ledgerblue.loadApp --appFlags 0x40 --path "44'/360'" --curve secp256k1 --curve ed25519 --tlv --targetId 0x31100004 --delete --fileName bin/app.hex --appName "VSYS" --appVersion 1.0.0 --icon 0100000000ffffff00ffffffffffffffffc7e38fe31ff71fff3ffe7ffe7ffefffeffffffffffffffff --dataSize `cat debug/app.map |grep _nvram_data_size | tr -s ' ' | cut -f2 -d' '` 
```

## Test
After install successfully, you can start to use ledger hardware to do the payment and leasing. To test payment and leasing, please refer [here](https://github.com/virtualeconomy/vsystems-ledger-js).