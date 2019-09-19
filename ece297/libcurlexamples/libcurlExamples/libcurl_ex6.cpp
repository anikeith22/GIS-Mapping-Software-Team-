/* This example shows how to use Boost to parse JSON data and store it into
 * a Property Tree data structure. The tree can than be searched based on
 * keyword paths, so prior knowledge of the JSON data's schema is required.
 *
 * Further reading and references:
 *  https://www.boost.org/doc/libs/1_65_1/doc/html/property_tree.html
 *  http://www.ce.unipr.it/~medici/boost_ptree.html
 */
#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::read_json;

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
    if (buffer && nmemb && userp) {
        MyCustomStruct *pMyStruct = (MyCustomStruct *)userp;

        // Writes to struct passed in from main
        if (pMyStruct->response == nullptr) {
            // Case when first time write_data() is invoked
            pMyStruct->response = new char[nmemb + 1];
            strncpy(pMyStruct->response, (char *)buffer, nmemb);
        }
        else {
            // Case when second or subsequent time write_data() is invoked
            char *oldResp = pMyStruct->response;

            pMyStruct->response = new char[pMyStruct->size + nmemb + 1];

            // Copy old data
            strncpy(pMyStruct->response, oldResp, pMyStruct->size);

            // Append new data
            strncpy(pMyStruct->response + pMyStruct->size, (char *)buffer, nmemb);

            delete []oldResp;
        }

        pMyStruct->size += nmemb;
        pMyStruct->response[pMyStruct->size] = '\n';
    }

    return nmemb;
}

/* Boost uses the following JSON to property tree mapping rules:
 *   1) JSON objects are mapped to nodes. Each property is a child node.
 *   2) JSON arrays are mapped to nodes.
 *      Each element is a child node with an empty name. If a node has both
 *      named and unnamed child nodes, it cannot be mapped to a JSON representation.
 *   3) JSON values are mapped to nodes containing the value.
 *      However, all type information is lost; numbers, as well as the literals
 *      "null", "true" and "false" are simply mapped to their string form.
 *   4) Property tree nodes containing both child nodes and data cannot be mapped.
 */
void PrintTTCVehicleInfo(ptree &ptRoot) {
    string busName;
    int busID = 0;
    double longitude = 0, latitude = 0;

    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("features")) {
        // "features" maps to a JSON array, so each child should have no name
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";

        busName = featVal.second.get<string>("properties.route_name");
        busID = featVal.second.get<int>("properties.vehicle_id");

        // Get GPS coordinates (stored as JSON array of 2 values)
        // Sanity checks: Only 2 values
        ptree coordinates = featVal.second.get_child("geometry.coordinates");
        if (coordinates.size() != 2)
            throw "Coordinates node does not contain 2 items";

        longitude = coordinates.front().second.get_value<double>();
        latitude = coordinates.back().second.get_value<double>();

        // Print bus info
        cout << "Bus " << busName << " with ID " << busID <<
            " is at coordinates: " << longitude << ", " << latitude << endl;
    }

    return;
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
        MyCustomStruct myStruct;
        char targetURL[] = "http://portal.cvst.ca/api/0.1/ttc/geojson";

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);

        myStruct.url = targetURL;

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }

        cout << endl << endl;
        if (res == CURLE_OK) {
            // Create an empty proper tree
            ptree ptRoot;

            /* Store JSON data into a Property Tree
             *
             * read_json() expects the first parameter to be an istream object,
             * or derived from istream (e.g. ifstream, istringstream, etc.).
             * The second parameter is an empty property tree.
             *
             * If your JSON data is in C-string or C++ string object, you can
             * pass it to the constructor of an istringstream object.
             */
            istringstream issJsonData(myStruct.response);
            read_json(issJsonData, ptRoot);

            // Parsing and printing the data
            cout << "Current TTC vehicle locations are as follows:" << endl;
            cout << "====================" << endl << endl;
            try {
                PrintTTCVehicleInfo(ptRoot);
            } catch (const char *errMsg) {
                cout << "ERROR: Unable to fully parse the TTC JSON data" << endl;
                cout << "Thrown message: " << errMsg << endl;
            }
            cout << endl << "====================" << endl;
            cout << "Done!" << endl;
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
