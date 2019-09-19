/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: bankaran
 *
 * Created on February 26, 2019, 12:50 AM
 */

#include <iostream>
#include <curl/curl.h>   


using namespace std; 

const char * CP24WEB = "https://www.cp24.com/commuter-centre/traffic";  

CURL *curlHandle = curl_easy_init(); 

int main() {
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        cout << "ERROR: Unable to initialize libcurl" << endl;
        cout << curl_easy_strerror(res) << endl;
        return 0;
    }
    CURL *curlHandle = curl_easy_init(); 
    if ( !curlHandle ) {
        cout << "ERROR: Unable to get easy handle" << endl;
        return 0;
    } else {
    // Set web URL to fetch page/data from
        res = curl_easy_setopt(curlHandle, CURLOPT_URL, "http://ug251.eecg.utoronto.ca"); //res = curl_easy_setopt(curlHandle, CURLOPT_URL, "http://shouldnotexist.ca");

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } else {
            // Perform web transfer request
            res = curl_easy_perform(curlHandle);
        }

        cout << endl << endl;
        
        if (res == CURLE_OK) {
            cout << "All good! res == CURLE_OK!" << endl;
        } else {
            cout << "ERROR: res == " << res << endl;
            cout << curl_easy_strerror(res) << endl;
            cout << "See https://curl.haxx.se/libcurl/c/libcurl-errors.html "
            "for error codes" << endl;
        }
 
        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    } 
    
    curl_global_cleanup(); 
    
    return 0; 
}
    


