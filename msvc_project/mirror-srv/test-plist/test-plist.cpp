// test-plist.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Plist.hpp>
//#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;

#pragma comment(lib,"libplistcpp.lib")

static void createMessage(map<string, boost::any>& dict)
{
	Plist::Date date;
	date.setTimeFromAppleEpoch(338610664);

	dict["testDate"] = date;
	dict["testInt"] = int(-3455);
	dict["testInt32"] = int32_t(-3455);
	dict["testInt64"] = int64_t(-3455);
	dict["testShort"] = short(3455);
	dict["testLong"] = long(-3455);
	dict["testDouble"] = 1.34223;
	dict["testFloat"] = 1.34223f;
	dict["testBoolTrue"] = true;
	dict["testBoolFalse"] = false;
	std::ifstream stream("testImage.png", std::ios::binary);
	if (!stream){
		//
		//throw std::runtime_error("Can't open file: testImage.png");
	}
	else{
		int start = stream.tellg();
		stream.seekg(0, std::ifstream::end);
		int size = ((int)stream.tellg()) - start;
		std::vector<char> actualData(size);
		if (size > 0)
		{
			stream.seekg(0, std::ifstream::beg);
			stream.read((char *)&actualData[0], size);
		}
		else
		{
			throw std::runtime_error("Can't read zero length data");
		}

		dict["testImage"] = actualData;
	}

	vector<boost::any> array(2);
	array[0] = 34;
	array[1] = string("string item in array");
	dict["testArray"] = array;
	dict["testString"] = string("hello there");
	dict["testEmptyString"] = string("");

	map<string, boost::any> innerDict;
	innerDict["test string"] = string("inner dict item");
	dict["testDict"] = innerDict;

	innerDict.clear();
	array.resize(256);

	for (int i = 0; i < 256; ++i)
	{
		stringstream ss;
		if (i < 10)
			ss << "00";
		if ((i >= 10) && (i < 100))
			ss << "0";
		ss << i;
		array[i] = i;
		innerDict[ss.str()] = i;
	}
	dict["testArrayLarge"] = array;
	dict["testDictLarge"] = innerDict;

}

void	test2()
{
	//
	map<string, boost::any> dict;
	dict["height"] = 1080;
	dict["width"] = 1920;
	dict["overscanned"] = 0;
	dict["refreshRate"] = (double)0.016666666666666666;
	dict["version"] = 150.33;
	dict["height"] = 1080;

	Plist::writePlistXML(std::cout, dict);
	
}
int _tmain(int argc, _TCHAR* argv[])
{
	test2();
	map<string, boost::any> dict;
	createMessage(dict);
	return 0;
}

