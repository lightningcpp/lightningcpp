/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string>
#include <fstream>

#include "../include/http/httpclient.h"
#include "../include/http/httpserver.h"
#include "../include/http/server.h"

#include "../include/http/mod/http.h"
#include "../include/http/mod/match.h"
#include "../include/http/mod/exec.h"

#include "testutils.h"

#include <gtest/gtest.h>

namespace http {

TEST ( HttpClientTest, SSL_Request ) {

    HttpClient< Https > client_ ( "www.google.com", "https" );
    Request request_ ( "/images/branding/googlelogo/1x/googlelogo_color_150x54dp.png" );
    request_.version_minor ( 0 );

    std::ofstream _sstream ( "/tmp/google-logo.png" );
    Response _response = client_.get ( request_, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 3170U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
}

TEST ( HttpClientTest, Simple_SSL_Request ) {

    std::ofstream _sstream ( "/tmp/google-logo.png" );
    Response _response = http::get ( "https://www.google.com//images/branding/googlelogo/1x/googlelogo_color_150x54dp.png", _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 3170U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
}

TEST ( HttpClientTest, Amazon_SSL_Request ) {

    std::ofstream _sstream ( "/tmp/cover.jpg" );
    Response _response = http::get ( "https://images-na.ssl-images-amazon.com/images/I/51QRG0GOwYL.jpg", _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 52741U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
}

TEST ( HttpClientTest, post ) {

    //create the server
    bool _call_back_called = false;
    HttpServer _server ( "127.0.0.1", "9721", [&_call_back_called] ( Request & request, Response & response ) {
        _call_back_called = true;
        EXPECT_EQ ( "POST", request.method() );
        EXPECT_EQ ( "/index.html", request.uri() );
        EXPECT_EQ ( "HTTP", request.protocol() );
        EXPECT_EQ ( 1, request.version_major() );
        EXPECT_EQ ( 1, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );
        EXPECT_EQ ( std::to_string ( strlen (  "content bla bla..." ) ), request.parameter ( http::header::CONTENT_LENGTH ) );
        EXPECT_EQ ( "content bla bla...", request.str() );

        response << "1\n22\n333\n4444\n55555\n";

        response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
        response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
    } );

    std::stringstream _sstream;

    HttpClient< Http > _client ( "127.0.0.1", "9721" );
    Request _request ( "/index.html" );
    _request.method ( "POST" );
    _request << "content bla bla...";
    EXPECT_EQ ( http::http_status::OK, _client.get ( _request, _sstream ).status() );
}

TEST ( HttpClientTest, file_post ) {

    //create the server
    bool _call_back_called = false;
    HttpServer _server ( "127.0.0.1", "9721", [&_call_back_called] ( Request & request, Response & response ) {
        _call_back_called = true;
        EXPECT_EQ ( "POST", request.method() );
        EXPECT_EQ ( "/index.html", request.uri() );
        EXPECT_EQ ( "HTTP", request.protocol() );
        EXPECT_EQ ( 1, request.version_major() );
        EXPECT_EQ ( 1, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );
        EXPECT_EQ ( "641024", request.parameter ( http::header::CONTENT_LENGTH ) );
        EXPECT_EQ ( 641024, request.tellp() );
        std::ifstream _file ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );
        EXPECT_TRUE ( compare_streams ( _file, request ) );

        response << "1\n22\n333\n4444\n55555\n";

        response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
        response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
    } );

    std::stringstream _sstream;
    std::ifstream _file ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );

    HttpClient< Http > _client ( "127.0.0.1", "9721" );
    Request _request ( "/index.html" );
    _request.method ( "POST" );
    char * buffer = new char [1024];
    _file.read ( buffer, 1024 );

    while ( _file.good() ) {
        _request.write ( buffer, _file.gcount() );
        _file.read ( buffer, 1024 );
    }

    delete[] buffer;
    auto _response = _client.get ( _request, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
}

TEST ( HttpClientTest, notify_post ) {

    //create the server
    bool _call_back_called = false;
    HttpServer _server ( "127.0.0.1", "9721", [&_call_back_called] ( Request & request, Response & response ) {
        _call_back_called = true;
        EXPECT_EQ ( "NOTIFY", request.method() );
        EXPECT_EQ ( "/index.html", request.uri() );
        EXPECT_EQ ( "HTTP", request.protocol() );
        EXPECT_EQ ( 1, request.version_major() );
        EXPECT_EQ ( 1, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );
        const char* _content = R"xml(<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0" xmlns:s="urn:schemas-upnp-org:service:ConnectionManager:1"><e:property><SourceProtocolInfo>http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_50_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_60_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AAC_MULT5,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AC3,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_520,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_AAC_940,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L31_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L32_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_HP_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_1080i_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_720p_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_VGA_AAC,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AAC,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AMR,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL_MULT5,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_MULT5_ISO,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:image/jpeg:*,http-get:*:video/avi:*,http-get:*:video/divx:*,http-get:*:video/x-matroska:*,http-get:*:video/mpeg:*,http-get:*:video/mp4:*,http-get:*:video/x-ms-wmv:*,http-get:*:video/x-msvideo:*,http-get:*:video/x-flv:*,http-get:*:video/x-tivo-mpeg:*,http-get:*:video/quicktime:*,http-get:*:audio/mp4:*,http-get:*:audio/x-wav:*,http-get:*:audio/x-flac:*,http-get:*:application/ogg:*</SourceProtocolInfo></e:property><e:property><SinkProtocolInfo></SinkProtocolInfo></e:property><e:property><CurrentConnectionIDs>0</CurrentConnectionIDs></e:property></e:propertyset>)xml";
        EXPECT_EQ ( std::to_string ( strlen ( _content ) ), request.parameter ( http::header::CONTENT_LENGTH ) );
        EXPECT_EQ ( strlen ( _content ), static_cast< size_t > ( request.tellp() ) );
        EXPECT_EQ ( _content, request.str() );

        response << "1\n22\n333\n4444\n55555\n";

        response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
        response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
    } );

    std::stringstream _sstream;
    std::ifstream _file ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );

    HttpClient< Http > _client ( "127.0.0.1", "9721" );
    Request _request ( "/index.html" );
    _request.method ( "NOTIFY" );
    const char* _content = R"xml(<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0" xmlns:s="urn:schemas-upnp-org:service:ConnectionManager:1"><e:property><SourceProtocolInfo>http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_50_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_60_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AAC_MULT5,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AC3,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_520,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_AAC_940,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L31_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L32_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_HP_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_1080i_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_720p_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_VGA_AAC,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AAC,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AMR,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL_MULT5,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_MULT5_ISO,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:image/jpeg:*,http-get:*:video/avi:*,http-get:*:video/divx:*,http-get:*:video/x-matroska:*,http-get:*:video/mpeg:*,http-get:*:video/mp4:*,http-get:*:video/x-ms-wmv:*,http-get:*:video/x-msvideo:*,http-get:*:video/x-flv:*,http-get:*:video/x-tivo-mpeg:*,http-get:*:video/quicktime:*,http-get:*:audio/mp4:*,http-get:*:audio/x-wav:*,http-get:*:audio/x-flac:*,http-get:*:application/ogg:*</SourceProtocolInfo></e:property><e:property><SinkProtocolInfo></SinkProtocolInfo></e:property><e:property><CurrentConnectionIDs>0</CurrentConnectionIDs></e:property></e:propertyset>)xml";
    _request << _content;
    auto _response = _client.get ( _request, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
}

TEST ( HttpClientTest, SimpleRequestV1_0 ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind ( http::mod::Match<> ( "/foo/bar" ), http::mod::Exec ( [] ( Request&, Response & response ) {
        response << "1\n22\n333\n4444\n55555\n";
        return http::http_status::OK;
    } ), http::mod::Http() );

    std::string _expected = "1\n22\n333\n4444\n55555\n";
    HttpClient< Http > _client ( "127.0.0.1", "9999" );
    Request _request ( "/foo/bar" );
    _request.version_minor ( 0 );

    std::stringstream _sstream;
    auto _response = _client.get ( _request, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 0, _response.version_minor() );
    EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
    EXPECT_EQ ( _expected, _sstream.str() );
}

TEST ( HttpClientTest, SimpleRequestV1_1 ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind ( http::mod::Match<> ( "/foo/bar" ), http::mod::Exec ( [] ( Request&, Response & response ) {
        response << "1\n22\n333\n4444\n55555\n";
        return http::http_status::OK;
    } ), http::mod::Http() );

    std::string _expected = "1\n22\n333\n4444\n55555\n";
    HttpClient< Http > _client ( "127.0.0.1", "9999" );
    Request _request ( "/foo/bar" );
    _request.version_minor ( 1 );

    std::stringstream _sstream;
    auto _response = _client.get ( _request, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 1, _response.version_minor() );
    EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
    EXPECT_EQ ( _expected, _sstream.str() );
}

TEST ( HttpClientTest, PersistentRequestV1_1 ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind ( http::mod::Match<> ( "/foo/bar" ), http::mod::Exec ( [] ( Request&, Response & response ) {
        response << "1\n22\n333\n4444\n55555\n";
        return http::http_status::OK;
    } ), http::mod::Http() );

    std::string _expected = "1\n22\n333\n4444\n55555\n";

    HttpClient< Http > client_ ( "localhost", "9999" );
    {   //request one
        Request request_ ( "/foo/bar" );
        std::stringstream _sstream;
        Response _response = client_.get ( request_, _sstream );
        EXPECT_EQ ( http_status::OK, _response.status() );
        EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
        EXPECT_EQ ( _expected, _sstream.str() );
    }
    {   //request two
        Request request_ ( "/foo/bar" );
        std::stringstream _sstream;
        Response _response = client_.get ( request_, _sstream );
        EXPECT_EQ ( http_status::OK, _response.status() );
        EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
        EXPECT_EQ ( _expected, _sstream.str() );
    }
}

TEST ( HttpClientTest, Request404 ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind ( http::mod::Match<> ( "/foo/bar" ), http::mod::Exec ( [] ( Request&, Response & response ) {
        response << "1\n22\n333\n4444\n55555\n";
        return http::http_status::OK;
    } ), http::mod::Http() );

    HttpClient< Http > _client ( "127.0.0.1", "9999" );
    Request _request ( "/bar/foo" );

    std::stringstream _sstream;
    auto _response = _client.get ( _request, _sstream );
    EXPECT_EQ ( http_status::NOT_FOUND, _response.status() );
}

TEST ( HttpClientTest, DISABLED_ChunkedEncoding ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind ( http::mod::Match<> ( "/foo/bar" ), http::mod::Exec ( [] ( Request&, Response & response ) {
        response << R"bin(4\r\n
Wiki\r\n
5\r\n
pedia\r\n
E\r\n
 in\r\n
\r\n
chunks.\r\n
0\r\n
\r\n)bin";

        response.parameter (  header::TRANSFER_ENCODING, "chunked" );

        return http::http_status::OK;
    } ), http::mod::Http() );

    HttpClient< Http > _client ( "127.0.0.1", "9999" );
    Request _request ( "/foo/bar" );

    std::stringstream _sstream;
    auto _response = _client.get ( _request, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_TRUE ( _response.contains_parameter ( http::header::TRANSFER_ENCODING ) );
    EXPECT_EQ ( R"bin(Wikipedia in

chunks.)bin", _sstream.str() );
}

}//namespace http
