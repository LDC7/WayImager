#pragma once
#include <Windows.h>
#include "Dirent.h"
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Snapshot.h"
#include "exif.h"
using namespace cv;
using namespace std;

class FileManager
{
public:

	string InputDir;

	string PathDir;

	string ResultDir;

	vector<string> Filenames;

	FileManager() { }

	FileManager(string inputDir, string pathDir, string resDir)
	{
		this->InputDir = inputDir;
		this->PathDir = pathDir;
		this->ResultDir = resDir;
		MakeDirectory(inputDir);
		MakeDirectory(pathDir);
		MakeDirectory(resDir);
		GetFilenames();
	}

	void SaveImage(Mat image, string directory, string filename)
	{
		string path = directory + filename;
		imwrite(path, image);
	}

	Mat OpenImage(string directory, string filename)
	{
		string path = directory + filename;
		return imread(path, IMREAD_COLOR);
	}

	Snapshot LoadSnapshot(int index)
	{
		Snapshot snapshot;
		snapshot.Image = OpenImage(InputDir, Filenames[index]);
		snapshot.Index = index;
		snapshot.Filename = Filenames[index];
		
		easyexif::EXIFInfo info = GetExifInfo(InputDir, Filenames[index]);
		
		//заменить на exif-тег
		snapshot.Altitude = 500;

		double focal_length = 28;
		double matrix_width = 36;
		double matrix_height = 24;
		double pi = 3.14159265;

		snapshot.AlphaX = (2 * atan(matrix_width / (2 * focal_length))) * 180 / pi;
		snapshot.AlphaY = (2 * atan(matrix_height / (2 * focal_length))) * 180 / pi;
		return snapshot;
	}

private:

	easyexif::EXIFInfo GetExifInfo(string directory, string filename)
	{
		// Read the JPEG file into a buffer
		FILE *fp = fopen((directory + filename).c_str(), "rb");
		if (!fp) {
			//printf("Can't open file.\n");
		}
		fseek(fp, 0, SEEK_END);
		unsigned long fsize = ftell(fp);
		rewind(fp);
		unsigned char *buf = new unsigned char[fsize];
		if (fread(buf, 1, fsize, fp) != fsize) {
			//printf("Can't read file.\n");
			delete[] buf;
		}
		fclose(fp);

		// Parse EXIF
		easyexif::EXIFInfo result;
		int code = result.parseFrom(buf, fsize);
		delete[] buf;
		if (code) {
			//printf("Error parsing EXIF: code %d\n", code);
		}

		//printf("GPS Latitude         : %f deg (%f deg, %f min, %f sec %c)\n",
		//	result.GeoLocation.Latitude, result.GeoLocation.LatComponents.degrees,
		//	result.GeoLocation.LatComponents.minutes,
		//	result.GeoLocation.LatComponents.seconds,
		//	result.GeoLocation.LatComponents.direction);
		//printf("GPS Longitude        : %f deg (%f deg, %f min, %f sec %c)\n",
		//	result.GeoLocation.Longitude, result.GeoLocation.LonComponents.degrees,
		//	result.GeoLocation.LonComponents.minutes,
		//	result.GeoLocation.LonComponents.seconds,
		//	result.GeoLocation.LonComponents.direction);
		//printf("GPS Altitude         : %f m\n", result.GeoLocation.Altitude);
		return result;
	}

	void MakeDirectory(string directoryName)
	{
		bool res = false;
		res = CreateDirectory(directoryName.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
	}

	bool IsJpegFilename(const std::string &filename)
	{
		auto d_name_len = filename.size();
		return (
			d_name_len > 4
			&& filename[d_name_len - 4] == '.'
			&& filename[d_name_len - 3] == 'j'
			&& filename[d_name_len - 2] == 'p'
			&& filename[d_name_len - 1] == 'g'
			||
			d_name_len > 5
			&& filename[d_name_len - 5] == '.'
			&& filename[d_name_len - 4] == 'j'
			&& filename[d_name_len - 3] == 'p'
			&& filename[d_name_len - 2] == 'e'
			&& filename[d_name_len - 1] == 'g'
			);
	}

	bool IsPictureFilename(const std::string &filename)
	{
		auto d_name_len = filename.size();
		return (
			IsJpegFilename(filename)
			||
			d_name_len > 4
			&& filename[d_name_len - 4] == '.'
			&& filename[d_name_len - 3] == 'b'
			&& filename[d_name_len - 2] == 'm'
			&& filename[d_name_len - 1] == 'p'
			||
			d_name_len > 4
			&& filename[d_name_len - 4] == '.'
			&& filename[d_name_len - 3] == 'p'
			&& filename[d_name_len - 2] == 'n'
			&& filename[d_name_len - 1] == 'g'
			);
	}

	void GetFilenames()
	{
		DIR* dir;
		struct dirent *ent;
		if ((dir = opendir(InputDir.c_str())) != NULL)
		{
			int picture = 0;
			while ((ent = readdir(dir)) != NULL)
			{
				auto filename = std::string(ent->d_name);
				if (IsPictureFilename(filename))
				{
					Filenames.push_back(filename);
				}
			}
			closedir(dir);
		}
	}
};