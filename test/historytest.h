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

// Cppunit import
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

// Application import
#include "noncopyable.h"

/*
 * @file historyTest.h
 * @brief       Regroups unitary tests related to the phone number cleanup function.
 */

#ifndef HISTORY_TEST_
#define HISTORY_TEST_

namespace ring {
class History;
} // namespace ring

namespace ring { namespace test {

class HistoryTest : public CppUnit::TestCase {

        /**
          * Use cppunit library macros to add unit test the factory
          */
        CPPUNIT_TEST_SUITE(HistoryTest);
        CPPUNIT_TEST(test_create_path);
        CPPUNIT_TEST(test_load_from_file);
        CPPUNIT_TEST(test_load_items);
        CPPUNIT_TEST(test_get_serialized);
        CPPUNIT_TEST_SUITE_END();

    public:
        HistoryTest() : CppUnit::TestCase("History Tests"), history_(0) {}

        /*
         * Code factoring - Common resources can be initialized here.
         * This method is called by unitcpp before each test
         */
        void setUp();

        void test_create_path();

        void test_load_from_file();

        void test_load_items();

        void test_save_to_file();

        void test_get_serialized();

        /*
         * Code factoring - Common resources can be released here.
         * This method is called by unitcpp after each test
         */
        void tearDown();

    private:
        NON_COPYABLE(HistoryTest);
        ring::History *history_;
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(HistoryTest, "HistoryTest");
CPPUNIT_TEST_SUITE_REGISTRATION(HistoryTest);


}} // namespace ring::test

#endif // HISTORY_TEST_
