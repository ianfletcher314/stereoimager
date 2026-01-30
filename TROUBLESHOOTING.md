# StereoImager Plugin - Logic Pro Troubleshooting

## Problem
The StereoImager plugin builds successfully, passes `auval` validation, but does not appear in Logic Pro's plugin menu under "Fletcher" manufacturer - while other Fletcher plugins (MasterBus, TapeWarm, VoxProc, NeveStrip) work fine.

## Environment
- macOS Darwin 24.6.0 (Apple Silicon)
- Logic Pro X
- JUCE Framework
- Xcode 26.01

## What Works
These plugins appear correctly in Logic Pro under "Fletcher":
- MasterBus
- TapeWarm
- VoxProc
- NeveStrip
- PDLBRD (under "Compstortion" manufacturer)

## What Doesn't Work
These plugins pass auval but don't appear in Logic Pro:
- StereoImager
- BusGlue
- Automaster

---

## Diagnostic Steps Taken

### 1. Verified Plugin Installation
```bash
ls ~/Library/Audio/Plug-Ins/Components/
# StereoImager.component is present
```

### 2. Verified AU Registration
```bash
auval -a | grep -i flet
# Output:
# aufx StIg Flet  -  Fletcher: StereoImager
# (Plugin IS registered with the system)
```

### 3. Verified AU Validation Passes
```bash
auval -v aufx StIg Flet
# Output: AU VALIDATION SUCCEEDED
```

### 4. Compared Info.plist with Working Plugin (TapeWarm)

**TapeWarm (WORKING):**
```
CFBundleIdentifier => "com.Fletcher.TapeWarm"
manufacturer => "Flet"
name => "Fletcher: TapeWarm"
type => "aufx"
```

**StereoImager (NOT WORKING - before fix):**
```
CFBundleIdentifier => "com.ianfletcher.stereoimager"  # DIFFERENT!
manufacturer => "Flet"
name => "Fletcher: StereoImager"
type => "aufx"
```

### 5. Compared Binary Architecture
```bash
file ~/Library/Audio/Plug-Ins/Components/TapeWarm.component/Contents/MacOS/TapeWarm
# arm64 only

file ~/Library/Audio/Plug-Ins/Components/StereoImager.component/Contents/MacOS/StereoImager
# universal (x86_64 + arm64)
```
Note: PDLBRD is also universal and works, so architecture alone isn't the issue.

### 6. Compared Jucer Project Files

**TapeWarm.jucer (WORKING):**
- `pluginFormats="buildAU,buildVST3"` (no Standalone)
- Only XCODE_MAC export format
- Module paths: absolute (`/Users/ianfletcher/JUCE/modules`)
- No extra JUCE options
- `pluginCharacteristicsValue=""`

**StereoImager.jucer (NOT WORKING - original):**
- `pluginFormats="buildAU,buildVST3,buildStandalone"`
- XCODE_MAC, VS2022, LINUX_MAKE export formats
- Module paths: empty (`path=""`)
- Extra JUCE options: `JUCE_WEB_BROWSER="0"`, `JUCE_USE_CURL="0"`, etc.
- Had various extra attributes

### 7. Checked for Quarantine/Gatekeeper Issues
```bash
xattr -l ~/Library/Audio/Plug-Ins/Components/StereoImager.component
# No quarantine flags
```

### 8. Checked Binary Dependencies
```bash
otool -L ~/Library/Audio/Plug-Ins/Components/StereoImager.component/Contents/MacOS/StereoImager
# Same system frameworks as working plugins
```

### 9. Checked Factory Function Exports
```bash
nm ~/Library/Audio/Plug-Ins/Components/StereoImager.component/Contents/MacOS/StereoImager | grep -i factory
# _StereoImagerAUFactory and _JuceAUFactory both present (correct)
```

### 10. Verified Standalone Runs Without Crash
```bash
open /Users/ianfletcher/stereoimager/Builds/MacOSX/build/Release/StereoImager.app
# App launches successfully - no crash
```

---

## Fixes Attempted

### Fix 1: Updated Manufacturer Settings
Changed in StereoImager.jucer:
- Added `pluginManufacturer="Fletcher"`
- Ensured `pluginManufacturerCode="Flet"`
- Added `pluginAUMainType="'aufx'"`

**Result:** No change in Logic Pro

### Fix 2: Removed pluginCharacteristicsValue Conflicts
BusGlue had `pluginCharacteristicsValue="pluginIsSynth,pluginWantsMidiIn,..."` which conflicted with `aufx` type.

**Result:** Fixed auval warning, but still no Logic Pro visibility

### Fix 3: Matched Jucer Configuration to Working Plugin
Rewrote StereoImager.jucer to exactly match TapeWarm.jucer structure:
- Removed VS2022, LINUX_MAKE export formats
- Removed Standalone from pluginFormats
- Used absolute module paths
- Removed extra JUCE options
- Matched all ID formats

**Result:** Builds successfully, auval passes, still no Logic Pro visibility

### Fix 4: Cleared All Caches
```bash
rm -rf ~/Library/Caches/AudioUnitCache/
rm -f ~/Library/Preferences/com.apple.logic.pro.cs
killall -9 AudioComponentRegistrar
killall -9 coreaudiod
```

**Result:** auval still sees plugin, Logic Pro still doesn't

### Fix 5: Complete Plugin Removal and Reinstall
```bash
rm -rf ~/Library/Audio/Plug-Ins/Components/StereoImager.component
# Clear caches
# Rebuild from scratch
```

**Result:** Still not visible in Logic Pro

### Fix 6: Discovered and Cleared Apple's AudioComponentCache

Found a DIFFERENT cache file that wasn't being cleared:
```bash
~/Library/Preferences/com.apple.audio.AudioComponentCache.plist
```

This is Apple's master Audio Component cache - separate from `~/Library/Caches/AudioUnitCache/`.

Compared cache entries between working and non-working plugins:

**TapeWarm (WORKING) in AudioComponentCache:**
```
ChannelConfigurations => [[1,1], [2,2]]  # Supports mono AND stereo
SupportedChannelLayoutTags => {
  Input => [6553601, 6619138, ...]
  Output => [6553601, 6619138, ...]
}
```

**StereoImager (NOT WORKING) in AudioComponentCache:**
```
ChannelConfigurations => [[2,2]]  # Stereo only
SupportedChannelLayoutTags => {}  # EMPTY!
```

Deleted the cache:
```bash
rm ~/Library/Preferences/com.apple.audio.AudioComponentCache.plist
killall -9 AudioComponentRegistrar
```

**Result:** Cache regenerated, but plugin still not visible in Logic Pro

---

## Current State

After all fixes, the plugin configuration now matches working plugins exactly:

```
CFBundleIdentifier => "com.Fletcher.StereoImager"
manufacturer => "Flet"
name => "Fletcher: StereoImager"
type => "aufx"
subtype => "StIg"
```

The plugin:
- ✅ Installs to correct location
- ✅ Registers with macOS Audio Unit system
- ✅ Passes auval validation
- ✅ Has correct Info.plist
- ✅ Has correct binary exports
- ✅ Standalone version runs without crash
- ❌ Does not appear in Logic Pro plugin menu

---

## Theories

1. **Channel Configuration / Layout Tags**: The AudioComponentCache shows working plugins have `SupportedChannelLayoutTags` populated while StereoImager has it empty. This may be how JUCE reports channel capabilities and Logic might filter based on this.

2. **Logic Pro Plugin Manager Blacklist**: Logic may have cached a "failed" status from an earlier scan when the plugin had issues. This cache may not be in the files we cleared.

3. **Sandbox/Entitlements Issue**: Logic Pro may require specific entitlements that the plugin doesn't have.

4. **Logic Pro-Specific Validation**: Logic may perform additional validation beyond auval that we're not aware of.

5. **Plugin Manager Database**: There may be a Logic-specific database file storing plugin validation results that survives cache clearing.

6. **JUCE Channel Configuration**: The way JUCE reports supported channel layouts may differ between plugins. Need to investigate how TapeWarm's PluginProcessor differs from StereoImager's in terms of `isBusesLayoutSupported()` implementation.

---

## Next Steps to Try

1. **Investigate Channel Layout Implementation** (HIGH PRIORITY):
   - Compare `isBusesLayoutSupported()` in TapeWarm vs StereoImager PluginProcessor
   - Check if TapeWarm supports mono while StereoImager doesn't
   - Try adding mono support to StereoImager
   - This is the most promising lead based on AudioComponentCache differences

2. **Check Logic Pro Plug-in Manager directly**:
   - Open Logic Pro > Settings > Plug-in Manager
   - Look for StereoImager in the list
   - Check if it's marked as "failed" or "incompatible"
   - Try "Reset & Rescan Selection"

2. **Check Console.app for errors**:
   - Open Console.app
   - Filter for "Logic" or "AudioComponent"
   - Launch Logic Pro and watch for errors during plugin scan

3. **Try different plugin code**:
   - Change `pluginCode` from "StIg" to something else
   - Rebuild and test

4. **Compare with fresh JUCE project**:
   - Create a brand new JUCE audio plugin project
   - Use minimal code (just pass-through audio)
   - See if that appears in Logic Pro

5. **Check codesigning**:
   - Compare codesign output between working and non-working plugins
   - Try signing with a developer certificate instead of ad-hoc

---

## Files Modified

- `/Users/ianfletcher/stereoimager/StereoImager.jucer` - Rewritten to match TapeWarm structure
- Plugin rebuilt multiple times with various configurations

## Current Jucer Configuration

```xml
<?xml version="1.0" encoding="UTF-8"?>
<JUCERPROJECT id="STIMG01" name="StereoImager" projectType="audioplug" useAppConfig="0"
              addUsingNamespaceToJuceHeader="0" jucerFormatVersion="1" companyName="Fletcher"
              companyCopyright="2025" companyWebsite="https://github.com/ianfletcher314/stereoimager"
              pluginFormats="buildAU,buildVST3" pluginCharacteristicsValue=""
              pluginName="StereoImager" pluginDesc="Stereo width and imaging plugin"
              pluginManufacturer="Fletcher" pluginManufacturerCode="Flet"
              pluginCode="StIg" pluginVST3Category="Spatial" pluginAUMainType="'aufx'">
  ...
</JUCERPROJECT>
```
