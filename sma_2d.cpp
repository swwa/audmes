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

#include "sma_2d.h"

//#include <math.h>
#include <stdio.h>


SMA_2D::SMA_2D(int numRecords, int numAverage) {
  std::cerr << "SMA_2D constructor\n";
  Init (numRecords, numAverage);
}
	

SMA_2D::~SMA_2D() {
  std::cerr << "SMA_2D destructor\n";
  index.clear();
  sum.clear();
  numSummed.clear();
  prevValues.clear();

}

int SMA_2D::AddVal (int recordNum, double val) {
  if ( !(recordNum < m_NumRecords) ) {
    return 1;
  }
// prevValues is a ring buffer, holding the last N values,
// with N the length of the average numAverage.
// A new value is added to the sum, the oldest value
// is substracted. The new value is stored in prevValues
// and remains there for N itereations.
  int curIndex = index[recordNum];
  sum[recordNum] -= prevValues[recordNum][curIndex];
  sum[recordNum] += val;
  prevValues[recordNum][curIndex] = val;
  if ( ++curIndex == m_NumAverage ) {
    curIndex = 0;
  }
  if ( ++numSummed[recordNum] > m_NumAverage ) {
    numSummed[recordNum] = m_NumAverage;
  }
  index[recordNum] = curIndex;
  return 0;
}

double SMA_2D::GetSMA (int recordNum) {
// Calculate the average only over the values which are
// added. If not, it would take until all values are added.
// until the average shows the final value.
// If no samples at all have been added prevent division by
// zero and return whatever is in the sum.
  if (numSummed[recordNum] == 0) {
    numSummed[recordNum] = 1;
  }
  double sma = sum[recordNum]/numSummed[recordNum];
  return sma;
}

int SMA_2D::GetNumSummed (int recordNum) {
  return numSummed[recordNum];
}

void SMA_2D::Init (int numRecords, int numAverage) {

  m_NumRecords = numRecords;
  m_NumAverage = numAverage;  

  index.clear();
  sum.clear();
  numSummed.clear();
  prevValues.clear();

  index.resize (numRecords, 0);
  sum.resize (numRecords, 0);
  numSummed.resize (numRecords, 0);
  prevValues.resize (numRecords, std::vector<double> (numAverage));
}
