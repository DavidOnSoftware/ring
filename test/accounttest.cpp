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

#include <cppunit/extensions/HelperMacros.h>
#include <map>

#include "account.h"
#include "account_schema.h"
#include "accounttest.h"
#include "manager.h"
#include "configurationmanager_interface.h"
#include "logger.h"

namespace ring { namespace test {

void AccountTest::TestAddRemove()
{
    RING_DBG("-------------------- %s --------------------\n", __PRETTY_FUNCTION__);

    auto details = DRing::getAccountTemplate();
    details[Conf::CONFIG_ACCOUNT_TYPE] = "SIP";
    details[Conf::CONFIG_ACCOUNT_ENABLE] = "false";
    details[Conf::CONFIG_LOCAL_INTERFACE] = "default";
    details[Conf::CONFIG_LOCAL_PORT] = "5060";

    auto accountId = Manager::instance().addAccount(details);
    CPPUNIT_ASSERT(not accountId.empty());
    CPPUNIT_ASSERT(Manager::instance().hasAccount(accountId));

    Manager::instance().removeAccount(accountId);
    CPPUNIT_ASSERT(!Manager::instance().hasAccount(accountId));
}

}} // namespace ring::test
