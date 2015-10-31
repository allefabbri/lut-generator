// LUT-GENERATOR
// Copyright 2015, Alessandro Fabbri
// for any question, please mail rommacc@yahoo.it

/************************************************************************
* This program is free software: you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

// Standard includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>

// Input and output file definitions
#define CONFIG      "config.conf"
#define SCALE       "lut.scale"
#define DATA        "lut.dat"
#define GNUPLOT     "lut.gp"
#define OUTPUT      "lut.out"
#define IMAGE       "lut.png"

// Amplitude Modulation parameters
#define OMEGA_AM         25
#define MODULATION_AM    0.5

using namespace std;

// Signal functor class
class periodic_signal{
    // Built-in functions
    static double signal_line   (double t) { return t; }
    static double signal_sin    (double t) { return .5*(1+sin(2*M_PI*t)); }
    static double signal_cos    (double t) { return .5*(1+cos(2*M_PI*t)); }
    static double signal_square (double t) { return (t<0.5)?1:0; }
    static double signal_triang (double t) { 
        if ( t < 0.25 ) return 0.5+2*t;
        else if ( t<0.75 ) return 1.0-2*(t-0.25);
        else return 2*(t-0.75); 
    }
    static double signal_am     (double t) {
        return 0.5+sin(OMEGA_AM*2*M_PI*t)*(1+MODULATION_AM*sin(2*M_PI*t))/(1+MODULATION_AM)*0.5; 
    }
    // Function pointer to dynamically address the choosen function
    double (* signal) (double t);
    // Method called by various contructor
    void select_signal(string sig_type){ 
        if      ( sig_type == "line"     ) signal = &signal_line;
        else if ( sig_type == "sine"     ) signal = &signal_sin;
        else if ( sig_type == "cosine"   ) signal = &signal_cos;
        else if ( sig_type == "square"   ) signal = &signal_square;
        else if ( sig_type == "triangle" ) signal = &signal_triang;
        else if ( sig_type == "am"       ) signal = &signal_am;
        else                               signal = &signal_line;
    }
public:
    periodic_signal() { select_signal( string("line") ); }
    periodic_signal(string sig_type){ select_signal(sig_type); }
    // Functor overloading
    double operator()(double t) { return (*signal)(t); }
};


int main(){
// Global parameters and variables
    double amplitude, omega, modulation;
    int samples, bits_number;                           
    string sig_type;

// Setting default parameters
// in case of parsing failure
    amplitude = 1;
    samples = 100;
    bits_number = 8;    

// Parsing config file
    ifstream config(CONFIG);
    string key, equals, value;
    while ( config >> key >> equals >> value ){
        if ( key == "signal_amplitude" ){
            amplitude = stof(value);
        }
        else if ( key == "signal_samples" ) {
            samples = stoi(value);
        }
        else if ( key == "signal_type" ){
            sig_type = value;
        }
        else if ( key == "dac_bit_size" ){
            bits_number = stoi(value);
        }
        else{
            cout << "Key \"" << key << "\" unknown in config file \"" << CONFIG << "\"\n" << "Quitting..." << endl;
            return -1;
        }
    }

// Bit scale calculations and dumping
    unsigned int max_bit = (1<<bits_number);
    double bin_width = amplitude/max_bit;
    vector<double> amptobit(max_bit);               
    for(size_t i=0; i<amptobit.size(); i++){        
        amptobit[i] = amplitude/(amptobit.size())*i;
    }
    ofstream test(SCALE);                   
    for(size_t i=0; i<amptobit.size(); i++){
        test << fixed << setprecision(6) << amptobit[i] << "\t\t" << i << endl;
    }
    test.close();

// Sampling the signal, converting to bit scale and dumping
    periodic_signal signal(sig_type);
    vector<double> sampled_signal(samples);         
    vector<int> bit_signal(samples);                
    for(size_t i=0; i<sampled_signal.size(); i++){  
        sampled_signal[i] = amplitude*signal((double) i/(samples-1));
        bit_signal[i] = int(sampled_signal[i]/bin_width);
        if ( bit_signal[i] == max_bit ) bit_signal[i]--;
    }
    test.open(DATA);                    
    for(size_t i=0; i<sampled_signal.size(); i++){
        test << sampled_signal[i] << "\t\t" << bit_signal[i] << endl;
    }
    test.close();

// Dumping LUT output
    test.open(OUTPUT);
    test << "---- PARAMETERS ----" << endl << endl;
    test << left << setw(16) << "signal_amplitude" << " = " << amplitude << endl;
    test << left << setw(16) << "signal_samples" << " = " << samples << endl;
    test << left << setw(16) << "signal_type" << " = " << sig_type << endl;
    test << left << setw(16) << "dac_bit_size" << " = " << bits_number << endl;
    test << left << setw(16) << "dac_bin_size" << " = " << max_bit << endl;
    test << left << setw(16) << "dac_bin_width" << " = " << bin_width << endl << endl;
    test << "---- LOOK UP TABLE ----" << endl;
    test << "\n---- DECIMAL\n{ ";
    for(size_t i=0; i<bit_signal.size(); i++){
        test << bit_signal[i];
        if ( i == bit_signal.size()-1 ) test << " }"; 
        else test << ", ";
    }
    test << "\n\n---- HEX\n{ ";
    for(size_t i=0; i<bit_signal.size(); i++){
        test << "0x" << hex << bit_signal[i];
        if ( i == bit_signal.size()-1 ) test << " }"; 
        else test << ", ";
    }
    test << "\n\n---- BINARY\n{ ";
    for(size_t i=0; i<bit_signal.size(); i++){
        test << bitset<32>(bit_signal[i]).to_string().substr(32-bits_number);
        if ( i == bit_signal.size()-1 ) test << " }"; 
        else test << ", ";
    }
    test.close();

// Creating gnuplot script
    test.open(GNUPLOT);
    test << R"(#!/usr/bin/gnuplot -persist
set term pngcairo size 900,600 enhanced font 'Verdana,10'
set output ')" << IMAGE << R"('
set style line 1 lc rgb '#77ac30' linetype 1 linewidth 2.5 # green line
set style line 2 lc rgb '#0072bd' pt 7 pointsize 0.5       # blue circles
set style line 102 lc rgb '#d6d7d9' lt 1 lw 1              # grid
set tics nomirror out scale 0.75
set nokey   
set multiplot layout 2,1 title 'LUT GENERATOR'
set lmargin at screen 0.08
set xlabel 'samples'
set y2label 'amplitude scale'
set grid xtics ytics back ls 102
set yrange [)"<< -0.1*amplitude << ":" << amplitude*1.1 << R"(]
plot ')" << DATA << R"(' u 1 w l ls 1
set y2label 'DAC scale'
set yrange [)"<< dec << -0.1*max_bit << ":" << 1.1*max_bit << R"(]
plot ')" << DATA << R"(' u 2 w p ls 2)";
    test.close();
    
    return 0;
}
