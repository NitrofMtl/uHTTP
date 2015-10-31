// This file is part of uHTTP.
//
// uHTTP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// uHTTP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

#ifndef uHTTP_H
#define uHTTP_H

#if (ARDUINO >= 100)
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "EthernetClient.h"
#include "EthernetServer.h"
#include <SD.h>
// #define uHTTP_DEBUG

// Sizes
#define uHTTP_BUFFER_SIZE    255
#define uHTTP_METHOD_SIZE    8
#define uHTTP_URI_SIZE       64
#define uHTTP_QUERY_SIZE     64
#define uHTTP_AUTH_SIZE      32
#define uHTTP_TYPE_SIZE      34
#define uHTTP_ORIG_SIZE      16
// #define uHTTP_HOST_SIZE      32
#define uHTTP_BODY_SIZE      255

#define uHTTP_METHOD_OPTIONS 0
#define uHTTP_METHOD_GET     1
#define uHTTP_METHOD_HEAD    2
#define uHTTP_METHOD_POST    3
#define uHTTP_METHOD_PUT     4
#define uHTTP_METHOD_PATCH   5
#define uHTTP_METHOD_DELETE  6
#define uHTTP_METHOD_TRACE   7
#define uHTTP_METHOD_CONNECT 8


#define TEXT_PLAIN  0
#define TEXT_HTML   1
#define TEXT_JS     2
#define TEXT_CSS    3
#define TEXT_XML    4
#define TEXT_JSON   5
#define X_ICON      6

typedef struct header_t{
    char type[uHTTP_TYPE_SIZE];
    char auth[uHTTP_AUTH_SIZE];
    char orig[uHTTP_ORIG_SIZE];
    //char host[uHTTP_HOST_SIZE];
    uint16_t length;
};

class uHTTP : public EthernetServer {
    private:


        header_t __head;

        uint8_t __method;

        char *__uri;
        char *__query;
        char *__body;
        const char *parse(const char *needle, char *haystack, const char*sep);
       
    public:
        uHTTP();
        uHTTP(uint16_t port);
        ~uHTTP();

        EthernetClient available();

        header_t head();

        uint8_t method();
        bool method(uint8_t type);

        const char *uri();
        const char *uri(uint8_t segment);
        bool uri(const char *uri);
        bool uri(uint8_t index, const char *uri);
        
        const char *query();
        const char *query(const char *key);

        const char *body();
        const char *data(const char *key);
        void webFile_Post(char url[32], EthernetClient response);
        void render(uint16_t code, uint8_t ctype , EthernetClient response);
        void render(uint16_t code, const char *body, EthernetClient response);
        void send_headers(uint16_t code , uint8_t ctype , EthernetClient response );
        void send_headers(uint16_t code, EthernetClient response);
        void send_body(const char *body, EthernetClient response);
        void send_method_headers(const char *uri, EthernetClient response);

        bool get_request(const char *uri, EthernetClient response);
        bool put_request(const char *uri, EthernetClient response);

        void post_JSON(String output, EthernetClient response);
        void send_JSON_headers(EthernetClient response);
    
};

#endif
