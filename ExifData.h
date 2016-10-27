#ifndef EXIFDATA_H
#define EXIFDATA_H

#include <vector>
#include <string>
#include <cstdint>
#include <ostream>
#include <stdio.h>

typedef unsigned char byte;

struct Rational {
	uint32_t numerator, denominator;
	operator double() const {
		if (denominator < 1e-20) {
  			return 0;
		}
		return static_cast<double>(numerator) / static_cast<double>(denominator);
	}
};

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
	bool mEndianFlag;
};

// Class to represent the 12 byte Image file directory entries.
class IFDEntry : public Base {
public:

//**** Member Variables ****//

	uint16_t mTag;
	byte mRawTag [2];
	uint16_t mFieldType; //
	uint32_t mCount; // The number of the specified field type.
    uint32_t mOffsetPtr;
	byte mRawValueOffset [4];
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

	IFDEntry();
	//~IFDEntry();

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
void setLength();

void parseIFDInfo(const byte*, const byte*);
void parseValue(const byte*, const byte*);

void parseUByte(const byte*, const byte*);
void parseUShort(const byte*, const byte*);
void parseULong(const byte*, const byte*);
void parseChar(const byte*, const byte*);
void parseRational(const byte*, const byte*);
void parseFloat(const byte*, const byte*);
void parseDouble(const byte*, const byte*);
void parseByte(const byte*, const byte*);
void parseShort(const byte*, const byte*);
void parseLong(const byte*, const byte*);




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
	bool mContainsSubDirectories;
	// add a generic to hold the type

//**************************//

//**** Constructors ****//

	IFD();
	IFD(unsigned int, bool, const byte*);
	//~IFD();

//**********************//

	void setNumEntries();
	bool readEntries(const byte*, const byte*);

//**** Functions to operate on data ****//

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

	ExifData();
	~ExifData();

//**********************//

//**** Member functions ****//

	bool readIn(std::ifstream&);
	bool readHeader(std::ifstream&, byte*);
	bool readIFDs(int);
	bool readSubIFDs(const IFD&);


//**************************************//
};

int hexToInt(const byte*, bool, int size = 4); // Parse 2 or 4 bytes
float hexToFloat(const byte*, bool);

#endif //ExifData
