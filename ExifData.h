#ifndef EXIFDATA_H
#define EXIFDATA_H

#include <vector>
#include <string>
#include <cstdint>
#include <ostream>
#include <stdio.h>

typedef unsigned char byte;

// Struct to hold a rational value so that is can be easily cast to a double
struct Rational {
	uint32_t numerator, denominator;
	operator double() const {
		if (denominator < 1e-20) {
  			return 0;
		}
		return static_cast<double>(numerator) / static_cast<double>(denominator);
	}
};

// Struct to hold GPS data. Used to hold information in a nice friendly way
struct Geolocation {
	double Latitude;
	double Longitude;
	double Altitude;
	char AltitudeRef;
	struct Coordinate {
		double degrees;
		double minutes;
		double seconds;
		char direction;
	} LatComponents, LonComponents;
	double latitude() {
		Latitude = LatComponents.degrees + (LatComponents.minutes / 60) + (LatComponents.seconds / 3600);
		if (LatComponents.direction == 'S') {
			Latitude = -Latitude;
		}
		return Latitude;
	}
	double longitude() {
		Longitude = LonComponents.degrees + (LonComponents.minutes / 60) + (LonComponents.seconds / 3600);
		if (LonComponents.direction == 'W') {
			Longitude = -Longitude;
		}
		return Longitude;
	}
	friend std::ostream& operator << (std::ostream& os, const Geolocation& g) {
		byte x = 167;
		os << "GPS Coordinates:\tLatitude\tLongitude\tAltitude\t\n";
		os << "Values:         \t" << g.Latitude << "  \t" << g.Longitude << "    \t" << g.Altitude << "\n";
		os << "DMS Latitude: " << g.LatComponents.direction  << " " << g.LatComponents.degrees << "\u00B0" << " " << g.LatComponents.minutes << "\' " << g.LatComponents.seconds << "\"\n";
		os << "DMS Longitude: " << g.LonComponents.direction << " " << g.LonComponents.degrees << "\u00B0 " << g.LonComponents.minutes << "\' " << g.LonComponents.seconds << "\"\n";
		return os;
	}
};

// Class to hold the byte order (i.e. little endian or big endian)
class Base {
public:
	bool mEndianFlag; // Endianness flag. True is Little Endian and False is Big
};

// Class to represent the 12 byte Image file directory entries.
class IFDEntry : public Base {
public:

//**** Member Variables ****//

	uint16_t mTag; // Tag value
	byte mRawTag [2]; // Raw tag stored in 2 bytes
	uint16_t mFieldType; // The field type of the data. Determines size of data
	uint32_t mCount; // The number of the specified field type.
    uint32_t mOffsetPtr; // Offset from beginning of data to where value starts. OR the actual data value
	byte mRawValueOffset [4]; // 4 byte offset value needed to compute when value is stored in here
	unsigned int mLength; // Length of the value in the number of bytes
	// Vectors of different types to hold different value fieldtype values
	// Apparently I can not declare a generic vector here (vector<T> mParsedValue)
	std::vector<uint8_t> mParsedUByte;
	std::vector<uint16_t> mParsedUShort;
	std::vector<uint32_t> mParsedULong;
	std::string mParsedChar;
	std::vector<Rational> mParsedRational;
	std::vector<float> mParsedFloat;
	std::vector<double> mParsedDouble;
	std::vector<int8_t> mParsedByte;
	std::vector<int16_t> mParsedShort;
	std::vector<int32_t> mParsedLong;

//**************************//

//**** Constructors ****//

	IFDEntry(); // Default empty constructor

//**********************//

//**** Functions to operate on data ****//

uint16_t tag() const { return mTag; }
void tag(uint16_t tag) { mTag = tag; }
uint16_t fieldType() const { return mFieldType; }
void fieldType(uint16_t field_type) { mFieldType = field_type; };
uint32_t count() const { return mCount; }
void count(int count) { mCount = count; }
uint32_t offsetVal() const { return mOffsetPtr; }
void offsetVal(int offsetVal) { mOffsetPtr = offsetVal; }
unsigned int length() const { return mLength; }
void setLength(); // Set the length of the vector based on the count data size

void parseIFDInfo(const byte*, const byte*); // Sets entry data values
void parseValue(const byte*, const byte*); // helper function to pass data to parse function below

void parseUByte(const byte*, const byte*); // Parse unsigned byte values
void parseUShort(const byte*, const byte*); // Parse unsigned short values
void parseULong(const byte*, const byte*); // Parse unsigned 4 byte Long
void parseChar(const byte*, const byte*); // Parse unsigned char values
void parseRational(const byte*, const byte*); // Parse 2 long values into Rational struct (signed or unsigned)
void parseFloat(const byte*, const byte*); // Parse 4 byte float values
void parseDouble(const byte*, const byte*); // Parse 8 byte double values
void parseByte(const byte*, const byte*); // Parse signed 1 byte values
void parseShort(const byte*, const byte*); // Parse signed  2 byte short values
void parseLong(const byte*, const byte*); // Parse signed 4 byte long values

//**************************************//
private:
	const int mSize = 12; // size of every entry
};

// Class to represent the Image File Directories. Contains IFDEntry entries.
class IFD : public Base {
public:

//**** Member Variables ****//

	unsigned int mOffset; // What byte position from the beginning of the file that this starts.
	uint16_t mNumEntries; // 2 byte count of the number of entries.
	std::vector<IFDEntry> mEntries; // will be of size count*12;
	uint32_t mNextIFD; // 4 btye offset to the next IFD from beginning of file.
	bool mContainsSubDirectories; // Whether or not this IFD holds sub IFDs
	// add a generic to hold the type

//**************************//

//**** Constructors ****//

	IFD(); // default constructor
	IFD(unsigned int, bool, const byte*); // Initialize with endianness, and set the number of entries

//**********************//

//**** Functions to operate on data ****//

	bool readEntries(const byte*, const byte*); // Add Image File Directory entries to vector

//**************************************//
};

class ExifData : public Base {
public:

//**** Member Variables ****//

	byte* mData; // Pointer to data stored on heap for faster access.
    unsigned long mSize; // Size of the metadata
	std::vector<IFD> mIFDs; // Container for parsed IFDs

//**************************//

//**** Constructors/Desstructor ****//

	ExifData(); // Default constructor values are set via readIn
	~ExifData(); // Desstructor to delete data

//**********************//

//**** Member functions ****//

	bool readIn(std::ifstream&); // Read in data to memory
	bool readHeader(std::ifstream&, byte*); // Parse header
	bool readIFDs(int); // Parse Image file directory
	bool readSubIFDs(const IFD&); // Parse sub IFDs like GPS and Exif


//**************************************//
};

int hexToInt(const byte*, bool, int size = 4); // Parse 2 or 4 bytes to get an integer
float hexToFloat(const byte*, bool); // parse bytes to floats

#endif //ExifData
