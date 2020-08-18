// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Amiga.h"

PaulaAudio::PaulaAudio(Amiga& ref) : AmigaComponent(ref)
{
    setDescription("AudioUnit");
    
    subComponents = vector<HardwareComponent *> {

        &channel0,
        &channel1,
        &channel2,
        &channel3,
        &filterL,
        &filterR
    };
    
    volume = maxVolume;
    targetVolume = maxVolume;
}

void
PaulaAudio::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS
    
    clearRingbuffer();
    
    stats.bufferUnderflows = 0;
    stats.bufferOverflows = 0;
}

long
PaulaAudio::getConfigItem(ConfigOption option)
{
    switch (option) {
            
        case OPT_SAMPLING_METHOD:
            return config.samplingMethod;
            
        case OPT_FILTER_TYPE:
            assert(filterL.getFilterType() == config.filterType);
            assert(filterR.getFilterType() == config.filterType);
            return config.filterType;
            
        case OPT_FILTER_ALWAYS_ON:
            return config.filterAlwaysOn;
            
        case OPT_AUDVOLL:
            return (long)(exp2(config.volL) * 100.0);

        case OPT_AUDVOLR:
            return (long)(exp2(config.volR) * 100.0);

        case OPT_AUDVOL0:
            return (long)(exp2(config.vol[0] / 0.0000025) * 100.0);

        case OPT_AUDVOL1:
            return (long)(exp2(config.vol[1] / 0.0000025) * 100.0);
            
        case OPT_AUDVOL2:
            return (long)(exp2(config.vol[2] / 0.0000025) * 100.0);
            
        case OPT_AUDVOL3:
            return (long)(exp2(config.vol[3] / 0.0000025) * 100.0);

        case OPT_AUDPAN0:
            return (long)(config.pan[0] * 100.0);
            
        case OPT_AUDPAN1:
            return (long)(config.pan[1] * 100.0);
            
        case OPT_AUDPAN2:
            return (long)(config.pan[2] * 100.0);
            
        case OPT_AUDPAN3:
            return (long)(config.pan[3] * 100.0);

        default: assert(false);
    }
}

void
PaulaAudio::setConfigItem(ConfigOption option, long value)
{
    bool wasMuted;
    
    switch (option) {
            
        case OPT_SAMPLING_METHOD:
            assert(isSamplingMethod(value));
            config.samplingMethod = (SamplingMethod)value;
            break;
            
        case OPT_FILTER_TYPE:
            assert(isFilterType(value));
            config.filterType = (FilterType)value;
            filterL.setFilterType((FilterType)value);
            filterR.setFilterType((FilterType)value);
            break;
            
        case OPT_FILTER_ALWAYS_ON:
            config.filterAlwaysOn = value;
            break;
            
        case OPT_AUDVOLL:
            wasMuted = isMuted();
            config.volL = log2((double)value / 100.0);
            if (wasMuted != isMuted())
                messageQueue.put(isMuted() ? MSG_MUTE_ON : MSG_MUTE_OFF);
            break;
            
        case OPT_AUDVOLR:
            wasMuted = isMuted();
            config.volR = log2((double)value / 100.0);
            if (wasMuted != isMuted())
                messageQueue.put(isMuted() ? MSG_MUTE_ON : MSG_MUTE_OFF);
            break;
            
        case OPT_AUDVOL0:
            config.vol[0] = log2((double)value / 100.0) * 0.0000025;
            break;
            
        case OPT_AUDVOL1:
            config.vol[1] = log2((double)value / 100.0) * 0.0000025;
            break;

        case OPT_AUDVOL2:
            config.vol[2] = log2((double)value / 100.0) * 0.0000025;
            break;

        case OPT_AUDVOL3:
            config.vol[3] = log2((double)value / 100.0) * 0.0000025;
            break;

        case OPT_AUDPAN0:
            config.pan[0] = MAX(0.0, MIN(value / 100.0, 1.0));
            break;

        case OPT_AUDPAN1:
            config.pan[1] = MAX(0.0, MIN(value / 100.0, 1.0));
            break;

        case OPT_AUDPAN2:
            config.pan[2] = MAX(0.0, MIN(value / 100.0, 1.0));
            break;

        case OPT_AUDPAN3:
            config.pan[3] = MAX(0.0, MIN(value / 100.0, 1.0));
            break;

        default:
            break;
    }
}

void
PaulaAudio::setSampleRate(double hz)
{
    debug(AUD_DEBUG, "setSampleRate(%f)\n", hz);

    config.sampleRate = hz;
    cyclesPerSample = MHz(masterClockFrequency) / hz;

    filterL.setSampleRate(hz);
    filterR.setSampleRate(hz);
}

/*
long
PaulaAudio::getVol(unsigned nr)
{
    assert(nr < 4);
    return (long)(exp2(config.vol[nr] / 0.0000025) * 100.0);
}



void
PaulaAudio::setVol(unsigned nr, long val)
{
    assert(nr < 4);

    config.vol[nr] = log2((double)val / 100.0) * 0.0000025;
}

long
PaulaAudio::getPan(unsigned nr)
{
    assert(nr < 4);
    return (long)(config.pan[nr] * 100.0);
}

void
PaulaAudio::setPan(unsigned nr, long val)
{
    assert(nr < 4);
    config.pan[nr] = MAX(0.0, MIN(val / 100.0, 1.0));
}

long
PaulaAudio::getVolL()
{
    return (long)(exp2(config.volL) * 100.0);
}

void
PaulaAudio::setVolL(long val)
{
    bool wasMuted = isMuted();
    config.volL = log2((double)val / 100.0);

    if (wasMuted != isMuted()) {
        messageQueue.put(isMuted() ? MSG_MUTE_ON : MSG_MUTE_OFF);
    }
}

long
PaulaAudio::getVolR()
{
    return (long)(exp2(config.volR) * 100.0);
}

void
PaulaAudio::setVolR(long val)
{
    bool wasMuted = isMuted();
    config.volR = log2((double)val / 100.0);

    if (wasMuted != isMuted()) {
        messageQueue.put(isMuted() ? MSG_MUTE_ON : MSG_MUTE_OFF);
    }
}
*/

size_t
PaulaAudio::didLoadFromBuffer(u8 *buffer)
{
    clearRingbuffer();
    return 0;
}

void
PaulaAudio::_inspect()
{
    synchronized {
        
        info.channel[0] = channel0.getInfo();
        info.channel[1] = channel1.getInfo();
        info.channel[2] = channel2.getInfo();
        info.channel[3] = channel3.getInfo();
    }
}

void
PaulaAudio::_dump()
{
}

void
PaulaAudio::_run()
{
    clearRingbuffer();
}

void
PaulaAudio::_pause()
{
    clearRingbuffer();
}

void
PaulaAudio::executeUntil(Cycle targetClock)
{
    switch (config.samplingMethod) {
        case SMP_NONE:    executeUntil<SMP_NONE>   (targetClock); return;
        case SMP_NEAREST: executeUntil<SMP_NEAREST>(targetClock); return;
        case SMP_LINEAR:  executeUntil<SMP_LINEAR> (targetClock); return;
    }
}

template <SamplingMethod method> void
PaulaAudio::executeUntil(Cycle targetClock)
{
    while (clock < targetClock) {

        double ch0 = channel0.interpolate<method>(clock) * config.vol[0];
        double ch1 = channel1.interpolate<method>(clock) * config.vol[1];
        double ch2 = channel2.interpolate<method>(clock) * config.vol[2];
        double ch3 = channel3.interpolate<method>(clock) * config.vol[3];
        
        double l =
        ch0 * config.pan[0] + ch1 * config.pan[1] +
        ch2 * config.pan[2] + ch3 * config.pan[3];
        
        double r =
        ch0 * (1 - config.pan[0]) + ch1 * (1 - config.pan[1]) +
        ch2 * (1 - config.pan[2]) + ch3 * (1 - config.pan[3]);
        
        writeData((float)(l * config.volL), (float)(r * config.volR));

        clock += cyclesPerSample;
    }
}

void
PaulaAudio::pokeAUDxPER(int nr, u16 value)
{
    switch (nr) {
        case 0: channel0.pokeAUDxPER(value); return;
        case 1: channel1.pokeAUDxPER(value); return;
        case 2: channel2.pokeAUDxPER(value); return;
        case 3: channel3.pokeAUDxPER(value); return;
    }
    assert(false);
}

void
PaulaAudio::pokeAUDxVOL(int nr, u16 value)
{
    switch (nr) {
         case 0: channel0.pokeAUDxVOL(value); return;
         case 1: channel1.pokeAUDxVOL(value); return;
         case 2: channel2.pokeAUDxVOL(value); return;
         case 3: channel3.pokeAUDxVOL(value); return;
     }
     assert(false);
}

void
PaulaAudio::rampUp()
{
    // Only proceed if the emulator is not running in warp mode
    if (warpMode) return;
    
    targetVolume = maxVolume;
    volumeDelta = 3;
    ignoreNextUnderOrOverflow();
}

void
PaulaAudio::rampUpFromZero()
{
    volume = 0;
    rampUp();
}
 
void
PaulaAudio::rampDown()
{    
    targetVolume = 0;
    volumeDelta = 50;
    ignoreNextUnderOrOverflow();
}

void
PaulaAudio::clearRingbuffer()
{
    debug(AUDBUF_DEBUG, "Clearing ringbuffer\n");
    
    // Wipe out the ringbuffers
    for (unsigned i = 0; i < bufferSize; i++) {
        ringBufferL[i] = ringBufferR[i] = 0.0;
    }

    // Wipe out the filter buffers
    filterL.clear();
    filterR.clear();

    // Put the write pointer ahead of the read pointer
    alignWritePtr();
}

void
PaulaAudio::readMonoSample(float *mono)
{
    float left, right;

    readStereoSample(&left, &right);
    *mono = left + right;
}

void
PaulaAudio::readStereoSample(float *left, float *right)
{
    // Read sound samples
    float l = ringBufferL[readPtr];
    float r = ringBufferR[readPtr];

    // Modify volume
    if (volume != targetVolume) {
        if (volume < targetVolume) {
            volume += MIN(volumeDelta, targetVolume - volume);
        } else {
            volume -= MIN(volumeDelta, volume - targetVolume);
        }
    }

    // Apply volume
    float divider = 10000.0f;
    if (volume > 0) {
        l *= (float)volume / divider;
        r *= (float)volume / divider;
    } else {
        l = 0.0;
        r = 0.0;
    }

    // Advance read pointer
    advanceReadPtr();

    // Write result
    *left = l;
    *right = r;
}

float
PaulaAudio::ringbufferDataL(size_t offset)
{
    return ringBufferL[(readPtr + offset) % bufferSize];
}

float
PaulaAudio::ringbufferDataR(size_t offset)
{
    return ringBufferR[(readPtr + offset) % bufferSize];
}

float
PaulaAudio::ringbufferData(size_t offset)
{
    return ringbufferDataL(offset) + ringbufferDataR(offset);
}

void
PaulaAudio::readMonoSamples(float *target, size_t n)
{
    // Check for a buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read sound samples
    for (size_t i = 0; i < n; i++) {
        readMonoSample(target + i);
    }
}

void
PaulaAudio::readStereoSamples(float *target1, float *target2, size_t n)
{
    // Check for a buffer underflow
    if (samplesInBuffer() < n)
        handleBufferUnderflow();
    
    // Read sound samples
    for (unsigned i = 0; i < n; i++)
        readStereoSample(target1 + i, target2 + i);
}

void
PaulaAudio::readStereoSamplesInterleaved(float *target, size_t n)
{
    // Check for a buffer underflow
    if (samplesInBuffer() < n)
        handleBufferUnderflow();
    
    // Read sound samples
    for (unsigned i = 0; i < n; i++)
        readStereoSample(target + 2*i, target + 2*i + 1);
}

void
PaulaAudio::writeData(float left, float right)
{
    // Check for buffer overflow
    if (bufferCapacity() == 0) handleBufferOverflow();

    // Apply audio filter if applicable
    if (ciaa.powerLED() || config.filterAlwaysOn) {
        left = filterL.apply(left);
        right = filterR.apply(right);
    }

    // Write samples into ringbuffer
    ringBufferL[writePtr] = left;
    ringBufferR[writePtr] = right;
    advanceWritePtr();
}

void
PaulaAudio::handleBufferUnderflow()
{
    // There are two common scenarios in which buffer underflows occur:
    //
    // (1) The consumer runs slightly faster than the producer.
    // (2) The producer is halted or not startet yet.
    
    debug(AUDBUF_DEBUG, "SID RINGBUFFER UNDERFLOW (r: %ld w: %ld)\n", readPtr, writePtr);
    
    // Determine the elapsed seconds since the last pointer adjustment.
    u64 now = mach_absolute_time();
    double elapsedTime = (double)(now - lastAlignment) / 1000000000.0;
    lastAlignment = now;
    
    // Adjust the sample rate, if condition (1) holds.
    if (elapsedTime > 10.0) {

        stats.bufferUnderflows++;
        
        // Increase the sample rate based on what we've measured.
        int offPerSecond = (int)(samplesAhead / elapsedTime);
        setSampleRate(getSampleRate() + offPerSecond);
    }
    
    // Reset the write pointer
    alignWritePtr();
}

void
PaulaAudio::handleBufferOverflow()
{
    // There are two common scenarios in which buffer overflows occur:
    //
    // (1) The consumer runs slightly slower than the producer.
    // (2) The consumer is halted or not startet yet.
    
    debug(AUDBUF_DEBUG, "SID RINGBUFFER OVERFLOW (r: %ld w: %ld)\n", readPtr, writePtr);
    
    // Determine the elapsed seconds since the last pointer adjustment.
    u64 now = mach_absolute_time();
    double elapsedTime = (double)(now - lastAlignment) / 1000000000.0;
    lastAlignment = now;
    
    // Adjust the sample rate, if condition (1) holds.
    if (elapsedTime > 10.0) {
        
        stats.bufferOverflows++;
        
        // Decrease the sample rate based on what we've measured.
        int offPerSecond = (int)(samplesAhead / elapsedTime);
        setSampleRate(getSampleRate() - offPerSecond);
    }
    
    // Reset the write pointer
    alignWritePtr();
}

float
PaulaAudio::drawWaveform(unsigned *buffer, int width, int height,
                        bool left, float highestAmplitude, unsigned color)
{
    int dw = bufferSize / width;
    float newHighestAmplitude = 0.001;
    float *ringBuffer = left ? ringBufferL : ringBufferR;
    
    // Clear buffer
    for (int i = 0; i < width * height; i++) {
        buffer[i] = color & 0xFFFFFF;
    }
    
    // Draw waveform
    for (int w = 0; w < width; w++) {
        
        // Read samples from ringbuffer
        float sample = abs(ringBuffer[(readPtr + w * dw) % bufferSize]);
        
        if (sample == 0) {
            
            // Draw some noise to make it look sexy
            unsigned *ptr = buffer + width * height / 2 + w;
            *ptr = color;
            if (rand() % 2) *(ptr + width) = color;
            if (rand() % 2) *(ptr - width) = color;
            
        } else {
            
            // Remember the highest amplitude
            if (sample > newHighestAmplitude) newHighestAmplitude = sample;
            
            // Scale the sample
            int scaled = sample * height / highestAmplitude;
            if (scaled > height) scaled = height;
            
            // Draw vertical line
            unsigned *ptr = buffer + width * ((height - scaled) / 2) + w;
            for (int j = 0; j < scaled; j++, ptr += width) *ptr = color;
        }
    }
    return newHighestAmplitude;
}

template<> u8 PaulaAudio::getState<0>() { return channel0.state; }
template<> u8 PaulaAudio::getState<1>() { return channel1.state; }
template<> u8 PaulaAudio::getState<2>() { return channel2.state; }
template<> u8 PaulaAudio::getState<3>() { return channel3.state; }
