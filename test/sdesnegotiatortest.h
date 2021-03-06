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

// Cppunit import
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <cstddef>
#include <stdio.h>
#include <sstream>

#include <vector>

// pjsip import
#include <pjsip.h>
#include <pjlib.h>
#include <pjsip_ua.h>
#include <pjlib-util.h>
#include <pjnath/stun_config.h>

#include "noncopyable.h"

/*
 * @file sdesnegotiationTest.cpp
 * @brief       Regroups unitary tests related to the plugin manager.
 */

#ifndef __SDESNEGOTIATOR_TEST_H__
#define __SDESNEGOTIATOR_TEST_H__

#include "sip/sdes_negotiator.h" // for CryptoSuiteDefinition

namespace ring { namespace test {

class Pattern;

class SdesNegotiatorTest : public CppUnit::TestCase {

        /*
         * Use cppunit library macros to add unit test the factory
         */
        CPPUNIT_TEST_SUITE(SdesNegotiatorTest);
        CPPUNIT_TEST(testTagPattern);
        CPPUNIT_TEST(testCryptoSuitePattern);
        CPPUNIT_TEST(testKeyParamsPattern);
        CPPUNIT_TEST(testKeyParamsPatternWithoutMKI);
        CPPUNIT_TEST(testNegotiation);
        CPPUNIT_TEST(testMostSimpleCase);
        CPPUNIT_TEST(test32ByteKeyLength);
        CPPUNIT_TEST_SUITE_END();

    public:
        /*
         * Code factoring - Common resources can be released here.
         * This method is called by unitcpp after each test
         */
        void testTagPattern();

        void testCryptoSuitePattern();

        void testKeyParamsPattern();

        void testKeyParamsPatternCiscoStyle();

        void testKeyParamsPatternWithoutMKI();

        void testNegotiation();

        void testComponent();

        void testMostSimpleCase();

        void test32ByteKeyLength();
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SdesNegotiatorTest, "SdesNegotiatorTest");
CPPUNIT_TEST_SUITE_REGISTRATION(SdesNegotiatorTest);

}} // namespace ring::test

#endif // __SDESNEGOTIATOR_TEST_H__
