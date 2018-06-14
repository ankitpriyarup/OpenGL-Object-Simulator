#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

class CSVWriter
{
private:
	std::string fileName;
	std::string delimeter;
	int linesCount;

public:
	CSVWriter(std::string filename, std::string delm = ",") :
		fileName(filename), delimeter(delm), linesCount(0) {}

	template<typename T>
	void addDatainRow(T first, T last);
};

template<typename T>
inline void CSVWriter::addDatainRow(T first, T last)
{
	std::fstream file;
	file.open(fileName, std::ios::out | (linesCount ? std::ios::app : std::ios::trunc));

	for (; first != last; )
	{
		file << *first;
		if (++first != last)
			file << delimeter;
	}
	file << "\n";
	linesCount++;
	file.close();
}