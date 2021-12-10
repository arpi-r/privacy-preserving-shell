# Secure and Privacy-Preserving Shell as a Service

We provide 4 system calls and wrappers for them:

 - pps_create: To create a new service
 - pps_call: To run an existing service
 - pps_list: To list all services a user can run
 - pps_show: To display all information of a given service

More details of this work can be found in our Final Report <TODO: add link>.

The implementation is based on the 5.14 release version of the Linux Kernel.

## Compiling and Booting the Linux Kernel code (including the PPS implementation)

 - Clone `https://github.com/arpi-r/privacy-preserving-shell`
 - Enter the repository's directory: `cd privacy-preserving-shell`
 - `cp /boot/config-$(uname -r) .config`
 - Set `CONFIG_SYSTEM_TRUSTED_KEYS=""` in the `.config` file
 - make menuconfig
 - `sudo make -j n`
 - `sudo make modules_install -j n`
 - `sudo make install -j n`
 - n in the above 3 commands is the number of processing units available: can be found by running `nproc`
 - Restart the system to boot into the newly built kernel (ensure that grub is updated with each new build)
 
## Using PPS wrappers

The PPS wrapper binaries can be found in the ppswrappers folder. Enter the directory: `cd ppswrappers`.

 - To create a new service run: `./pps_create`
 - To run an existing service: `./pps_call <name> <owner_uid> [-p]` (-p is optional, if you want to run the service using a password)
 - To list all services: `./pps_list`
 - To display information about a given service: `./pps_show`
