/*
 * Copyright (C) 2017 Omnissa, LLC. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-3.0
 *
 */

#ifdef _DEBUG_

#include <gtest/gtest.h>
#include "base/Error.h"
#include "base/Protocol.h"
#include "packet/UploadPacketParser.h"

static IPacketParser *parser = NULL;
static IPacketCreator *creator = NULL;
static UploadProcessor *processor = NULL;
static std::string request_id="1234";

static std::string un = "test";
static std::string pass = "password";
static std::string server = "example.com";
static std::string wg = "workgroup";

TEST(UploadParser, Setup)
{
    processor = ALLOCATE(UploadProcessor);
    EXPECT_EQ(SMB_SUCCESS, processor->Init(request_id));
    parser = processor->PacketParser();
    creator = processor->PacketCreator();
    EXPECT_TRUE(processor != NULL);
    EXPECT_TRUE(parser != NULL);
    EXPECT_TRUE(creator != NULL);
    RequestProcessor::SetInstance(processor);
    processor->SetUrl(server);
    processor->SetUserName(un);
    processor->SetPassword(pass);
    processor->SetWorkGroup(wg);
}

TEST(UploadParser, ParsePacket)
{
    std::string wrong_request_id = "2345";
    packet_data data;
    data.download_upload_data.payload = ALLOCATE_ARR(char, 100);
    data.download_upload_data.payload_len = 100;
    Packet *packet = ALLOCATE(Packet);
    EXPECT_EQ(SMB_SUCCESS, creator->CreatePacket(packet, UPLOAD_INIT_REQ, NULL));
    RequestProcessor::GetInstance()->SetRequestId(wrong_request_id);
    EXPECT_EQ(SMB_ERROR, parser->ParsePacket(packet));
    RequestProcessor::GetInstance()->SetRequestId(request_id);
    EXPECT_EQ(SMB_SUCCESS, creator->CreatePacket(packet, UPLOAD_INIT_REQ, NULL));
    EXPECT_EQ(SMB_SUCCESS, parser->ParsePacket(packet));
    EXPECT_EQ(SMB_SUCCESS, creator->CreateStatusPacket(packet, UPLOAD_INIT_RESP, 0));
    EXPECT_EQ(SMB_SUCCESS, parser->ParsePacket(packet));
    EXPECT_EQ(SMB_SUCCESS, creator->CreatePacket(packet, UPLOAD_DATA_REQ, &data));
    EXPECT_EQ(SMB_SUCCESS, parser->ParsePacket(packet));
    FREE_ARR(data.download_upload_data.payload);
    EXPECT_EQ(SMB_SUCCESS, creator->CreatePacket(packet, UPLOAD_END_REQ, NULL));
    EXPECT_EQ(SMB_SUCCESS, parser->ParsePacket(packet));
    EXPECT_EQ(SMB_SUCCESS, creator->CreateStatusPacket(packet, UPLOAD_ERROR, 0));
    EXPECT_EQ(SMB_SUCCESS, parser->ParsePacket(packet));
}

TEST(UploadParser, TearDown)
{
    RequestProcessor::SetInstance(NULL);
    processor->Quit();
    FREE(processor);
}

#endif //_DEBUG_

