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

#ifndef home_page
#define home_page "index.htm"
#endif

//#define uHTTP_DEBUG   //unmute to activate serial debugger
#ifdef uHTTP_DEBUG
 #define uHTTP_PRINT(x)  Serial.print (x); response->print(x);
 #define uHTTP_PRINTLN(x) Serial.println (x); response->println(x);
#else
 #define uHTTP_PRINT(x)     response->print(x)
 #define uHTTP_PRINTLN(x)   response->println(x)
#endif

//#define uHTTP_BODY_DEBUG      //unmute to activate serial debugger
#ifdef uHTTP_BODY_DEBUG
 #define uHTTP_BODY_PRINT(x)  Serial.print (x);
 #define uHTTP_BODY_PRINTLN(x) Serial.println (x);
#else
 #define uHTTP_BODY_PRINT(x)
 #define uHTTP_BODY_PRINTLN(x)
#endif

// Sizes
#define uHTTP_BUFFER_SIZE    255
#define uHTTP_METHOD_SIZE    8
#define uHTTP_URI_SIZE       64
#define uHTTP_QUERY_SIZE     64
#define uHTTP_AUTH_SIZE      32
#define uHTTP_TYPE_SIZE      34
#define uHTTP_ORIG_SIZE      16
// #define uHTTP_HOST_SIZE      32
#define uHTTP_BODY_SIZE      511
/*
#define uHTTP_METHOD_OPTIONS 0
#define uHTTP_METHOD_GET     1
#define uHTTP_METHOD_HEAD    2
#define uHTTP_METHOD_POST    3
#define uHTTP_METHOD_PUT     4
#define uHTTP_METHOD_PATCH   5
#define uHTTP_METHOD_DELETE  6
#define uHTTP_METHOD_TRACE   7
#define uHTTP_METHOD_CONNECT 8*/
enum method_t {OPTIONS, GET, HEAD, POST, PUT, PATCH, DELETE, TRACE, CONNECT};

/*
#define TEXT_PLAIN  0
#define TEXT_HTML   1
#define TEXT_JS     2
#define TEXT_CSS    3
#define TEXT_XML    4
#define TEXT_JSON   5
#define X_ICON      6*/
enum content_t {TEXT_PLAIN, TEXT_HTML, TEXT_JS, TEXT_CSS, TEXT_XML, TEXT_JSON, X_ICON};

const uint8_t containerSize = 10;

typedef struct header_t{
    char type[uHTTP_TYPE_SIZE];
    char auth[uHTTP_AUTH_SIZE];
    char orig[uHTTP_ORIG_SIZE];
    //char host[uHTTP_HOST_SIZE];
    uint16_t length;
};

/** 
 * uHTTP request obj container 
 **/

class uHTTP_request {
public:
  uHTTP_request( const char *uId, void (*uCallback)() );
  const char *id;
  void (*callback)();
};

class uHTTP : public EthernetServer {
    private:
        header_t __head;
        method_t __method;
        char *__uri;
        char *__query;
        const char *parse(const char *needle, char *haystack, const char*sep);
        EthernetClient *response;

        uHTTP_request *container_Get;
        uHTTP_request *container_Put;
        uint8_t sizeGetContainer = 0;
        uint8_t sizePutContainer = 0;
                      
    public:

        char *__body;
        uHTTP();
        uHTTP(uint16_t port);
        ~uHTTP();

        void uHTTPclient(EthernetClient *response);
        EthernetClient available();

        void requestHandler();

        header_t head();

        method_t method();
        bool method(method_t type);

        const char *uri();
        const char *uri(uint8_t segment);
        bool uri(const char *uri);
        bool uri(uint8_t index, const char *uri);
        
        const char *query();
        const char *query(const char *key);

        const char *body();
        const char *data(const char *key);

        bool webFile_Post(char url[32]);
        bool webFile_Post_Head(char url[32]);
        void render(uint16_t code, content_t ctype);
        void render(uint16_t code, const char *body);
        void send_headers(uint16_t code);
        void send_headers(uint16_t code , content_t ctype);

        void send_body(const char *body);
        void send_method_headers(const char *uri);

        bool get_request(const char *uri);
        bool put_request(const char *uri);

        void post_JSON(String *output);
        void send_JSON_headers();

        void addToContainer(method_t method, uHTTP_request *container, uint8_t sizeArray);        
};

#endif
