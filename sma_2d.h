/////////////////////////////////////////////////////////////////////////////
// Name:        sma-2d
// Purpose:		Simple Moving average over N values for R rows
// Author:		Johannes Linkels
// Modified by:
// Created:     2023-05-29
// RCS-ID:
// Copyright:
// License:
/////////////////////////////////////////////////////////////////////////////
/*
 * Copyright (C) 2007-2008 Vaclav Peroutka <vaclavpe@seznam.cz>
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

#ifndef _SMA_2D_
#define _SMA_2D_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "sma_2d.h"
#endif

/*!
 * Includes
 */

#include <iostream>
#include <vector>

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * Compatibility
 */

/*!
 * SMA_2D class declaration
 */

class SMA_2D {

 public:
  /// Constructors
  SMA_2D(int numRecords, int numAverage);
  ~SMA_2D ();

  /// (Re)Initializes the data values
  void Init(int numRecords, int numAverage);

  /// Adds a new value to the average at record recordNum
  int AddVal (int recordNum, double val);

  /// Gets the current moving average value at record recordNum
  double GetSMA (int recordNum);

private:

  int m_NumRecords;						// number of records (samples)
  int m_NumAverage;						// number of values used for the average
  std::vector<int> index;					// index points to latest value in each record
  std::vector<int> numSummed;				// number of values summed, avg = sum/numSummed
  std::vector<double> sum;					// current sum of values for each record
  std::vector<std::vector<double>> prevValues;	// the actual 2D store
};

#endif 
// _SMA_2D
  
