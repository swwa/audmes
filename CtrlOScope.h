// CtrlOScope.h : header file
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

#ifndef __CTRLOSCOPE_H__
#define __CTRLOSCOPE_H__

#include <wx/dynarray.h>
#include <wx/wx.h>

class CtrlOScope : public wxControl {
 public:
  CtrlOScope(wxWindow* parent, wxString xname = _T("X"), wxString yname = _T("Y"));

  virtual ~CtrlOScope();
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void PaintAll(wxDC& dc);
  void PaintAllFunction(wxDC& dc);
  void OnEraseBackground(wxEraseEvent& WXUNUSED(event)){};

  void SetXRange(double dLower, double dUpper, int logrange);
  void SetYRange(double dLower, double dUpper, int logrange);
  void SetFsample(int fsampling);

  void SetXUnits(wxString string, wxString XMin = "", wxString XMax = "");
  void SetYUnits(wxString string, wxString YMin = "", wxString YMax = "");
  void SetTrack1(wxArrayDouble ardbl);
  void SetTrack2(wxArrayDouble ardbl);
  void SetTrackX(wxArrayDouble ardbl);
  void SetNumOfVerticals(int num) { m_NumberOfVerticals = num; };

  void ShowUserText(wxString text, int xpos, int ypos) {
    m_UserText = text;
    m_UserTextPosX = xpos;
    m_UserTextPosY = ypos;
  };

 protected:
  int m_fsampling;

  wxArrayDouble m_points1;
  wxArrayDouble m_points2;
  wxArrayDouble m_pointsX;
  double m_MaxXValue;
  double m_MinXValue;
  double m_MaxYValue;
  double m_MinYValue;
  int m_LogX;
  int m_LogY;
  wxString m_XUnit;
  wxString m_YUnit;

  wxColour m_bgColor;
  wxColour m_plColor;
  wxColour m_trColor;
  wxColour m_tr2Color;
  wxColour m_whColor;

  int m_NumberOfVerticals;

  wxString m_UserText;
  int m_UserTextPosX;
  int m_UserTextPosY;
};
#endif  // __CTRLOSCOPE_H__
