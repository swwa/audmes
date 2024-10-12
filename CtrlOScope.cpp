/* CtrlOScope.cpp
 *
 *  Change Log:
 *  ===========
 *  20080822 - renamed from OScopeCtrl (because of mess in the project)
 *           - added hard-setting of vertical lines for linear plot
 *           - double buffering !!!
 *           - added event EVT_ERASE_BACKGROUND
 *           - some other small improvements
 *
 *  verze 0.0.1
 *   - X osa logaritmicka
 *   - Y osa linearni
 *   - zadna kontrola rozsahu !!!!
 */
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

#include "CtrlOScope.h"

#include <cmath>

CtrlOScope::CtrlOScope(wxWindow* parent, wxString xname, wxString yname)
    : wxControl(parent, -1, wxDefaultPosition, wxSize(300, 200)) {
  m_bgColor.Set(0, 0, 0);
  m_plColor.Set(0, 128, 0);
  m_trColor.Set(192, 192, 0);
  m_tr2Color.Set(0, 192, 192);
  m_whColor.Set(255, 100, 100);

  m_MaxXValue = 0;
  m_MinXValue = 0;
  m_MaxYValue = 0;
  m_MinYValue = 0;

  m_LogX = 0;
  m_LogY = 0;

  m_YUnit = yname;
  m_XUnit = xname;

  m_NumberOfVerticals = 0;

  m_UserText = wxT("");
  m_UserTextPosX = 0;
  m_UserTextPosY = 0;

  Bind(wxEVT_SIZE, &CtrlOScope::OnSize, this);
  Bind(wxEVT_PAINT, &CtrlOScope::OnPaint, this);
  Bind(wxEVT_ERASE_BACKGROUND, &CtrlOScope::OnEraseBackground, this);
}

CtrlOScope::~CtrlOScope() {}

void CtrlOScope::SetTrack1(wxArrayDouble const& ardbl) { m_points1 = ardbl; }

void CtrlOScope::SetTrack2(wxArrayDouble const& ardbl) { m_points2 = ardbl; }

void CtrlOScope::SetTrackX(wxArrayDouble const& ardbl) { m_pointsX = ardbl; }

void CtrlOScope::SetXRange(double dLower, double dUpper, int logrange) {
  m_MinXValue = dLower;
  m_MaxXValue = dUpper;
  m_LogX = logrange;
}

void CtrlOScope::SetYRange(double dLower, double dUpper, int logrange) {
  m_MinYValue = dLower;
  m_MaxYValue = dUpper;
  m_LogY = logrange;
}

void CtrlOScope::OnSize(wxSizeEvent& event) { event.Skip(); }

void CtrlOScope::OnPaint(wxPaintEvent& WXUNUSED(event)) {
  wxPaintDC cdc(this);  // device context for painting
  // here will be the Double Buffer
  wxRect rec = GetClientRect();

  wxBitmap* bmpBlit = new wxBitmap(rec.width, rec.height, 32);
  wxMemoryDC* memDC = new wxMemoryDC();
  // clear the memdc with a certain background color
  memDC->SelectObject(*bmpBlit);
  memDC->Clear();

  PaintGraph(*memDC);

  cdc.Blit(rec.x, rec.y, rec.width, rec.height, memDC, 0, 0, wxCOPY);

  delete bmpBlit;
  delete memDC;
}

void CtrlOScope::PaintGraph(wxDC& dc) {
  wxRect rec = GetClientRect();
  wxString bla;
  int tw, th;
  double xstep = 0;
  static const int fSize = 10;

  /* kresli pozadi - draw background */
  dc.SetPen(wxPen(m_bgColor, 1, wxPENSTYLE_SOLID));
  dc.SetBrush(wxBrush(m_bgColor, wxBRUSHSTYLE_SOLID));
  dc.DrawRectangle(0, 0, rec.width, rec.height);

  dc.SetPen(wxPen(m_plColor, 1, wxPENSTYLE_DOT));
  dc.SetBrush(wxBrush(m_plColor, wxBRUSHSTYLE_SOLID));
  dc.SetTextForeground(m_whColor);
  dc.SetFont(wxFont(wxFontInfo(fSize)));

  /* spocitat vysku pisma pro spodni odstup a sirky pto odstup zleva */
  /* calculate space for legend */
  if (!m_YUnit.empty()) {
    dc.GetTextExtent(m_YUnit, &tw, &th);
    udist = tw;
  }
  bla.Printf(wxT("%.1f"), m_MinYValue);
  dc.GetTextExtent(bla, &tw, &th);
  ldist = udist + tw + 8;
  bla.Printf(wxT("%.1f"), m_MaxYValue);
  dc.GetTextExtent(bla, &tw, &th);
  if (ldist < (udist + tw + 8)) {
    ldist = udist + tw + 8;
  }
  if (!m_XUnit.empty()) {
    dc.GetTextExtent(_T("T"), &tw, &th);
    udist = th;
  }
  dc.GetTextExtent(wxT("0"), &tw, &th);
  bdist = udist + th + 8;

  /* tisk legendy - display legend */
  dc.SetBrush(wxBrush(m_plColor, wxBRUSHSTYLE_SOLID));
  dc.DrawText(m_YUnit, 4, rec.height / 2);
  dc.DrawText(m_XUnit, rec.width / 2, rec.height - udist);

  int ydiv = 10; /* number of horizontal lines */
  if (m_MaxYValue - m_MinYValue > 19) ydiv = (m_MaxYValue - m_MinYValue) / 10;

  for (int i = 0; i <= ydiv; i++) {
    /* kresli vsechny cary - draw the lines*/
    double ystep = 1.0 * (rec.height - bdist - tdist) / ydiv;
    dc.DrawLine(ldist, (int)(rec.height - ystep * i - bdist), rec.width - rdist,
                (int)(rec.height - ystep * i - bdist));
    bla.Printf(wxT("%.1f"), m_MinYValue + (m_MaxYValue - m_MinYValue) * i / ydiv);
    dc.GetTextExtent(bla, &tw, &th);
    dc.DrawText(bla, (int)(ldist - tw - 4), (int)(rec.height - ystep * i - bdist - th / 2));
  }

  /* spocitat jak casto se budou kreslit vertikalni cary */
  /* vertical lines depending on linear or log scale  */
  if (m_LogX) {
    /* draw vertical lines with log distance */
    xstep = (rec.width - ldist - rdist) / log10(m_MaxXValue / m_MinXValue);
    if (m_MinXValue < 1) m_MinXValue = 1;  // avoid log10(0) and rounding errors
    int decade = log10(m_MinXValue);
    double freq = m_MinXValue;
    while (freq <= m_MaxXValue) {
      dc.DrawLine((int)(ldist + xstep * log10(freq / m_MinXValue)), tdist,
                  (int)(ldist + xstep * log10(freq / m_MinXValue)), rec.height - bdist);
      if (log10(freq) == decade || log10(freq / 2) == decade || log10(freq / 5) == decade) {
        int cf = (int)freq;
        int cfk = cf / 1000;
        if (cfk >= 1)
          bla.Printf(wxT("%dk"), cfk);
        else
          bla.Printf(wxT("%d"), cf);
        dc.GetTextExtent(bla, &tw, &th);
        dc.DrawText(bla, (int)(ldist + xstep * log10(freq / m_MinXValue) - tw / 2),
                    rec.height - bdist + 8);
      }
      freq += pow(10, decade);
      if (log10(freq) - 1 >= decade) {
        decade++;
      }
    }
  } else {
    /* draw vertical lines with linear distance */
    xstep = (double)(rec.width - ldist - rdist) / m_NumberOfVerticals;
    for (int i = 0; i <= m_NumberOfVerticals; i++) {
      dc.DrawLine((int)(ldist + xstep * i), tdist, (int)(ldist + xstep * i), rec.height - bdist);
      double cl = m_MinXValue + (m_MaxXValue - m_MinXValue) * i / m_NumberOfVerticals;
      int clk = cl / 1000;
      int clm = cl * 1000;
      int clu = cl * 1000000;
      if (clk >= 1)
        bla.Printf(wxT("%dk"), clk);
      else if (cl >= 1)
        bla.Printf(wxT("%d"), (int)cl);
      else if (clm >= 1)
        bla.Printf(wxT("%dm"), clm);
      else
        bla.Printf(wxT("%du"), clu);
      dc.GetTextExtent(bla, &tw, &th);
      if ((xstep * i + ldist) < (tw / 2)) {
        dc.DrawText(bla, (int)(ldist + xstep * i), rec.height - bdist + 8);
      } else if ((ldist + xstep * i + tw / 2) < rec.width) {
        dc.DrawText(bla, (int)(ldist + xstep * i - tw / 2), rec.height - bdist + 8);
      } else {
        dc.DrawText(bla, (int)(ldist + xstep * i - tw), rec.height - bdist + 8);
      }
    }
  }

  /* zobrazit body - draw data */
  if (m_pointsX.GetCount() > 0) {
    // limit drawing region to the graph
    dc.SetClippingRegion(ldist, tdist, rec.width - ldist - rdist + 1,
                         rec.height - tdist - bdist + 1);

    size_t ilow = 0, ihigh = 0;
    // iterate though all X points in the data
    for (size_t i = 0; i < m_pointsX.GetCount(); i++) {
      if (m_pointsX.Item(i) < m_MinXValue) {
        ilow = i;
      }
      if (m_pointsX.Item(i) <= m_MaxXValue) {
        ihigh = i;
      }
    }
    if (ilow > 0) ilow--;
    if ((int)ihigh < ((int)m_pointsX.GetCount() - 1)) ihigh++;

    // left channel
    PaintTrack(dc, ilow, ihigh, xstep, m_trColor, m_pointsX, m_points1);

    // right channel
    PaintTrack(dc, ilow, ihigh, xstep, m_tr2Color, m_pointsX, m_points2);
  }
}

void CtrlOScope::PaintTrack(wxDC& dc, size_t from, size_t to, double xstep, const wxColor& color,
                            wxArrayDouble& xpoints, wxArrayDouble& ypoints) {
  dc.SetPen(wxPen(color, 1, wxPENSTYLE_SOLID));
  wxRect rec = GetClientRect();
  std::vector<wxPoint> pv;
  // iterate trough the data points in range
  for (size_t i = from; i <= to; i++) {
    int xpos;
    if (m_LogX)
      xpos = ldist + xstep * log10(m_pointsX.Item(i) / m_MinXValue);
    else
      xpos = ldist + xpoints.Item(i) * xstep * m_NumberOfVerticals / (m_MaxXValue - m_MinXValue);
    // find the point in the graph and limit to the graph area
    double ydatapoint = ypoints.Item(i);
    if (ydatapoint > m_MaxYValue) ydatapoint = m_MaxYValue;
    if (ydatapoint < m_MinYValue) ydatapoint = m_MinYValue;
    double ypoint =
        rec.height - bdist -
        (rec.height - bdist - tdist) * (ydatapoint - m_MinYValue) / (m_MaxYValue - m_MinYValue);
    wxPoint pt = {xpos, (int)ypoint};
    pv.push_back(pt);
  }
  dc.DrawLines(pv.size(), &pv[0]);

  if (wxT("") != m_UserText) {
    dc.SetTextForeground(m_whColor);
    dc.DrawText(m_UserText, m_UserTextPosX, m_UserTextPosY);
  }
}
