/* This is a basic libcurl example for showing how to fetch a website, but
 * using a custom callback function (invoked by curl_easy_perform()) to
 * process the fetched website data.
 */
#include <iostream>
#include <string.h>
#include <curl/curl.h>

using namespace std;

/* buffer holds the data received from curl_easy_perform()
 * size is always 1
 * nmemb is the number of bytes in buffer
 * userp is a pointer to user data (not used in this example)
 *
 * Should return same value as nmemb, else it will signal an error to libcurl
 * and curl_easy_perform() will return an error (CURLE_WRITE_ERROR). This is
 * useful if you want to signal an error has occured during processing.
 */
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    cout << "In my own custom callback function" << endl;

    if (buffer && nmemb) {
        /* NOTE: This example allocates an extra byte for NULL-terminating the
         *       buffer as we know the received data is ASCII text. This may
         *       not be always true (e.g. binary data like images).
         */
        char myBuf[nmemb + 1] = {0}; // Zeroes out the array
        strncpy(myBuf, (char *)buffer, nmemb);

        cout << "Received buffer is:" << endl;
        cout << "====================" << endl << endl;
        cout << myBuf << endl << endl;
        cout << "====================" << endl;
        cout << "End of buffer reached" << endl;
    }

    return nmemb;
}

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
        char errbuf[CURL_ERROR_SIZE] = {0};

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, "http://ug251.eecg.utoronto.ca");
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);

        // Specify our custom callback function for handling the fetched web page/data
        if (res == CURLE_OK)
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }

        cout << endl << endl;
        if (res == CURLE_OK) {
            cout << "All good! res == CURLE_OK!" << endl;
        } else {
            cout << "ERROR: res == " << res << endl;
            cout << errbuf << endl;
        }

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }

    curl_global_cleanup();

    return 0;
}
