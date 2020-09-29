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

#include <math.h>

#include <wx/arrimpl.cpp>  // this is a magic incantation which must be done!

/////////////////////////////////////////////////////////////////////////////
// CtrlOScope
CtrlOScope::CtrlOScope(wxWindow* parent, wxString xname, wxString yname, int tracks)
    : wxControl(parent, -1, wxDefaultPosition, wxSize(300, 200)) {
  m_width = 350;
  m_height = 250;
  m_tracks = tracks;
  m_fsampling = 44100;

  m_bgColor.Set(0, 0, 0);
  m_plColor.Set(0, 128, 0);
  m_trColor.Set(192, 192, 0);
  m_tr2Color.Set(0, 192, 192);
  m_whColor.Set(255, 0, 0);

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

  Connect(-1, wxEVT_SIZE, (wxObjectEventFunction)&CtrlOScope::OnSize);
  Connect(-1, wxEVT_PAINT, (wxObjectEventFunction)&CtrlOScope::OnPaint);
  Connect(-1, wxEVT_ERASE_BACKGROUND, (wxObjectEventFunction)&CtrlOScope::OnEraseBackground);

}  // CtrlOScope

/////////////////////////////////////////////////////////////////////////////
CtrlOScope::~CtrlOScope() {}  // ~CtrlOScope

void CtrlOScope::SetTrack(wxArrayDouble ardbl) {
  m_points = ardbl;
  wxClientDC dc(this);
  PaintAll(dc);
}

void CtrlOScope::SetTrack2(wxArrayDouble ardbl) {
  m_points2 = ardbl;
  wxClientDC dc(this);
  PaintAll(dc);
}

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetXRange(double dLower, double dUpper, int logrange) {
  m_MinXValue = dLower;
  m_MaxXValue = dUpper;
  m_LogX = logrange;
}  // SetRange

void CtrlOScope::SetYRange(double dLower, double dUpper, int logrange, int WXUNUSED(itrack)) {
  m_MinYValue = dLower;
  m_MaxYValue = dUpper;
  m_LogY = logrange;
}  // SetRange

void CtrlOScope::SetFsample(int fsampling) { m_fsampling = fsampling; }

void CtrlOScope::SetXUnits(wxString WXUNUSED(string), wxString WXUNUSED(XMin),
                           wxString WXUNUSED(XMax)) {}  // SetXUnits

void CtrlOScope::SetYUnits(wxString WXUNUSED(string), wxString WXUNUSED(YMin),
                           wxString WXUNUSED(YMax)) {}  // SetYUnits

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetGridColor(wxColour WXUNUSED(color)) {}  // SetGridColor

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetPlotColor(wxColour WXUNUSED(color), int WXUNUSED(iTrack)) {}  // SetPlotColor

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetBackgroundColor(wxColour WXUNUSED(color)) {}  // SetBackgroundColor

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::AppendPoints(double WXUNUSED(dNewPoint)[], int WXUNUSED(iTrack)) {
}  // AppendPoints

wxSize CtrlOScope::GetSize(void) const {
  //      wxMessageBox("GetSize", "Application Error", wxOK | wxICON_ERROR, this);
  return wxSize(m_width, m_height);
}

wxSize CtrlOScope::DoGetBestSize(void) const {
  //  wxMessageBox("GetBestSize", "Application Error", wxOK | wxICON_ERROR, this);
  return wxSize(m_width, m_height);
}

////////////////////////////////////////////////////////////////////////////
void CtrlOScope::OnPaint(wxPaintEvent& WXUNUSED(event)) {
  wxPaintDC dc(this);  // device context for painting
  PaintAll(dc);
}

void CtrlOScope::PaintAll(wxDC& dc) {
  // here will be the Double Buffer
  wxRect rec = GetClientRect();

  wxBitmap* bmpBlit = new wxBitmap(rec.width, rec.height, 32);
  wxMemoryDC* memDC = new wxMemoryDC();
  // clear the memdc with a certain background color
  memDC->SelectObject(*bmpBlit);
  memDC->Clear();

  PaintAllFunction(*memDC);

  dc.Blit(rec.x, rec.y, rec.width, rec.height, memDC, 0, 0, wxCOPY);

  delete bmpBlit;
  delete memDC;
}

void CtrlOScope::PaintAllFunction(wxDC& dc) {
  wxRect rec = GetClientRect();
  wxString bla;
  int tw, th;
  int ldist = 0;   // legend space on the left
  int rdist = 20;  // space on the right
  int tdist = 10;  // space on top
  int bdist = 0;   // legend space on bottom
  int udist = 0;   // unit text space

  /* kresli pozadi - draw background */
  dc.SetPen(wxPen(m_bgColor, 1, wxSOLID));
  dc.SetBrush(wxBrush(m_bgColor, wxSOLID));
  dc.DrawRectangle(0, 0, rec.width, rec.height);

  dc.SetPen(wxPen(m_plColor, 1, wxDOT));
  dc.SetBrush(wxBrush(m_plColor, wxSOLID));
  dc.SetTextForeground(m_whColor);
  dc.SetFont(wxFont(8, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));

  /* spocitat vysku pisma pro spodni odstup a sirky pto odstup zleva */
  /* calculate space for legend */
  if (m_YUnit != wxT("")) {
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
  if (m_XUnit != wxT("")) {
    dc.GetTextExtent(_T("T"), &tw, &th);
    udist = th;
  }
  dc.GetTextExtent(wxT("0"), &tw, &th);
  bdist = udist + th + 8;

  /* tisk legendy - display legend */
  dc.SetBrush(wxBrush(m_plColor, wxSOLID));
  dc.DrawText(m_YUnit, 4, rec.height / 2);
  dc.DrawText(m_XUnit, rec.width / 2, rec.height - udist);

  int ydiv = 10; /* fixed number of horizontal lines */
  for (int i = 0; i <= ydiv; i++) {
    /* kresli vsechny cary - draw the lines*/
    float ystep = 1.0 * (rec.height - bdist - tdist) / ydiv;
    dc.DrawLine(ldist, (int)(rec.height - ystep * i - bdist), rec.width - rdist,
                (int)(rec.height - ystep * i - bdist));
    bla.Printf(wxT("%.1f"), m_MinYValue + (m_MaxYValue - m_MinYValue) * i / ydiv);
    dc.GetTextExtent(bla, &tw, &th);
    dc.DrawText(bla, (int)(ldist - tw - 4), (int)(rec.height - ystep * i - bdist - th / 2));
  }

  /* spocitat jak casto se budou kreslit vertikalni cary */
  /* ******************************************************************* */
  /* ******************************************************************* */
  /* vertical lines depending on linear or log scale  */
  if (1 == m_LogX) {
    /* logaritmicke meritko - spocitat pocet dekad a potom neco dale */
    /* cara bude minimalne kazdych 100 pixelu na dekadu; pak bude 1, 3, 10 */
    float ndecs = log10(m_MaxXValue / m_MinXValue);
    float npix = rec.width / ndecs;
    int xdiv;

    if (npix < 100) {
      xdiv = 1;
    } else if (npix < 300) {
      xdiv = 3;
    } else {
      xdiv = 10;
    }

    float xstep = 1.0 * (rec.width - ldist - rdist) / ndecs;  // pocet bodu na dekadu
    for (int i = 0; i <= ndecs; i++) {
      /* kresli vsechny cary */
      dc.DrawLine((int)(ldist + xstep * i), tdist, (int)(ldist + xstep * i),
                  rec.height - bdist + 8);
      /* nakresli vnitrni cary */
      if (3 == xdiv) {
        dc.DrawLine((int)(ldist + xstep * (i + log10(2))), tdist,
                    (int)(ldist + xstep * (i + log10(2))), rec.height - bdist);
        dc.DrawLine((int)(ldist + xstep * (i + log10(5))), tdist,
                    (int)(ldist + xstep * (i + log10(5))), rec.height - bdist);
      }
      if (10 == xdiv) {
        for (int j = 2; j < 10; j++) {
          dc.DrawLine((int)(ldist + xstep * (i + log10(j))), tdist,
                      (int)(ldist + xstep * (i + log10(j))), rec.height - bdist);
        }
      }
      int cf = (int)(m_MinXValue * pow(10, i));
      int cfk = cf / 1000;
      if (cfk >= 1)
        bla.Printf(wxT("%dk"), cfk);
      else
        bla.Printf(wxT("%d"), cf);
      dc.GetTextExtent(bla, &tw, &th);
      if ((xstep * i + ldist) < (tw / 2)) {
        dc.DrawText(bla, (int)(ldist + xstep * i), rec.height - bdist + 8);
      } else if ((ldist + xstep * i + tw / 2) < rec.width) {
        dc.DrawText(bla, (int)(ldist + xstep * i - tw / 2), rec.height - bdist + 8);
      } else {
        dc.DrawText(bla, (int)(ldist + xstep * i - tw), rec.height - bdist + 8);
      }
    }

    /* zobrazit body - draw data */
    float binsize = 0.5 * m_fsampling / m_points.GetCount();

    // left channel
    dc.SetPen(wxPen(m_trColor, 1, wxSOLID));
    int lastx = 0, lasty = 0;
    float lastxfreq = 0.0;
    // new plot function - walking through X points and select values from the table
    if (m_points.GetCount() > 0) {
      for (long int i = 0; i < rec.width - ldist - rdist; i++) {
        // find the frequency
        float xfreq = m_MinXValue * pow(10, i / xstep);
        if ((binsize > xfreq - lastxfreq) && i > 0) continue;
        // find the position in the m_points[] array
        unsigned long int xposit = (unsigned long int)(xfreq / binsize);

        if (xposit < m_points.GetCount()) {
          /* iterate though all fft bins between x and the next x position in the graph */
          float nxfreq = m_MinXValue * pow(10, (i + 1) / xstep);
          unsigned long int nxposit = (unsigned long int)(nxfreq / binsize);
          if (nxposit >= m_points.GetCount()) {
            nxposit = m_points.GetCount() - 1;
          }
          float ydatapoint = -150.0;
          for (unsigned long int j = xposit; j <= nxposit; j++) {
            if ((float)m_points[j] > ydatapoint) ydatapoint = m_points[j];
          }

          if (ydatapoint > m_MaxYValue) ydatapoint = m_MaxYValue;
          if (ydatapoint < m_MinYValue) ydatapoint = m_MinYValue;
          float ypoint = rec.height - bdist -
                         (rec.height - bdist - tdist) * (ydatapoint - m_MinYValue) /
                             (m_MaxYValue - m_MinYValue);
          if (0 == i) {
            dc.DrawPoint((int)(i + ldist), (int)(ypoint));
          } else {
            dc.DrawLine(lastx, lasty, (int)(i + ldist), (int)(ypoint));
          }
          lastx = (int)(i + ldist);
          lasty = (int)(ypoint);
          if (i > 0) lastxfreq = xposit * binsize;
        }
      }
    }

    // right channel
    dc.SetPen(wxPen(m_tr2Color, 1, wxSOLID));
    lastx = 0, lasty = 0;
    lastxfreq = 0.0;
    if (m_points2.GetCount() > 0) {
      for (long int i = 0; i < rec.width - ldist - rdist; i++) {
        // find the frequency
        float xfreq = m_MinXValue * pow(10, i / xstep);
        if ((binsize > xfreq - lastxfreq) && i > 0) continue;
        // find the position in the m_points2[] array
        unsigned long int xposit = (unsigned long int)(xfreq / binsize);

        if (xposit < m_points2.GetCount()) {
          /* iterate though all fft bins between x and the next x position in the graph */
          float nxfreq = m_MinXValue * pow(10, (i + 1) / xstep);
          unsigned long int nxposit = (unsigned long int)(nxfreq / binsize);
          if (nxposit >= m_points2.GetCount()) {
            nxposit = m_points2.GetCount() - 1;
          }
          float ydatapoint = -150.0;
          for (unsigned long int j = xposit; j <= nxposit; j++) {
            if ((float)m_points2[j] > ydatapoint) ydatapoint = m_points2[j];
          }

          if (ydatapoint > m_MaxYValue) ydatapoint = m_MaxYValue;
          if (ydatapoint < m_MinYValue) ydatapoint = m_MinYValue;
          float ypoint = rec.height - bdist -
                         (rec.height - bdist - tdist) * (ydatapoint - m_MinYValue) /
                             (m_MaxYValue - m_MinYValue);
          if (0 == i) {
            dc.DrawPoint((int)(i + ldist), (int)(ypoint));
          } else {
            dc.DrawLine(lastx, lasty, (int)(i + ldist), (int)(ypoint));
          }
          lastx = (int)(i + ldist);
          lasty = (int)(ypoint);
          if (i > 0) lastxfreq = xposit * binsize;
        }
      }
    }

    /*  **************** linearni meritko *********************** */
  } else {
    /* cara bude minimalne kazdych 30 pixelu; pocet 2,5,10,20,50 atd. */
    // if we have m_NumberOfVerticals == 0, let's compute steps. But otherwise not
    int nline = 100;
    int xdiv = m_NumberOfVerticals;
    int nstrt = nline;
    while (xdiv == 0) {
      if (rec.width < nline) {
        xdiv = 1 * nline / nstrt;
      } else if (rec.width < 2 * nline) {
        xdiv = 2 * nline / nstrt;
      } else if (rec.width < 5 * nline) {
        xdiv = 5 * nline / nstrt;
      }
      nline *= 10;
    }

    float xstep = 1.0 * (rec.width - ldist - rdist) / xdiv;
    for (int i = 0; i <= xdiv; i++) {
      /* kresli vsechny cary */
      dc.DrawLine((int)(ldist + xstep * i), tdist, (int)(ldist + xstep * i), rec.height - bdist);
      bla.Printf(wxT("%.0f"), m_MinXValue + (m_MaxXValue - m_MinXValue) * i / xdiv);
      dc.GetTextExtent(bla, &tw, &th);
      if ((xstep * i + ldist) < (tw / 2)) {
        dc.DrawText(bla, (int)(ldist + xstep * i), rec.height - bdist + 8);
      } else if ((ldist + xstep * i + tw / 2) < rec.width) {
        dc.DrawText(bla, (int)(ldist + xstep * i - tw / 2), rec.height - bdist + 8);
      } else {
        dc.DrawText(bla, (int)(ldist + xstep * i - tw), rec.height - bdist + 8);
      }
    }
    /* kresli body v SetTracks */
    dc.SetPen(wxPen(m_trColor, 1, wxSOLID));
    int lastx = 0, lasty = 0;
    float xpoint;
    for (unsigned int i = 0; i < m_points.GetCount(); i++) {
      /* prepocitat vstupni bod na X osu */
      // float xpoint = i+ldist; // linearni zavislost
      xpoint = ldist + i * (rec.width - ldist - rdist) / (m_MaxXValue - m_MinXValue);
      /* prepocitat Y hodnotu na Y osu - orezat hodnoty */
      if (m_points[i] > m_MaxYValue) {
        m_points[i] = m_MaxYValue;
      }
      if (m_points[i] < m_MinYValue) {
        m_points[i] = m_MinYValue;
      }
      float ypoint =
          rec.height - bdist -
          (rec.height - bdist - tdist) * (m_points[i] - m_MinYValue) / (m_MaxYValue - m_MinYValue);
      if (0 == i) {
        dc.DrawLine((int)(xpoint), (int)(ypoint), (int)(xpoint), (int)(ypoint));
      } else {
        dc.DrawLine(lastx, lasty, (int)(xpoint), (int)(ypoint));
      }
      lastx = (int)(xpoint);
      lasty = (int)(ypoint);
    }
    dc.SetPen(wxPen(m_tr2Color, 1, wxSOLID));
    for (unsigned int i = 0; i < m_points2.GetCount(); i++) {
      /* prepocitat vstupni bod na X osu */
      // float xpoint = i+ldist; // linearni zavislost
      xpoint = ldist + i * (rec.width - ldist - rdist) / (m_MaxXValue - m_MinXValue);
      /* prepocitat Y hodnotu na Y osu - orezat hodnoty */
      if (m_points2[i] > m_MaxYValue) {
        m_points2[i] = m_MaxYValue;
      }
      if (m_points2[i] < m_MinYValue) {
        m_points2[i] = m_MinYValue;
      }
      float ypoint =
          rec.height - bdist -
          (rec.height - bdist - tdist) * (m_points2[i] - m_MinYValue) / (m_MaxYValue - m_MinYValue);
      if (0 == i) {
        dc.DrawLine((int)(xpoint), (int)(ypoint), (int)(xpoint), (int)(ypoint));
      } else {
        dc.DrawLine(lastx, lasty, (int)(xpoint), (int)(ypoint));
      }
      lastx = (int)(xpoint);
      lasty = (int)(ypoint);
    }
  }
  if (wxT("") != m_UserText) {
    dc.SetTextForeground(m_whColor);
    dc.DrawText(m_UserText, m_UserTextPosX, m_UserTextPosY);
  }
}  // OnPaint

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::OnSize(wxSizeEvent& event)  // UINT nType, int cx, int cy)
{
  event.Skip();
}  // OnSize
