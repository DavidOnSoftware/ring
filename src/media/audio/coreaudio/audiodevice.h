/*
 *  Copyright (C) 2004-2016 Savoir-faire Linux Inc.
 *
 *  Author: Philippe Groarke <philippe.groarke@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

#ifndef AUDIO_DEVICE_H
#define AUDIO_DEVICE_H

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>

#include <string>

namespace ring {

class AudioDevice {
public:
    AudioDevice() : id_(kAudioDeviceUnknown) { }
    AudioDevice(AudioDeviceID devid, bool isInput);
    void init(AudioDeviceID devid, bool isInput);
    bool valid() const;
    void setBufferSize(UInt32 size);

public:
    AudioDeviceID                   id_;
    std::string                     name_;
    bool                            isInput_;
    int                             channels_;
    UInt32                          safetyOffset_;
    UInt32                          bufferSizeFrames_;
    AudioStreamBasicDescription     format_;

private:
    int     countChannels() const;
    std::string  getName() const;
};

}

#endif /* defined(AUDIO_DEVICE_H) */
