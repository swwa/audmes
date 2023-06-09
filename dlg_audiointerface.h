/////////////////////////////////////////////////////////////////////////////
// Name:        dlg_audiointerface.h
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

#ifndef _DLG_AUDIOINTERFACE_H_
#define _DLG_AUDIOINTERFACE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "dlg_audiointerface.h"
#endif

/*!
 * Includes
 */

#include "RWAudio_IO.h"

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
#define SYMBOL_AUDIOINTERFACEDIALOG_STYLE wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCLOSE_BOX
#define SYMBOL_AUDIOINTERFACEDIALOG_TITLE _("Audio Interface Configuration")
#define SYMBOL_AUDIOINTERFACEDIALOG_IDNAME 32000
#define SYMBOL_AUDIOINTERFACEDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_AUDIOINTERFACEDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * AudioInterfaceDialog class declaration
 */

struct AIStreamSettings {
	unsigned int playDev;
	unsigned int recordDev;
	unsigned long int freq;
};


class AudioInterfaceDialog : public wxDialog {
  DECLARE_DYNAMIC_CLASS(AudioInterfaceDialog)
  DECLARE_EVENT_TABLE()

  RWAudioDevList m_DevRecList;
  RWAudioDevList m_DevPlayList;
  AIStreamSettings m_StreamSettings;
  unsigned long int m_freq;

 public:
  /// Constructors
  AudioInterfaceDialog();
  AudioInterfaceDialog(wxWindow* parent, wxWindowID id = SYMBOL_AUDIOINTERFACEDIALOG_IDNAME,
                       const wxString& caption = SYMBOL_AUDIOINTERFACEDIALOG_TITLE,
                       const wxPoint& pos = SYMBOL_AUDIOINTERFACEDIALOG_POSITION,
                       const wxSize& size = SYMBOL_AUDIOINTERFACEDIALOG_SIZE,
                       long style = SYMBOL_AUDIOINTERFACEDIALOG_STYLE);

  /// Creation
  bool Create(wxWindow* parent, wxWindowID id = SYMBOL_AUDIOINTERFACEDIALOG_IDNAME,
              const wxString& caption = SYMBOL_AUDIOINTERFACEDIALOG_TITLE,
              const wxPoint& pos = SYMBOL_AUDIOINTERFACEDIALOG_POSITION,
              const wxSize& size = SYMBOL_AUDIOINTERFACEDIALOG_SIZE,
              long style = SYMBOL_AUDIOINTERFACEDIALOG_STYLE);

  /// Initialises member variables
  void Init();

  /// Creates the controls and sizers
  void CreateControls();

  ////@begin AudioInterfaceDialog event handler declarations

  ////@end AudioInterfaceDialog event handler declarations

  ////@begin AudioInterfaceDialog member function declarations

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource(const wxString& name);

  /// Retrieves icon resources
  wxIcon GetIconResource(const wxString& name);
  ////@end AudioInterfaceDialog member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

  ////@begin AudioInterfaceDialog member variables
  ////@end AudioInterfaceDialog member variables
  void SetDevices(RWAudioDevList devreclist, RWAudioDevList devpllist, AIStreamSettings streamSettings);
  void GetSelectedDevs(unsigned int* recdev, unsigned int* playdev, unsigned long int* newfreq);
  void OnChoiceChanged(wxCommandEvent& event);
};

#endif
// _DLG_AUDIOINTERFACE_H_
