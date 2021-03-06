/*
 *  Copyright (C) 2004-2016 Savoir-faire Linux Inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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
 * @file audiorecorderTest.cpp
 * @brief       Regroups unitary tests related to the plugin manager.
 */

#ifndef AUDIOLAYER_TEST_
#define AUDIOLAYER_TEST_

// Cppunit import
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include "noncopyable.h"

namespace ring {

class Manager;
class PulseLayer;

} // namespace ring

namespace ring { namespace test {

class AudioLayerTest: public CppUnit::TestFixture {

        CPPUNIT_TEST_SUITE(AudioLayerTest);
        CPPUNIT_TEST(testAudioLayerConfig);
        //CPPUNIT_TEST(testPulseConnect);
        //TODO: this test ends the test sequence when using on a alsa only system
        //CPPUNIT_TEST(testAudioLayerSwitch);
        CPPUNIT_TEST_SUITE_END();

    public:
        AudioLayerTest();
        void testAudioLayerConfig();
        void testPulseConnect();
        void testAudioLayerSwitch();

    private:
        NON_COPYABLE(AudioLayerTest);

        Manager* manager_;
        PulseLayer* pulselayer_;
        int layer_;
};
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AudioLayerTest, "AudioLayerTest");
CPPUNIT_TEST_SUITE_REGISTRATION(AudioLayerTest);

}} // namespace ring::test

#endif // AUDIOLAYER_TEST_
