/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CODEC_BASE_H_

#define CODEC_BASE_H_

#include <stdint.h>

#define STRINGIFY_ENUMS

#include <media/IOMX.h>
#include <media/MediaCodecInfo.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/ColorUtils.h>
#include <media/hardware/HardwareAPI.h>

#include <utils/NativeHandle.h>

#include <system/graphics.h>

namespace android {

struct ABuffer;
struct PersistentSurface;

struct CodecBase : public AHandler, /* static */ ColorUtils {
    enum {
        kWhatFillThisBuffer      = 'fill',
        kWhatDrainThisBuffer     = 'drai',
        kWhatEOS                 = 'eos ',
        kWhatShutdownCompleted   = 'scom',
        kWhatFlushCompleted      = 'fcom',
        kWhatOutputFormatChanged = 'outC',
        kWhatError               = 'erro',
        kWhatComponentAllocated  = 'cAll',
        kWhatComponentConfigured = 'cCon',
        kWhatInputSurfaceCreated = 'isfc',
        kWhatInputSurfaceAccepted = 'isfa',
        kWhatSignaledInputEOS    = 'seos',
        kWhatBuffersAllocated    = 'allc',
        kWhatOutputFramesRendered = 'outR',
    };

    enum {
        kMaxCodecBufferSize = 8192 * 4096 * 4, // 8K RGBA
    };

    virtual void setNotificationMessage(const sp<AMessage> &msg) = 0;

    virtual void initiateAllocateComponent(const sp<AMessage> &msg) = 0;
    virtual void initiateConfigureComponent(const sp<AMessage> &msg) = 0;
    virtual void initiateCreateInputSurface() = 0;
    virtual void initiateSetInputSurface(
            const sp<PersistentSurface> &surface) = 0;
    virtual void initiateStart() = 0;
    virtual void initiateShutdown(bool keepComponentAllocated = false) = 0;

    // require an explicit message handler
    virtual void onMessageReceived(const sp<AMessage> &msg) = 0;

    virtual status_t queryCapabilities(
            const AString &name, const AString &mime, bool isEncoder,
            sp<MediaCodecInfo::Capabilities> *caps /* nonnull */) { return INVALID_OPERATION; }

    virtual status_t setSurface(const sp<Surface> &surface) { return INVALID_OPERATION; }

    virtual void signalFlush() = 0;
    virtual void signalResume() = 0;

    virtual void signalRequestIDRFrame() = 0;
    virtual void signalSetParameters(const sp<AMessage> &msg) = 0;
    virtual void signalEndOfInputStream() = 0;
    bool mGtsExoPlayer;
    bool mSoftCodecPref;
    void setSoftCodecPref(bool forcesoft){mSoftCodecPref = forcesoft;};

    struct PortDescription : public RefBase {
        virtual size_t countBuffers() = 0;
        virtual IOMX::buffer_id bufferIDAt(size_t index) const = 0;
        virtual sp<ABuffer> bufferAt(size_t index) const = 0;
        virtual sp<NativeHandle> handleAt(size_t index) const { return NULL; };
        virtual sp<RefBase> memRefAt(size_t index) const { return NULL; }

    protected:
        PortDescription();
        virtual ~PortDescription();

    private:
        DISALLOW_EVIL_CONSTRUCTORS(PortDescription);
    };

    /*
     * Codec-related defines
     */

protected:
    CodecBase();
    virtual ~CodecBase();

private:
    DISALLOW_EVIL_CONSTRUCTORS(CodecBase);
};

}  // namespace android

#endif  // CODEC_BASE_H_

