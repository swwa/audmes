/////////////////////////////////////////////////////////////////////////////
// Name:        dlg_audiointerface.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     29/03/2007 11:48:34
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dlg_audiointerface.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "dlg_audiointerface.h"
#include "event_ids.h"

////@begin XPM images
////@end XPM images

/*!
 * AudioInterfaceDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( AudioInterfaceDialog, wxDialog )

/*!
 * AudioInterfaceDialog event table definition
 */

BEGIN_EVENT_TABLE( AudioInterfaceDialog, wxDialog )

////@begin AudioInterfaceDialog event table entries
////@end AudioInterfaceDialog event table entries

END_EVENT_TABLE()

/*!
 * AudioInterfaceDialog constructors
 */

AudioInterfaceDialog::AudioInterfaceDialog( )
{
    Init();
}

AudioInterfaceDialog::AudioInterfaceDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * AudioInterfaceDialog creator
 */

bool AudioInterfaceDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin AudioInterfaceDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end AudioInterfaceDialog creation
    return true;
}

/*!
 * Member initialisation 
 */

void AudioInterfaceDialog::Init()
{
////@begin AudioInterfaceDialog member initialisation
////@end AudioInterfaceDialog member initialisation
}
/*!
 * Control creation for AudioInterfaceDialog
 */

void AudioInterfaceDialog::CreateControls()
{    
////@begin AudioInterfaceDialog content construction
    AudioInterfaceDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Output Audio Device"));
    wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxString* itemChoice4Strings = NULL;
    wxChoice* itemChoice4 = new wxChoice( itemDialog1, ID_OUTDEV_CHO, wxDefaultPosition, wxDefaultSize, 0, itemChoice4Strings, 0 );
    itemStaticBoxSizer3->Add(itemChoice4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer5Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Input Audio Device"));
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(itemStaticBoxSizer5Static, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxString* itemChoice6Strings = NULL;
    wxChoice* itemChoice6 = new wxChoice( itemDialog1, ID_INDEV_CHO, wxDefaultPosition, wxDefaultSize, 0, itemChoice6Strings, 0 );
    itemStaticBoxSizer5->Add(itemChoice6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer7->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton10 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(itemButton10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end AudioInterfaceDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool AudioInterfaceDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap AudioInterfaceDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin AudioInterfaceDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end AudioInterfaceDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon AudioInterfaceDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin AudioInterfaceDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end AudioInterfaceDialog icon retrieval
}

void AudioInterfaceDialog::SetDevices( wxArrayString devreclist, wxArrayString devpllist)
{
  int i;

    wxChoice* p_cho = (wxChoice *) FindWindow( ID_OUTDEV_CHO);
    if(!p_cho) {
        return;
    }
    p_cho->Clear();

    for(i = 0; i< devpllist.GetCount(); i++) {
      p_cho->Append(devpllist[i]);
    }
    p_cho->SetSelection(0);

    p_cho = (wxChoice *) FindWindow( ID_INDEV_CHO);
    if(!p_cho) {
        return;
    }
    p_cho->Clear();

    for(i = 0; i< devreclist.GetCount(); i++) {
      p_cho->Append(devreclist[i]);
    }
    p_cho->SetSelection(0);
}

void AudioInterfaceDialog::GetSelectedDevs( int * recdev, int * playdev)
{
    wxChoice* p_cho = (wxChoice *) FindWindow( ID_OUTDEV_CHO);
    if(!p_cho) {
        return;
    }
    *playdev = p_cho->GetSelection();

    p_cho = (wxChoice *) FindWindow( ID_INDEV_CHO);
    if(!p_cho) {
        return;
    }
    *recdev = p_cho->GetSelection();

}
