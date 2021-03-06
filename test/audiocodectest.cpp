/*
 *  Copyright (C) 2004-2016 Savoir-faire Linux Inc.
 *
 *  Author: Tristan Matthews <tristan.matthews@savoirfairelinux.com>
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

/*
 * ebail - 2015/02/18
 * testCodecs unit test is based on audiocodecfactory
 * we are not using it anymore
 * we should make this unit test work with libav
 * this test is disabled for the moment
 * */
#if 0

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "audiocodectest.h"
#include "audio/codecs/audiocodecfactory.h"
#include "plugin_manager.h"

#include "test_utils.h"
#include "ring_types.h" // for AudioSample

#include <cmath>
#include <climits>

namespace ring { namespace test {


/*
 * Detect the power of a signal for a given frequency.
 * Adapted from:
 * http://netwerkt.wordpress.com/2011/08/25/goertzel-filter/
 */
static double
goertzelFilter(AudioSample *samples, double freq, unsigned N, double sample_rate)
{
    double s_prev = 0.0;
    double s_prev2 = 0.0;
    const double normalizedfreq = freq / sample_rate;
    double coeff = 2 * cos(M_2_PI * normalizedfreq);
    for (unsigned i = 0; i < N; i++) {
        double s = samples[i] + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }

    return s_prev2 * s_prev2 + s_prev * s_prev - coeff * s_prev * s_prev2;
}

void AudioCodecTest::testCodecs()
{
    TITLE();

    PluginManager pluginMgr;
    AudioCodecFactory factory(pluginMgr);
    const auto payloadTypes = factory.getCodecList();

    std::vector<std::shared_ptr<AudioCodec>> codecs;

    for (auto p : payloadTypes)
        codecs.push_back(factory.getCodec(p));

    std::vector<std::vector<AudioSample>> sine = {};
    std::vector<std::vector<AudioSample>> pcm;

    unsigned sampleRate = 0;
    double referencePower = 0.0;

    for (auto c : codecs) {

        // generate the sine tone if rate has changed
        if (sampleRate != c->getCurrentClockRate()) {
            sampleRate = c->getCurrentClockRate();
            const unsigned nbSamples = sampleRate * 0.02; // 20 ms worth of samples
            sine = {std::vector<AudioSample>(nbSamples)};
            pcm = {std::vector<AudioSample>(nbSamples)};

            const float theta = M_2_PI * frequency_ / sampleRate;

            for (unsigned i = 0; i < nbSamples; ++i) {
                sine[0][i] = SHRT_MAX * sin(theta * i);
                sine[0][i] >>= 3; /* attenuate it a bit */
            }

            /* Store the raw signal's power detected at 440 Hz, this is much cheaper
             * than an FFT */
            referencePower = goertzelFilter(sine[0].data(), frequency_, sine[0].size(), sampleRate);
        }

        std::vector<uint8_t> data(RAW_BUFFER_SIZE);

        const size_t encodedBytes = c->encode(sine, data.data(), sine[0].size());

        unsigned decoded = c->decode(pcm, data.data(), encodedBytes);
        CPPUNIT_ASSERT(decoded == sine[0].size());

        const auto decodedPower = goertzelFilter(pcm[0].data(), frequency_, pcm[0].size(), sampleRate);
        const auto decodedRatio = decodedPower / referencePower;
        CPPUNIT_ASSERT(decodedRatio > 0.0);
    }
}

}} // namespace ring::test
#endif
