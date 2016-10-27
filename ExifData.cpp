/*
 * ExifData.cpp
 *	Implementation of Exif data file.
 *
 *  Created on: October 1, 2016
 *      Author: Isaiah Torain
 */
#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include "ExifData.h"

using namespace std;

IFDEntry::IFDEntry() {
	this->mTag = -1;
	this->mFieldType = -1;
	this->mCount = -1;
	this->mOffsetPtr = -1;
	//this->mSubIFD = NULL;
}

// IFDEntry::~IFDEntry() {
// 	if (this->mSubIFD != NULL) {
// 		delete this->mSubIFD;
// 		this->mSubIFD = NULL;
// 	}
// }

// Calculated field using the count and size of field type. NOT PORTABLE
void IFDEntry::setLength() {
	switch (this->mFieldType) {
		case 0x1:
			this->mLength = this->mCount * sizeof(uint8_t); //
			break;
		case 0x2:
			this->mLength = this->mCount * sizeof(byte);
			break;
		case 0x3:
			this->mLength = this->mCount * sizeof(uint16_t);
			break;
		case 0x4:
			this->mLength = this->mCount * sizeof(uint32_t);
			break;
		case 0x5:
			this->mLength = this->mCount * (2 * sizeof(uint32_t));
			break;
		case 0x6:
			this->mLength = this->mCount * sizeof(int8_t);
			break;
		case 0x7:
			this->mLength = this->mCount * sizeof(byte);
			break;
		case 0x8:
			this->mLength = this->mCount * sizeof(int16_t);
			break;
		case 0x9:
			this->mLength = this->mCount * sizeof(int32_t);
			break;
		case 0xa:
			this->mLength = this->mCount * (2 * sizeof(uint32_t));
			break;
		case 0xb:
			this->mLength = this->mCount * sizeof(float);
			break;
		case 0xc:
			this->mLength = this->mCount * sizeof(double);
			break;
		default:
			this->mLength = this->mCount * sizeof(byte);
			break;
	}
}

void IFDEntry::parseIFDInfo(const byte* buffer, const byte* tiff_head) {
	this->mTag = static_cast<uint16_t>(hexToInt(buffer, this->mEndianFlag, 2));
	this->mRawTag[0] = buffer[0]; // first byte of the entry tag
	this->mRawTag[1] = buffer[1]; // second byte of the entry tag
	this->mFieldType = static_cast<uint16_t>(hexToInt(buffer + 2, this->mEndianFlag, 2));
	this->mCount = static_cast<uint32_t>(hexToInt(buffer + 4, this->mEndianFlag));
	this->mOffsetPtr = static_cast<uint32_t>(hexToInt(buffer + 8, this->mEndianFlag));
	this->mRawValueOffset[0] = buffer[8];
	this->mRawValueOffset[1] = buffer[9];
	this->mRawValueOffset[2] = buffer[10];
	this->mRawValueOffset[3] = buffer[11];
	setLength();
	parseValue(buffer + 8, tiff_head);
}

void IFDEntry::parseValue(const byte* buf, const byte* tiff_head) {
	switch (this->mFieldType) {
		case 0x1:
			this->mParsedUByte = vector<uint8_t>(this->mCount);
			parseUByte(buf, tiff_head);
			break;
		case 0x2:
			this->mParsedChar = string();
			parseChar(buf, tiff_head);
			break;
		case 0x3:
			this->mParsedUShort = vector<uint16_t>(this->mCount);
			parseUShort(buf, tiff_head);
			break;
		case 0x4:
			this->mParsedULong = vector<uint32_t>(this->mCount);
			parseULong(buf, tiff_head);
			break;
		case 0x5:
			this->mParsedRational = vector<Rational>();
			parseRational(buf, tiff_head);
			break;
		case 0x6:
			this->mParsedByte = vector<int8_t>(this->mCount);
			parseByte(buf, tiff_head);
			break;
		case 0x7:
			this->mParsedChar = string();
			parseChar(buf, tiff_head);
			break;
		case 0x8:
			this->mParsedShort = vector<int16_t>(this->mCount);
			parseShort(buf, tiff_head);
			break;
		case 0x9:
			this->mParsedLong = vector<int32_t>(this->mCount);
			parseLong(buf, tiff_head);
			break;
		case 0xa:
			this->mParsedRational = vector<Rational>();
			parseRational(buf, tiff_head);
			break;
		case 0xb:
			this->mParsedFloat = vector<float>(this->mCount);
			parseFloat(buf, tiff_head);
			break;
		case 0xc:
			this->mParsedDouble = vector<double>(this->mCount);
			parseDouble(buf, tiff_head);
			break;
		default:
			this->mParsedUByte = vector<uint8_t>(this->mCount);
			parseUByte(buf, tiff_head);
			break;
	}
}

void IFDEntry::parseUByte(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		this->mParsedUByte.push_back(static_cast<uint8_t>(this->mOffsetPtr));
		//buf
		//uint8_t = hexToInt()
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			this->mParsedUByte.push_back(buf[i]);
		}
	}
}

void IFDEntry::parseUShort(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		this->mParsedUShort.push_back(static_cast<uint16_t>(this->mOffsetPtr));
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			uint16_t val = static_cast<uint16_t>(hexToInt(buf, this->mEndianFlag, 2));
			this->mParsedUShort.push_back(val);
			buf += 2;
		}
	}
}

void IFDEntry::parseULong(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		this->mParsedULong.push_back(static_cast<uint32_t>(this->mOffsetPtr));
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			uint32_t val = static_cast<uint32_t>(hexToInt(buf, this->mEndianFlag));
			this->mParsedULong.push_back(val);
			buf += 4;
		}
	}
}

void IFDEntry::parseChar(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		for (int i = 0; i < this->mCount; i++) {
			this->mParsedChar.push_back(buf[i]);
		}
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			this->mParsedChar.push_back(buf[i]);
		}
	}
}

void IFDEntry::parseRational(const byte* buf, const byte* tiff_head) {
	buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
	for (int i = 0; i < this->mCount; i++) {
		Rational r;
		r.numerator = static_cast<uint32_t>(hexToInt(buf, this->mEndianFlag));
		r.denominator = static_cast<uint32_t>(hexToInt(buf + 4, this->mEndianFlag));
		this->mParsedRational.push_back(r);
		buf += 8;
	}
}

void IFDEntry::parseFloat(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		float val = hexToFloat(this->mRawValueOffset, this->mEndianFlag);
		this->mParsedFloat.push_back(val);
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			float val = hexToFloat(buf, this->mEndianFlag);
			this->mParsedUShort.push_back(val);
			buf += 4;
		}
	}
}

void IFDEntry::parseDouble(const byte* buf, const byte* tiff_head) {
	buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
	for (int i = 0; i < this->mCount; i++) {
		double num;
		copy(buf, buf + sizeof(double), reinterpret_cast<byte*>(&num));
		this->mParsedDouble.push_back(num);
		buf += 8;
	}
}

void IFDEntry::parseByte(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		this->mParsedByte.push_back(static_cast<int8_t>(this->mOffsetPtr));
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			this->mParsedByte.push_back(buf[i]);
		}
	}
}

void IFDEntry::parseShort(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		this->mParsedShort.push_back(static_cast<int16_t>(this->mOffsetPtr));
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			int16_t val = static_cast<int16_t>(hexToInt(buf, this->mEndianFlag, 2));
			this->mParsedShort.push_back(val);
			buf += 2;
		}
	}
}

void IFDEntry::parseLong(const byte* buf, const byte* tiff_head) {
	if (this->mLength <= 4) {
		this->mParsedLong.push_back(static_cast<int32_t>(this->mOffsetPtr));
	}
	else {
		buf = tiff_head + this->mOffsetPtr; // Offset of data values are from start of file
		for (int i = 0; i < this->mCount; i++) {
			int32_t val = static_cast<int32_t>(hexToInt(buf, this->mEndianFlag));
			this->mParsedLong.push_back(val);
			buf += 4;
		}
	}
}

IFD::IFD() {
	this->mOffset = 0;
	this->mNumEntries = 0;
	this->mNextIFD = 0;
	this->mContainsSubDirectories = false;
}

IFD::IFD(const unsigned int ptr, bool endianFlag, const byte* tiff_head) {
	const byte* entry_head = tiff_head + ptr;
	uint16_t numOfEntries = static_cast<uint16_t>(hexToInt(entry_head, endianFlag, 2));
	entry_head += (2 + (numOfEntries * 12));
	this->mOffset = ptr;
	this->mEndianFlag = endianFlag;
	this->mNumEntries = numOfEntries;
	this->mNextIFD = hexToInt(entry_head, this->mEndianFlag);
	this->mContainsSubDirectories = false;
}

bool IFD::readEntries(const byte* data, const byte* tiff_head) {
	bool success = true;
	this->mEntries = vector<IFDEntry>();
	int i = 0;
	if (this->mNumEntries == 0) {
		success = false;
		return success;
	}
	while (i < this->mNumEntries) {
		IFDEntry entry = IFDEntry();
		entry.mEndianFlag = this->mEndianFlag;
		entry.parseIFDInfo(data, tiff_head);
		if (entry.mTag == 0x8825 || entry.mTag == 0x8769) {
			this->mContainsSubDirectories = true;
		}
		this->mEntries.push_back(entry);
		data += 12;
		i++;
	}
	return success;
}

//**** Constructors ****//

ExifData::ExifData() {
    this->mSize = 0;
    this->mData = NULL;
}

//**************************//

//**** Desstructor ****//

ExifData::~ExifData() {

    delete this->mData;
	this->mData = NULL;
}

//**********************//

//****** Member Functions ******//

// Read in all data to memory
bool ExifData::readIn(ifstream& istr) {
	byte item;

	this->mSize = istr.tellg();
	this->mData = new byte[this->mSize];
	istr.seekg(0, ios::beg);
	istr.read((char*)this->mData, this->mSize); //read in entire file into memory have to cast to byte to use read
	this->mData = (byte*)this->mData; // cast back to unsigned characters

	if (istr.fail()) {
		cerr << "Error: binary reading in failed. " << endl;
		return false;
	}

	if (!this->readHeader(istr, this->mData)) {
        return false;
	}
    else {
	    return true;
    }
}

// Function to read in exif header data
bool ExifData::readHeader(ifstream& istr, byte* const block) {
	bool success = false;
	byte* currentByteValue = block; // Keep a pointer to the memory location
    byte little_endian [] = {'I', 'I'}; // Tiff specification for little-endian
    byte big_endian [] = {'M', 'M'}; // Tiff specification for big-endian
    byte tiff_identifier [] = {'4', '2'}; // How to identify Tiff spec

    if (block[0] == little_endian[0] && block[1] == little_endian[1]) {
        this->mEndianFlag = true; // True is for little endian
    }
    else if (block[0] == big_endian[0] && block[1] == big_endian[1]) {
        this->mEndianFlag = false; // false is for big endian
    }

    if (!block[2] == tiff_identifier[0] && !block[3] == tiff_identifier[1]) {
        cerr << "Error: This is not a tiff file!" << endl; // change this because byte order could be different
        return success;
    }

	// grab the offset
	currentByteValue += 4;
	int offset = hexToInt(currentByteValue, this->mEndianFlag);
    if (offset == -1) {
        cerr << "Error: Received an invalid offset!" << endl;
        return success;
    }
	else {
		success = readIFDs(offset);
	}
    return success;
}

bool ExifData::readIFDs(int offset_ptr) {
	bool success = true;
	byte* currentByteValue = this->mData + offset_ptr;
	IFD temp = IFD((unsigned int)offset_ptr, this->mEndianFlag, this->mData);
	currentByteValue += 2;
	if (!temp.readEntries(currentByteValue, this->mData)) {
		cerr << "Error parsing IFD! Data may be corrupt." << endl;
		success = false;
		return success;
	}
	unsigned int nextIFDPtr = temp.mNextIFD;
	this->mIFDs.push_back(temp);
	if (temp.mContainsSubDirectories) {
		if (!readSubIFDs(temp)) {
			cerr << "Error parsing sub IFD! Data may be corrupt." << endl;
			success = false;
			return success;
		}
	}
	while (nextIFDPtr != 0) {
		temp = IFD((unsigned int)nextIFDPtr, this->mEndianFlag, this->mData);
		currentByteValue = this->mData + nextIFDPtr + 2;
		if (!temp.readEntries(currentByteValue, this->mData)) {
			cerr << "Error parsing IFD! Data may be corrupt." << endl;
			success = false;
			break;
		}
		nextIFDPtr = temp.mNextIFD;
		this->mIFDs.push_back(temp);
		if (temp.mContainsSubDirectories) {
			if (!readSubIFDs(temp)) {
				cerr << "Error parsing sub IFD! Data may be corrupt." << endl;
				success = false;
				return success;
			}
		}
	}
	return success;
}

bool ExifData::readSubIFDs(const IFD& subIFD) {
	bool success = true;
	for (auto& entry : subIFD.mEntries) {
		if (entry.mTag == 0x8825 || entry.mTag == 0x8769) {
			// Create a sub IFD and parse it's entries.
			IFD temp = IFD(entry.mOffsetPtr, this->mEndianFlag, this->mData);
			if (!temp.readEntries(this->mData + entry.mOffsetPtr + 2, this->mData)) {;
				cerr << "Error reading in sub IFD!" << endl;
				success = false;
				break;
			}
			this->mIFDs.push_back(temp);
		}
	}
	return success;
}

//******************************//

int hexToInt(const byte* data, bool flag, int size) {
	int num = -1;
	if (flag) { // litte endian
		if (size == 2) {
			num = ((data[1] << 8) | data[0]);
		}
		else {
			num = ((data[3]  << 24) |
					(data[2] << 16) |
					(data[1] << 8) |
					(data[0]));
		}

	}
	else { // big endian
		if (size == 2) {
			num = ((data[0] << 8) | data[1]);
		}
		else {
			num = ((data[0]  << 24) |
					(data[1] << 16) |
					(data[2] << 8) |
					(data[3]));
		}
	}
	return num;
}

float hexToFloat(const byte* data, bool flag) {
	float num;
	if (flag) { // little endian
		*((byte*)(&num) + 3) = data[0];
		*((byte*)(&num) + 2) = data[1];
		*((byte*)(&num) + 1) = data[2];
		*((byte*)(&num) + 0) = data[3];
		return num;
	}
	else { // big endian
		*((byte*)(&num) + 3) = data[3];
		*((byte*)(&num) + 2) = data[2];
		*((byte*)(&num) + 1) = data[1];
		*((byte*)(&num) + 0) = data[0];
		return num;
	}
}
;
