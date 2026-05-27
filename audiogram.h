/*
 * Copyright (C) 2026 Petter Reinholdtsen <pere@hungry.com>
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

#ifndef AUDIOGRAM_H
#define AUDIOGRAM_H

#include <chrono>
#include <cmath>
#include <cstddef>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include <wx/datetime.h>
#include <wx/dynarray.h>
#include <wx/string.h>

#include "RWAudio_IO.h"

class CtrlOScope;

/**
 * Audiogram: hearing threshold test module.
 *
 * Plays sine tones at configurable frequencies and volumes, asking the user to
 * press a button when they hear each tone.  The result is a plot of hearing
 * threshold (dB HL) vs frequency for left and right ear separately.
 */
class Audiogram {
 public:
  struct ResultEntry {
    wxString ear;       // "Left", "Right"
    double frequency;   // Hz
    int thresholdPct;   // logarithmic loudness % (0=quiet/-80dB, 100=loud/0dB) at which tone was heard
    bool audible;
  };

  enum FreqSet { ISO_389_BASIC, ISO_389_FULL, FINE_200HZ, CUSTOM };

  Audiogram();

  void SetAudio(RWAudio* audio);

  /**
   * Callbacks invoked by the test loop to update the UI.
   * @param statusCb called with status messages (e.g., "Testing Left ear at 1000 Hz...")
   * @param refreshCb called after results change to trigger chart redraw
   */
void SetCallbacks(std::function<void(const wxString&)> statusCb,
                     std::function<void()> refreshCb,
                     std::function<void()> doneCb = nullptr);

 /** Start a hearing test with the given frequency set. Threshold reported as % of max amplitude (0-100). */
   void StartTest(FreqSet freqset, const std::vector<double>& customFreqs = {});

  /** Stop an ongoing test early. */
  void Stop();

  /** Called by UI when user presses "I heard it!" button. */
  void RegisterHeard();

  bool IsRunning() const { return m_running; }

  /** Get threshold results for chart plotting (sorted by frequency). */
  wxArrayDouble GetThresholdsLeft() const;
  wxArrayDouble GetThresholdsRight() const;
  wxArrayDouble GetFrequencies() const;

  /** Save results to CSV in AudMeS format. */
  bool SaveCSV(const wxString& filename, const wxString& subjectName = wxT("Subject"));

  /** Load results from an AudMeS CSV file. Also accepts legacy FreeHearingTest format. Returns true on success. */
  bool LoadCSV(const wxString& filename);

  /** Get the internal result list (for inspection after load or test). */
  const std::vector<ResultEntry>& GetResults() const { return m_results; }

 private:
  void RunTestLoop();
  int FindThreshold(double freq, bool left);
  bool WaitWithTimeout(int ms);
  void ClearResults();

  RWAudio* m_audio;

  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_testThread;

  bool m_running;
  bool m_heardFlag;

  std::function<void(const wxString&)> m_statusCb;
  std::function<void()> m_refreshCb;
  std::function<void()> m_doneCb;

  FreqSet m_freqset;
  std::vector<double> m_testFreqs;

  std::vector<ResultEntry> m_results;
};

#endif // AUDIOGRAM_H
