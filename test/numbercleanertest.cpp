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

#include <cstdio>
#include <sstream>
#include <dlfcn.h>

#include "logger.h"

#include "numbercleanertest.h"

#define NUMBER_TEST_1   "514 333 4444"
#define NUMBER_TEST_2   "514-333-4444"
#define NUMBER_TEST_3   "(514) 333 4444"
#define NUMBER_TEST_4   "(514)-333-4444"
#define NUMBER_TEST_5   "(514) 333-4444"
#define NUMBER_TEST_6   "514 333  4444"
#define NUMBER_TEST_7   "ext 136"
#define NUMBER_TEST_8   "514 333  4444 ext. 136"
#define NUMBER_TEST_9   "514 333  4444 ext 136"
#define NUMBER_TEST_10   "136"

#define VALID_NUMBER                "5143334444"
#define VALID_PREPENDED_NUMBER      "95143334444"
#define VALID_EXTENSION             "136"

namespace ring { namespace test {

void NumberCleanerTest::test_format_1(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_1 --------------------\n");
    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_1) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_2(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_2 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_2) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_3(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_3 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_3) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_4(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_4 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_4) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_5(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_5 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_5) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_6(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_6 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_6) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_7(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_7 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_7) == VALID_EXTENSION);
}

void NumberCleanerTest::test_format_8(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_8 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_8) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_9(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_9 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_9) == VALID_NUMBER);
}

void NumberCleanerTest::test_format_10(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_10 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_1, "9") == VALID_PREPENDED_NUMBER);
}

void NumberCleanerTest::test_format_11(void)
{
    RING_DBG("-------------------- NumberCleanerTest::test_format_11 --------------------\n");

    CPPUNIT_ASSERT(NumberCleaner::clean(NUMBER_TEST_10, "9") == VALID_EXTENSION);
}

}} // namespace ring::test
