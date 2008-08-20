// OScopeCtrl.cpp
// verze 0.0.1
//  - X osa logaritmicka
//  - Y osa linearni
//  - zadna kontrola rozsahu !!!!

#include "OScopeCtrl.h"
//#include "event_ids.h"
#include <math.h>
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayDouble);

/////////////////////////////////////////////////////////////////////////////
// OScopeCtrl
OScopeCtrl::OScopeCtrl( wxWindow* parent, wxString xname, wxString yname, int tracks)
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
  //m_trColor.Set(255,255,255);
  m_YUnit = yname;
  m_XUnit = xname;

  Connect( -1, wxEVT_SIZE,(wxObjectEventFunction)& OScopeCtrl::OnSize);
  Connect( -1, wxEVT_PAINT,(wxObjectEventFunction)& OScopeCtrl::OnPaint);

}  // OScopeCtrl

/////////////////////////////////////////////////////////////////////////////
OScopeCtrl::~OScopeCtrl()
{
} // ~OScopeCtrl

void OScopeCtrl::SetTrack( wxArrayDouble ardbl)
{
  m_points = ardbl;
  wxClientDC dc( this);
  PaintAll( dc);
}

void OScopeCtrl::SetTrack2( wxArrayDouble ardbl)
{
  m_points2 = ardbl;
  wxClientDC dc( this);
  PaintAll( dc);
}

/////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::SetXRange(double dLower, double dUpper, int logrange)
{
  m_MinXValue = dLower;
  m_MaxXValue = dUpper;
  m_LogX = logrange;
}  // SetRange

void OScopeCtrl::SetYRange(double dLower, double dUpper, int logrange, int itrack)
{
  m_MinYValue = dLower;
  m_MaxYValue = dUpper;
  m_LogY = logrange;
}  // SetRange

void OScopeCtrl::SetXUnits(wxString string, wxString XMin, wxString XMax )
{
}  // SetXUnits

void OScopeCtrl::SetYUnits(wxString string, wxString YMin, wxString YMax )
{
}  // SetYUnits

/////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::SetGridColor(wxColour color)
{
}  // SetGridColor


/////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::SetPlotColor(wxColour color, int iTrack)
{
}  // SetPlotColor

/////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::SetBackgroundColor(wxColour color)
{
}  // SetBackgroundColor

/////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::AppendPoints(double dNewPoint[], int iTrack )
{
} // AppendPoints
 
wxSize OScopeCtrl::GetSize( void) const
{
  //      wxMessageBox("GetSize", "Application Error", wxOK | wxICON_ERROR, this);
  return wxSize( m_width, m_height);
}

wxSize OScopeCtrl::DoGetBestSize( void) const
{
  //  wxMessageBox("GetBestSize", "Application Error", wxOK | wxICON_ERROR, this);
  return wxSize(  m_width, m_height);
}


////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::OnPaint(wxPaintEvent& evt) 
{
	wxPaintDC dc(this) ;  // device context for painting
	PaintAll( dc);
}

void OScopeCtrl::PaintAll( wxDC & dc)
{
	wxRect rec = GetClientRect();
	wxString bla;
	int tw, th;
	int bdist, ldist;

	//	bla.Printf ("Rect is %d, %d, %d, %d", rec.x, rec.y, rec.width, rec.height);
	//	wxMessageBox( bla, "Application Error", wxOK | wxICON_ERROR, this);

	// kresli pozadi
	dc.SetPen( wxPen( m_bgColor,1,1));
	dc.SetBrush( wxBrush(m_bgColor, wxSOLID));
	dc.DrawRectangle( rec.x, rec.y, rec.width, rec.height);

	dc.SetPen( wxPen( m_plColor,1,1));
	dc.SetBrush( wxBrush(m_plColor, wxSOLID));
	dc.SetTextForeground( m_plColor);
      	dc.SetFont(wxFont(8, wxMODERN, wxNORMAL, wxNORMAL, 0, ""));
	/* spocitat vysku pisma pro spodni odstup a sirky pto odstup zleva */
	dc.GetTextExtent( m_XUnit, &tw, &th); bdist = th+5;
	dc.GetTextExtent( m_YUnit, &tw, &th); ldist = tw + 5;
	bla.Printf("%.1f", m_MinYValue); dc.GetTextExtent(bla, &tw, &th);
	if (ldist < (tw+5)) { ldist = tw+5;}
	bla.Printf("%.1f", m_MaxYValue); dc.GetTextExtent(bla, &tw, &th);
	if (ldist < (tw+5)) { ldist = tw+5;}

	/* tisk legendy */
	dc.DrawText(m_XUnit, rec.x+rec.width/2, rec.y+rec.height-bdist);
	dc.DrawText(m_YUnit, 5, rec.y+rec.height/2);

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
	//bla.Printf("ydiv = %d", ydiv); dc.DrawText(bla, rec.x+40,rec.y+20);
	for (int i = 0; i <= ydiv; i++) {
	  /* kresli vsechny cary */
	  float ystep = 1.0*(rec.height-bdist-5)/ydiv;
	  dc.DrawLine(rec.x+ldist, (int)(rec.height+rec.y-ystep*i-bdist),rec.x+rec.width-5, (int)(rec.height+rec.y-ystep*i-bdist));
	  bla.Printf("%.1f", m_MinYValue+(m_MaxYValue-m_MinYValue)*i/ydiv); dc.GetTextExtent(bla, &tw, &th);
	  if (ydiv == i) {
	    dc.DrawText(bla, (int)(rec.x+ldist-tw),(int)(rec.height+rec.y-ystep*i-bdist) );
	  } else {
	    dc.DrawText(bla, (int)(rec.x+ldist-tw),(int)(rec.height+rec.y-ystep*i-bdist-th) );
	  }
	}
	if ( (m_MinYValue+m_MaxYValue) < 0.01) {
	  /* kresli nulovou caru */
	  dc.DrawLine(rec.x+ldist, rec.y+5+(rec.height-bdist-5)/2,rec.x+rec.width-5, rec.y+5+(rec.height-bdist-5)/2);
	}

	/* spocitat jak casto se budou kreslit vertikalni cary */
	/* ******************************************************************* */
	/* ******************************************************************* */
	/* ******************************************************************* */
	if( 1 == m_LogX) {
	/* logaritmicke meritko - spocitat pocet dekad a potom neco dale */
	/* cara bude minimalne kazdych 100 pixelu na dekadu; pak bude 1, 3, 10 */
	  int ndecs = (int) log10( m_MaxXValue/m_MinXValue);
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
	    dc.DrawLine((int)(rec.x+ldist+xstep*i), rec.y+5, (int)(rec.x+ldist+xstep*i), rec.y+rec.height-bdist);
	    /* nakresli vnitrni cary */
	    if (3 == xdiv) {
	      dc.DrawLine((int)(rec.x+ldist+xstep*(i+log10(2))), rec.y+5, (int)(rec.x+ldist+xstep*(i+log10(2))), rec.y+rec.height-bdist);
	      dc.DrawLine((int)(rec.x+ldist+xstep*(i+log10(5))), rec.y+5, (int)(rec.x+ldist+xstep*(i+log10(5))), rec.y+rec.height-bdist);
	    }
	    if (10 == xdiv) {
	      for( int j = 2; j <10; j++) {
		dc.DrawLine((int)(rec.x+ldist+xstep*(i+log10(j))), rec.y+5, (int)(rec.x+ldist+xstep*(i+log10(j))), rec.y+rec.height-bdist);
	      }
	    }
	    bla.Printf("%.1f", m_MinXValue*pow(10,i)); dc.GetTextExtent(bla, &tw, &th);
	    if ((xstep*i+ldist)<(tw/2)) {
	      dc.DrawText(bla, (int)(rec.x+ldist+xstep*i), rec.y+rec.height-bdist);
	    } else if ((ldist+xstep*i+tw/2) < rec.width) {
	      dc.DrawText(bla, (int)(rec.x+ldist+xstep*i-tw/2), rec.y+rec.height-bdist);
	    } else {
	      dc.DrawText(bla, (int)(rec.x+ldist+xstep*i-tw), rec.y+rec.height-bdist);
	    }
	  }

	  /* zobrazit body */
	  //bla.Printf("Pocet bodu: %d", m_points.GetCount()); wxMessageBox(bla,"D",wxOK);
	  dc.SetPen( wxPen( m_trColor,1,1));
	  int lastx = 0, lasty = 0;
	  float xpoint;
	  for (unsigned int i = 0; i < m_points.GetCount(); i++) {
	    /* prepocitat vstupni bod na X osu */
	    //float xpoint = i+ldist; // linearni zavislost
	    if ( i < m_MinXValue) {
	      xpoint = ldist;
	    } else {
	      xpoint = ldist+ xstep*log10(i/m_MinXValue);
	    }
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
	} else {
	  /*  **************** linearni meritko *********************** */
	/* cara bude minimalne kazdych 30 pixelu; pocet 2,5,10,20,50 atd. */
	int nline = 100; int xdiv = 0; int nstrt = nline;
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

	  dc.DrawLine((int)(rec.x+xstep*i+ldist),rec.height+rec.y-bdist, (int)(rec.x+xstep*i+ldist),rec.y+5);
	  bla.Printf("%.1f", m_MinXValue+(m_MaxXValue-m_MinXValue)*i/xdiv); dc.GetTextExtent(bla, &tw, &th);
	  if ((xstep*i+ldist)<(tw/2)) {
	    dc.DrawText(bla, (int)(rec.x+ldist+xstep*i), rec.y+rec.height-bdist);
	  } else if ((ldist+xstep*i+tw/2) < rec.width) {
	    dc.DrawText(bla, (int)(rec.x+ldist+xstep*i-tw/2), rec.y+rec.height-bdist);
	  } else {
	    dc.DrawText(bla, (int)(rec.x+ldist+xstep*i-tw), rec.y+rec.height-bdist);
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

	}

} // OnPaint

/////////////////////////////////////////////////////////////////////////////
void OScopeCtrl::OnSize(wxSizeEvent& evt) //UINT nType, int cx, int cy)
{
  evt.Skip();
} // OnSize

