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

#if (ARDUINO >= 100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "uHTTP.h"

/**
 *	uHTTP constructor
 **/
 uHTTP::uHTTP() : EthernetServer(80){
   uHTTP(80);
 }

/**
 *	uHTTP constructor
 *
 *	@param uint16_t port
 **/
 uHTTP::uHTTP(uint16_t port) : EthernetServer(port){
   __uri = new char[uHTTP_URI_SIZE];
   __query = new char[uHTTP_QUERY_SIZE];
   __body = new char[uHTTP_BODY_SIZE];
 }

/**
 *	uHTTP destructor
 **/
 uHTTP::~uHTTP(){
   delete [] __uri;
   delete [] __query;
   delete [] __body;
 }

/**
 *	Process HTTP request
 *
 *	@return EthernetClient client
 **/
 EthernetClient uHTTP::available(){
  EthernetClient client;

  memset(__uri, 0, sizeof(__uri));
  memset(__query, 0, sizeof(__query));
  memset(__body, 0, sizeof(__body));
  memset(&__head, 0, sizeof(__head));

  if(client = EthernetServer::available()){
    uint8_t cursor = 0, cr = 0;
    char buffer[uHTTP_BUFFER_SIZE] = {0};
    bool sub = false;

    enum state_t {METHOD, URI, QUERY, PROTO, KEY, VALUE, BODY};
    state_t state = METHOD;

    enum header_t {START, AUTHORIZATION, CONTENT_TYPE, CONTENT_LENGTH, ORIGIN};
    header_t header = START;

    while(client.connected() && client.available()){
      char c = client.read();

      (c == '\r' || c == '\n') ? cr++ : cr = 0;
      //if (c == '\r') Serial.println(); //for debuging
      //Serial.print(c);                  //for debuging
      switch(state){
       case METHOD:
       if(c == ' '){
        if(strncmp(buffer, PSTR("OP"), 2) == 0) __method = uHTTP_METHOD_OPTIONS;
        else if(strncmp(buffer, PSTR("HE"), 2) == 0) __method = uHTTP_METHOD_HEAD;
        else if(strncmp(buffer, PSTR("PO"), 2) == 0) __method = uHTTP_METHOD_POST;
        else if(strncmp(buffer, PSTR("PU"), 2) == 0) __method = uHTTP_METHOD_PUT;
        else if(strncmp(buffer, PSTR("PA"), 2) == 0) __method = uHTTP_METHOD_PATCH;
        else if(strncmp(buffer, PSTR("DE"), 2) == 0) __method = uHTTP_METHOD_DELETE;
        else if(strncmp(buffer, PSTR("TR"), 2) == 0) __method = uHTTP_METHOD_TRACE;
        else if(strncmp(buffer, PSTR("CO"), 2) == 0) __method = uHTTP_METHOD_CONNECT;
        else __method = uHTTP_METHOD_GET;
        state = URI; 
        cursor = 0; 
      }else if(cursor < uHTTP_METHOD_SIZE - 1){
       buffer[cursor++] = c; 
       buffer[cursor] = '\0'; 
     }
     break;
     case URI:
     if(c == ' '){
       state = PROTO;
       cursor = 0;
     }else if(c == '?'){
       state = QUERY;
       cursor = 0;
     }else if(cursor < uHTTP_URI_SIZE - 1){
       __uri[cursor++] = c;
       __uri[cursor] = '\0';
     }
     break;
     case QUERY:
     if(c == ' '){
       state = PROTO;
       cursor = 0;
     }else if(cursor < uHTTP_QUERY_SIZE - 1){
       __query[cursor++] = c;
       __query[cursor] = '\0';
     }
     break;
     case PROTO:
     if(cr == 2){ state = KEY; cursor = 0; }
     break;
     case KEY:
     if (cr == 4){ state = BODY; cursor = 0; }
     else if(c == ' '){ state = VALUE; cursor = 0; }
     else if(c != ':' && cursor < uHTTP_BUFFER_SIZE){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
     break;
     case VALUE:
     if(cr == 2){
       switch(header){
         case AUTHORIZATION:
         strncpy(__head.auth, buffer, uHTTP_AUTH_SIZE);
         break;
         case CONTENT_TYPE:
         strncpy(__head.type, buffer, uHTTP_TYPE_SIZE);
         break;
         case ORIGIN:
         strncpy(__head.orig, buffer, uHTTP_ORIG_SIZE);
         break;
         case CONTENT_LENGTH:
         __head.length = atoi(buffer);
         break;
         break;
       }
       state = KEY; header = START; cursor = 0; sub = false;
     }else if(c != '\r' && c!= '\n'){
       if(header == START){
         if(strncmp(buffer, PSTR("Auth"), 4) == 0) header = AUTHORIZATION;
         else if(strncmp(buffer, PSTR("Content-T"), 9) == 0) header = CONTENT_TYPE;
         else if(strncmp(buffer, PSTR("Content-L"), 9) == 0) header = CONTENT_LENGTH;
       }

              			// Fill buffer
       if(cursor < uHTTP_BUFFER_SIZE - 1){
         switch(header){
          case AUTHORIZATION:
          if(sub){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          else if(c == ' ') sub = true;
          break;
          case CONTENT_TYPE:
          case CONTENT_LENGTH:
          buffer[cursor++] = c; buffer[cursor] = '\0';
          break;
        }
      }
    }
    break;
    case BODY:
    if(cr == 2 || __head.length == 0) client.flush();
    else if(cursor < uHTTP_BODY_SIZE - 1){ __body[cursor++] = c; __body[cursor] = '\0'; }
    break;
  }
}
}
return client;
}

/**
 *	Get request method
 *
 *	@return uint8_t method
 **/
 uint8_t uHTTP::method(){
   return __method;
 }

/**
 *	Check if request method is equals to type passed
 *
 *	@param uint8_t type
 *	@return boolean
 **/
 bool uHTTP::method(uint8_t type){
   return (__method == type) ? true : false;
 }

/**
 *	Get request uri
 *
 *	@return *char uri
 **/
 const char *uHTTP::uri(){
   return __uri;
 }

/**
 *	Get request uri segment
 *
 *	@param uint8_t index
 *	@return *char segment
 **/
 const char *uHTTP::uri(uint8_t index){
  uint8_t slash = 0, cursor = 0;
  char buffer[uHTTP_BUFFER_SIZE] = {0};
  for(uint8_t i = 0; i < strlen(__uri); i++){
    if(__uri[i] == '/'){ slash++; }
    else if(slash == index){
      buffer[cursor++] = __uri[i];
      buffer[cursor] = '\0';
    }
  }
  return buffer;
}

/**
 *	Check if request uri is equas to passed uri
 *
 *	@param char *uri
 *	@return boolean
 **/
 bool uHTTP::uri(const char *uri){
   return (strcmp(this->uri(), uri) == 0) ? true : false;
 }



/**
 *	Check if request uri segment is equas to passed uri
 *
 *	@param uint8_t index
  *	@param char *uri
 *	@return boolean
 **/
 bool uHTTP::uri(uint8_t index, const char *uri){
   return (strcmp(this->uri(index), uri) == 0) ? true : false;
 }



/**
 *	Get request query string
 *
 *	@return *char query
 **/
 const char *uHTTP::query(){
   return __query;
 }


/**
 *	Get request headers
 *
 *	@return header_t header
 **/
 header_t uHTTP::head(){
   return __head;
 }

/**
 *	Get request body
 *
 *	@return *char body
 **/
 const char *uHTTP::body(){
   return __body;
 }

/**
 *  Get request data value by key
 *
 *  @return *char value
 **/
 const char *uHTTP::data(const char *key){
  return parse(key, __body, "&");
}


/**
 *  Find needle on haystack
 *
 *  @param const char *needle
 *  @param char *haystack
 *  @return *char value
 **/
 const char *uHTTP::parse(const char *needle, char *haystack, const char *sep){
  char *act, *sub, *ptr;
  char buffer[uHTTP_BUFFER_SIZE];
  strcpy(buffer, haystack);
  for(act = buffer; strncmp(sub, needle, strlen(needle)); act = NULL){
    sub = strtok_r(act, (const char *) sep, &ptr);
    if(sub == NULL) break;
  }
  return (sub) ? strchr(sub, '=') + 1 : NULL;
}


void uHTTP::webFile_Post( char url[32], EthernetClient response){

  if(SD.exists(url)){ 
    //Serial.println("file exsist"); //for debuggin
    File webFile = SD.open(url, FILE_READ);
    if(webFile){
      char *ext = strchr(url, '.') + 1;
      uint8_t ctype = TEXT_PLAIN;


      if(strcmp(ext, "js") == 0) ctype = TEXT_JS;
      else if(strcmp(ext, "mjs") == 0) ctype = TEXT_JS; // rename web library *.min.js to mjs to fit FAT format of sd card
      else if(strcmp(ext, "css") == 0) ctype = TEXT_CSS;
      else if(strcmp(ext, "htm") == 0) ctype = TEXT_HTML;
      else if(strcmp(ext, "xml") == 0) ctype = TEXT_XML;
      else if(strcmp(ext, "jsn") == 0) ctype = TEXT_JSON;
      else if(strcmp(ext,"ico") == 0) ctype = X_ICON;
      send_headers(200, ctype, response);

      while(webFile.available()) response.write(webFile.read());
      webFile.close();
    }
  }
}

// header system for file web file sending
void uHTTP::send_headers(uint16_t code, uint8_t ctype, EthernetClient response){
  response.print("HTTP/1.1 ");

  switch(code){
    case 200:
    response.println("200 OK");
    break;
    case 404:
       case 302:
            response.println("302 Found");
            break;
    response.println("404 Not Found");
    break;
    case 500:
    response.println("500 Internal Server Error");
    break;
  }

  response.print("Content-Type: ");
  switch(ctype){
    case TEXT_JS:
    response.println("text/javascript");
    break;
    case TEXT_CSS:
    response.println("text/css");
    break;
    case TEXT_HTML:
    response.println("text/html");
    break;
    case TEXT_XML:
    response.println("text/xml");
    break;
    case X_ICON:
    response.println("image/x-icon");
    break;
    case TEXT_JSON:
    response.println("text/json");
    break;
    default:
    response.println("text/plain");
    break;
  }
  response.println("Connection: close");
  response.println();
}




//method header selector for rest server
void uHTTP::send_method_headers(const char *uri, EthernetClient response){
  if(method(uHTTP_METHOD_GET))
    send_headers(302, response);
  else if(method(uHTTP_METHOD_OPTIONS)) 
    send_headers(200, response);
  else 
    send_headers(404, response);
}

//header for rest server
void uHTTP::send_headers(uint16_t code, EthernetClient response){
    header_t head = __head;
  //header_t head = __head;

  response.print("HTTP/1.1");
  response.print(" ");
  switch(code){
    case 200:
    response.println("200 OK");
    response.println("Content-Type: application/json");
    if(strlen(head.orig)){
      response.print("Access-Control-Allow-Origin: ");
      response.println(head.orig);
      response.println("Access-Control-Allow-Methods: GET,PUT");
      response.println("Access-Control-Allow-Headers: Authorization, Content-Type");
      response.println("Access-Control-Allow-Credentials: true");
      response.println("Access-Control-Max-Age: 1000");
    }
    break;
    case 204:
    response.println("204 OK");
    response.println("100 continue");
    break;
  }
}

// body for rest server
void send_body(const char *body, EthernetClient response){
  response.println(body);
}
    //to be checked added
void uHTTP::send_body(const char *body, EthernetClient response){
  response.println(body);
}

//render for rest server
void uHTTP::render(uint16_t code, uint8_t ctype, EthernetClient response){
  send_headers(code, ctype, response);
  response.println("<html lang=\"en\">");
  response.println("<head>");
  response.println("<meta http-equiv=\"Cache-control\" content=\"public\">");
  response.println("<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>");
  response.println("<style>");                
  response.println("body{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}");
  response.println(".container{text-align: center; display: table-cell; vertical-align: middle;}");
  response.println(".content {text-align: center; display: inline-block;}");
  response.println(".title{font-size: 96px; margin-bottom: 40px;}");
  response.println(".quote{font-size: 24px; font-weight: bold;}");
  response.println("</style>") ;
  response.println("<title>uHTTP</title>");
  response.println("</head>");
  response.println("<body>");
  response.println("<div class=\"container\">");
  response.println("<div class=\"content\">");
  switch(code){
    case 404:
    response.println("<div class=\"title\">404 Not Found</div>");
    response.println("<div class=\"quote\">Ooops! The requested page was not found!</div>");
    break;
    case 500:
    response.println("<div class=\"title\">500 Internal Server Error</div>");
    response.println("<div class=\"quote\">Ooops! Something goes wrong!</div>");
    break;
  }
  response.println("</div>");
  response.println("</div>");
  response.println("</body>");
  response.println("</html>");
}

void uHTTP::render(uint16_t code, const char *body, EthernetClient response){
  send_headers(code, response);
  if(body) send_body(body, response);
} 


//get request
bool uHTTP::get_request( const char *uri, EthernetClient response){
  if((method(uHTTP_METHOD_GET) && (strcmp(this->uri(1), uri) == 0))){
    return true;
  }
  else {
    return false;
  }
}


bool uHTTP::put_request(const char *uri, EthernetClient response){
send_headers(200,response);
  if(method(uHTTP_METHOD_PUT) && (strcmp(this->uri(1), uri) == 0)){
    return true;
  }
  else {
    return false;
  }
}

void uHTTP::post_JSON(String output, EthernetClient response){
 response.println("HTTP/1.1 200 OK");
 response.println("Access-Control-Allow-Origin:*");
 response.println("Content-Type: application/json");
 response.println();
 response.println(output);

}

void uHTTP::send_JSON_headers(EthernetClient response){
  response.println("HTTP/1.1 200 OK");
  response.println("Access-Control-Allow-Origin:*");
  response.println("Content-Type: application/json");
  response.println();

}
