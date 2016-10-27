#include <iostream>
#include <fstream>
#include <memory>
#include <stdlib.h>

#include "ExifData.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Check args
    if (argc < 2) {
        cerr << "Usage: ./exifRead filename" << endl;
        return -1;
    }

    // Open up file in binary mode at the end of the file. Do this to use tellg to get size.
    ifstream file (argv[1], ios::in | ios::binary | ios::ate);

    // Allocate a pointer to an ExifData structure on the heap
    shared_ptr<ExifData> data (new ExifData);

    // Read in data
    if (!data->readIn(file)) {
        cerr << "Main received error code exiting." << endl;
        return -1;
    }

    // Print out the GPS Coordinates
    Geolocation gps_data;
    gps_data.LatComponents.direction = data->mIFDs[2].mEntries[0].mParsedChar[0]; // Latitude North or South South lat is negative
    gps_data.LatComponents.degrees = (double)data->mIFDs[2].mEntries[1].mParsedRational[0]; // latitude degrees
    gps_data.LatComponents.minutes = (double)data->mIFDs[2].mEntries[1].mParsedRational[1]; // latitude degrees
    gps_data.LatComponents.seconds = (double)data->mIFDs[2].mEntries[1].mParsedRational[2]; // latitude degrees
    gps_data.latitude(); // Calculates Latitude
    gps_data.LonComponents.direction = data->mIFDs[2].mEntries[2].mParsedChar[0]; // Longitude West or East. West long is negative
    gps_data.LonComponents.degrees = (double)data->mIFDs[2].mEntries[3].mParsedRational[0]; // longitude degrees
    gps_data.LonComponents.minutes = (double)data->mIFDs[2].mEntries[3].mParsedRational[1]; // longitude degrees
    gps_data.LonComponents.seconds = (double)data->mIFDs[2].mEntries[3].mParsedRational[2]; // longitude degrees
    gps_data.longitude(); // Calculates Longitude
    gps_data.AltitudeRef = data->mIFDs[2].mEntries[2].mOffsetPtr; // Use this value -1 for below sea level 0 for above
    gps_data.Altitude = (double)data->mIFDs[2].mEntries[5].mParsedRational[0]; // Altitude
    if (gps_data.AltitudeRef == -1) {
        gps_data.Altitude = -gps_data.Altitude;
    }

    // Print out Coordinates
    cout << gps_data << endl;
    return 0;
}
