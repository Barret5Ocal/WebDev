#define GB_REGEX_IMPLEMENTATION
//#include "gb_regex.h"
#include "b50_regex.h"

/*
*Successful Connection Regex*
begins with a ':'
tmi.twitch.tv
' '
three numbers
' '
between 4 and 25 alphanumeric characters and'_'
' '
':' 
any number of characters 
\r\n\n
*/

/*
*Join Regex*
begins with a ':'
between 4 and 25 alphanumeric characters and'_'
  '!'
  between 4 and 25 alphanumeric characters and'_'
  '@'
   between 4 and 25 alphanumeric characters and'_'
  '.'
  tmi.twitch.tv
  ' '
  JOIN
  ' '
  '#'
  between 4 and 25 alphanumeric characters and'_'
  /r/n/n
  
  begins with a ':'
  between 4 and 25 alphanumeric characters and'_'
  .tmi.twitch.tv
  ' '
  three numbers
  ' '
  between 4 and 25 alphanumeric characters and'_'
  ' '
  '='
  ' '
  '#'
  between 4 and 25 alphanumeric characters and'_'
  ' '
  ':'
  between 4 and 25 alphanumeric characters and'_'
  \r\n\n
  
  begins with a ':'
  between 4 and 25 alphanumeric characters and'_'
  .tmi.twitch.tv
  ' '
  three numbers
  ' '
  between 4 and 25 alphanumeric characters and'_'
  ' '
  '#'
  between 4 and 25 alphanumeric characters and'_'
  ' '
  End of /NAMES list 
  \r\n\n
*/

/*
*Part Channel*
begins with a ':'
between 4 and 25 alphanumeric characters and'_'
  '!'
  between 4 and 25 alphanumeric characters and'_'
  '@'
   between 4 and 25 alphanumeric characters and'_'
  '.'
  tmi.twitch.tv
  ' '
  PART
  ' '
  '#'
  between 4 and 25 alphanumeric characters and'_'
  /r/n/n
*/


/*
  *Chat Message Regex*
  begins with a ':'
  between 4 and 25 alphanumeric characters and'_'
  '!'
  between 4 and 25 alphanumeric characters and'_'
  '@'
   between 4 and 25 alphanumeric characters and'_'
  '.'
  tmi.twitch.tv
  ' '
  PRIVMSG
  ' '
  '#'
  between 4 and 25 alphanumeric characters and'_'
  ' '
  ':'
  the actual message
  \r\n\n
*/

enum response_type 
{
    NO_RESPONSE, 
    INIT_CONNECTION,
    JOIN,
    PART,
    MSG_REC,
    PING, 
};

struct response
{
    response_type Type;
    string Message;
    string User;
    string Channel; 
};

void FillResponse(response *Response, b50Capture2 *Captures)
{
    Response->Type = MSG_REC; 
    Response->Message = InitString((unsigned char *)Captures->str[2], 0, Captures->len[2]);
    Response->User = InitString((unsigned char *)Captures->str[0], 0, Captures->len[0]);
    Response->Channel = InitString((unsigned char *)Captures->str[1], 0, Captures->len[1]);
}

void DestroyResponse(response *Response)
{
    if(Response->Message.String)
        DestoryString(&Response->Message);
    if(Response->User.String)
        DestoryString(&Response->User);
    if(Response->Channel.String)
        DestoryString(&Response->Channel);
}

// TODO(Barret5Ocal): create expressions for the other messages
char *ChatMessageRegex = R"Yes(:([\w]+)![\w]+@[\w]+\.tmi\.twitch\.tv PRIVMSG #([\w]+) :([\p]+))Yes";

response ParseResponse(char *Buffer)
{
    response Result = {};
    
    b50Regex Regex;
    b50Error Error = b50_compile(&Regex, ChatMessageRegex, Q_strlen(ChatMessageRegex));
    
    int Count = b50_capture_count(&Regex);
    b50Capture2 Captures = {}; 
    b50Bool Match = b50_match(&Regex, Buffer, Q_strlen(Buffer), &Captures, Count);
    
    if(Match)
    {
        FillResponse(&Result, &Captures);
    }
    return Result; 
}



