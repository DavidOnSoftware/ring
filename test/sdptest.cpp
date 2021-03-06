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
 * ebail - 2015/02/18
 * unit test is based on old SDP manager code
 * this test is disabled for the moment
 * */
#if 0
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "sdptest.h"
#include <iostream>
#include <cstring>

#include "audio/codecs/audiocodec.h"

namespace ring { namespace test {

enum session_type {
    REMOTE_OFFER,
    LOCAL_OFFER,
};

static const char *sdp_answer1 = "v=0\r\n"
                                 "o=bob 2890844730 2890844730 IN IP4 host.example.com\r\n"
                                 "s= \r\n"
                                 "c=IN IP4 host.example.com\r\n"
                                 "t=0 0\r\n"
                                 "m=audio 49920 RTP/AVP 0\r\n"
                                 "a=rtpmap:0 PCMU/8000\r\n"
                                 "m=video 53002 RTP/AVP 32\r\n"
                                 "a=rtpmap:32 MPV/90000\r\n";

static const char *sdp_offer1 = "v=0\r\n"
                                "o=bob 2890844730 2890844730 IN IP4 host.example.com\r\n"
                                "s= \r\n"
                                "c=IN IP4 host.example.com\r\n"
                                "t=0 0\r\n"
                                "m=audio 49920 RTP/AVP 0\r\n"
                                "a=rtpmap:0 PCMU/8000\r\n"
                                "m=video 53002 RTP/AVP 32\r\n"
                                "a=rtpmap:32 MPV/90000\r\n";

static const char *sdp_answer2 = "v=0\r\n"
                                 "o=bob 2890844730 2890844730 IN IP4 host.example.com\r\n"
                                 "s= \r\n"
                                 "c=IN IP4 host.example.com\r\n"
                                 "t=0 0\r\n"
                                 "m=audio 49920 RTP/AVP 3 97 9\r\n"
                                 "a=rtpmap:3 GSM/8000\r\n"
                                 "a=rtpmap:97 iLBC/8000\r\n"
                                 "a=rtpmap:9 G722/8000\r\n"
                                 "m=video 53002 RTP/AVP 32\r\n"
                                 "a=rtpmap:32 MPV/90000\r\n";

static const char *sdp_offer2 = "v=0\r\n"
                                "o=bob 2890844730 2890844730 IN IP4 host.example.com\r\n"
                                "s= \r\n"
                                "c=IN IP4 host.example.com\r\n"
                                "t=0 0\r\n"
                                "m=audio 49920 RTP/AVP 3 97 9\r\n"
                                "a=rtpmap:3 GSM/8000\r\n"
                                "a=rtpmap:97 iLBC/8000\r\n"
                                "a=rtpmap:9 G722/8000\r\n"
                                "m=video 53002 RTP/AVP 32\r\n"
                                "a=rtpmap:32 MPV/90000\r\n";

static const char *sdp_reinvite = "v=0\r\n"
                                  "o=bob 2890844730 2890844730 IN IP4 host.example.com\r\n"
                                  "s= \r\n"
                                  "c=IN IP4 host.exampleReinvite.com\r\n"
                                  "t=0 0\r\n"
                                  "m=audio 42445 RTP/AVP 0\r\n"
                                  "a=rtpmap:0 PCMU/8000\r\n"
                                  "m=video 53002 RTP/AVP 32\r\n"
                                  "a=rtpmap:32 MPV/90000\r\n";

static const char *const LOCALHOST = "127.0.0.1";

void SDPTest::setUp()
{
    session_.reset(new Sdp("123456789"));
}

void SDPTest::tearDown()
{
    session_.reset();
}

void SDPTest::receiveAnswerAfterInitialOffer(const pjmedia_sdp_session* remote)
{
    CPPUNIT_ASSERT(pjmedia_sdp_neg_get_state(session_->negotiator_) == PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER);
    CPPUNIT_ASSERT(pjmedia_sdp_neg_set_remote_answer(session_->memPool_.get(), session_->negotiator_, remote) == PJ_SUCCESS);
    CPPUNIT_ASSERT(pjmedia_sdp_neg_get_state(session_->negotiator_) == PJMEDIA_SDP_NEG_STATE_WAIT_NEGO);
}

namespace {
std::vector<std::map<std::string, std::string> >
    createVideoCodecs() {
        std::vector<std::map<std::string, std::string> > videoCodecs;
#ifdef RING_VIDEO
        std::map<std::string, std::string> codec;
        codec["name"] = "H264";
        codec["enabled"] = "true";
        videoCodecs.push_back(codec);
        codec["name"] = "H263";
        videoCodecs.push_back(codec);
#endif
        return videoCodecs;
    }
}

void SDPTest::testInitialOfferFirstCodec()
{
    std::cout << "------------ SDPTest::testInitialOfferFirstCodec --------------" << std::endl;

    CPPUNIT_ASSERT(session_->getPublishedIP().empty());
    CPPUNIT_ASSERT(session_->getRemoteIP().empty());

    std::vector<int> codecSelection;

    codecSelection.push_back(PAYLOAD_CODEC_ULAW);
    codecSelection.push_back(PAYLOAD_CODEC_ALAW);
    codecSelection.push_back(PAYLOAD_CODEC_G722);

    std::vector<std::map<std::string, std::string> > videoCodecs(createVideoCodecs());

    session_->setPublishedIP(LOCALHOST);
    session_->setLocalPublishedAudioPort(49567);

    session_->createOffer(codecSelection, videoCodecs);

    pjmedia_sdp_session *remoteAnswer;
    pjmedia_sdp_parse(session_->memPool_.get(), (char*) sdp_answer1, strlen(sdp_answer1), &remoteAnswer);

    receiveAnswerAfterInitialOffer(remoteAnswer);
    session_->startNegotiation();

    session_->setMediaTransportInfoFromRemoteSdp();

    CPPUNIT_ASSERT(session_->getPublishedIP() == LOCALHOST);
    CPPUNIT_ASSERT(session_->getRemoteIP() == "host.example.com");
}

void SDPTest::testInitialAnswerFirstCodec()
{
    std::cout << "------------ SDPTest::testInitialAnswerFirstCodec -------------" << std::endl;

    CPPUNIT_ASSERT(session_->getPublishedIP().empty());
    CPPUNIT_ASSERT(session_->getRemoteIP().empty());

    std::vector<int> codecSelection;
    pjmedia_sdp_session *remoteOffer;

    codecSelection.push_back(PAYLOAD_CODEC_ULAW);
    codecSelection.push_back(PAYLOAD_CODEC_ALAW);
    codecSelection.push_back(PAYLOAD_CODEC_G722);

    pjmedia_sdp_parse(session_->memPool_.get(), (char*) sdp_offer1, strlen(sdp_offer1), &remoteOffer);

    session_->setPublishedIP(LOCALHOST);
    session_->setLocalPublishedAudioPort(49567);

    session_->receiveOffer(remoteOffer, codecSelection, createVideoCodecs());

    session_->startNegotiation();

    session_->setMediaTransportInfoFromRemoteSdp();

    CPPUNIT_ASSERT(session_->getPublishedIP() == LOCALHOST);
    CPPUNIT_ASSERT(session_->getRemoteIP() == "host.example.com");
}

void SDPTest::testInitialOfferLastCodec()
{
    std::cout << "------------ SDPTest::testInitialOfferLastCodec --------------------" << std::endl;

    CPPUNIT_ASSERT(session_->getPublishedIP().empty());
    CPPUNIT_ASSERT(session_->getRemoteIP().empty());

    std::vector<int> codecSelection;

    codecSelection.push_back(PAYLOAD_CODEC_ULAW);
    codecSelection.push_back(PAYLOAD_CODEC_ALAW);
    codecSelection.push_back(PAYLOAD_CODEC_G722);

    session_->setPublishedIP(LOCALHOST);
    session_->setLocalPublishedAudioPort(49567);

    session_->createOffer(codecSelection, createVideoCodecs());

    pjmedia_sdp_session *remoteAnswer;
    pjmedia_sdp_parse(session_->memPool_.get(), (char*) sdp_answer2, strlen(sdp_answer2), &remoteAnswer);

    receiveAnswerAfterInitialOffer(remoteAnswer);
    session_->startNegotiation();

    session_->setMediaTransportInfoFromRemoteSdp();

    CPPUNIT_ASSERT(session_->getPublishedIP() == LOCALHOST);
    CPPUNIT_ASSERT(session_->getRemoteIP() == "host.example.com");
}

void SDPTest::testInitialAnswerLastCodec()
{
    std::cout << "------------ SDPTest::testInitialAnswerLastCodec ------------" << std::endl;

    CPPUNIT_ASSERT(session_->getPublishedIP().empty());
    CPPUNIT_ASSERT(session_->getRemoteIP().empty());

    std::vector<int> codecSelection;
    pjmedia_sdp_session *remoteOffer;

    codecSelection.push_back(PAYLOAD_CODEC_ULAW);
    codecSelection.push_back(PAYLOAD_CODEC_ALAW);
    codecSelection.push_back(PAYLOAD_CODEC_G722);

    pjmedia_sdp_parse(session_->memPool_.get(), (char*)sdp_offer2, strlen(sdp_offer2), &remoteOffer);

    session_->setPublishedIP(LOCALHOST);
    session_->setLocalPublishedAudioPort(49567);

    session_->receiveOffer(remoteOffer, codecSelection, createVideoCodecs());

    session_->startNegotiation();

    session_->setMediaTransportInfoFromRemoteSdp();

    CPPUNIT_ASSERT(session_->getPublishedIP() == LOCALHOST);
    CPPUNIT_ASSERT(session_->getRemoteIP() == "host.example.com");
}


void SDPTest::testReinvite()
{
    std::cout << "------------ SDPTest::testReinvite --------------------" << std::endl;

    CPPUNIT_ASSERT(session_->getPublishedIP().empty());
    CPPUNIT_ASSERT(session_->getRemoteIP().empty());

    std::vector<int> codecSelection;
    codecSelection.push_back(PAYLOAD_CODEC_ULAW);
    codecSelection.push_back(PAYLOAD_CODEC_ALAW);
    codecSelection.push_back(PAYLOAD_CODEC_G722);

    session_->setPublishedIP(LOCALHOST);
    session_->setLocalPublishedAudioPort(49567);

    std::vector<std::map<std::string, std::string> > videoCodecs(createVideoCodecs());
    session_->createOffer(codecSelection, videoCodecs);

    pjmedia_sdp_session *remoteAnswer;
    // pjmedia_sdp_parse(session_->memPool_.get(), test[0].offer_answer[0].sdp2, strlen(test[0].offer_answer[0].sdp2), &remoteAnswer);
    pjmedia_sdp_parse(session_->memPool_.get(), (char*) sdp_answer1, strlen(sdp_answer1), &remoteAnswer);

    receiveAnswerAfterInitialOffer(remoteAnswer);
    session_->startNegotiation();

    session_->setMediaTransportInfoFromRemoteSdp();

    CPPUNIT_ASSERT(session_->getPublishedIP() == LOCALHOST);
    CPPUNIT_ASSERT(session_->getRemoteIP() == "host.example.com");
    std::vector<ring::AudioCodec*> codecs(session_->getSessionAudioMedia());
    ring::AudioCodec *codec = codecs[0];
    CPPUNIT_ASSERT(codec and codec->getMimeSubtype() == "PCMU");

    pjmedia_sdp_session *reinviteOffer;
    pjmedia_sdp_parse(session_->memPool_.get(), (char*) sdp_reinvite, strlen(sdp_reinvite), &reinviteOffer);
    session_->receiveOffer(reinviteOffer, codecSelection, videoCodecs);

    session_->startNegotiation();
    session_->setMediaTransportInfoFromRemoteSdp();

    CPPUNIT_ASSERT(session_->getRemoteIP() == "host.exampleReinvite.com");
}

}} // namespace ring::test
#endif
