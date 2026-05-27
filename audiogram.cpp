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

#include "audiogram.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <random>
#include <sstream>

Audiogram::Audiogram() : m_audio(NULL), m_running(false), m_heardFlag(false), m_freqset(ISO_389_BASIC) {
}

void Audiogram::SetAudio(RWAudio* audio) { m_audio = audio; }

void Audiogram::SetCallbacks(std::function<void(const wxString&)> statusCb, std::function<void()> refreshCb, std::function<void()> doneCb) {
  m_statusCb = statusCb;
  m_refreshCb = refreshCb;
  m_doneCb = doneCb;
}

/** Get the default frequency set for a given preset. */
static std::vector<double> GetDefaultFreqs(Audiogram::FreqSet fs) {
  switch (fs) {
    case Audiogram::ISO_389_FULL:
      return {125, 250, 500, 750, 1000, 1500, 2000, 3000, 4000, 6000, 8000, 12000, 16000};
    case Audiogram::FINE_200HZ: {
      std::vector<double> freqs;
      for (int f = 100; f <= 16000; f += 200) freqs.push_back((double)f);
      return freqs;
    }
    case Audiogram::ISO_389_BASIC:
    default:
      return {125, 250, 500, 1000, 2000, 4000, 8000, 16000};
  }
}

void Audiogram::StartTest(FreqSet freqset, const std::vector<double>& customFreqs) {
 // If a previous test thread exists, always join it first (even if already completed)
    if (m_testThread.joinable()) {
      m_testThread.join();
    }

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_freqset = freqset;

    if (freqset == CUSTOM && !customFreqs.empty()) {
      m_testFreqs = customFreqs;
    } else {
      m_testFreqs = GetDefaultFreqs(freqset);
    }

    ClearResults();
    m_running = true;
    m_heardFlag = false;
  }

  if (m_statusCb) m_statusCb(wxT("Testing in progress — press <space> when you hear a tone."));

  // Start the audio stream so PlaySetAudiogram can produce sound
  if (m_audio) m_audio->StartSnd();

  // Launch test in background thread so UI stays responsive
  m_testThread = std::thread([this]() { RunTestLoop(); });
}

void Audiogram::Stop() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_running = false;
  }
  m_cv.notify_one();
  if (m_testThread.joinable()) {
    m_testThread.join();
  }
}

void Audiogram::RegisterHeard() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_heardFlag = true;
  }
  m_cv.notify_one();
}

/** Wait on CV with a timeout in milliseconds, checking flags periodically so Stop/RegisterHeard interrupt immediately. */
bool Audiogram::WaitWithTimeout(int ms) {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_cv.wait_for(lock, std::chrono::milliseconds(ms), [this]() { return !m_running || m_heardFlag; });

  bool wasRunning = m_running;
  bool wasHeard = m_heardFlag;
  lock.unlock();
  return wasRunning && !wasHeard; // true if we timed out without being interrupted
}

int Audiogram::FindThreshold(double freq, bool left) {
  // Logarithmic sweep: pct maps linearly to dB from -80dB to 0dB.
  // This gives fine resolution at low levels where it matters for threshold detection.
  // pct=0 → gain≈0 (-80dB, near silence), pct=50 → gain=0.01 (-40dB), pct=100 → gain=1.0 (max)
  const int minDb = -80;

  for (int pct = 0; pct <= 100; pct += 1) {
    float volume = powf(10.0f, ((float)pct * (-minDb) / 100.0f + minDb) / 20.0f);

    if (left) {
      m_audio->PlaySetAudiogram((float)freq, volume, 0.0f, 0.0f);
    } else {
      m_audio->PlaySetAudiogram(0.0f, 0.0f, (float)freq, volume);
    }

    // Hold at each level long enough for user to react — 350ms × 101 steps ≈ 35s full sweep
    WaitWithTimeout(350);

    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_running || m_heardFlag) return pct; // heard at this level
    }
  }

  // User never heard the tone even at max — record as inaudible threshold (100%)
  return 100;
}

void Audiogram::RunTestLoop() {
  if (!m_audio) return;

  std::vector<double> testFreqs = m_testFreqs;
  int nfreqs = (int)testFreqs.size();

  // Build test list: each frequency tested on both ears
  struct TestItem {
    double freq;
    bool left;
  };
  std::vector<TestItem> items;
  for (int i = 0; i < nfreqs; i++) {
    items.push_back({testFreqs[i], true});
    items.push_back({testFreqs[i], false});
  }

  // Randomize test order to prevent prediction
  std::seed_seq seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  std::shuffle(items.begin(), items.end(), std::mt19937(seed));

  for (size_t ti = 0; ti < items.size(); ti++) {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_running) break;
    }

    double freq = items[ti].freq;
    bool left = items[ti].left;

    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_heardFlag = false;
    }

    int thresholdPct = FindThreshold(freq, left);

    // Silence immediately so user doesn't hear the tone continuing during pause
    m_audio->PlaySetAudiogram(0.0f, 0.0f, 0.0f, 0.0f);

    ResultEntry entry;
    entry.ear = left ? wxT("Left") : wxT("Right");
    entry.frequency = freq;
    entry.thresholdPct = thresholdPct; // logarithmic %: low=quiet, high=loud
    entry.audible = thresholdPct < 100;

    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_results.push_back(entry);
    }

    if (m_refreshCb) m_refreshCb();

    // Unpredictable pause between tones: 2-4 seconds, so user can't anticipate next beep.
    // Interruptible by Stop() via the condition variable.
    {
      std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
      std::uniform_int_distribution<int> dist(3000, 5000);
      int pause_ms = dist(rng);
      for (int waited = 0; waited < pause_ms; waited += 250) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait_for(lock, std::chrono::milliseconds(250), [this]() { return !m_running; });
        if (!m_running) break;
      }
    }
  }

  // Final silence (zero frequency → zero gain in callback)
  m_audio->PlaySetAudiogram(0.0f, 0.0f, 0.0f, 0.0f);

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_running = false;

    // Sort results by frequency, then left before right
    std::sort(m_results.begin(), m_results.end(), [](const ResultEntry& a, const ResultEntry& b) {
      if (a.frequency != b.frequency) return a.frequency < b.frequency;
      return a.ear < b.ear;
    });
  }

  // Stop the audio stream so it doesn't interfere with other tabs
  if (m_audio) m_audio->StopSnd();

  wxString done;
  done.Printf(wxT("Test complete. %zu results recorded."), m_results.size());
  if (m_statusCb) m_statusCb(done);
  if (m_refreshCb) m_refreshCb();
  if (m_doneCb) m_doneCb();
}

wxArrayDouble Audiogram::GetThresholdsLeft() const {
   wxArrayDouble freqs = GetFrequencies();
   wxArrayDouble thresholds;
   for (size_t fi = 0; fi < freqs.size(); fi++) {
     for (const auto& r : m_results) {
       if (r.frequency == freqs[fi] && (r.ear == wxT("Left") || r.ear == wxT("Both"))) {
          thresholds.Add((double)r.thresholdPct);
          break;
       }
     }
   }
   return thresholds;
}

wxArrayDouble Audiogram::GetThresholdsRight() const {
   wxArrayDouble freqs = GetFrequencies();
   wxArrayDouble thresholds;
   for (size_t fi = 0; fi < freqs.size(); fi++) {
     for (const auto& r : m_results) {
       if (r.frequency == freqs[fi] && (r.ear == wxT("Right") || r.ear == wxT("Both"))) {
          thresholds.Add((double)r.thresholdPct);
          break;
       }
     }
   }
   return thresholds;
}

wxArrayDouble Audiogram::GetFrequencies() const {
   // Only return frequencies that actually have results (not all planned testFreqs)
   std::vector<double> freqs;
   for (const auto& r : m_results) {
     bool found = false;
     for (size_t j = 0; j < freqs.size(); j++) {
       if (freqs[j] == r.frequency) { found = true; break; }
     }
     if (!found) freqs.push_back(r.frequency);
   }
   std::sort(freqs.begin(), freqs.end());

   wxArrayDouble out;
   for (double f : freqs) out.Add(f);
   return out;
}

void Audiogram::ClearResults() { m_results.clear(); }

/** Save results to CSV in FreeHearingTest format. */
bool Audiogram::SaveCSV(const wxString& filename, const wxString& subjectName) {
  if (m_results.empty()) return false;

  // Header line with metadata
  std::ostringstream buf;
  buf << "; AudMeS audiogram data" << std::endl;
  buf << "; Subject: " << subjectName.mb_str() << std::endl;
  wxDateTime now = wxDateTime::Now();
  wxString dateStr;
  dateStr.Printf(wxT("; Date: %s"), now.FormatISODate().mb_str());
  buf << dateStr.mb_str() << std::endl;

  // CSV header: threshold as % on logarithmic loudness scale (0%=near silence/-80dB, 100%=max output/0dB)
  buf << "Ear, Frequency(Hz), Threshold(%log), Audible" << std::endl;

  for (size_t i = 0; i < m_results.size(); i++) {
    wxString audible = m_results[i].audible ? wxT("true") : wxT("false");
    int freqInt = (int)m_results[i].frequency;
    buf << m_results[i].ear.mb_str() << "," << freqInt << "," << m_results[i].thresholdPct << ","
        << audible.mb_str() << std::endl;
  }

  // Write entire buffer at once to avoid zero-byte file on failure
  std::ofstream out(filename.mb_str(), std::ios::trunc);
  if (!out.is_open()) return false;
  out << buf.str();
  out.close();
  return true;
}

/** Load results from an FreeHearingTest CSV file. */
bool Audiogram::LoadCSV(const wxString& filename) {
  std::ifstream in(filename.mb_str());
  if (!in.is_open()) return false;

  ClearResults();

  std::string line;
  while (std::getline(in, line)) {
    // Skip comment lines and header (accept both old "Amplitude" and new "Threshold(%)" formats)
    if (line.empty() || line[0] == ';') continue;
    if (line.substr(0, 3) == "Ear") continue;

    // Parse CSV: Ear, Frequency, Amplitude, Audible
    std::stringstream ss(line);
    std::string earStr, freqStr, ampStr, audibleStr;

    if (!std::getline(ss, earStr, ',') || !std::getline(ss, freqStr, ',') ||
        !std::getline(ss, ampStr, ',') || !std::getline(ss, audibleStr)) {
      continue; // skip malformed line
    }

    ResultEntry entry;
    entry.ear = wxString(earStr.c_str(), wxConvUTF8);

    try {
      entry.frequency = std::stod(freqStr);
      entry.thresholdPct = std::stoi(ampStr);
    } catch (...) {
      continue; // skip malformed numbers
    }

    entry.audible = (audibleStr == "true");
    m_results.push_back(entry);
  }

  in.close();

  if (m_results.empty()) return false;

  // Reconstruct frequency list from results
  std::vector<double> freqs;
  for (size_t i = 0; i < m_results.size(); i++) {
    bool found = false;
    for (size_t j = 0; j < freqs.size(); j++) {
      if (freqs[j] == m_results[i].frequency) {
        found = true;
        break;
      }
    }
    if (!found) freqs.push_back(m_results[i].frequency);
  }
  std::sort(freqs.begin(), freqs.end());
  m_testFreqs = freqs;

  return true;
}
