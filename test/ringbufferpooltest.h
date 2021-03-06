/*
 *  Copyright (C) 2004-2016 Savoir-faire Linux Inc.
 *
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#ifndef RINGBUFFERPOOL_TEST_
#define RINGBUFFERPOOL_TEST_

// Cppunit import
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <memory>

/*
 * @file audiorecorderTest.cpp
 * @brief       Regroups unit tests related to the main buffer.
 */

namespace ring {
class RingBufferPool;
} // namespace ring

namespace ring { namespace test {

class RingBufferPoolTest : public CppUnit::TestCase {

        /*
         * Use cppunit library macros to add unit test the factory
         */
        CPPUNIT_TEST_SUITE(RingBufferPoolTest);
        CPPUNIT_TEST(testBindUnbindBuffer);
        CPPUNIT_TEST(testGetPutData);
        CPPUNIT_TEST(testDiscardFlush);
        CPPUNIT_TEST(testConference);
        CPPUNIT_TEST_SUITE_END();

    public:

        RingBufferPoolTest();

        void testBindUnbindBuffer();

        void testGetPutData();

        void testGetAvailableData();

        void testDiscardFlush();

        void testConference();

    private:

        std::unique_ptr<ring::RingBufferPool> rbPool_;
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(RingBufferPoolTest, "RingBufferPoolTest");
CPPUNIT_TEST_SUITE_REGISTRATION(RingBufferPoolTest);

}} // namespace ring::test

#endif  // RINGBUFFERPOOL_TEST_
