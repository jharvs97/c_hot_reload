#include <Windows.h>
#include <timeapi.h>
#include <fileapi.h>
#include <stdint.h>

typedef void* RunFunc();

typedef struct game_code game_code;

struct game_code
{
    HMODULE GameDLL;
    uint64_t LastGameDLLWriteTime;
    RunFunc* Run;
};

uint64_t GetFileLastWriteTime(const char* FileName)
{
    FILETIME Result = {0};
    WIN32_FILE_ATTRIBUTE_DATA Data;
    
    if(GetFileAttributesExA("game.dll", GetFileExInfoStandard, &Data))
    {
        Result = Data.ftLastWriteTime;
    }
    
    return (uint64_t) (Result.dwLowDateTime | (Result.dwHighDateTime << 31));
}

game_code LoadGameCode()
{
    game_code GameCode = {0};
    
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (!GetFileAttributesExA("lock.temp", GetFileExInfoStandard, &Data))
    {
        GameCode.LastGameDLLWriteTime = GetFileLastWriteTime("game.dll");
        
        for (int i = 0; i < 10; i++) 
        {
            if (CopyFile("game.dll", "game.temp.dll", 0))
                break;
        }
    }
    
    GameCode.GameDLL = LoadLibraryA("game.temp.dll");
    GameCode.Run = (RunFunc*)GetProcAddress(GameCode.GameDLL, "Run");
    
    return GameCode;
}

int main(int ArgCount, char** Args)
{
    game_code GameCode = LoadGameCode();
    
    while (1)
    {
        uint64_t CurrentWriteTime = GetFileLastWriteTime("game.dll");
        
        if (CurrentWriteTime > GameCode.LastGameDLLWriteTime)
        {
            if (GameCode.GameDLL)
                FreeLibrary(GameCode.GameDLL);
            
            GameCode = LoadGameCode();
        }
        
        GameCode.Run();
    }
    
    return 0;
}