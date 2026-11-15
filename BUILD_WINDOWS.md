# Build and test SKIIV Ritual Sounds on Windows (FL Studio)

## GitHub Actions (recommended)

1. Create a **private** repository at https://github.com/new (repository name: `skiiv-ritual-sounds`).
2. Upload all files and folders from this source ZIP to the repository root, including the hidden `.github/workflows` folder.
3. Commit to the `main` branch. Open the repository **Actions** tab and open **Build Windows VST3**.
4. Run the workflow (or use the run launched by your commit). Wait until the run is green.
5. On its summary page, download artifact **Skiiv-Ritual-Sounds-Windows-VST3**.
6. Unzip the artifact, then unzip the packaged `Skiiv-Ritual-Sounds-Windows-VST3.zip`.
7. Copy the entire **Skiiv Ritual Sounds.vst3** directory to `C:\Program Files\Common Files\VST3\`.
8. In FL Studio open **Options > Manage plugins** and choose **Find installed plugins**.
9. On an FL Studio **Mixer** track, add the effect **Skiiv Ritual Sounds**, then play a loop. Host Sync is enabled by default and reads the project BPM if FL Studio exposes it.

**If the GitHub build turns red:** open the failed build step, copy the compiler errors, and share them for a patch. This ZIP is source only and has not been compiled or tested against FL Studio in this environment.

## System requirements
- Windows 10/11 x64
- FL Studio with VST3 support
- Internet access for the GitHub Actions JUCE download

## What is not yet verified
- Initial JUCE/Windows build
- FL Studio plugin scanning and audio playback
- Real-time performance on your machine
- Pixel-perfect match with the browser demo
