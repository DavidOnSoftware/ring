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
 * @file configurationTest.cpp
 * @brief       Regroups unitary tests related to the user configuration.
 *              Check if the default configuration has been successfully loaded
 */

#ifndef CONFIGURATION_TEST_
#define CONFIGURATION_TEST_

// Cppunit import
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

namespace ring { namespace test {

class ConfigurationTest: public CppUnit::TestFixture {

        /*
         * Use cppunit library macros to add unit test the factory
         */
        CPPUNIT_TEST_SUITE(ConfigurationTest);
        CPPUNIT_TEST(testNodeParse);
        CPPUNIT_TEST(test_expand_path);
        CPPUNIT_TEST_SUITE_END();

    public:

        void testNodeParse();
        void test_expand_path();
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ConfigurationTest, "ConfigurationTest");
CPPUNIT_TEST_SUITE_REGISTRATION(ConfigurationTest);

}} // namespace ring::test

#endif // CONFIGURATION_TEST_
