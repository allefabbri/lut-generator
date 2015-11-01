# lut-generator

This repo contains a multipurpose _Look Up Table_ generator. It is oriented to microcontrollers world where runtime computation is something you may want to avoid when generating signal samples. This tool generates the sampled signal in the form of an array which can be copy-pasted directly in the microcontroller script. There is also a gnuplot-based display functionality

A number of _built-in_ wave forms are avalaible:
* straight line
* sine
* cosine
* square
* triangle
* amplitude modulation

## Requirements
* A **C++11** compatible compiler
* GNU ``make``
* Gnuplot (only for the graphic part)

## Installation
To install type ``make all`` from within the project folder. The code has been tested successfully with __GCC 4.8.4__ under Ubuntu 14.04 but, due to the use of the functions ``std::stof`` and ``std::stoi`` which are bugged in MinGW, it will throw compilation errors if compiled in Windows enviroment. A __VS__ version will come, sooner o later...

## Usage
When executed the program reads the parameters from the configuration file ``config.conf``, e.g.
```
signal_amplitude = 5
signal_samples = 500
signal_type = sine
dac_bit_size = 12
```
where
* ``signal_amplitude`` is the maximum amplitude of the wave form, which ranges from 0 to this value. Default is ``1``.
* ``signal_samples`` the number of samplings of the LUT. Default is ``100``.
* ``signal_type`` selects the wave form, pick one from ``line sine cosine square triangle am``. Default is ``line``.
* ``dac_bit_size`` is the number of bits used by the _Digital To Analog_ converter to store the amplitude range.

## Output
A number of files are generated:
* ``lut.scale`` displays the amplitude bin in terms of the DAC bit representation.
* ``lut.dat`` contains the sampled signal in amplitude scale and DAC scale, used by gnuplot script.
* ``lut.gp`` contains the auto-generated gnuplot script to create the plot.
* ``lut.out`` contains a report of the parameters used and also the LUT in decimal, hex and binary format.
* ``lut.png`` image with the plots of the signal contained in ``lut.dat``.

## Mathematical Note
The code is open source so feel free to modify it according to your needs. If you want to add your own wave form remind that the logic of the program requires that you use a mathematical representation of it which ranges from 0 to 1 and is periodic with period 1.

## Programming Note
For what concern the amplitude modulation wave form the default parameters are
* ``omega = 25`` the carrier period
* ``modulation = 0.5`` the modulation parameter

These values are hard-coded, as preprocessor definitions, into the program so you need to modify them inside ``lutg.cpp``.
