#ifndef APP_DIR_PATH_HPP
#define APP_DIR_PATH_HPP

#include <filesystem>
#include <climits>
#ifdef __unix__
    #include <unistd.h>
#endif
#ifdef  __WIN32
    #include <windows.h>
#endif

namespace fs = std::filesystem;

namespace app_dir_path
{
    inline fs::path get_application_dir_path()
    {
        fs::path app_path{fs::current_path()};
    #ifdef __unix__
        char path_to_proc[PATH_MAX + 1];                                     //unix doesn't add null termination
        ssize_t length = readlink("/proc/self/exe", path_to_proc, PATH_MAX); // ssize_t can be -1
        if(length != -1)
        {
            path_to_proc[length] = '\0';
            app_path = std::move(fs::path{path_to_proc}.parent_path());
        }
    #endif
    #ifdef _WIN32
        wchar_t path_to_proc[PATH_MAX]; 
        DWORD length = GetModuleFileNameW(NULL, path_to_proc, PATH_MAX);
        if(length != 0)
        {
            app_path = std::move(fs::path{path_to_proc}.parent_path());
        }
    #endif
        return app_path;
    }
}
 


#endif