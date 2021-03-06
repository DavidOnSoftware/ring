/*
 *  Copyright (C) 2014-2016 Savoir-faire Linux Inc.
 *
 *  Author: Adrien Béraud <adrien.beraud@savoirfairelinux.com>
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


#include "sipaccountbase.h"
#include "sipvoiplink.h"

#ifdef RING_VIDEO
#include "libav_utils.h"
#endif

#include "account_schema.h"
#include "manager.h"
#include "ice_transport.h"

#include "config/yamlparser.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <yaml-cpp/yaml.h>
#pragma GCC diagnostic pop

#include "client/ring_signal.h"
#include "string_utils.h"

#include <type_traits>

namespace ring {

SIPAccountBase::SIPAccountBase(const std::string& accountID)
    : Account(accountID), link_(getSIPVoIPLink())
{}

SIPAccountBase::~SIPAccountBase() {}

template <typename T>
static void
validate(std::string &member, const std::string &param, const T& valid)
{
    const auto begin = std::begin(valid);
    const auto end = std::end(valid);
    if (find(begin, end, param) != end)
        member = param;
    else
        RING_ERR("Invalid parameter \"%s\"", param.c_str());
}

static void
updateRange(uint16_t min, uint16_t max, std::pair<uint16_t, uint16_t> &range)
{
    if (min > 0 and (max > min) and max <= SIPAccountBase::MAX_PORT - 2) {
        range.first = min;
        range.second = max;
    }
}

static void
unserializeRange(const YAML::Node &node, const char *minKey, const char *maxKey, std::pair<uint16_t, uint16_t> &range)
{
    int tmpMin = 0;
    int tmpMax = 0;
    yaml_utils::parseValue(node, minKey, tmpMin);
    yaml_utils::parseValue(node, maxKey, tmpMax);
    updateRange(tmpMin, tmpMax, range);
}

static void
addRangeToDetails(std::map<std::string, std::string> &a, const char *minKey,
                  const char *maxKey,
                  const std::pair<uint16_t, uint16_t> &range)
{
    a.emplace(minKey, ring::to_string(range.first));
    a.emplace(maxKey, ring::to_string(range.second));
}

void SIPAccountBase::serialize(YAML::Emitter &out)
{
    Account::serialize(out);

    out << YAML::Key << Conf::AUDIO_PORT_MAX_KEY << YAML::Value << audioPortRange_.second;
    out << YAML::Key << Conf::AUDIO_PORT_MIN_KEY << YAML::Value << audioPortRange_.first;
    out << YAML::Key << Conf::DTMF_TYPE_KEY << YAML::Value << dtmfType_;
    out << YAML::Key << Conf::INTERFACE_KEY << YAML::Value << interface_;
    out << YAML::Key << Conf::PUBLISH_ADDR_KEY << YAML::Value << publishedIpAddress_;
    out << YAML::Key << Conf::PUBLISH_PORT_KEY << YAML::Value << publishedPort_;
    out << YAML::Key << Conf::SAME_AS_LOCAL_KEY << YAML::Value << publishedSameasLocal_;

    out << YAML::Key << VIDEO_ENABLED_KEY << YAML::Value << videoEnabled_;
    out << YAML::Key << Conf::VIDEO_PORT_MAX_KEY << YAML::Value << videoPortRange_.second;
    out << YAML::Key << Conf::VIDEO_PORT_MIN_KEY << YAML::Value << videoPortRange_.first;

    out << YAML::Key << Conf::STUN_ENABLED_KEY << YAML::Value << stunEnabled_;
    out << YAML::Key << Conf::STUN_SERVER_KEY << YAML::Value << stunServer_;
    out << YAML::Key << Conf::TURN_ENABLED_KEY << YAML::Value << turnEnabled_;
    out << YAML::Key << Conf::TURN_SERVER_KEY << YAML::Value << turnServer_;
    out << YAML::Key << Conf::TURN_SERVER_UNAME_KEY << YAML::Value << turnServerUserName_;
    out << YAML::Key << Conf::TURN_SERVER_PWD_KEY << YAML::Value << turnServerPwd_;
    out << YAML::Key << Conf::TURN_SERVER_REALM_KEY << YAML::Value << turnServerRealm_;
}

void SIPAccountBase::serializeTls(YAML::Emitter &out)
{
    out << YAML::Key << Conf::CALIST_KEY << YAML::Value << tlsCaListFile_;
    out << YAML::Key << Conf::CERTIFICATE_KEY << YAML::Value << tlsCertificateFile_;
    out << YAML::Key << Conf::TLS_PASSWORD_KEY << YAML::Value << tlsPassword_;
    out << YAML::Key << Conf::PRIVATE_KEY_KEY << YAML::Value << tlsPrivateKeyFile_;
}

void SIPAccountBase::unserialize(const YAML::Node &node)
{
    using yaml_utils::parseValue;
    using yaml_utils::parseVectorMap;

    Account::unserialize(node);

    parseValue(node, VIDEO_ENABLED_KEY, videoEnabled_);

    parseValue(node, Conf::INTERFACE_KEY, interface_);
    parseValue(node, Conf::SAME_AS_LOCAL_KEY, publishedSameasLocal_);
    std::string publishedIpAddress;
    parseValue(node, Conf::PUBLISH_ADDR_KEY, publishedIpAddress);
    IpAddr publishedIp = publishedIpAddress;
    if (publishedIp and not publishedSameasLocal_)
        setPublishedAddress(publishedIp);

    int port = sip_utils::DEFAULT_SIP_PORT;
    parseValue(node, Conf::PUBLISH_PORT_KEY, port);
    publishedPort_ = port;

    parseValue(node, Conf::DTMF_TYPE_KEY, dtmfType_);

    // get tls submap
    const auto &tlsMap = node[Conf::TLS_KEY];
    parseValue(tlsMap, Conf::CERTIFICATE_KEY, tlsCertificateFile_);
    parseValue(tlsMap, Conf::CALIST_KEY, tlsCaListFile_);
    parseValue(tlsMap, Conf::TLS_PASSWORD_KEY, tlsPassword_);
    parseValue(tlsMap, Conf::PRIVATE_KEY_KEY, tlsPrivateKeyFile_);

    unserializeRange(node, Conf::AUDIO_PORT_MIN_KEY, Conf::AUDIO_PORT_MAX_KEY, audioPortRange_);
    unserializeRange(node, Conf::VIDEO_PORT_MIN_KEY, Conf::VIDEO_PORT_MAX_KEY, videoPortRange_);

    // ICE - STUN/TURN
    if (not isIP2IP()) {
        parseValue(node, Conf::STUN_ENABLED_KEY, stunEnabled_);
        parseValue(node, Conf::STUN_SERVER_KEY, stunServer_);
        parseValue(node, Conf::TURN_ENABLED_KEY, turnEnabled_);
        parseValue(node, Conf::TURN_SERVER_KEY, turnServer_);
        parseValue(node, Conf::TURN_SERVER_UNAME_KEY, turnServerUserName_);
        parseValue(node, Conf::TURN_SERVER_PWD_KEY, turnServerPwd_);
        parseValue(node, Conf::TURN_SERVER_REALM_KEY, turnServerRealm_);
    }
}

void SIPAccountBase::setAccountDetails(const std::map<std::string, std::string> &details)
{
    Account::setAccountDetails(details);

    parseBool(details, Conf::CONFIG_VIDEO_ENABLED, videoEnabled_);

    // general sip settings
    parseString(details, Conf::CONFIG_LOCAL_INTERFACE, interface_);
    parseBool(details, Conf::CONFIG_PUBLISHED_SAMEAS_LOCAL, publishedSameasLocal_);
    parseString(details, Conf::CONFIG_PUBLISHED_ADDRESS, publishedIpAddress_);
    parseInt(details, Conf::CONFIG_PUBLISHED_PORT, publishedPort_);

    parseString(details, Conf::CONFIG_ACCOUNT_DTMF_TYPE, dtmfType_);

    int tmpMin = -1;
    parseInt(details, Conf::CONFIG_ACCOUNT_AUDIO_PORT_MIN, tmpMin);
    int tmpMax = -1;
    parseInt(details, Conf::CONFIG_ACCOUNT_AUDIO_PORT_MAX, tmpMax);
    updateRange(tmpMin, tmpMax, audioPortRange_);
#ifdef RING_VIDEO
    tmpMin = -1;
    parseInt(details, Conf::CONFIG_ACCOUNT_VIDEO_PORT_MIN, tmpMin);
    tmpMax = -1;
    parseInt(details, Conf::CONFIG_ACCOUNT_VIDEO_PORT_MAX, tmpMax);
    updateRange(tmpMin, tmpMax, videoPortRange_);
#endif

    // TLS
    parseString(details, Conf::CONFIG_TLS_CA_LIST_FILE, tlsCaListFile_);
    parseString(details, Conf::CONFIG_TLS_CERTIFICATE_FILE, tlsCertificateFile_);
    parseString(details, Conf::CONFIG_TLS_PRIVATE_KEY_FILE, tlsPrivateKeyFile_);
    parseString(details, Conf::CONFIG_TLS_PASSWORD, tlsPassword_);

    // ICE - STUN
    parseBool(details, Conf::CONFIG_STUN_ENABLE, stunEnabled_);
    parseString(details, Conf::CONFIG_STUN_SERVER, stunServer_);

    // ICE - TURN
    parseBool(details, Conf::CONFIG_TURN_ENABLE, turnEnabled_);
    parseString(details, Conf::CONFIG_TURN_SERVER, turnServer_);
    parseString(details, Conf::CONFIG_TURN_SERVER_UNAME, turnServerUserName_);
    parseString(details, Conf::CONFIG_TURN_SERVER_PWD, turnServerPwd_);
    parseString(details, Conf::CONFIG_TURN_SERVER_REALM, turnServerRealm_);
}

std::map<std::string, std::string>
SIPAccountBase::getAccountDetails() const
{
    auto a = Account::getAccountDetails();
    a.emplace(Conf::CONFIG_VIDEO_ENABLED, videoEnabled_ ? TRUE_STR : FALSE_STR);

    addRangeToDetails(a, Conf::CONFIG_ACCOUNT_AUDIO_PORT_MIN, Conf::CONFIG_ACCOUNT_AUDIO_PORT_MAX, audioPortRange_);
#ifdef RING_VIDEO
    addRangeToDetails(a, Conf::CONFIG_ACCOUNT_VIDEO_PORT_MIN, Conf::CONFIG_ACCOUNT_VIDEO_PORT_MAX, videoPortRange_);
#endif

    a.emplace(Conf::CONFIG_ACCOUNT_DTMF_TYPE,       dtmfType_);
    a.emplace(Conf::CONFIG_LOCAL_INTERFACE,         interface_);
    a.emplace(Conf::CONFIG_PUBLISHED_PORT,          ring::to_string(publishedPort_));
    a.emplace(Conf::CONFIG_PUBLISHED_SAMEAS_LOCAL,  publishedSameasLocal_ ? TRUE_STR : FALSE_STR);
    a.emplace(Conf::CONFIG_PUBLISHED_ADDRESS,       publishedIpAddress_);

    a.emplace(Conf::CONFIG_STUN_ENABLE, stunEnabled_ ? TRUE_STR : FALSE_STR);
    a.emplace(Conf::CONFIG_STUN_SERVER, stunServer_);
    a.emplace(Conf::CONFIG_TURN_ENABLE, turnEnabled_ ? TRUE_STR : FALSE_STR);
    a.emplace(Conf::CONFIG_TURN_SERVER, turnServer_);
    a.emplace(Conf::CONFIG_TURN_SERVER_UNAME, turnServerUserName_);
    a.emplace(Conf::CONFIG_TURN_SERVER_PWD, turnServerPwd_);
    a.emplace(Conf::CONFIG_TURN_SERVER_REALM, turnServerRealm_);

    a.emplace(Conf::CONFIG_TLS_CA_LIST_FILE,        tlsCaListFile_);
    a.emplace(Conf::CONFIG_TLS_CERTIFICATE_FILE,    tlsCertificateFile_);
    a.emplace(Conf::CONFIG_TLS_PRIVATE_KEY_FILE,    tlsPrivateKeyFile_);
    a.emplace(Conf::CONFIG_TLS_PASSWORD,            tlsPassword_);

    return a;
}

std::map<std::string, std::string>
SIPAccountBase::getVolatileAccountDetails() const
{
    auto a = Account::getVolatileAccountDetails();

    // replace value from Account for IP2IP
    if (isIP2IP())
        a[Conf::CONFIG_ACCOUNT_REGISTRATION_STATUS] = "READY";

    a.emplace(Conf::CONFIG_TRANSPORT_STATE_CODE,    ring::to_string(transportStatus_));
    a.emplace(Conf::CONFIG_TRANSPORT_STATE_DESC,    transportError_);
    return a;
}

auto
SIPAccountBase::getPortsReservation() noexcept -> decltype(getPortsReservation())
{
    // Note: static arrays are zero-initialized
    static std::remove_reference<decltype(getPortsReservation())>::type portsInUse;
    return portsInUse;
}

uint16_t
SIPAccountBase::getRandomEvenPort(const std::pair<uint16_t, uint16_t>& range) const
{
    std::uniform_int_distribution<uint16_t> dist(range.first/2, range.second/2);
    uint16_t result;
    do {
        result = 2 * dist(rand_);
    } while (getPortsReservation()[result / 2]);
    return result;
}

uint16_t
SIPAccountBase::acquireRandomEvenPort(const std::pair<uint16_t, uint16_t>& range) const
{
    std::uniform_int_distribution<uint16_t> dist(range.first/2, range.second/2);
    uint16_t result;

    do {
        result = 2 * dist(rand_);
    } while (getPortsReservation()[result / 2]);

    getPortsReservation()[result / 2] = true;
    return result;
}

uint16_t
SIPAccountBase::acquirePort(uint16_t port)
{
    getPortsReservation()[port / 2] = true;
    return port;
}

void
SIPAccountBase::releasePort(uint16_t port) noexcept
{
    getPortsReservation()[port / 2] = false;
}

uint16_t
SIPAccountBase::generateAudioPort() const
{
    return acquireRandomEvenPort(audioPortRange_);
}

#ifdef RING_VIDEO
uint16_t
SIPAccountBase::generateVideoPort() const
{
    return acquireRandomEvenPort(videoPortRange_);
}
#endif

const IceTransportOptions
SIPAccountBase::getIceOptions() const noexcept
{
    auto opts = Account::getIceOptions();
    if (stunEnabled_)
        opts.stunServer = stunServer_;
    if (turnEnabled_) {
        opts.turnServer = turnServer_;
        opts.turnServerUserName = turnServerUserName_;
        opts.turnServerPwd = turnServerPwd_;
        opts.turnServerRealm = turnServerRealm_;
    }
    return opts;
}

void
SIPAccountBase::onTextMessage(const std::string& from,
                              const std::map<std::string, std::string>& payloads)
{
    RING_DBG("Text message received from %s, %zu part(s)",  from.c_str(), payloads.size());
    emitSignal<DRing::ConfigurationSignal::IncomingAccountMessage>(accountID_, from, payloads);
}

void
SIPAccountBase::setPublishedAddress(const IpAddr& ip_addr)
{
    publishedIp_ = ip_addr;
    publishedIpAddress_ = ip_addr.toString();
    RING_DBG("[Account %s] Using public address %s", getAccountID().c_str(),
             publishedIpAddress_.c_str());
}

} // namespace ring
