/*
 *  Copyright (C) 2004-2016 Savoir-faire Linux Inc.
 *
 *  Author: Julien Bonjean <julien.bonjean@savoirfairelinux.com>
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

#include "logger.h"
#include "manager.h"
#include "constants.h"
#include "fileutils.h"

#include <cstdlib>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

namespace {
    void restore()
    {
        if (system("mv " CONFIG_SAMPLE_BAK " " CONFIG_SAMPLE) < 0)
            RING_ERR("Restoration of %s failed", CONFIG_SAMPLE);
    }
    void backup()
    {
        if (system("cp " CONFIG_SAMPLE " " CONFIG_SAMPLE_BAK) < 0)
            RING_ERR("Backup of %s failed", CONFIG_SAMPLE);
    }
}

void cleanup()
{
    int ret = system("killall sipp");
    std::cerr << "Killed all sip processes with status " << ret << std::endl;
}

int main(int argc, char* argv[])
{
    atexit(cleanup);
    printf("\nRing Daemon Test Suite, by Savoir-faire Linux 2004-2016\n\n");
    setConsoleLog(true);
    setDebugMode(true);
    ring::fileutils::FileHandle f(ring::fileutils::create_pidfile());
    if (f.fd == -1) {
        fprintf(stderr, "An dring instance is already running, quitting...\n");
        return 1;
    }

    int argvIndex = 1;
    bool xmlOutput = false;

    if (argc > 1) {
        if (strcmp("--help", argv[1]) == 0) {
            argvIndex++;

            CPPUNIT_NS::Test* suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry("All Tests").makeTest();

            int testSuiteCount = suite->getChildTestCount();
            printf("Usage: test [OPTIONS] [TEST_SUITE]\n");
            printf("\nOptions:\n");
            printf(" --xml - Output results in an XML file, instead of standard output.\n");
            printf(" --debug - Debug mode\n");
            printf(" --help - Print help\n");
            printf("\nAvailable test suites:\n");

            for (int i = 0; i < testSuiteCount; i++) {
                printf(" - %s\n", suite->getChildTestAt(i)->getName().c_str());
            }

            return 0;
        } else if (strcmp("--debug", argv[1]) == 0) {
            argvIndex++;

            setDebugMode(true);
            RING_INFO("Debug mode activated");

        } else if (strcmp("--xml", argv[1]) == 0) {
            argvIndex++;

            xmlOutput = true;
            RING_INFO("Using XML output");
        }
    }

    // Default test suite : all tests
    std::string testSuiteName = "All Tests";

    if (argvIndex < argc) {
        testSuiteName = argv[argvIndex];
        argvIndex++;
    }

    printf("\n\n=== SFLphone initialization ===\n\n");
    backup();
    ring::Manager::instance().init(CONFIG_SAMPLE);

    // Get the top level suite from the registry
    printf("\n\n=== Test Suite: %s ===\n\n", testSuiteName.c_str());
    CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry(testSuiteName).makeTest();

    if (suite->getChildTestCount() == 0) {
        RING_ERR("Invalid test suite name: %s", testSuiteName.c_str());
        restore();
        return 1;
    }

    // Adds the test to the list of test to run
    CppUnit::TextTestRunner runner;
    runner.addTest(suite);
    /* Specify XML output */
    std::ofstream outfile("cppunitresults.xml");

    if (xmlOutput) {
        CppUnit::XmlOutputter* outputter = new CppUnit::XmlOutputter(&runner.result(), outfile);
        runner.setOutputter(outputter);
    } else {
        // Change the default outputter to a compiler error format outputter
        runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
    }

    // Run the tests.
    bool wasSuccessful = runner.run();

    printf("=== Test suite ending ===\n");
    ring::Manager::instance().finish();

    restore();

    return wasSuccessful ? 0 : 1;
}
