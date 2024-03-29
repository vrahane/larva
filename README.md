# Mynewt Repository 

# Overview

This is the main Mynewt development repository.  It contains the major Mynewt
packages that are being worked on, along with a few projects used for 
compiling the packages into working projects, these are: 

* bin2img: takes a compiled binary, and generates a image file suitable 
  for use with the stack bootloader. 
* boot: Project to build the bootloader for test platforms. 
* main: Basic project for test platforms, that includes and builds all 
  relevant packages. 
* test: Test project which can be compiled either with the simulator, or 
  on a per-architecture basis.  Test will run all the package's unit 
  tests. 

# Packages 

The dev\_test repository includes the following packages: 

* OS: Core RTOS 
* CMSIS-CORE: Cortex M abstractions, packaged from ARM mbed distribution.
* FFS: Flash filesystem, optimized for small flashes. 
* FFSUTIL: Utility functions for using the FFS package. 
* BOOTUTIL: Core bootloader functions used by the boot project. 
* NETUTIL: Network utilities shared amongst networking stacks. 

These packages are imported into this repository using git subtrees.  To see a 
full set of remote repositories, look at the setup-remotes.sh script, which 
adds the remote repositories.

Additionally, this repostiory contains board support packages for the following 
boards: 

* olimex\_stm32-e407\_devboard: Board Support for the Olimex STM32-E407 
* olimex\_stm32-e407\_devboard-boot: Board Support for the Olimex STM32-E407 bootloader. 

And MCU related packages for the following microcontrollers: 

* STM32F4XX: MCU definitions for the STMicro F4XX processor series.
* Native: Native (Simulated) MCU definitions.

In order to update these repositories, git subtree merging should be employed, 
where changes are made locally and tested within the dev\_test repository first, 
and then pushed upstream once tested. 

# License 

The code in this repository is all under either the Apache 2 license, or a license 
compatible with the Apache 2 license.  See the LICENSE file for more information. 

# Contact 

For any questions on the dev\_test repository, please contact the Mynewt 
developer's list (stack-dev@googlegroups.com).  
