# Audiogram — Hearing Threshold Test

## Overview

The **Audiogram** tab in AudMeS implements a basic hearing threshold test (pure-tone audiometry). It plays sine tones at various frequencies and volumes, asking you to press a button when you hear each tone. The result is displayed as an audiogram chart showing your hearing thresholds across frequency for left and right ear separately.

## Quick Start

1. **Select headphones.** Use good-quality closed-back headphones for accurate results.
2. **Choose frequency set** from the dropdown:
   - *ISO 389 Basic* (default): 8 frequencies — 125, 250, 500, 1000, 2000, 4000, 8000, 16000 Hz
   - *ISO 389 Full*: 13 frequencies — adds 750, 1500, 3000, 6000, 12000 Hz
   - *Fine-grained 200 Hz steps*: 80 frequencies from 100 to 16000 Hz in 200 Hz increments for detailed threshold mapping
3. **Click "Start"** to begin the test. A generic status message will appear during testing.
4. When you hear a tone, click **"I heard it!"** or press **Spacebar**.
5. The test proceeds automatically through all frequencies for both ears (order randomized).
6. Results appear on the chart as colored markers with an in-graph legend indicating left and right ear.

## How It Works

The test uses a seamless ascending sweep: each tone starts nearly silent and increases continuously using logarithmic gain scaling until you indicate you heard it, or reaches maximum level. The logarithmic scale provides fine resolution at quiet levels where threshold detection matters most. This is more clinically accurate than descending sweeps. The volume ramps smoothly (not discrete jumps), so the transition between levels is imperceptible.

### Result Display

- **X-axis**: Frequency in Hz (logarithmic scale, matching standard audiogram convention)
- **Y-axis**: Percentage-based logarithmic gain scale (0–100%). Higher values = better hearing (quieter threshold detected). The Y-axis label reads vertically as "% loud (log)".
- **Color legend**: Displayed in the top-left corner of the graph, showing which marker color corresponds to left ear vs right ear.

Good hearing indicators appear at the top of the graph (high percentage = quiet threshold detected). Poorer thresholds appear lower on the chart.

## CSV Import / Export

You can save and load audiogram results in FreeHearingTest CSV format.

### File Format

```csv
; AudMeS audiogram data
; Subject: John Doe
; Date: 2026-05-27
Ear, Frequency(Hz), Threshold(%log), Audible
Left,100,85,true
Right,100,78,true
Left,300,90,true
...
```

Fields:
- **Ear**: `Left`, `Right`, or `Both` (the exporter produces Left/Right pairs; the importer also accepts `Both`)
- **Frequency**: Test frequency in Hz
- **Threshold(%log)**: Threshold level as percentage on logarithmic gain scale (0–100%)
- **Audible**: `true` if the tone was heard at some point, `false` if not audible even at maximum

### Menu Commands

- **File → Save audiogram CSV…** — saves current results to a `.csv` file
- **File → Load audiogram CSV…** — loads results from a compatible `.csv` file and updates the chart

## Accuracy Notes

This tool provides an approximate screening test. It is not calibrated to clinical standards:
- The percentage values are relative, not calibrated to absolute dB HL
- Speaker/headphone frequency response affects accuracy significantly
- No calibration microphone or coupler is used
- Environmental noise can interfere with low-level tones

For a proper hearing assessment, consult an audiologist who uses calibrated equipment in a sound-treated booth.

## Keyboard Shortcuts

| Key | Action |
|---|---|
| **Spacebar** | Press "I heard it!" during active test (only works on Audiogram tab) |
