# SKIIV Ritual Sounds — VST3 test build

A native **JUCE/C++ VST3 audio effect** project based on our dark-red/black circuit-board interface and the updated flat geometric red knobs. This is a **first native test build**, not a release-ready commercial plugin.

## What works in the source

- Five stereo processing modules: **Delay, Chorus, Reverse, Space (reverb), Fracture (grain/pitch jumps)**
- Five rotary macro knobs, each selecting **20 named effect settings**
- **100 factory master presets**, loads the five macro knobs together
- Manual BPM or **host tempo reading** via JUCE's `AudioPlayHead` API
- Tempo-division selector and effect on/off toggles
- Quick controls (mix, feedback, stereo width, tone)
- FL Studio project-save state persistence via JUCE parameters
- Dark-red/black native GUI, flat round red geometric controls

## Build in GitHub — no compiler installation on your laptop

1. Create a **new GitHub repository**, for example `skiiv-ritual-sounds`.
2. Upload the **contents** of this ZIP into the repository root (keep `.github/workflows/windows-vst3.yml` and `CMakeLists.txt` at the root). Commit to `main`.
3. Open **Actions → Build Windows VST3 → Run workflow** (or wait for the automatic push build).
4. If it succeeds, open the run's **Artifacts** section and download `Skiiv-Ritual-Sounds-Windows-VST3`.
5. Extract the downloaded artifact zip, and extract the enclosed `Skiiv-Ritual-Sounds-Windows-VST3.zip`. Copy the resulting `Skiiv Ritual Sounds.vst3` **directory** to `C:\Program Files\Common Files\VST3\`.
6. In **FL Studio → Options → Manage plugins → Find installed plugins**, rescan the default VST3 folder.
7. Open the Mixer, select an insert's effect slot, and load **Skiiv Ritual Sounds**. Play audio through the insert to hear processing.

**On Windows, the `.vst3` may be a folder containing a DLL and other files. Copy the entire `.vst3` folder, not just the DLL.** You may need administrator permission to copy into `Program Files`.

## Build locally (Windows)

Install Visual Studio 2022 with C++ Desktop workload, Git, and CMake 3.22+:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target SkiivRitualSounds_VST3
```

JUCE is fetched automatically during CMake configuration. GitHub Actions also fetches JUCE when building. The current test project pins JUCE at `8.0.9`.

## Notes / known limits

- This package contains **source code and an automated Windows build**, not a precompiled or host-tested VST3. The GitHub Actions run is the first compilation check.
- The visual interface recreates the **layout and geometric control language** in native JUCE graphics. It is **not a pixel-perfect reproduction** of the browser or image mockup.
- Effect presets are named macro settings on five DSP blocks; the algorithms are **early test implementations**, not equivalent to commercial reverse/granular effects.
- Reverb and reverse can be improved; real-time and FL Studio validation are still needed.
- Host sync reads the host tempo when FL Studio supplies it. Manual BPM is used as fallback; sample-accurate transport/bar alignment has not yet been implemented.
- This is an **audio effect**. Load it into a **Mixer effect slot**, not an instrument channel.
- Check JUCE's license before publicly distributing or commercially shipping a product.
