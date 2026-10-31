# NightShift — Analog Wavetable Synthesizer (VST3 / AU / Standalone)

A tech-house focused wavetable synth with an analog heart: 2 morphing wavetable oscillators, sub + noise,
a saturating ladder filter, 3 envelopes, 2 LFOs, an 8-slot mod matrix, 4 macros, chord memory and an
8-unit FX rack (incl. a tempo-synced sidechain "Pump") — plus **80 factory presets** in label-style folders.

## Features

| Section | What you get |
|---|---|
| **Oscillators A & B** | 12 band-limited morphing wavetables (16 frames each, mip-mapped = no aliasing), up to 8-voice unison with detune / blend / stereo width, warp modes **Sync, Bend, Mirror, FM** (A↔B), octave/semi/fine, phase + phase randomise |
| **Sub & Noise** | Sine / triangle / square sub (−1 or −2 oct, optional filter bypass), white / pink / vinyl-crackle noise |
| **Analog engine** | Per-voice pitch drift, component-tolerance detune, filter variance and warm mixer saturation (the **ANALOG** knob) |
| **Filter** | Zero-delay-feedback **Ladder 24/12** (Moog-style, self-oscillating, tanh drive) + SVF **LP12/LP24/HP12/HP24/BP/Notch**, drive, key tracking, bipolar Env 2 amount |
| **Modulation** | Amp Env + Env 2 + Env 3 (analog exponential curves), 2 LFOs (7 shapes, tempo sync incl. triplets/dotted, free/retrig, rise time), 8-slot mod matrix with 14 sources → 20 destinations, 4 macros |
| **Voicing** | Poly (1–16 voices) / Mono / Legato, glide, pitch-bend range, velocity sensitivity, sustain pedal, mod wheel, aftertouch, **Chord Memory** (minor, major, m7, maj7, m9, sus4, fifth, octave, dub m7, 4ths) |
| **FX rack** | Distortion (soft / hard / tube / fold / bitcrush), 3-band EQ, Chorus, Phaser, Ping-pong Delay (sync), Reverb, Compressor, **Pump** (sidechain-style ducking locked to host tempo) |
| **UI** | Resizable (60–160%), wavetable / envelope / LFO displays, oscilloscope, on-screen keyboard, preset browser with folders + user preset saving |

## Factory presets (80)

**Init** (1): Init

**Moodchild Style** (10): Rolling Sub Groove, Warm Rubber Bass, Mood Pluck, Late Night Keys, Hypno Bleep, Velvet Pad, Swing Stab, Bouncy Mid Bass, Wonky Lead, Afterglow Chords

**Deeperfect Style** (10): Driving Reese, Rumble Low End, Acid Groove 303, Hypnotic Sequence, Dark Stab, Warehouse Lead, Tension Riser, Deep Pulse Bass, Metal Perc Synth, Tunnel Pad

**Tomango Style** (10): Funk Slap Bass, Talkbox Lead, Organ Hit, Bubble Bass, Groove Pluck, Disco Rubber, Vowel Chop, Bright Brass Stab, Sunset Keys, Party Lead

**Toolroom Style** (10): Rubber Bass Classic, Classic House Organ, Big Rave Stab, Chord Memory Stab, Main Room Lead, Pitch Drop Bass, Uplift Pad, Hoover Rave, Piano Stab House, Pumping Saw Chords

**Defected Style** (10): Deep House Chords, Organ Bass, Soul Keys, Disco Strings, Warm Analog Pad, Garage Sub, Jackin Stab, Sunrise Pluck, Smooth Lead, Funky Clav

**La Pera Style** (10): Minimal Bleep, Rolling Mono Bass, Quirky Perc Synth, Glitch Pluck, Wobble Groove, Micro Stab, Shaker Noise Synth, Bouncing Sync, Deep Mallet, Hypno Arp Bass

**Tech House Pro** (19): TH Rubber Sub, TH Gritty Mid Bass, TH FM Bass, TH Pluck Classic, TH Dub Chord, TH Stab Minor7, TH Vocal Lead, TH Acid Line, TH Reese, TH Organ Stab Bright, TH Noise Riser, TH Downlifter, TH Atmos Pad, TH Bell Lead, TH Sub 808 Glide, TH Percussive Zap, TH Organ Bass, TH Bleep Sequence, TH Pumping Pad

> Preset folders are named after the *styles* these labels are known for, as a sound-design reference.
> NightShift is not affiliated with or endorsed by any of these labels.

All presets are loudness-matched (measured automatically by the test suite) so you can browse without volume jumps.
Your own presets save to **Documents/NightShift/User Presets** and appear in the **User** folder.

---

## Build the installers on GitHub (no coding tools needed on your computer)

GitHub's free build servers compile the synth for **Windows and Mac** and hand you ready-to-run installers.

### 1. Create the repository
1. Sign in at **github.com** (free account is fine).
2. Click **+ → New repository**, name it `NightShift`, choose **Public**
   (public repos get unlimited free build minutes; private repos are limited and Mac minutes count 10×), click **Create repository**.

### 2. Upload the project
1. Unzip `NightShift.zip` on your computer.
2. On the new repo page click **uploading an existing file**, then drag **everything inside** the `NightShift` folder
   (`Source`, `Tests`, `installer`, `.github`, `CMakeLists.txt`, `README.md`, `.gitignore`) into the browser.
3. Click **Commit changes**.

> ⚠️ **The `.github` folder is hidden on Mac/Windows by default** and is easy to miss. Check that your repo shows
> `.github/workflows/build.yml`. If it doesn't: in the repo click **Add file → Create new file**, type
> `.github/workflows/build.yml` as the name, paste the contents of that file from the zip, and commit.
> (Mac: press **Cmd + Shift + .** in Finder to show hidden files.)

### 3. Let it build
1. Open the **Actions** tab. A run called **Build NightShift Installers** starts automatically
   (if Actions asks to be enabled, click the green button). You can also start it with **Run workflow**.
2. Wait ~10–20 minutes until all jobs are green ✅.
3. Click the finished run and scroll to **Artifacts**:
   - `NightShift-Windows-Installer` → `NightShift-1.0.0-Windows-Setup.exe`
   - `NightShift-macOS-Installer` → `NightShift-1.0.0-macOS.pkg` (Universal: Apple Silicon + Intel)
   - `NightShift-Linux-VST3` (bonus)

### 4. (Optional) Publish a proper Release page
Create a tag starting with `v`: **Releases → Draft a new release → Choose a tag → type `v1.0.0` → Create new tag → Publish**.
The workflow builds again and attaches both installers to that release automatically.

---

## Installing

**Windows:** run the `Setup.exe`. If SmartScreen appears (the installer isn't code-signed), click **More info → Run anyway**.
The VST3 goes to `C:\Program Files\Common Files\VST3`.

**macOS:** double-click the `.pkg`. Because it isn't notarized by Apple, macOS may block it the first time:
open **System Settings → Privacy & Security** and click **Open Anyway** (or right-click the pkg → **Open**).
Installs VST3, AU and the standalone app.

**Ableton Live:** Preferences → **Plug-ins** → turn on **Use VST3 Plug-in System Folders** (Mac: also *Use Audio Units v2*)
→ **Rescan**. NightShift appears under *Plug-ins → NightShift Audio*.

## Quick tips
- **Macro 1** sweeps the filter on Rolling Sub Groove, Acid Groove 303, TH Rubber Sub and TH Acid Line (and the vowel on TH Vocal Lead) — automate it for builds, or map any macro yourself in the MOD MATRIX tab.
- **Pump** (FX page) gives instant sidechain ducking locked to your project tempo; works best while Live is playing.
- **Chord Memory** + **Delay 1/8D** + **Reverb** = instant dub-chord stabs.
- Double-click any knob to reset it. Drag the window corner to resize.

## Building locally (developers)
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
JUCE 8 is downloaded automatically. Run the headless test suite with `-DNS_BUILD_TESTS=ON` then
`build/NightShiftTests_artefacts/Release/NightShiftTests` (renders every preset and checks for NaNs, silence,
level, CPU, random-parameter fuzz and state save/restore). Add `--wav` to render every preset to WAV files.

## License note
NightShift is built with the [JUCE framework](https://juce.com). JUCE is dual-licensed (AGPLv3 or a commercial licence,
with a free tier for small developers). Personal use is fine; if you plan to **sell or distribute** NightShift,
check JUCE's licence terms first.
