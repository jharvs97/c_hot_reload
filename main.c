#include <Windows.h>
#include <timeapi.h>
#include <fileapi.h>
#include <stdint.h>

typedef void* RunFunc();
static RunFunc* Run = 0;

uint64_t GetFileLastWriteTime(const char* FileName)
{
    FILETIME Result = {0};
    WIN32_FILE_ATTRIBUTE_DATA Data;
    
    if(GetFileAttributesExA("game.dll", GetFileExInfoStandard, &Data)) {
        Result = Data.ftLastWriteTime;
    }
    
    return (uint64_t) (Result.dwLowDateTime | (Result.dwHighDateTime << 31));
}

HMODULE LoadGameCode()
{
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (!GetFileAttributesExA("lock.temp", GetFileExInfoStandard, &Data))
    {
        for (int i = 0; i < 10; i++) {
            if (CopyFile("game.dll", "game.temp.dll", 0))
                break;
        }
    }
    
    HMODULE GameDLL = LoadLibraryA("game.temp.dll");
    Run = (RunFunc*)GetProcAddress(GameDLL, "Run");
    return GameDLL;
}

int main(int ArgCount, char** Args)
{
    uint64_t LastWriteTime = GetFileLastWriteTime("game.dll");
    HMODULE GameDLL = LoadGameCode();
    
    while (1)
    {
        uint64_t CurrentWriteTime = GetFileLastWriteTime("game.dll");
        
        if (CurrentWriteTime > LastWriteTime)
        {
            if (GameDLL)
                FreeLibrary(GameDLL);
            
            GameDLL = LoadGameCode();
            LastWriteTime = CurrentWriteTime;
        }
        
        Run();
    }
    
    
    return 0;
}