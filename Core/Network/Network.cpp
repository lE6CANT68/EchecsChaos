#include "Network.h"
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
using json = nlohmann::json;

std::string getApiKeyFromEnv() {
    std::vector<std::string> paths = {
        ".env",
        "../.env",
    };

    std::ifstream file;
    for (const auto& path : paths) {
        file.open(path);
        if (file.is_open()) break;
    }

    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "[ERREUR] Impossible d'ouvrir le fichier .env a la racine !" << std::endl;
        std::cerr << "[ERREUR] Impossible d'ouvrir le fichier .env !" << std::endl;
        return "";
    }

    while (std::getline(file, line)) {
        // On ignore les lignes vides ou les commentaires
        if (line.empty() || line[0] == '#') continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            if (key == "GEMINI_API_KEY") {
                if (!value.empty() && (value.front() == '"' || value.front() == '\'')) {
                    value = value.substr(1, value.length() - 2);
                }
                return value;
            }
        }
    }
    return "";
}
std::string sendPromptToGemini(const std::string& prompt) {
    std::string apiKey = getApiKeyFromEnv();
    if (apiKey.empty()) {
        return "{\"error\": \"Cle API manquante ou fichier .env introuvable.\"}";
    }

    std::string responseRaw = "";
    HINTERNET hInternet = InternetOpenA("EchecsChaosBot", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "{\"error\": \"WinINet failed to initialize.\"}";
    HINTERNET hConnect = InternetConnectA(hInternet, "generativelanguage.googleapis.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) { 
        InternetCloseHandle(hInternet); 
        return "{\"error\": \"Impossible de se connecter a Google API.\"}"; 
    }
    std::string path = "/v1/models/gemini-3.5-flash:generateContent?key=" + apiKey;
    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) { 
        InternetCloseHandle(hConnect); 
        InternetCloseHandle(hInternet); 
        return "{\"error\": \"Erreur d'ouverture de requete.\"}"; 
    }
    std::string headers = "Content-Type: application/json\r\n";
    json body;
    body["contents"] = json::array({
        {{"parts", json::array({{{"text", prompt}}})}}
    });
    std::string payload = body.dump(); 

    
    BOOL bSend = HttpSendRequestA(hRequest, headers.c_str(), headers.length(), (LPVOID)payload.c_str(), payload.length());
    if (bSend) {
        char buffer[2048];
        DWORD bytesRead = 0;
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            responseRaw += buffer;
        }
    } else {
        responseRaw = "{\"error\": \"Le serveur Google n'a pas repondu.\"}";
    }
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    try {
        auto parsedJson = json::parse(responseRaw);
        if (parsedJson.contains("candidates") && !parsedJson["candidates"].empty()) {
            auto content = parsedJson["candidates"][0]["content"];
            if (content.contains("parts") && !content["parts"].empty()) {
                return content["parts"][0]["text"];
            }
        }
        return "Erreur: Structure JSON inattendue. Brute : " + responseRaw;
    } 
    catch (const std::exception& e) {
        return "Erreur Parsing JSON. Brut : " + responseRaw;
    }
}

static std::string trimString(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

static std::string extractJsonBlock(const std::string& text, char openChar, char closeChar) {
    int depth = 0;
    size_t start = std::string::npos;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == openChar) {
            if (depth == 0) {
                start = i;
            }
            depth++;
        } else if (text[i] == closeChar && depth > 0) {
            depth--;
            if (depth == 0 && start != std::string::npos) {
                return text.substr(start, i - start + 1);
            }
        }
    }
    return "";
}

static std::string extractJson(const std::string& aiText) {
    std::string trimmed = trimString(aiText);
    try {
        auto parsed = nlohmann::json::parse(trimmed);
        (void)parsed;
        return trimmed;
    } catch (...) {
    }

    std::string jsonObject = extractJsonBlock(trimmed, '{', '}');
    if (!jsonObject.empty()) {
        try {
            auto parsed = nlohmann::json::parse(jsonObject);
            (void)parsed;
            return jsonObject;
        } catch (...) {
        }
    }

    std::string jsonArray = extractJsonBlock(trimmed, '[', ']');
    if (!jsonArray.empty()) {
        try {
            auto parsed = nlohmann::json::parse(jsonArray);
            (void)parsed;
            return jsonArray;
        } catch (...) {
        }
    }

    return trimmed;
}

std::string askAI(const std::string& prompt) {
    std::string aiText = sendPromptToGemini(prompt);
    return extractJson(aiText);
}

std::string askAIAnalysis(const std::string& prompt) {
    std::string aiText = sendPromptToGemini(prompt);
    return extractJson(aiText);
}