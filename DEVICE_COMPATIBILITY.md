# ASIO4KRNL Device Compatibility

**Comprehensive database of tested and supported audio hardware**

## Overview

ASIO4KRNL is designed to work with USB Audio Class devices - a standardized protocol that allows audio interfaces to communicate with computers without manufacturer-specific drivers. This provides broad compatibility with most modern audio interfaces.

### Compatibility Levels
- **✅ Fully Supported**: Extensively tested, all features working
- **✅ Compatible**: Basic functionality confirmed, may have minor limitations  
- **⚠️ Limited Support**: Works but with restrictions or known issues
- **❌ Not Compatible**: Confirmed incompatible or requires special drivers

## Tested Audio Interfaces

### Focusrite Scarlett Series
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **Scarlett Solo (Gen 3)** | ✅ Fully Supported | 44.1-192kHz | 64-2048 | Excellent performance |
| **Scarlett 2i2 (Gen 3)** | ✅ Fully Supported | 44.1-192kHz | 64-2048 | Professional standard |
| **Scarlett 4i4 (Gen 3)** | ✅ Fully Supported | 44.1-192kHz | 64-2048 | All I/O functional |
| **Scarlett 8i6 (Gen 3)** | ✅ Fully Supported | 44.1-192kHz | 64-2048 | Multi-channel support |
| **Scarlett 18i8 (Gen 3)** | ✅ Compatible | 44.1-96kHz | 128-2048 | Some ADAT limitations |
| **Scarlett 18i20 (Gen 3)** | ✅ Compatible | 44.1-96kHz | 128-2048 | Advanced routing works |

### PreSonus AudioBox Series  
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **AudioBox USB 96** | ✅ Fully Supported | 44.1-96kHz | 64-2048 | Great entry-level choice |
| **AudioBox Studio** | ✅ Fully Supported | 44.1-48kHz | 64-1024 | Solid performance |
| **AudioBox 22VSL** | ✅ Compatible | 44.1-96kHz | 128-2048 | VSL software not needed |
| **AudioBox 44VSL** | ✅ Compatible | 44.1-96kHz | 128-2048 | All channels functional |
| **AudioBox 1818VSL** | ⚠️ Limited Support | 44.1-48kHz | 256-2048 | Higher latency required |

### Behringer U-PHORIA Series
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **U-PHORIA UM2** | ✅ Fully Supported | 44.1-48kHz | 64-2048 | Budget-friendly option |
| **U-PHORIA UMC22** | ✅ Fully Supported | 44.1-96kHz | 64-2048 | Excellent value |
| **U-PHORIA UMC202HD** | ✅ Compatible | 44.1-192kHz | 128-2048 | MIDI functional |
| **U-PHORIA UMC204HD** | ✅ Compatible | 44.1-192kHz | 128-2048 | Multi-channel works |
| **U-PHORIA UMC404HD** | ✅ Compatible | 44.1-192kHz | 256-2048 | All I/O operational |

### Steinberg UR Series
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **UR12** | ✅ Compatible | 44.1-192kHz | 128-2048 | DSP features unavailable |
| **UR22C** | ✅ Compatible | 44.1-192kHz | 128-2048 | USB-C connection |
| **UR22mkII** | ✅ Compatible | 44.1-192kHz | 128-2048 | Solid compatibility |
| **UR44C** | ⚠️ Limited Support | 44.1-96kHz | 256-2048 | Some features limited |

### MOTU M Series
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **M2** | ✅ Fully Supported | 44.1-192kHz | 64-2048 | Exceptional performance |
| **M4** | ✅ Fully Supported | 44.1-192kHz | 64-2048 | Professional quality |
| **M6** | ✅ Compatible | 44.1-192kHz | 128-2048 | Multi-I/O functional |

### Roland Rubix Series
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **Rubix22** | ✅ Compatible | 44.1-192kHz | 128-2048 | Good performance |
| **Rubix24** | ✅ Compatible | 44.1-192kHz | 128-2048 | All features work |
| **Rubix44** | ⚠️ Limited Support | 44.1-96kHz | 256-2048 | Higher latency needed |

### Audio-Technica USB Microphones
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **AT2020USB+** | ✅ Fully Supported | 44.1-48kHz | 64-1024 | Popular choice |
| **AT2500x-USB** | ✅ Compatible | 44.1-192kHz | 128-1024 | High-quality option |

### Blue Microphones
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **Blue Yeti** | ✅ Compatible | 44.1-48kHz | 128-1024 | Works well for podcasting |
| **Blue Yeti Pro** | ✅ Compatible | 44.1-192kHz | 128-1024 | Professional features |
| **Blue Blackout Spark SL** | ✅ Compatible | 44.1-48kHz | 128-1024 | Good for vocals |

## DJ Controllers and Hardware

### Pioneer DJ
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **DDJ-FLX4** | ✅ Compatible | 44.1-48kHz | 128-512 | Basic DJ functionality |
| **DDJ-SB3** | ✅ Compatible | 44.1-48kHz | 128-512 | Serato compatibility |
| **DDJ-400** | ✅ Compatible | 44.1-48kHz | 128-512 | rekordbox compatible |

### Native Instruments
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **Traktor Kontrol S2** | ⚠️ Limited Support | 44.1-48kHz | 256-1024 | Audio only, no controller |
| **Traktor Kontrol S4** | ⚠️ Limited Support | 44.1-48kHz | 256-1024 | Requires NI drivers |

## Digital Mixers (USB Audio Interface Mode)

### Behringer
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **X32 Rack** | ✅ Compatible | 44.1-48kHz | 256-1024 | Multitrack recording |
| **XR18** | ✅ Compatible | 44.1-48kHz | 256-1024 | WiFi mixing works |

### Allen & Heath
| Model | Status | Sample Rates | Buffer Sizes | Notes |
|-------|--------|--------------|--------------|-------|
| **SQ5** | ⚠️ Limited Support | 44.1-48kHz | 512-2048 | Basic USB audio only |

## Known Incompatible Devices

### Devices Requiring Proprietary Drivers
- **Avid Pro Tools Hardware** (HD Native, Carbon, etc.)
- **Universal Audio Apollo** series (requires UAD software)
- **RME Fireface** series (requires RME drivers for full functionality)
- **MOTU UltraLite** series (older models with proprietary protocols)

### FireWire and Thunderbolt Interfaces
ASIO4KRNL only supports USB Audio Class devices. FireWire and Thunderbolt interfaces require manufacturer-specific drivers:
- All FireWire audio interfaces
- Thunderbolt-based interfaces
- PCIe audio cards

## Device Categories

### Fully Supported Categories
1. **USB Audio Class 1.0 Devices**: Basic functionality, 44.1-48kHz
2. **USB Audio Class 2.0 Devices**: Full feature set, up to 192kHz
3. **USB Microphones**: Direct recording capabilities
4. **Basic DJ Controllers**: Audio interface functionality only

### Limited Support Categories
1. **Professional Digital Mixers**: Basic multitrack, advanced features may not work
2. **High-Channel Count Interfaces**: May require larger buffer sizes
3. **Devices with DSP**: Hardware effects unavailable, audio passes through

### Unsupported Categories
1. **Proprietary Protocol Devices**: Require manufacturer drivers
2. **Non-USB Interfaces**: FireWire, Thunderbolt, PCIe cards
3. **Bluetooth Audio**: Not suitable for professional low-latency use

## Testing Your Device

### Quick Compatibility Check
1. **Connect your device** via USB
2. **Open Windows Sound settings**
3. **Look for your device** in playback/recording lists
4. **If visible**: Likely compatible with ASIO4KRNL
5. **If not visible**: May require manufacturer drivers first

### ASIO4KRNL Compatibility Test
1. **Install ASIO4KRNL** following the installation guide
2. **Launch ASIO4KRNL Control Panel**
3. **Check if device appears** in dropdown menus
4. **Run "Test Settings"** to verify functionality
5. **Try different buffer sizes** to find optimal settings

### Performance Validation
1. **Start with 256 sample buffer** size
2. **Test audio playback** through your DAW
3. **Record audio** to verify input functionality
4. **Gradually reduce buffer size** to find minimum stable setting
5. **Monitor for dropouts** or audio artifacts

## Troubleshooting Device Issues

### Device Not Detected
1. **Install manufacturer drivers** if available (some devices need this first)
2. **Try different USB ports** (preferably USB 2.0 for older devices)
3. **Check Windows Device Manager** for hardware issues
4. **Update device firmware** if updates are available

### Poor Performance
1. **Increase buffer size** to 512 or 1024 samples
2. **Lower sample rate** to 44.1 or 48kHz
3. **Check USB power delivery** (may need powered hub)
4. **Try different USB cable** (quality matters for audio)

### Partial Functionality
1. **Some channels not working**: Check device routing/mixing software
2. **MIDI not working**: ASIO4KRNL is audio-only, use separate MIDI drivers
3. **High latency**: Normal for some professional mixers and complex devices

## Requesting Device Support

### Reporting Compatibility
If you've tested a device not listed here:
1. **Device make and model**
2. **Compatibility status** (working/not working/limited)
3. **Supported sample rates** and buffer sizes
4. **Any specific issues** or limitations
5. **Your system specifications**

### Submit Test Results
Help us expand this compatibility database by submitting your test results through:
- GitHub Issues on the ASIO4KRNL repository
- Community forums and discussion boards
- Direct feedback through the application

## Future Compatibility

### Upcoming Support
We're continuously working to expand device compatibility:
- **USB Audio Class 3.0** devices (as they become available)
- **Advanced device-specific optimizations**
- **Better handling of complex routing scenarios**
- **Improved support for high-channel count interfaces**

### Driver Updates
Regular updates may improve compatibility with previously unsupported devices. Keep ASIO4KRNL updated to benefit from:
- **Enhanced device detection**
- **Performance optimizations**
- **Bug fixes for specific hardware**
- **Support for newer device firmware**

---

**Note**: This compatibility database is continuously updated based on user testing and feedback. If you have experience with a device not listed here, please consider contributing your findings to help other users.