/*
Hopefully, this will be my working version of the gb_regex.h library 
PUT MORE NOTES BELOW HERE

ALL OF THE BYTE CODES HERE:
\0         B50_OP_BEGIN_CAPTURE            agrs: (capture index)
\x1        B50_OP_END_CAPTURE              agrs: (capture index)
\x2        B50_OP_BEGINNING_OF_LINE        
\x3        B50_OP_END_OF_LINE
\x4        B50_OP_EXACT_MATCH
\x5        B50_OP_META_MATCH
\x6        B50_OP_RANGE
\a         B50_OP_ANY
\b         B50_OP_ANY_OF                   agrs: (length of buffer)
\t         B50_OP_ANY_BUT                  agrs: (length of buffer)
\n         B50_OP_ZERO_OR_MORE
\v         B50_OP_ONE_OR_MORE
\f         B50_OP_ZERO_OR_MORE_SHORTEST
\r         B50_OP_ONE_OR_MORE_SHORTEST
\xe        B50_OP_ZERO_OR_ONE
\xf        B50_OP_BRANCH_START
\x10       B50_OP_BRANCH_END

*/


#ifndef B50_REGEX_INCLUDE_GB_REGEX_H
#define B50_REGEX_INCLUDE_GB_REGEX_H

#include <stddef.h>  
#include <stdarg.h>
#include <string.h>


#if defined(__cplusplus)
extern "C"
{
#endif
    
    
#if !defined(B50_NO_MALLOC)
#ifndef B50_MALLOC
#define B50_MALLOC(sz) malloc(sz)
#endif
    
#ifndef B50_REALLOC
#define B50_REALLOC(ptr, sz) realloc(ptr, sz)
#endif
    
#ifndef B50_FREE
#define B50_FREE(ptr) free(ptr)
#endif
#endif /* !defined(GBRE_NO_MALLOC) */
    
    
    typedef ptrdiff_t isize; /* TODO(bill): Should this be replaced with int? */
    typedef int       b50Bool;
    
#define b50_size_of(x) ((isize)sizeof(x))
    
#define B50_TRUE  (0 == 0)
#define B50_FALSE (0 != 0)
    
    
    typedef struct b50Regex
    {
        isize capture_count;
        unsigned char *buf;
        isize          buf_len, buf_cap;
        b50Bool       can_realloc;
    } b50Regex;
    
    typedef struct b50Capture 
    {
        char const *str;
        isize len;
    } b50Capture;
    
    typedef struct b50Capture2
    {
        char *str[10];
        int len[10];
    }b50Capture2; 
    
    typedef enum b50Error
    {
        B50_ERROR_NONE,
        B50_ERROR_NO_MATCH,
        B50_ERROR_TOO_LONG,
        B50_ERROR_MISMATCHED_CAPTURES,
        B50_ERROR_MISMATCHED_BLOCKS,
        B50_ERROR_INVALID_QUANTIFIER,
        B50_ERROR_BRANCH_FAILURE,
        B50_ERROR_INTERNAL_FAILURE
    } b50Error;
    
    
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
extern "C"
{
#endif
    
    
    typedef enum b50Op
    {
        B50_OP_BEGIN_CAPTURE,
        B50_OP_END_CAPTURE,
        
        B50_OP_BEGINNING_OF_LINE,
        B50_OP_END_OF_LINE,
        
        B50_OP_EXACT_MATCH,
        B50_OP_META_MATCH,
        
        B50_OP_RANGE,
        
        B50_OP_ANY,
        B50_OP_ANY_OF,
        B50_OP_ANY_BUT,
        
        B50_OP_ZERO_OR_MORE,
        B50_OP_ONE_OR_MORE,
        B50_OP_ZERO_OR_MORE_SHORTEST,
        B50_OP_ONE_OR_MORE_SHORTEST,
        B50_OP_ZERO_OR_ONE,
        
        B50_OP_BRANCH_START,
        B50_OP_BRANCH_END
    } b50Op;
    
    
    typedef enum b50Code
    {
        B50_CODE_NULL              = 0x0000,
        B50_CODE_WHITESPACE        = 0x0100,
        B50_CODE_NOT_WHITESPACE    = 0x0200,
        B50_CODE_DIGIT             = 0x0300,
        B50_CODE_NOT_DIGIT         = 0x0400,
        B50_CODE_ALPHA             = 0x0500,
        B50_CODE_LOWER             = 0x0600,
        B50_CODE_UPPER             = 0x0700,
        B50_CODE_WORD              = 0x0800,
        B50_CODE_NOT_WORD          = 0x0900,
        
        B50_CODE_XDIGIT            = 0x0a00,
        B50_CODE_PRINTABLE         = 0x0b00,
    } b50Code;
    
    
    typedef struct b50Context
    {
        isize op, offset;
    } b50Context;
    
    enum
    { /* TODO(bill): Should these be defines or is an enum good enough? */
        B50__NO_MATCH = -1,
        B50__INTERNAL_FAILURE = -2
    };
    
    static char const B50__META_CHARS[]  = "^$()[].*+?|\\";
    static char const B50__WHITESPACE[] = " \r\t\n\v\f";
#define B50__LITERAL(str) (str), b50_size_of(str)-1
    
    
    static b50Context b50__exec_single(b50Regex *re, isize op, char const *str, isize str_len, isize offset,
                                       b50Capture *captures, isize max_capture_count);
    
    static b50Context b50__exec(b50Regex *re, isize op, char const *str, isize str_len, isize offset,
                                b50Capture *captures, isize max_capture_count);
    
    // NOTE(Barret5Ocal): used to tell the system that the match failed
    // the c.op is used to tell the user what operation that the match failled on
    static b50Context b50__context_no_match(isize op) {
        b50Context c;
        c.op = op;
        c.offset = B50__NO_MATCH;
        return c;
    }
    
    static b50Context b50__context_internal_failure(isize op)
    {
        b50Context c;
        c.op = op;
        c.offset = B50__INTERNAL_FAILURE;
        return c;
    }
    
    static b50Bool b50__is_hex(char const *s)
    {
        if ((s[0] < '0' || s[0] > '9') &&
            (s[0] < 'a' || s[0] > 'f') &&
            (s[0] < 'A' || s[0] > 'F'))
        {
            return B50_FALSE;
        }
        if ((s[1] < '0' || s[1] > '9') &&
            (s[1] < 'a' || s[1] > 'f') &&
            (s[1] < 'A' || s[1] > 'F'))
        {
            return B50_FALSE;
        }
        return B50_TRUE;
    }
    
    static unsigned char b50__hex_digit(char const *s)
    {
        if (s[0] >= '0' && s[0] <= '9')
            return (unsigned char)(s[0] - '0');
        if (s[0] >= 'a' && s[0] <= 'f')
            return (unsigned char)(10 + s[0] - 'a');
        if (s[0] >= 'A' && s[0] <= 'F')
            return (unsigned char)(10 + s[0] - 'A');
        return 0;
    }
    
    
    static unsigned char b50__hex(char const *s)
    {
        return ((b50__hex_digit(s) << 4) & 0xf0) | (b50__hex_digit(s+1) & 0x0f);
    }
    
    
    static isize b50__strfind(char const *str, isize len, char c, isize offset) {
        if (offset < len) {
            char const *found = (char const *)memchr(str+offset, c, len-offset);
            if (found)
                return found-str;
        }
        return -1;
    }
    
    
    static b50Bool b50__match_escape(char c, int code)
    {
        // NOTE(Barret5Ocal): char c is the character from the str 
        // and int code is the code from the expression  
        switch (code)
        {
            case B50_CODE_NULL:           return c == 0;
            case B50_CODE_WHITESPACE:     return b50__strfind(B50__LITERAL(B50__WHITESPACE), c, 0) >= 0;
            case B50_CODE_NOT_WHITESPACE: return b50__strfind(B50__LITERAL(B50__WHITESPACE), c, 0) < 0;
            case B50_CODE_DIGIT:          return (c >= '0' && c <= '9');
            case B50_CODE_NOT_DIGIT:      return !(c >= '0' && c <= '9');
            case B50_CODE_ALPHA:          return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
            case B50_CODE_LOWER:          return (c >= 'a' && c <= 'z');
            case B50_CODE_UPPER:          return (c >= 'A' && c <= 'Z');
            
            /* TODO(bill): Make better? */
            case B50_CODE_WORD:           return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
            case B50_CODE_NOT_WORD:       return !((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');
            
            /* TODO(bill): Maybe replace with between tests? */
            case B50_CODE_XDIGIT:         return b50__strfind(B50__LITERAL("0123456789ABCDEFabcdef"), c, 0) >= 0;
            case B50_CODE_PRINTABLE:      return c >= 0x20 && c <= 0x7e;
            default: break;
        }
        return B50_FALSE;
    }
    
    
    
    static int b50__encode_espace(char code)
    {
        switch (code)
        {
            default:   break; /* NOTE(bill): It's a normal character */
            
            /* TODO(bill): Are there anymore? */
            case 't':  return '\t';
            case 'n':  return '\n';
            case 'r':  return '\r';
            case 'f':  return '\f';
            case 'v':  return '\v';
            
            case '0':  return B50_CODE_NULL;
            
            case 's':  return B50_CODE_WHITESPACE;
            case 'S':  return B50_CODE_NOT_WHITESPACE;
            
            case 'd':  return B50_CODE_DIGIT;
            case 'D':  return B50_CODE_NOT_DIGIT;
            
            case 'a':  return B50_CODE_ALPHA;
            case 'l':  return B50_CODE_LOWER;
            case 'u':  return B50_CODE_UPPER;
            
            case 'w':  return B50_CODE_WORD;
            case 'W':  return B50_CODE_NOT_WORD;
            
            case 'x':  return B50_CODE_XDIGIT;
            case 'p':  return B50_CODE_PRINTABLE;
        }
        return code;
    }
    
    
    
    
    
    
    static b50Error b50__emit_ops(b50Regex *re, isize op_count, ...)
    {
        isize i;
        va_list va;
        
        if (re->buf_len + op_count > re->buf_cap)
        {
            if (!re->can_realloc) {
                return B50_ERROR_TOO_LONG;
            }
            else
            {
#if !defined(B50_NO_MALLOC)
                isize new_cap = (re->buf_cap * 2) + op_count;
                re->buf = (unsigned char *)B50_REALLOC(re->buf, new_cap);
                re->buf_cap = new_cap;
#else
#error B50_NO_MALLOC defined
#endif
            }
        }
        
        va_start(va, op_count);
        for (i = 0; i < op_count; i++)
        {
            int value = va_arg(va, int);
            if (value > 256)
                return B50_ERROR_TOO_LONG;
            re->buf[re->buf_len++] = (unsigned char)value;
        }
        va_end(va);
        
        return B50_ERROR_NONE;
    }
    
    
    static b50Error b50__emit_ops_buffer(b50Regex *re, isize op_count, unsigned char const *buffer)
    {
        isize i;
        
        if (re->buf_len + op_count > re->buf_cap)
        {
            if (!re->can_realloc)
            {
                return B50_ERROR_TOO_LONG;
            }
            else
            {
#if !defined(B50_NO_MALLOC)
                isize new_cap = (re->buf_cap * 2) + op_count;
                re->buf = (unsigned char *)B50_REALLOC(re->buf, new_cap);
                re->buf_cap = new_cap;
#else
#error B50_NO_MALLOC defined
#endif
            }
        }
        
        for (i = 0; i < op_count; i++)
        {
            re->buf[re->buf_len++] = buffer[i];
        }
        
        return B50_ERROR_NONE;
    }
    
    static b50Error b50__compile_quantifier(b50Regex *re, isize last_buf_len, unsigned char quantifier) {
        b50Error err;
        isize move_size;
        if ((re->buf[last_buf_len] == B50_OP_EXACT_MATCH) &&
            (re->buf[last_buf_len+1] > 1)) {
            unsigned char last_char = re->buf[re->buf_len-1];
            
            re->buf[last_buf_len+1]--;
            re->buf_len--;
            err = b50__emit_ops(re, 4, (int)quantifier, (int)B50_OP_EXACT_MATCH, 1, (int)last_char);
            if (err) return err;
            return B50_ERROR_NONE;
        }
        
        move_size = re->buf_len - last_buf_len + 1;
        
        err = b50__emit_ops(re, 1, 0);
        if (err) return err;
        
        memmove(re->buf+last_buf_len+1, re->buf+last_buf_len, move_size);
        re->buf[last_buf_len] = quantifier;
        
        return B50_ERROR_NONE;
    }
    
    
    
    static b50Error b50__parse_group(b50Regex *re, char const *pattern, isize len, isize offset, isize *new_offset)
    {
        b50Error err = B50_ERROR_NONE;
        unsigned char buffer[256] = {0}; /* NOTE(bill): ascii is only 7/8 bits */
        isize buffer_len = 0, buffer_cap = b50_size_of(buffer);
        b50Bool closed = B50_FALSE;
        b50Op op = B50_OP_ANY_OF;
        
        if (pattern[offset] == '^') {
            offset++;
            op = B50_OP_ANY_BUT;
        }
        
        while (!closed &&
               err == B50_ERROR_NONE &&
               offset < len) 
        {
            if (pattern[offset] == ']')
            {
                err = b50__emit_ops(re, 2, (int)op, (int)buffer_len);
                if (err) break;
                
                err = b50__emit_ops_buffer(re, buffer_len, buffer);
                if (buffer && err) break;
                offset++;
                closed = B50_TRUE;
                break;
            }
            
            if (buffer_len >= buffer_cap)
                return B50_ERROR_TOO_LONG;
            
            if (pattern[offset] == '\\')
            {
                offset++;
                
                if ((offset+1 < len) && b50__is_hex(pattern+offset)) 
                {
                    buffer[buffer_len++] = b50__hex(pattern+offset);
                    offset++;
                } 
                else if (offset < len)
                {
                    int code = b50__encode_espace(pattern[offset]);
                    if (!code || code > 0xff) 
                    {
                        buffer[buffer_len++] = 0; // NOTE(Barret5Ocal): It puts a zero in for some reason 
                        if (buffer_len >= buffer_cap)
                            return B50_ERROR_TOO_LONG;
                        buffer[buffer_len++] = (code >> 8) & 0xff;
                    } 
                    else
                    {
                        buffer[buffer_len++] = code & 0xff;
                    }
                }
            } 
            else 
            {
                
                // TODO(Barret5Ocal): maybe the logic for ranges should go here
                if(pattern[offset + 1] == '-')
                {
                    //err = b50__emit_ops(re, 1, (int)B50_OP_RANGE);
                    buffer[buffer_len++] = (unsigned char)B50_OP_RANGE;
                    //unsigned char Range[2];
                    buffer[buffer_len++] = pattern[offset];
                    buffer[buffer_len++] = pattern[offset + 2];
                    
                    //err = b50__emit_ops_buffer(re, (isize)2, Range);
                    offset += 2;
                    
                }
                else 
                {
                    buffer[buffer_len++] = (unsigned char)pattern[offset];
                }
            }
            offset++;
        }
        
        if (err) return err;
        if (!closed) return B50_ERROR_MISMATCHED_BLOCKS;
        if (new_offset) *new_offset = offset;
        return B50_ERROR_NONE;
    }
    
    static b50Error b50__parse(b50Regex *re, char const *pattern, isize len, isize offset, isize level, isize *new_offset)
    {
        b50Error err = B50_ERROR_NONE;
        isize last_buf_len = re->buf_len;
        isize branch_begin = re->buf_len;
        isize branch_op = -1;
        
        while (offset < len)
        {
            switch (pattern[offset++])
            {
                case '^': 
                {
                    err = b50__emit_ops(re, 1, B50_OP_BEGINNING_OF_LINE);
                    if (err) return err;
                } break;
                
                case '$':
                {
                    err = b50__emit_ops(re, 1, B50_OP_END_OF_LINE);
                    if (err) return err;
                } break;
                
                case '(': 
                {
                    isize capture = re->capture_count++;
                    last_buf_len = re->buf_len;
                    err = b50__emit_ops(re, 2, B50_OP_BEGIN_CAPTURE, (int)capture);
                    if (err) return err;
                    
                    err = b50__parse(re, pattern, len, offset, level+1, &offset);
                    
                    if ((offset > len) || (pattern[offset-1] != ')'))
                        return B50_ERROR_MISMATCHED_CAPTURES;
                    
                    err = b50__emit_ops(re, 2, B50_OP_END_CAPTURE, (int)capture);
                    if (err) return err;
                } break;
                
                case ')':
                {
                    if (branch_op != -1)
                        re->buf[branch_op + 1] = (unsigned char)(re->buf_len - (branch_op+2));
                    
                    if (level == 0)
                        return B50_ERROR_MISMATCHED_CAPTURES;
                    if (new_offset) *new_offset = offset;
                    return B50_ERROR_NONE;
                } break;
                
                case '[':
                {
                    //err = b50__emit_ops(re, 1, B50_OP_BEGIN_GROUP);
                    last_buf_len = re->buf_len;
                    err = b50__parse_group(re, pattern, len, offset, &offset);
                    if (offset > len)
                        return err;
                } break;
                
                /* NOTE(bill): Branching magic! */
                case '|': 
                {
                    if (branch_begin >= re->buf_len) 
                    {
                        return B50_ERROR_BRANCH_FAILURE;
                    } else
                    {
                        isize size = re->buf_len - branch_begin;
                        err = b50__emit_ops(re, 4, 0, 0, B50_OP_BRANCH_END, 0);
                        if (err) return err;
                        
                        memmove(re->buf + branch_begin + 2, re->buf + branch_begin, size);
                        re->buf[branch_begin] = B50_OP_BRANCH_START;
                        re->buf[branch_begin+1] = (size+2) & 0xff;
                        branch_op = re->buf_len-2;
                    }
                } break;
                
                case '.':
                {
                    last_buf_len = re->buf_len;
                    err = b50__emit_ops(re, 1, B50_OP_ANY);
                    if (err) return err;
                } break;
                
                case '*':
                case '+':
                {
                    // TODO(Barret5Ocal): fix this now
                    unsigned char quantifier = B50_OP_ONE_OR_MORE;
                    if (pattern[offset-1] == '*')
                        quantifier = B50_OP_ZERO_OR_MORE;
                    
                    if (last_buf_len >= re->buf_len)
                        return B50_ERROR_INVALID_QUANTIFIER;
                    if ((re->buf[last_buf_len] < B50_OP_EXACT_MATCH) ||
                        (re->buf[last_buf_len] > B50_OP_ANY_BUT))
                        return B50_ERROR_INVALID_QUANTIFIER;
                    
                    if ((offset < len) && (pattern[offset] == '?'))
                    {
                        quantifier = B50_OP_ONE_OR_MORE_SHORTEST;
                        if (quantifier == B50_OP_ZERO_OR_MORE)
                            quantifier = B50_OP_ZERO_OR_MORE_SHORTEST;
                        offset++;
                    }
                    
                    err = b50__compile_quantifier(re, last_buf_len, quantifier);
                    if (err) return err;
                } break;
                
                case '?': 
                {
                    if (last_buf_len >= re->buf_len)
                        return B50_ERROR_INVALID_QUANTIFIER;
                    if ((re->buf[last_buf_len] < B50_OP_EXACT_MATCH) ||
                        (re->buf[last_buf_len] > B50_OP_ANY_BUT))
                        return B50_ERROR_INVALID_QUANTIFIER;
                    
                    err = b50__compile_quantifier(re, last_buf_len,
                                                  (unsigned char)B50_OP_ZERO_OR_ONE);
                    if (err) return err;
                } break;
                
                case '\\': 
                {
                    last_buf_len = re->buf_len;
                    if ((offset+1 < len) && b50__is_hex(pattern+offset)) 
                    {
                        unsigned char hex_value = b50__hex(pattern+offset);
                        offset += 2;
                        err = b50__emit_ops(re, 2, B50_OP_META_MATCH, (int)hex_value);
                        if (err) return err;
                    }
                    else if (offset < len)
                    {
                        int code = b50__encode_espace(pattern[offset++]);
                        if (!code || (code > 0xff))
                        {
                            err = b50__emit_ops(re, 3, B50_OP_META_MATCH, 0, (int)((code >> 8) & 0xff));
                            if (err) return err;
                        } 
                        else
                        {
                            // TODO(Barret5Ocal): is B50_OP_META_MATCH supposed to be B50_OP_EXACT_MATCH. figure it out first
                            err = b50__emit_ops(re, 2, B50_OP_META_MATCH, (int)code);
                            if (err) return err;
                        }
                    }
                } break;
                
                /* NOTE(bill): Exact match */
                default:
                {
                    char const *match_start;
                    isize size = 0;
                    offset--;
                    match_start = pattern+offset;
                    while ((offset < len) &&
                           (b50__strfind(B50__LITERAL(B50__META_CHARS), pattern[offset], 0) < 0)) 
                    {
                        size++, offset++;
                    }
                    
                    last_buf_len = re->buf_len;
                    err = b50__emit_ops(re, 2, B50_OP_EXACT_MATCH, (int)size);
                    if (err) return err;
                    err = b50__emit_ops_buffer(re, size, (unsigned char const *)match_start);
                    if (err) return err;
                } break;
            }
        }
        
        if (new_offset) *new_offset = offset;
        return B50_ERROR_NONE;
    }
    
    
#if !defined(B50_NO_MALLOC)
    b50Error b50_compile(b50Regex *re, char const *pattern, isize len)
    {
        b50Error err;
        isize cap = len+128;
        isize offset = 0;
        
        re->capture_count = 0;
        re->buf           = (unsigned char *)B50_MALLOC(cap);
        re->buf_len       = 0;
        re->buf_cap       = cap;
        re->can_realloc   = B50_TRUE;
        
        
        err = b50__parse(re, pattern, len, 0, 0, &offset);
        if (offset != len)
            B50_FREE(re->buf);
        return err;
    }
#endif
    
    isize b50_capture_count(b50Regex *re) { return re->capture_count; }
    
    int b50_get_empty_capture(b50Capture2 *Captures)
    {
        int Index = 0;
        while(Captures->str[Index])
        {
            Index++; 
        }
        return Index; 
    }
    
    b50Bool b50__match_sigle(char *Expression, char *Pattern, b50Context *Context, b50Capture2 *Captures, int MaxCaptureCount)
    {
        
        switch(Expression[Context->op++])
        {
            
            case B50_OP_BEGIN_CAPTURE:
            {
                int Index = (int)Expression[Context->op++];
                char *StartingPoint = Pattern + Context->offset;
                Captures->str[Index] = (Pattern + Context->offset); 
                b50__match_sigle(Expression, Pattern, Context, Captures, MaxCaptureCount);
                
                Captures->len[Index] =(int) ((Pattern + Context->offset) - StartingPoint); 
            }break;
            case B50_OP_END_CAPTURE:
            {
                int Index = (int)Expression[Context->op++];
                return B50_TRUE;
                
            }break;
            case B50_OP_BEGINNING_OF_LINE:
            {
                Assert(false);
            }break;
            case B50_OP_END_OF_LINE:
            {
                Assert(false);
                
            }break;
            case B50_OP_EXACT_MATCH:
            {
                int NumOfMatches = Expression[Context->op++];
                for(int Index = 0;
                    Index < NumOfMatches;
                    ++Index)
                {
                    if (!(Pattern[Context->offset++] == Expression[Context->op++]))
                    {
                        return B50_FALSE;
                    }
                }
                
            }break;
            case B50_OP_META_MATCH:
            {
                char cin = (char)Expression[Context->op++];
                char cmatch = Pattern[Context->offset++];
                if(!cin)
                {
                    if(b50__match_escape(cmatch, Expression[Context->op++]))
                        break; 
                }
                else 
                {
                    if(cin == cmatch)
                        break; 
                }
                *Context = b50__context_no_match(Context->op);
                return B50_FALSE;
            }break;
            case B50_OP_RANGE:
            {
                int fill = 0;
            }break;
            case B50_OP_ANY:
            {
                Assert(false);
                // TODO(Barret5Ocal): NEXT!!!
            }break;
            case B50_OP_ANY_OF:
            {
                char Length = Expression[Context->op++];
                char *Symbols = Expression + Context->op++;
                
                // NOTE(Barret5Ocal): I need to collect all of the symbols in the [] that need to be matched
                // then run through the characters in pattern check if there is a match 
                bool Matching = true;
                bool FirstRound = true; 
                while(Matching)
                {
                    char TestChar = Pattern[Context->offset];
                    // NOTE(Barret5Ocal): I need to test each character in Pattern with every symbol in Symbols until something does not match 
                    // break if match, go through if  no match 
                    bool Matched = 0; 
                    for(int Index = 0;
                        Index < Length;
                        ++Index)
                    {
                        
                        if(Symbols[Index] == 0)
                        {
                            ++Index; 
                            // NOTE(Barret5Ocal): escape
                            if(b50__match_escape(TestChar, Symbols[Index] << 8))
                            {
                                Matched = true; 
                                break; 
                            }
                        }
                        else
                        {
                            // NOTE(Barret5Ocal): match 
                            if(Symbols[Index] == TestChar)
                            {
                                Matched = true; 
                                break; 
                            }
                        }
                        
                    }
                    
                    // NOTE(Barret5Ocal): The only way this op should cause the whole match to fail is if the first TestChar does not match
                    if(FirstRound)
                    {
                        if(!Matched)
                        {
                            *Context = b50__context_no_match(Context->op);
                            Matching = false; 
                            break;
                        }
                        FirstRound = false; 
                    }
                    else 
                    {
                        if(!Matched)
                        {
                            Matching = false; 
                            break; 
                        }
                    }
                    Context->offset++;
                }
                
                Context->op += (Length - 1); 
                
            }break;
            case B50_OP_ANY_BUT:
            {
                Assert(false);
            }break;
            case B50_OP_ZERO_OR_MORE:
            {
                Assert(false);
            }break;
            case B50_OP_ONE_OR_MORE:
            {
                // TODO(Barret5Ocal): this needs to loop to get more than one or it needs to tell the other stuff to loop(ladder might be easier)
                
                b50__match_sigle(Expression, Pattern, Context, Captures, MaxCaptureCount);
            }break;
            case B50_OP_ZERO_OR_MORE_SHORTEST:
            {
                Assert(false);
            }break;
            case B50_OP_ONE_OR_MORE_SHORTEST:
            {
                Assert(false);
            }break;
            case B50_OP_ZERO_OR_ONE:
            {
                Assert(false);
            }break;
            case B50_OP_BRANCH_START:
            {
                Assert(false);
            }break;
            case B50_OP_BRANCH_END:
            {
                Assert(false);
            }break; 
            default:
            {
                Assert(false);
            }break; 
            
        }
        return B50_TRUE;
    }
    
    b50Bool b50_match(b50Regex *re, char const *str, isize len, b50Capture2 *captures, isize max_capture_count)
    {
        b50Context Context = {0, 0};
        char *Expression = (char *)re->buf; 
        char *Pattern = (char *)str; 
        
        b50Bool Match = B50_TRUE; 
        while(Match && Pattern[Context.offset] && Context.op < re->buf_len)
        {
            Match = b50__match_sigle(Expression, Pattern, &Context, captures, max_capture_count);
        }
        return(Match);
    }
    
#if defined(__cplusplus)
}
#endif

#endif 