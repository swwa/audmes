// CtrlOScope.h : header file
//

#ifndef __CTRLOSCOPE_H__
#define __CTRLOSCOPE_H__

#include <wx/wx.h>
#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY( double, wxArrayDouble);


class CtrlOScope : public wxControl
{
public:
	CtrlOScope( wxWindow* parent, wxString xname = _T("X"), wxString yname = _T("Y"), int tracks = 1);

	virtual ~CtrlOScope();
	//void OnTimer(wxTimerEvent& evt);
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void PaintAll( wxDC & dc);
	void PaintAllFunction( wxDC & dc, int rectX, int rectY);
	void OnEraseBackground( wxEraseEvent& event ) { };

	void AppendPoints(double dNewPoint[], int iTrack = 0);
	//	void SetRange(double dLower, double dUpper, int iTrack = 0, int logrange = false);
	void SetTrack(double dTrack [], int iTrack = 0);
	void SetXRange(double dLower, double dUpper, int logrange);
	void SetYRange(double dLower, double dUpper, int logrange, int itrack);



	void SetXUnits(wxString string, wxString XMin = "", wxString XMax = "");
	void SetYUnits(wxString string, wxString YMin = "", wxString YMax = "");
	void SetGridColor(wxColour color);
	void SetPlotColor(wxColour color, int iTrack = 0);
	void SetBackgroundColor(wxColour color);
	void Reset( int iTrack = 0);
	void SetTrack( wxArrayDouble ardbl);
	void SetTrack2( wxArrayDouble ardbl);
	void SetNumOfVerticals( int num) { m_NumberOfVerticals = num; };

	void ShowUserText( wxString text, int xpos, int ypos) { m_UserText = text; m_UserTextPosX = xpos; m_UserTextPosY = ypos; };

	virtual wxSize GetSize() const;
	virtual wxSize DoGetBestSize() const;
/* 	void GetSize( int *w, int *h) { *w = 300; *h = 150; } */
/* 	wxSize GetMinSize( void) { return wxSize( 300,150); } */
/* 	wxSize GetClientSize( void) { return wxSize( 300,150); } */
/* 	wxSize GetAdjustedBestSize( void) { return wxSize( 300,150); } */

protected:
	int m_tracks;
	int m_width, m_height;

	wxArrayDouble m_points;
	wxArrayDouble m_points2;
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
#endif // __CTRLOSCOPE_H__
