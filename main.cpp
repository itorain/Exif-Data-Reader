#include <iostream>
#include <fstream>
#include <memory>
#include <stdlib.h>

#include "ExifData.h"

using namespace std;

//typedef unsigned char byte;
const int header_size = 8;

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
    cout << gps_data << endl;
    // int size = data->mFirstIFD.mEntries.size();
    // for (int i = 0; i < size; i++) {
    //     cout << data->mFirstIFD.mEntries.at(i).mRawTag << endl;
    // }
    // int j = 22;
    // for (int i = 10; i < 134; i++) {
    //     if (i == j) {
    //         cout << endl;
    //         j+=12;
    //     }
    //     cout << hex << (int)data->mData[i] << " ";
    // }
    // cout << endl;
    // // for (int i = 134; i < 226; i++) {
    // //     cout << data->mData[i] << " ";
    // // }
    // // cout << endl;
    // // for (int i = 226; i < 318; i++) {
    // //     cout << data->mData[i] << " ";
    // // }
    // //cout << endl;
    // // for (int i = 356; i < 360; i++) {
    // //     cout << hex << (int)data->mData[i] << " ";
    // // }
    // // IFD for Exifdata mostly camera settings
    // cout << "IFD for camera settings " << endl;
    // j=368;
    // for (int i = 356; i < 780; i++) {
    //     if (i == j) {
    //         cout << endl;
    //         j+=12;
    //     }
    //     cout << hex << (int)data->mData[i] << " ";
    // }
    // cout << endl;
    // cout << "IFD for GPS" << endl;
    // //IFD for GPS data
    // j=1032;
    // for (int i = 1020; i < 1132; i++) {
    //     if (i == j) {
    //         cout << endl;
    //         j+=12;
    //     }
    //     cout << hex << (int)data->mData[i] << " ";
    // }
    // cout << endl;
    // for (int i = 1132; i < 1156; i++) {
    //     cout << hex << (int)data->mData[i] << " ";
    // }
    // // Other IFD in the photo
    // cout << "Other IFD" << endl;
    // j=1250;
    // for (int i = 1238; i < 1338; i++) {
    //     if (i == j) {
    //         cout << endl;
    //         j+=12;
    //     }
    //     cout << hex << (int)data->mData[i] << " ";
    // }
    // cout << endl;

    return 0;
}
