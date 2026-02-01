# StereoImager - Usage Guide

**Stereo Width and Imaging Plugin**

StereoImager is a precision stereo manipulation tool featuring global width control, pan, balance, mono bass functionality, and multiband stereo processing. Perfect for creating wide mixes, fixing stereo problems, and adding dimension to your productions.

---

## Use Cases in Modern Rock Production

### Drum Bus Processing

Stereo imaging on drums can create width while maintaining punch.

**Wide Drum Bus:**
- Width: 110-130%
- Pan: 0% (centered)
- Mono Bass: Enabled, 120 Hz
- Multiband: Disabled (use global width)

**Multiband Drum Bus:**
- Multiband: Enabled
- Low-Mid Crossover: 250 Hz
- Mid-High Crossover: 4000 Hz
- Low Width: 80% (tight kick/low toms)
- Mid Width: 110% (snare body)
- High Width: 140% (cymbals, room)
- Mono Bass: Enabled, 100 Hz

**Drum Room Mics:**
- Width: 150-180%
- Mono Bass: Enabled, 150 Hz
- Blend with close mics for depth

### Guitar Bus / Individual Tracks

Stereo imaging helps guitars sit in the mix without masking other elements.

**Rhythm Guitar Bus (Stereo Pair):**
- Width: 100-120%
- Balance: 0% (or adjust if one side is louder)
- Mono Bass: Enabled, 100 Hz

**Wide Rhythm Guitars:**
- Width: 140-160%
- Mono Bass: Enabled, 120 Hz
- Use sparingly - too wide loses impact

**Narrow Guitars (Make Room for Vocals):**
- Width: 70-90%
- Keeps guitars out of center for vocal clarity

### Bass Guitar

Bass should typically be mono or near-mono for tight low end.

**Mono Bass:**
- Width: 0-50%
- Mono Bass: Enabled, 200 Hz
- Or simply collapse to mono

**Stereo DI + Amp Blend:**
- Width: 60-80%
- Mono Bass: Enabled, 150 Hz
- Preserves some stereo interest in upper harmonics

### Vocals

Stereo imaging on vocals creates space and interest.

**Lead Vocal (Centered):**
- Width: 100% (leave natural)
- Or slight narrowing to 90% for focus
- Mono Bass: Not needed for vocals

**Doubled Vocals:**
- Width: 120-150%
- Spreads doubles wider for effect

**Backing Vocal Stack:**
- Width: 150-180%
- Creates wide spread behind lead vocal

**Vocal Bus with Widener:**
- Width: 110-120%
- Adds subtle width to vocal group

### Mix Bus / Mastering

Stereo imaging on the mix bus should be subtle but can make a big difference.

**Subtle Mix Bus Widening:**
- Width: 105-115%
- Mono Bass: Enabled, 80-100 Hz
- Adds polish without being obvious

**Multiband Mix Bus Imaging:**
- Multiband: Enabled
- Low-Mid Crossover: 200 Hz
- Mid-High Crossover: 3500 Hz
- Low Width: 80% (tight low end)
- Mid Width: 105% (slight widening)
- High Width: 120-140% (air and sparkle)
- Mono Bass: Enabled, 100 Hz

**Mastering Width Enhancement:**
- Multiband: Enabled
- Low Width: 70-90%
- Mid Width: 100-110%
- High Width: 115-130%
- Mono Bass: Enabled, 80 Hz

**Fix Narrow/Mono Mix:**
- Width: 130-160%
- Be careful - artificial widening can cause phase issues
- Check mono compatibility

---

## Recommended Settings

### Quick Reference Table

| Application | Width | Mono Bass Freq | Multiband |
|------------|-------|----------------|-----------|
| Drum Bus | 110-130% | 120 Hz | Optional |
| Rhythm Guitars | 100-140% | 100-120 Hz | No |
| Bass | 0-50% | 200 Hz | No |
| Lead Vocal | 90-100% | N/A | No |
| Backing Vocals | 150-180% | N/A | No |
| Mix Bus (subtle) | 105-115% | 80-100 Hz | Optional |
| Mastering | See multiband | 80 Hz | Yes |

### Width Settings Guide

- **0%**: Mono (both channels identical)
- **50%**: Narrow (reduced stereo image)
- **100%**: Natural (unchanged stereo image)
- **150%**: Wide (enhanced stereo spread)
- **200%**: Maximum (extreme widening - use with caution)

### Mono Bass Frequency Guide

- **60-80 Hz**: Mastering, full mixes - only affects sub-bass
- **100-120 Hz**: Drums, bass - standard rock setting
- **150-200 Hz**: Bass guitar, kick - more aggressive mono-ing
- **200+ Hz**: Special cases - very narrow low-mid focus

### Multiband Crossover Suggestions

**Rock/Metal:**
- Low-Mid: 180-250 Hz
- Mid-High: 3000-4000 Hz

**Pop/Modern:**
- Low-Mid: 200-300 Hz
- Mid-High: 4000-5000 Hz

**Acoustic/Classical:**
- Low-Mid: 150-200 Hz
- Mid-High: 2500-3500 Hz

---

## Signal Flow Tips

### Where to Place StereoImager

1. **End of Insert Chain**: After EQ and compression, before any final limiting
2. **On Buses**: Control width of grouped tracks (drums, guitars, vocals)
3. **Mix Bus**: As one of the last processors before mastering

### Checking Mono Compatibility

1. Widen your track/mix with StereoImager
2. Switch your monitoring to mono (or use a mono button)
3. Listen for:
   - Phase cancellation (thinning of sound)
   - Disappearing elements
   - Frequency response changes
4. Reduce width if mono compatibility suffers

### Using Balance vs. Pan

- **Balance**: Adjusts the relative level of L/R channels - useful for correcting uneven recordings
- **Pan**: Moves the entire stereo image left or right - for repositioning sources

### Multiband Strategy

1. Keep **low frequencies narrow** (50-100%) for solid low end
2. Keep **mid frequencies natural to slightly wide** (100-120%)
3. Widen **high frequencies** (120-150%) for air and sparkle
4. This creates perceived width without sacrificing low-end focus

---

## Combining with Other Plugins

### Drum Bus Chain
1. **Bus Glue** - compression
2. **TapeWarm** - analog character
3. **StereoImager** - width control (last before send to mix)

### Guitar Bus Chain
1. **Bus Glue** - glue
2. **StereoImager** - control spread
3. Check mono compatibility

### Mix Bus Chain
1. **Bus Glue** - glue compression
2. **MasterBus** - EQ and compression
3. **StereoImager** - final width
4. **Automaster** - limiting

### Mastering Chain
1. **MasterBus** - EQ/dynamics
2. **StereoImager** - multiband stereo control
3. **Automaster** - limiting and loudness

---

## Quick Start Guide

**Widen your drum bus in 30 seconds:**

1. Insert StereoImager on your drum bus
2. Set **Width** to 120%
3. Enable **Mono Bass**
4. Set **Mono Bass Freq** to 120 Hz
5. Play drums and verify kick stays centered
6. Check mono compatibility
7. Adjust Width up/down to taste

**Set up multiband mix bus imaging in 60 seconds:**

1. Insert StereoImager on your mix bus (before limiter)
2. Enable **Multiband**
3. Set **Low-Mid Crossover** to 200 Hz
4. Set **Mid-High Crossover** to 4000 Hz
5. Set **Low Width** to 80% (tight low end)
6. Set **Mid Width** to 105% (subtle widening)
7. Set **High Width** to 130% (airy highs)
8. Enable **Mono Bass** at 100 Hz
9. Play mix and A/B with bypass
10. Check mono compatibility
11. Adjust individual bands as needed

**Fix a narrow or mono mix in 30 seconds:**

1. Insert StereoImager on the track/bus
2. Set **Width** to 140%
3. Play and listen for phase issues
4. Enable **Mono Bass** at 100 Hz
5. Check mono - reduce Width if it sounds thin in mono
6. Find the sweet spot where it sounds wide in stereo but still works in mono
7. Use multiband if you need more control

**Narrow guitars to make room for vocals:**

1. Insert StereoImager on guitar bus
2. Set **Width** to 75-85%
3. This narrows the guitar spread
4. Creates a "hole" in the center for vocals
5. Lead vocal should sit more clearly
6. Adjust to taste - don't over-narrow
