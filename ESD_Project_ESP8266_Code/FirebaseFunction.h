#include <FirebaseESP8266.h>

FirebaseAuth   auth;          // Login Firebase 用
FirebaseData   firebaseData;  // Firebase Real Time Database 用
FirebaseConfig config;        // save Database 的 Key & URL

void LoginFirebase(char* api, char* db_url, char* email, char* password)
{
  config.api_key      = api;
  config.database_url = db_url;

  auth.user.email = email;
  auth.user.password = password;

  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);          // reconnect nectwork login Firebase
  Firebase.reconnectWiFi(true);             // reconnect WiFi login Firebase

  Firebase.setMaxRetry(firebaseData, 5);    // try 5 time reconnect Firebase 5 time
}
