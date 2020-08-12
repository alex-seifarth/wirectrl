# Description
*wirectrl* provides a systemd service('daemon') *wirectrld* that allows to set
GPIO line values via an DBus interface on the system bus. The GPIO line must
be configured with public name in a configuration file 
(*/etc/wirectrl/wirectrld.conf*).

To simplify access to DBus a client library and a console application is provided 
(**to be done**).

*wirectrl* is based on *systemd* (sd-event, sd-bus, sd-journal) and *gpiod* and
Linux Debian **bullseye* and Pi OS **buster** (raspbian). There is no support for 
other systems.

# Building, Installation
Version 0.1 can be installed only via building from source. It is planned to provide
deb-packages in later versions.

## Build from Source
### Prerequisites
* cmake >= V3.13
* C++ compiler wiuth C++ 2017 support (tested is GCC >= 8.3)
* git (required to checkout source from github)
* systemd and systemd-dev (tested with version 241 and 246)
* gpiod (libgpiod2) and gpiod-dev (tested with libgpiod-dev 1.2)

These requirements can be installed in one rush on Debian (bullseye) 
or Pi OS (version 10 buster) with
```bash
sudo apt install build-essential cmake git libsystemd-dev libgpiod-dev gpiod
```

### Checkout Source, Configure, Build and Install
After all prerequisites have been installed you should now download the source and
configure, build and install the software. *wirectrl* uses cmake (Kitware) as build
system:
```bash
git clone https://github.com/alex-seifarth/wirectrl.git
mkdir wirectrl-build
cd wirectrl-build
cmake -DCMAKE_BUILD_TYPE=Release ../wirectrl
make
sudo make install
```
Note that it is not possible to set the installation directory via 
CMAKE_INSTALL_PREFIX, because wirectrld is a daemon that will always be installed
in root's binary directory (see below). 

The service is not started or enabled yet - see below. 

### Installed Artifacts
*wirectrl* v0.1 installs the following components in the system
* */usr/sbin/wirectrld*: This is the systemd service executable (e.g. the daemon). It 
    is expected to run as root.
* */etc/dbus-1/system.d/de.titnc.pi.wirectrl.conf*: DBus policy file that allows 
    *wirectrld* to claim the name **de.titnc.pi.wirectrl** on the system bus and enables
    DBus property and inspection functions.
* */etc/systemd/system/wirectrl.service*: Systemd configuration file required to manage
    *wirectrld* by system (e.g. use **systemctl** commands)
* */etc/wirectrl/wirectrld.conf*: The configuration file for the *wirectrld* service.
    This is the only file you should have to touch, in it the GPIO line have to be 
    configured.
 
### Logging, Configuring, Starting and Enabling *wirectrld*
After installation the service is not started or enabled. 
It requires some configuration before it can do useful things. The configuration
of the service is done in the file */etc/wirectrl/wirectrld.conf*. Changing this 
file requires super-user privileges.

While doing changes on the configuration file it might be useful to watch the log 
output from *wirectrld*. Therefore one should open a terminal and enter:
```bash
sudo journalctr -f
```
It is necessary to use *sudo* here because normal user don't see the output of other 
users on the systemd journal.

Now we can edit the configuration file. Note that the daemon must be restarted to have
your changes taking effect.
```bash
sudo nano /etc/wirectrl/wirectrld.conf
```
The leading section [dbus] should not be touched except if you're going to develop 
extension of modifications to the DBus-interface of *wirectrld*. Wrong values may easily
put the service in a dysfunctional state.

To configure a single GPIO line a [gpio] section should be added to the configuration file:
```
[dbus]
connection-id = "de.titnc.pi.wirectrl"
object-id = /de/titnc/pi/wirectrl/v1
use-session-bus = false

# configuration per gpio line
[gpio = 0-17]
name = "myGpioLineName"
consumer = "myGpioLineNameOrSomethingElse"
init-level = inactive
active-level = high
```
The [gpio] section requires a value in the form *<gpio-chip>-<line-number>* where
*<gpio-chip>* is the identity of a gpio chip or its index. This follows the nomenclatura 
of *libgpiod*. You can check which gpio chips exist either by
```bash 
ls /dev | grep "gpio"
```
or 
```bash
gpiodetect
```
If the latter command is not available, the *gpiod* package is not installed on your 
system.
The *<line-number>* is an unsigned integer identifying the GPIO in/out line of the
chip.

There are four fields in a gpio section:
* name: The name is a string that is used to identify the GPIO line on the DBus 
    interface. The name must be unique.
* consumer: This string is set on the GPIO line when *wirectrld* takes hold of the 
    GPIO line. This name appears then on the ```gpioinfo``` command on the respective
    line.
* init-level: This can be ``active`` or ``inactive`` and is set as initial line level
    when *wirectrld* starts.
* active-level: This cane ``low`` or ``high`` and sets wether an active line means
    high or low voltage on the output.

After the configuration is complete you can save the file and start the service:
```bash 
sudo systemctl start wirectrl
```
Check the systemd journal output for failure messages regarding *wirectrl*.

To start the service automatically with each system start you have to enter
```bash
sudo systemctl enable wirectrl
```

# Maintainers
* Alexander Seifarth