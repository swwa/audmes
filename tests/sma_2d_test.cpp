//////////////////////////////////////////////////////////////////////
// RWAudio_IO.cpp: impementation of audio interface
//
//////////////////////////////////////////////////////////////////////
/*
 * Copyright (C) 2008 Vaclav Peroutka <vaclavpe@seznam.cz>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../sma_2d.h"

//#include <math.h>
#include <stdio.h>
#include <cmath>

#define EPSILON 1e-6

int main () {
  SMA_2D realAvg (3, 5);
  std::cout << "Hello World\n";

  //Check numRecords out of bounds
  if ( ! realAvg.AddVal (3, 14.3) ) {
	std::cout << "Out of bounds check recordNum failed\n";
  }
  // Check inbounds numRecords passes
  if ( realAvg.AddVal (2, 14) ) {
	std::cout << "In bounds check recordNum failed\n";
  }
  // Check average when less than all samples are added
  realAvg.AddVal (1,14.2);
  realAvg.AddVal (1,14.4);
  if ( fabs(realAvg.GetSMA(1) - 14.3) > EPSILON ) {
    std::cout << "Average with 2 samples failed\n";
  }
  // Check re-initialization
  realAvg.Init (3, 5);
  if ( fabs(realAvg.GetSMA(1)) > EPSILON ) {
    std::cout << "Re-initialization failed, value: " << std::to_string(realAvg.GetSMA(1)) << "\n";
  }
  // Check average with exactly 5 samples
  for ( int i = 0; i < 5; i++ ) {
    realAvg.AddVal (1, (double) i);
  }
  if ( fabs(realAvg.GetSMA(1) - 2.0) > EPSILON ) {
    std::cout << "Average with full average buffer failed, value: " << std::to_string(realAvg.GetSMA(1)) << "\n";
  }
  // Check continuation of average after 5 samples  
  realAvg.AddVal (1, 5);
  if ( fabs(realAvg.GetSMA(1) - 3.0) > EPSILON ) {
    std::cout << "Average with continue average buffer failed, value: " << std::to_string(realAvg.GetSMA(1)) << "\n";
  }
  // Check other record for zero and division by zero error
  if ( fabs(realAvg.GetSMA(2) > EPSILON) ) {
    std::cout << "Record with no entries remain zero failed, value: " << std::to_string(realAvg.GetSMA(2))<< "\n";
  }
  // Let's test with some real values now
  realAvg.Init (3, 5);
  std::vector<double> outputs {10.0, 10.5, 10.0, 11.0, 10.6, 11.0, 11.4, 11.2, 11.8, 12.0, 9.6, 7.0, 5.4, 2.0, 0.0};
  std::vector<double> inputs {10.0, 11.0, 9.0, 14.0, 9.0, 12.0, 13.0, 8.0, 17.0, 10.0, 0, 0, 0, 0, 0};
  for ( int i = 0; i < inputs.size(); i++ ) {
    realAvg.AddVal(1, inputs[i]);
    std::cout << "iterator: " << std::to_string(i) << "  SMA: " << std::to_string(realAvg.GetSMA(1)) << "\n" ;
    if ( fabs(realAvg.GetSMA(1) - outputs[i]) > EPSILON ) {
      std::cout << "Calculating average failed in step: " << std::to_string(i) << "\n";
    }
  }

return 0;
}

