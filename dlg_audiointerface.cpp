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

IMPLEMENT_DYNAMIC_CLASS(AudioInterfaceDialog, wxDialog)

/*!
 * AudioInterfaceDialog event table definition
 */

BEGIN_EVENT_TABLE(AudioInterfaceDialog, wxDialog)

////@begin AudioInterfaceDialog event table entries
////@end AudioInterfaceDialog event table entries
EVT_CHOICE(ID_OUTDEV_CHO, AudioInterfaceDialog::OnChoiceChanged)
EVT_CHOICE(ID_INDEV_CHO, AudioInterfaceDialog::OnChoiceChanged)

END_EVENT_TABLE()

/*!
 * AudioInterfaceDialog constructors
 */

AudioInterfaceDialog::AudioInterfaceDialog() { Init(); }

AudioInterfaceDialog::AudioInterfaceDialog(wxWindow* parent, wxWindowID id, const wxString& caption,
                                           const wxPoint& pos, const wxSize& size, long style) {
  Init();
  Create(parent, id, caption, pos, size, style);
}

/*!
 * AudioInterfaceDialog creator
 */

bool AudioInterfaceDialog::Create(wxWindow* parent, wxWindowID id, const wxString& caption,
                                  const wxPoint& pos, const wxSize& size, long style) {
  ////@begin AudioInterfaceDialog creation
  SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
  wxDialog::Create(parent, id, caption, pos, size, style);

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  ////@end AudioInterfaceDialog creation
  return true;
}

/*!
 * Member initialisation
 */

void AudioInterfaceDialog::Init() {
  ////@begin AudioInterfaceDialog member initialisation
  ////@end AudioInterfaceDialog member initialisation
}
/*!
 * Control creation for AudioInterfaceDialog
 */

void AudioInterfaceDialog::CreateControls() {
  ////@begin AudioInterfaceDialog content construction
  AudioInterfaceDialog* itemDialog1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemDialog1->SetSizer(itemBoxSizer2);

  wxStaticBox* itemFreqStaticBox =
      new wxStaticBox(itemDialog1, wxID_ANY, _("Available Frequencies [Hz]"));
  wxStaticBoxSizer* itemFreqStaticBoxSizer = new wxStaticBoxSizer(itemFreqStaticBox, wxHORIZONTAL);
  itemBoxSizer2->Add(itemFreqStaticBoxSizer, 1, wxEXPAND | wxALL, 5);
  wxString* itemFreqChoiceStrings = NULL;
  wxChoice* itemFreqChoice = new wxChoice(itemDialog1, ID_FREQ_CHO, wxDefaultPosition,
                                          wxDefaultSize, 0, itemFreqChoiceStrings, 0);
  itemFreqStaticBoxSizer->Add(itemFreqChoice, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxStaticBox* itemStaticBoxSizer3Static =
      new wxStaticBox(itemDialog1, wxID_ANY, _("Output Audio Device"));
  wxStaticBoxSizer* itemStaticBoxSizer3 =
      new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxHORIZONTAL);
  itemBoxSizer2->Add(itemStaticBoxSizer3, 1, wxEXPAND | wxALL, 5);

  wxString* itemChoice4Strings = NULL;
  wxChoice* itemChoice4 = new wxChoice(itemDialog1, ID_OUTDEV_CHO, wxDefaultPosition, wxDefaultSize,
                                       0, itemChoice4Strings, 0);
  itemStaticBoxSizer3->Add(itemChoice4, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxStaticBox* itemStaticBoxSizer5Static =
      new wxStaticBox(itemDialog1, wxID_ANY, _("Input Audio Device"));
  wxStaticBoxSizer* itemStaticBoxSizer5 =
      new wxStaticBoxSizer(itemStaticBoxSizer5Static, wxHORIZONTAL);
  itemBoxSizer2->Add(itemStaticBoxSizer5, 0, wxEXPAND | wxALL, 5);

  wxString* itemChoice6Strings = NULL;
  wxChoice* itemChoice6 = new wxChoice(itemDialog1, ID_INDEV_CHO, wxDefaultPosition, wxDefaultSize,
                                       0, itemChoice6Strings, 0);
  itemStaticBoxSizer5->Add(itemChoice6, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

  wxButton* itemButton8 =
      new wxButton(itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0);
  itemBoxSizer7->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  itemBoxSizer7->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxButton* itemButton10 =
      new wxButton(itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
  itemBoxSizer7->Add(itemButton10, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  ////@end AudioInterfaceDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool AudioInterfaceDialog::ShowToolTips() { return true; }

/*!
 * Get bitmap resources
 */

wxBitmap AudioInterfaceDialog::GetBitmapResource(const wxString& name) {
  // Bitmap retrieval
  ////@begin AudioInterfaceDialog bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
  ////@end AudioInterfaceDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon AudioInterfaceDialog::GetIconResource(const wxString& name) {
  // Icon retrieval
  ////@begin AudioInterfaceDialog icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
  ////@end AudioInterfaceDialog icon retrieval
}

void AudioInterfaceDialog::SetDevices(RWAudioDevList devreclist, RWAudioDevList devpllist,
                                      AIStreamSettings streamSettings) {
  unsigned int pldev = 0, recdev = 0;
  unsigned int cfreq = 0;
  m_DevRecList = devreclist;
  m_DevPlayList = devpllist;
  m_freq = streamSettings.freq;

  wxChoice* p_cho = (wxChoice*)FindWindow(ID_OUTDEV_CHO);
  if (!p_cho) {
    return;
  }
  p_cho->Clear();

  int curSelection = 0;
  for (unsigned int i = 0; i < devpllist.card_info.size(); i++) {
    wxString newstr(devpllist.card_info[i].name.c_str(), wxConvUTF8);
    p_cho->Append(newstr);
    if (devpllist.card_pos[i] == streamSettings.playDev) {
      curSelection = i;
    }
  }
  p_cho->SetSelection(curSelection);

  p_cho = (wxChoice*)FindWindow(ID_INDEV_CHO);
  if (!p_cho) {
    return;
  }
  p_cho->Clear();

  curSelection = 0;
  for (unsigned int i = 0; i < devreclist.card_info.size(); i++) {
    wxString newstr(devreclist.card_info[i].name.c_str(), wxConvUTF8);
    p_cho->Append(newstr);
    if (devreclist.card_pos[i] == streamSettings.recordDev) {
      curSelection = i;
    }
  }
  p_cho->SetSelection(curSelection);

  p_cho = (wxChoice*)FindWindow(ID_FREQ_CHO);
  if (!p_cho) {
    return;
  }
  p_cho->Clear();

  if (0 < devreclist.card_info.size() && 0 < devpllist.card_info.size()) {
    // compute the new list - find the same values in DevRecList and DevPlayList
    for (unsigned int i = 0; i < m_DevPlayList.card_info[pldev].sampleRates.size(); i++) {
      for (unsigned int j = 0; j < m_DevRecList.card_info[recdev].sampleRates.size(); j++) {
        unsigned int srateplay = m_DevPlayList.card_info[pldev].sampleRates[i];
        unsigned int sraterec = m_DevRecList.card_info[recdev].sampleRates[j];

        if (srateplay == sraterec) {
          if (srateplay == m_freq) cfreq = p_cho->GetCount();
          p_cho->Append(wxString::Format(wxT("%d "), srateplay));
        }
      }
    }
  }
  if (p_cho->GetCount() > 0) {
    p_cho->SetSelection(cfreq);
  }
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
}

void AudioInterfaceDialog::GetSelectedDevs(unsigned int* recdev, unsigned int* playdev,
                                           unsigned int* newfreq) {
  wxString strfreq;
  int seldev;

  wxChoice* p_cho = (wxChoice*)FindWindow(ID_OUTDEV_CHO);
  if (!p_cho) {
    return;
  }
  if (0 >= m_DevPlayList.card_info.size()) return;
  seldev = p_cho->GetSelection();
  *playdev = m_DevPlayList.card_pos[seldev];

  p_cho = (wxChoice*)FindWindow(ID_INDEV_CHO);
  if (!p_cho) {
    return;
  }
  if (0 >= m_DevRecList.card_info.size()) return;
  seldev = p_cho->GetSelection();
  *recdev = m_DevRecList.card_pos[seldev];

  p_cho = (wxChoice*)FindWindow(ID_FREQ_CHO);
  if (!p_cho) {
    return;
  }
  strfreq = p_cho->GetString(p_cho->GetSelection());
  *newfreq = (unsigned int)wxAtoi(strfreq);
}

// fill the frequency table
void AudioInterfaceDialog::OnChoiceChanged(wxCommandEvent& WXUNUSED(event)) {
  unsigned int pldev, recdev;
  unsigned int cfreq = 0;

  wxChoice* p_cho = (wxChoice*)FindWindow(ID_OUTDEV_CHO);
  if (!p_cho) {
    return;
  }
  pldev = p_cho->GetSelection();

  p_cho = (wxChoice*)FindWindow(ID_INDEV_CHO);
  if (!p_cho) {
    return;
  }
  recdev = p_cho->GetSelection();

  // clean the frequency list
  p_cho = (wxChoice*)FindWindow(ID_FREQ_CHO);
  if (!p_cho) {
    return;
  }
  p_cho->Clear();

  if (0 < m_DevRecList.card_info.size() && 0 < m_DevPlayList.card_info.size()) {
    // compute the new list - find the same values in DevRecList and DevPlayList
    for (unsigned int i = 0; i < m_DevPlayList.card_info[pldev].sampleRates.size(); i++) {
      for (unsigned int j = 0; j < m_DevRecList.card_info[recdev].sampleRates.size(); j++) {
        unsigned int srateplay = m_DevPlayList.card_info[pldev].sampleRates[i];
        unsigned int sraterec = m_DevRecList.card_info[recdev].sampleRates[j];

        if (srateplay == sraterec) {
          if (srateplay == m_freq) cfreq = p_cho->GetCount();
          p_cho->Append(wxString::Format(wxT("%d "), srateplay));
        }
      }
    }
  }
  if (p_cho->GetCount() > 0) {
    p_cho->SetSelection(cfreq);
  }
}
