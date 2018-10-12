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

#include "CtrlOScope.h"

#include <math.h>
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!

/////////////////////////////////////////////////////////////////////////////
// CtrlOScope
CtrlOScope::CtrlOScope( wxWindow* parent, wxString xname, wxString yname, int tracks)
  : wxControl(parent,-1,wxDefaultPosition,wxSize(300,200))
{
  //  int i_cnt;

  m_width = 350;
  m_height = 250;
  m_tracks = tracks;

  m_bgColor.Set(0,0,0);
  m_plColor.Set(128,128,128);
  m_trColor.Set(192,192,0);
  m_tr2Color.Set(0,192, 192);
  m_whColor.Set( 255, 255, 255);

  m_YUnit = yname;
  m_XUnit = xname;

  m_NumberOfVerticals = 0;

  m_UserText = wxT("");

  Connect( -1, wxEVT_SIZE,(wxObjectEventFunction)& CtrlOScope::OnSize);
  Connect( -1, wxEVT_PAINT,(wxObjectEventFunction)& CtrlOScope::OnPaint);
  Connect( -1, wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)& CtrlOScope::OnEraseBackground);

}  // CtrlOScope

/////////////////////////////////////////////////////////////////////////////
CtrlOScope::~CtrlOScope()
{
} // ~CtrlOScope

void CtrlOScope::SetTrack( wxArrayDouble ardbl)
{
  m_points = ardbl;
  wxClientDC dc( this);
  PaintAll( dc);
}

void CtrlOScope::SetTrack2( wxArrayDouble ardbl)
{
  m_points2 = ardbl;
  wxClientDC dc( this);
  PaintAll( dc);
}

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetXRange(double dLower, double dUpper, int logrange)
{
  m_MinXValue = dLower;
  m_MaxXValue = dUpper;
  m_LogX = logrange;
}  // SetRange

void CtrlOScope::SetYRange(double dLower, double dUpper, int logrange, int WXUNUSED(itrack))
{
  m_MinYValue = dLower;
  m_MaxYValue = dUpper;
  m_LogY = logrange;
}  // SetRange

void CtrlOScope::SetXUnits(wxString WXUNUSED(string), wxString WXUNUSED(XMin), wxString WXUNUSED(XMax) )
{
}  // SetXUnits

void CtrlOScope::SetYUnits(wxString WXUNUSED(string), wxString WXUNUSED(YMin), wxString WXUNUSED(YMax) )
{
}  // SetYUnits

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetGridColor(wxColour WXUNUSED(color))
{
}  // SetGridColor


/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetPlotColor(wxColour WXUNUSED(color), int WXUNUSED(iTrack))
{
}  // SetPlotColor

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::SetBackgroundColor(wxColour WXUNUSED(color))
{
}  // SetBackgroundColor

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::AppendPoints(double WXUNUSED(dNewPoint)[], int WXUNUSED(iTrack) )
{
} // AppendPoints
 
wxSize CtrlOScope::GetSize( void) const
{
  //      wxMessageBox("GetSize", "Application Error", wxOK | wxICON_ERROR, this);
  return wxSize( m_width, m_height);
}

wxSize CtrlOScope::DoGetBestSize( void) const
{
  //  wxMessageBox("GetBestSize", "Application Error", wxOK | wxICON_ERROR, this);
  return wxSize(  m_width, m_height);
}


////////////////////////////////////////////////////////////////////////////
void CtrlOScope::OnPaint(wxPaintEvent& WXUNUSED(evt)) 
{
	wxPaintDC dc(this) ;  // device context for painting
	PaintAll( dc);
}

void CtrlOScope::PaintAll( wxDC & dc)
{
  // here will be the Double Buffer
  wxRect rec = GetClientRect();

            wxBitmap* bmpBlit = new wxBitmap(rec.width,rec.height, 32);
            wxMemoryDC* memDC= new wxMemoryDC();
            //clear the memdc with a certain background color
            memDC->SelectObject(*bmpBlit);
#if defined(__WXMSW__)
            memDC->BeginDrawing();
#endif
            memDC->Clear();

  PaintAllFunction( *memDC, 0, 0);

#if defined(__WXMSW__)
            memDC->EndDrawing();
#endif

            dc.Blit(rec.x,rec.y,rec.width,rec.height,memDC,0,0,wxCOPY);

            delete bmpBlit;
            delete memDC;

}

void CtrlOScope::PaintAllFunction( wxDC & dc, int rectX, int rectY)
{
  wxRect rec = GetClientRect();
  wxString bla;
  int tw, th;
  int twY, thY;
  int twX, thX;
  int bdist, ldist;

  //	bla.Printf ("Rect is %d, %d, %d, %d", rectX, rectY, rec.width, rec.height);
  //	wxMessageBox( bla, "Application Error", wxOK | wxICON_ERROR, this);

  // kresli pozadi
  dc.SetPen( wxPen( m_bgColor,1,1));
  dc.SetBrush( wxBrush(m_bgColor, wxSOLID));
  dc.DrawRectangle( rectX, rectY, rec.width, rec.height);

  dc.SetPen( wxPen( m_plColor,1,1));
  dc.SetBrush( wxBrush(m_plColor, wxSOLID));
  dc.SetTextForeground( m_whColor);
  dc.SetFont(wxFont(8, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));
  /* spocitat vysku pisma pro spodni odstup a sirky pto odstup zleva */
  dc.GetTextExtent( m_XUnit, &twX, &thX); bdist = thX+5;
  dc.GetTextExtent( m_YUnit, &twY, &thY); ldist = twY + 5;
  bla.Printf(wxT("%.1f"), m_MinYValue); dc.GetTextExtent(bla, &tw, &th);
  if (ldist < (tw+5)) { ldist = tw+5;}
  bla.Printf(wxT("%.1f"), m_MaxYValue); dc.GetTextExtent(bla, &tw, &th);
  if (ldist < (tw+5)) { ldist = tw+5;}

  /* tisk legendy */
  if (m_YUnit != wxT("")) {
    dc.SetBrush( wxBrush(m_bgColor, wxSOLID));
    dc.DrawRectangle( 3, rectY+rec.height/2-2,twY+4, thY+4);
    dc.SetBrush( wxBrush(m_plColor, wxSOLID));
    dc.DrawText(m_YUnit, 5, rectY+rec.height/2);
  } else {
    ldist = tw;
  }
  if (m_XUnit == wxT("")) {
    dc.GetTextExtent( _T("T"), &twX, &thX); bdist = thX+5;
  } else {
    dc.GetTextExtent( m_XUnit, &twX, &thX); bdist = thX+5;
    dc.SetBrush( wxBrush(m_bgColor, wxSOLID));
    dc.DrawRectangle(rectX+rec.width/2-2, rectY+rec.height-bdist-2,twX+4, thX+4);
    dc.SetBrush( wxBrush(m_plColor, wxSOLID));
    dc.DrawText(m_XUnit, rectX+rec.width/2, rectY+rec.height-bdist);
  }

  /* spocitat jak casto se budou kreslit horizontalni cary */
  /* cara bude minimalne kazdych 30 pixelu; pocet 2,5,10,20,50 atd. */
  int nline = 50; int ydiv = 0; int nstrt = nline;
  while (ydiv == 0) {
    if ( rec.height < nline) {
      ydiv = 1*nline/nstrt;
    } else if ( rec.height < 2*nline) {
      ydiv = 2*nline/nstrt;
    } else if ( rec.height < 5*nline) {
      ydiv = 5*nline/nstrt;
    }
    nline *= 10;
  }
  //bla.Printf("ydiv = %d", ydiv); dc.DrawText(bla, rectX+40,rectY+20);
  for (int i = 0; i <= ydiv; i++) {
    /* kresli vsechny cary */
    float ystep = 1.0*(rec.height-bdist-5)/ydiv;
    dc.DrawLine(rectX+ldist, (int)(rec.height+rectY-ystep*i-bdist),rectX+rec.width-5, (int)(rec.height+rectY-ystep*i-bdist));
    bla.Printf(wxT("%.1f"), m_MinYValue+(m_MaxYValue-m_MinYValue)*i/ydiv); dc.GetTextExtent(bla, &tw, &th);
    if (ydiv == i) {
      dc.DrawText(bla, (int)(rectX+ldist-tw),(int)(rec.height+rectY-ystep*i-bdist) );
    } else {
      dc.DrawText(bla, (int)(rectX+ldist-tw),(int)(rec.height+rectY-ystep*i-bdist-th) );
    }
  }
  if ( ((m_MinYValue+m_MaxYValue) < 0.01)&&(1 == ydiv%2)) {
    /* kresli nulovou caru */
    dc.DrawLine(rectX+ldist, rectY+5+(rec.height-bdist-5)/2,rectX+rec.width-5, rectY+5+(rec.height-bdist-5)/2);
  }

  /* spocitat jak casto se budou kreslit vertikalni cary */
  /* ******************************************************************* */
  /* ******************************************************************* */
  /* ******************************************************************* */
  if( 1 == m_LogX) {
    /* logaritmicke meritko - spocitat pocet dekad a potom neco dale */
    /* cara bude minimalne kazdych 100 pixelu na dekadu; pak bude 1, 3, 10 */
    int ndecs = (int)( log10( m_MaxXValue/m_MinXValue)+.9);
    float npix = rec.width/ndecs;
    int xdiv;

    if ( npix < 70) {
      xdiv = 1;
    } else if ( npix < 200) {
      xdiv = 3;
    } else {
      xdiv = 10;
    }

    float xstep = 1.0*(rec.width-ldist-5)/ndecs; // pocet bodu na dekadu
    for (int i = 0; i <= ndecs; i++) {
      /* kresli vsechny cary */
      dc.DrawLine((int)(rectX+ldist+xstep*i), rectY+5, (int)(rectX+ldist+xstep*i), rectY+rec.height-bdist);
      /* nakresli vnitrni cary */
      if (3 == xdiv) {
	dc.DrawLine((int)(rectX+ldist+xstep*(i+log10(2))), rectY+5, (int)(rectX+ldist+xstep*(i+log10(2))), rectY+rec.height-bdist);
	dc.DrawLine((int)(rectX+ldist+xstep*(i+log10(5))), rectY+5, (int)(rectX+ldist+xstep*(i+log10(5))), rectY+rec.height-bdist);
      }
      if (10 == xdiv) {
	for( int j = 2; j <10; j++) {
	  dc.DrawLine((int)(rectX+ldist+xstep*(i+log10(j))), rectY+5, (int)(rectX+ldist+xstep*(i+log10(j))), rectY+rec.height-bdist);
	}
      }
      bla.Printf(wxT("%.1f"), m_MinXValue*pow(10,i)); dc.GetTextExtent(bla, &tw, &th);
      if ((xstep*i+ldist)<(tw/2)) {
	dc.DrawText(bla, (int)(rectX+ldist+xstep*i), rectY+rec.height-bdist);
      } else if ((ldist+xstep*i+tw/2) < rec.width) {
	dc.DrawText(bla, (int)(rectX+ldist+xstep*i-tw/2), rectY+rec.height-bdist);
      } else {
	dc.DrawText(bla, (int)(rectX+ldist+xstep*i-tw), rectY+rec.height-bdist);
      }
    }

    /* zobrazit body */
    //bla.Printf("Pocet bodu: %d", m_points.GetCount()); wxMessageBox(bla,"D",wxOK);
    dc.SetPen( wxPen( m_trColor,1,1));
    int lastx = 0, lasty = 0;
    // new plot function - walking through X points and select values from the table
    if ( m_points.GetCount() > 0) {
      for (long int i = 0; i < rec.width-ldist-5; i++) {
	// find the frequency
	float xfreq = m_MinXValue*pow(10,i/xstep);
	// find the position in the m_points[] array
	unsigned long int xposit = (unsigned long int)((xfreq/m_MaxXValue)*m_points.GetCount());

	if ( xposit < 0) { xposit = 0; }
	if ( xposit < m_points.GetCount() ) { 

	  float ydatapoint = m_points[xposit];

	  if (ydatapoint > m_MaxYValue) {ydatapoint = m_MaxYValue; }
	  if (ydatapoint < m_MinYValue) { ydatapoint = m_MinYValue; }

	  float ypoint = rec.height-bdist - (rec.height-bdist-5)*( ydatapoint-m_MinYValue)/(m_MaxYValue-m_MinYValue);
	  if ( 0 == i ) {
	    dc.DrawPoint( (int) (i+rectX+ldist),  (int)(ypoint));
	  } else {
	    dc.DrawLine( lastx, lasty,  (int) (i+rectX+ldist),  (int)(ypoint));
	  }
	  lastx = (int)(i+rectX+ldist); lasty = (int)(ypoint);
	} // else skip the frequencies out ot FFT
      }
    }
    // right channel
    dc.SetPen( wxPen( m_tr2Color,1,1));
    if ( m_points2.GetCount() > 0) {
      for (long int i = 0; i < rec.width-ldist-5; i++) {
	// find the frequency
	float xfreq = m_MinXValue*pow(10,i/xstep);
	// find the position in the m_points[] array
	unsigned long int xposit = (unsigned long int)((xfreq/m_MaxXValue)*m_points2.GetCount());

	if ( xposit < 0) { xposit = 0; }
	if ( xposit < m_points2.GetCount() ) { 

	  float ydatapoint = m_points2[xposit];

	  if (ydatapoint > m_MaxYValue) {ydatapoint = m_MaxYValue; }
	  if (ydatapoint < m_MinYValue) { ydatapoint = m_MinYValue; }

	  float ypoint = rec.height-bdist - (rec.height-bdist-5)*( ydatapoint-m_MinYValue)/(m_MaxYValue-m_MinYValue);
	  if ( 0 == i ) {
	    dc.DrawPoint( (int) (i+rectX+ldist),  (int)(ypoint));
	  } else {
	    dc.DrawLine( lastx, lasty,  (int) (i+rectX+ldist),  (int)(ypoint));
	  }
	  lastx = (int)(i+rectX+ldist); lasty = (int)(ypoint);
	} // else skip the frequencies out ot FFT
      }
    }

    /*  **************** linearni meritko *********************** */
  } else {
    /* cara bude minimalne kazdych 30 pixelu; pocet 2,5,10,20,50 atd. */
    // if we have m_NumberOfVerticals == 0, let's compute steps. But otherwise not
    int nline = 100; int xdiv = m_NumberOfVerticals; int nstrt = nline;
    while (xdiv == 0) {
      if ( rec.width < nline) {
	xdiv = 1*nline/nstrt;
      } else if ( rec.width < 2*nline) {
	xdiv = 2*nline/nstrt;
      } else if ( rec.width < 5*nline) {
	xdiv = 5*nline/nstrt;
      }
      nline *= 10;
    }

    for (int i = 0; i <= xdiv; i++) {
      /* kresli vsechny cary */
      float xstep = 1.0*(rec.width-ldist-5)/xdiv;

      dc.DrawLine((int)(rectX+xstep*i+ldist),rec.height+rectY-bdist, (int)(rectX+xstep*i+ldist),rectY+5);
      bla.Printf(wxT("%.1f"), m_MinXValue+(m_MaxXValue-m_MinXValue)*i/xdiv); dc.GetTextExtent(bla, &tw, &th);
      if ((xstep*i+ldist)<(tw/2)) {
	dc.DrawText(bla, (int)(rectX+ldist+xstep*i), rectY+rec.height-bdist);
      } else if ((ldist+xstep*i+tw/2) < rec.width) {
	dc.DrawText(bla, (int)(rectX+ldist+xstep*i-tw/2), rectY+rec.height-bdist);
      } else {
	dc.DrawText(bla, (int)(rectX+ldist+xstep*i-tw), rectY+rec.height-bdist);
      }
    }
    /* kresli body v SetTracks */
    dc.SetPen( wxPen( m_trColor,1,1));
    int lastx = 0, lasty = 0;
    float xpoint;
    for (unsigned int i = 0; i < m_points.GetCount(); i++) {
      /* prepocitat vstupni bod na X osu */
      //float xpoint = i+ldist; // linearni zavislost
      xpoint = ldist+i*(rec.width-ldist-5)/(m_MaxXValue-m_MinXValue);
      /* prepocitat Y hodnotu na Y osu - orezat hodnoty */
      if (m_points[i] > m_MaxYValue) { m_points[i] = m_MaxYValue; }
      if (m_points[i] < m_MinYValue) { m_points[i] = m_MinYValue; }
      float ypoint = rec.height-bdist - (rec.height-bdist-5)*(m_points[i]-m_MinYValue)/(m_MaxYValue-m_MinYValue);
      if ( 0 == i) {
	dc.DrawLine( (int)(xpoint),(int)(ypoint),(int)(xpoint),(int)(ypoint));
      } else {
	dc.DrawLine(lastx, lasty,(int)(xpoint),(int)(ypoint));
      }
      lastx = (int)(xpoint); lasty = (int)(ypoint);
    }
    dc.SetPen( wxPen( m_tr2Color,1,1));
    for (unsigned int i = 0; i < m_points2.GetCount(); i++) {
      /* prepocitat vstupni bod na X osu */
      //float xpoint = i+ldist; // linearni zavislost
      xpoint = ldist+i*(rec.width-ldist-5)/(m_MaxXValue-m_MinXValue);
      /* prepocitat Y hodnotu na Y osu - orezat hodnoty */
      if (m_points2[i] > m_MaxYValue) { m_points2[i] = m_MaxYValue; }
      if (m_points2[i] < m_MinYValue) { m_points2[i] = m_MinYValue; }
      float ypoint = rec.height-bdist - (rec.height-bdist-5)*(m_points2[i]-m_MinYValue)/(m_MaxYValue-m_MinYValue);
      if ( 0 == i) {
	dc.DrawLine( (int)(xpoint),(int)(ypoint),(int)(xpoint),(int)(ypoint));
      } else {
	dc.DrawLine(lastx, lasty,(int)(xpoint),(int)(ypoint));
      }
      lastx = (int)(xpoint); lasty = (int)(ypoint);
    }

    if (wxT("") != m_UserText) {
      dc.SetTextForeground( m_whColor);
      dc.DrawText( m_UserText, rectX+m_UserTextPosX, rectY+m_UserTextPosY);
    }
  }

} // OnPaint

/////////////////////////////////////////////////////////////////////////////
void CtrlOScope::OnSize(wxSizeEvent& evt) //UINT nType, int cx, int cy)
{
  evt.Skip();
} // OnSize

