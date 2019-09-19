/* This is a basic libcurl example for showing how to fetch a website, but
 * using a custom callback function (invoked by curl_easy_perform()) and
 * passing it a custom data object. The data object can be used to input
 * extra information to the callback function, or used to get output from it.
 */
#include <iostream>
#include <string.h>
#include <curl/curl.h>

using namespace std;

typedef struct MyCustomStruct {
    char *url = nullptr;
    unsigned int size = 0;
    char *response = nullptr;
} MyCustomStruct;

/* buffer holds the data received from curl_easy_perform()
 * size is always 1
 * nmemb is the number of bytes in buffer
 * userp is a pointer to user data (i.e. myStruct from main)
 *
 * Should return same value as nmemb, else it will signal an error to libcurl
 * and curl_easy_perform() will return an error (CURLE_WRITE_ERROR). This is
 * useful if you want to signal an error has occured during processing.
 */
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    cout << "In my own custom callback function" << endl;

    // Rather than printing the data out here, we store it into the struct
    if (buffer && nmemb && userp) {
        MyCustomStruct *pMyStruct = (MyCustomStruct *)userp;

        // Reads from struct passed in from main
        cout << "Sucessfully queried page at URL: " << pMyStruct->url << endl;

        // Writes to struct passed in from main
        cout << "Storing received buffer into custom struct..." << endl;
        pMyStruct->response = new char[nmemb + 1];
        strncpy(pMyStruct->response, (char *)buffer, nmemb);
        pMyStruct->response[nmemb] = '\n'; // NULL-terminate pMyStruct->response

        pMyStruct->size = nmemb;
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
        char targetURL[] = "http://portal.cvst.ca/api/0.1/ttc/geojson";

        // Creating instance of custom object (to be supplied to callback)
        MyCustomStruct myStruct;

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);

        // Specify pointer to data that will be passed to our custom callback
        if (res == CURLE_OK)
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);

        // "Input" data to pass into custom callback
        myStruct.url = targetURL;

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }

        cout << endl << endl;
        if (res == CURLE_OK) {
            cout << "Back in main" << endl;
            cout << "Received buffer within struct is " <<
                myStruct.size << " bytes:" << endl;
            cout << "====================" << endl << endl;

            // "Output" data received from custom callback
            cout << myStruct.response << endl << endl;
            cout << "====================" << endl;
            cout << "End of buffer reached" << endl << endl;

            cout << "All good! res == CURLE_OK!" << endl;
        } else {
            cout << "ERROR: res == " << res << endl;
            cout << errbuf << endl;
        }

        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }

    curl_global_cleanup();

    return 0;
}
