#if !defined(STRING_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Barret Gaylor $
   $Notice: (C) Copyright 2015 by Barret Gaylor. All Rights Reserved. $
   ======================================================================== */

extern "C"
{
    typedef struct string
    {
        unsigned char *String;
        int Size; 
    }string;
    
    unsigned char *RemoveNullTerminators(unsigned char *Source[], int Index, int Limit)
    {
        unsigned char *Target = Source[Index];
        while (Index < Limit - 1)
        {
            unsigned char *Char =  Source[Index];
            while(*Char)
                ++Char;
            *Char = ' ';
            
            ++Index;
        }
        return(Target);
    }
    
    void MakeLowerCase(unsigned char *Char)
    {
        while(*Char)
        {
            if((*Char >= 65) && (*Char < 91))
                *Char += 32;
            
            ++Char;
        }
    }
    
    enum string_flags
    {
        STRING_NUL = 0, 
        END_NEWLINE = 1 << 0,
        END_SPACE = 1 << 1,
        BETWEEN_SPACE = 1 << 2,
        BETWEEN_NEWLINE = 1 << 3,
        FREE_S1 = 1 << 4,
        FREE_S2 = 1 << 5
    };
    
    // TODO(Barret5Ocal): Allow the user to determine the length of the string
    string InitString(unsigned char *String, int StringFlag, int Size)
    {
        string Result = {0};
        
        // TODO(barret): do better memory management
        unsigned char *Char = String;
        if(!Size)
        {
            while(*Char)
            {
                ++Result.Size;
                ++Char;
            }
        }
        else 
        {
            Result.Size = Size; 
        }
        // NOTE(barret): get a compiler warning if compiled in 64-bit
        // going to try compiling in 32-bit space from now on
        Char = String;
        
        if(StringFlag & END_NEWLINE)
            Result.String =  (unsigned char *)malloc(Result.Size + 2);
        else
            Result.String =  (unsigned char *)malloc(Result.Size + 1);        
        
        for(int Index = 0;
            Index < Result.Size;
            ++Index)
        {
            Result.String[Index] = *Char;
            ++Char;
        }
        
        if(StringFlag & END_NEWLINE)
        {
            Result.String[Result.Size] = '\n';
            
            Result.String[Result.Size+1] = '\0';
        }
        else
        {
            Result.String[Result.Size] = '\0';
        }
        
        return(Result);
    }
    
    void ModifyString(string *String, unsigned char *NewString, int StringFlag)
    {
        if(String->String)
            free(String->String);
        
        String->Size = 0; 
        // TODO(barret): do better memory management     
        unsigned char *Char = NewString;
        while(*Char)
        {
            ++String->Size;
            ++Char;
        }
        
        Char = NewString;
        if(StringFlag & END_NEWLINE)
        {
            String->Size += 2;
            String->String =  (unsigned char *)malloc(String->Size);
        }
        else
        {
            String->Size += 1;
            String->String =  (unsigned char *)malloc(String->Size);        
        }
        
        for(int Index = 0;
            Index < String->Size;
            ++Index)
        {
            String->String[Index] = *Char;
            ++Char;
        }
        
        if(StringFlag & END_NEWLINE)
        {
            String->String[String->Size - 1] = '\n';
            
            String->String[String->Size] = '\0';
        }
        else
        {
            String->String[String->Size] = '\0';
        }
        
    }
    
    // TODO(barret): freeing the memory of the first string
    // probably a memory leak
    string CombineString(string *S1, string *S2, int StringFlag)
    {
        string Result = {0};
        
        Result.Size = S1->Size + S2->Size;
        
        if(StringFlag & BETWEEN_SPACE)
        {
            ++Result.Size;
        }
        if(StringFlag & END_NEWLINE)
            ++Result.Size;
        if(StringFlag & BETWEEN_NEWLINE)
        {
            
            ++Result.Size;        
        }
        
        Result.String = (unsigned char *)malloc(Result.Size + 1);    
        
        int ResultIndex = 0; 
        for(int Index = 0;
            Index < S1->Size;
            ++Index)
        {
            Result.String[ResultIndex] = S1->String[Index];
            ++ResultIndex;
        }
        
        //NOTE(barret): adds a space between strings. might want to create version that does not
        // do that. 
        if(StringFlag & BETWEEN_SPACE)
        {
            Result.String[ResultIndex] = ' ';
            ++ResultIndex;
        }    
        
        if(StringFlag & BETWEEN_NEWLINE)
        {
            Result.String[ResultIndex] = '\n';
            ++ResultIndex; 
        }
        
        for(int Index = 0;
            Index < S2->Size;
            ++Index)
        {
            Result.String[ResultIndex] = S2->String[Index];
            ++ResultIndex;
        }
        
        if(StringFlag & END_NEWLINE)
        {
            Result.String[ResultIndex] = '\n';
            ++ResultIndex;
        }
        
        Result.String[ResultIndex] = '\0';
        
        return Result; 
    }
    
    // TODO(barret): freeing the memory of the first string
    // probably a memory leak
    string AddStrings(string *S1, unsigned char *Chars, int StringFlags)
    {
        string Result = {0};
        
        int CharsSize = 0;
        unsigned char *Char = Chars; 
        while(*Char)
        {
            ++Char;
            ++CharsSize;
        }
        
        int FlagCount = 0;
        if(StringFlags & END_NEWLINE)
            ++FlagCount;
        if(StringFlags & BETWEEN_SPACE)
            ++FlagCount;
        
        Result.Size += (CharsSize + FlagCount);
        Result.String = (unsigned char *)malloc(S1->Size + 1);
        
        
        int ResultIndex = 0; 
        for(int Index = 0;
            Index < S1->Size;
            ++Index)
        {
            Result.String[ResultIndex] = S1->String[Index];
            ++ResultIndex;
        }
        
        //NOTE(barret): adds a space between strings. might want to create version that does not
        // do that.
        if(StringFlags & BETWEEN_SPACE)
        {
            Result.String[ResultIndex] = ' ';
            ++ResultIndex;
        }
        
        Char = Chars;
        for(int Index = 0;
            Index < CharsSize;
            ++Index)
        {
            Result.String[ResultIndex] = *Char;
            ++Char;
            ++ResultIndex;
        }
        
        if(StringFlags & END_NEWLINE)
        {
            Result.String[ResultIndex] = '\n';
            ++ResultIndex;
        }
        
        Result.String[ResultIndex] = '\0';
        
        if(StringFlags & FREE_S1)
            free(S1->String);
        
        return(Result);
    }
    
    void DestoryString(string *String)
    {
        // TODO(barret): When I malloc the entire string, i need to free the string struct to 
        free(String->String);
        
    }
    
    int Q_strlen(char *S)
    {
        int Count = 0;
        while(*S)
        {
            ++S;
            ++Count; 
        }
        
        return Count; 
    }
    
    int Q_strcmp(unsigned char *S1, unsigned char *S2)
    {
        while(*S1 == *S2)
        {
            if(!*S1)
                return 0; 
            ++S1;
            ++S2;
        }
        
        return ((*S1 < *S2) ? -1 : 1);
    }
    
    int Q_atoi(unsigned char *Str)
    {
        int Sign = 1;
        int Value = 0;
        unsigned char C;
        
        if(*Str == '-')
        {
            Sign = -1;
            ++Str;
        }
        
        if(Str[0] == '0' && (Str[1] == 'x' || Str[1] == 'X'))
        {
            Str += 2; 
            while(1)
            {
                C = *Str;
                ++Str;
                if(C >= '0' && C <= '9')
                    Value = Value * 16 + C - '0';
                else if (C >= 'a' && C <= 'f')
                    Value = Value * 16 + C - 'a' + 10;
                else if (C >= 'A' && C <= 'F')
                    Value = Value * 16 + C - 'A' + 10;
                else
                    return Value * Sign;
            }
        }
        
        while(1)
        {
            C = *Str;
            ++Str;
            if((C < '0') || (C > '9'))
            
                return Sign * Value;
            
            Value = Value * 10 + C - '0';
        }
    }
}
#define STRING_H
#endif
