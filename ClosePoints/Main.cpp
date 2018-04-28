#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include "FileManager.h"
#include "Filter.h"
#include "Drawer.h"
#include <fstream>
using namespace std;

void main()
{
	ofstream output;
	output.open("output.txt", ios::trunc);

	FileManager manager = FileManager(
		"./samples/input_snapshots/",
		"./samples/result_path/",
		"./samples/result_snapshots/"
	);

	Filter filter = Filter();
	filter.Manager = manager;
	
	filter.BlobSquare1Px = 0;
	filter.BlobSquare2Px = 20;
	filter.CorrWindowSize = 13;
	filter.MinCorrValue = 0.3;
	filter.MinMaxWindowSize = 7;
	filter.NeighbourRadius = 4;
	filter.WavePercent = 20;
	filter.ZonePercent = 100;
	filter.MaxTimeDead = 10;

	Drawer drawer = Drawer();
	drawer.BestTrianglesCount = 5;
	drawer.PointZonePercent = 10;

	int minLifeTime = 10;
	int correctionTime = 5;

	PointsInfo info;
	info.SumLifeTime = 0;
	info.NumberOfDeadPoints = 0;

	for (int i = 0; i < manager.Filenames.size(); i++)
	{
		info.MaxBlobsMap.release();
		info.MinBlobsMap.release();
		info.Blobs.clear();

		Snapshot snapshot = manager.LoadSnapshot(i);
		printf("%s\n", snapshot.Filename.c_str());

		filter.ProcessSnapshot(snapshot, info);
		
		Mat outImg = Mat(snapshot.Image);
		snapshot.Image.copyTo(outImg);
		cvtColor(outImg, outImg, CV_GRAY2RGB);

		drawer.DrawBlobs(info, outImg);
		drawer.DrawLines(info, outImg, minLifeTime);

		if ((i % correctionTime) == 0 && i > 0)
		{
			Mat fullImg;
			Mat oldImg = manager.OpenImage(manager.PathDir, manager.Filenames[i - correctionTime]);

			Mat pathImg = Mat(snapshot.Image);
			snapshot.Image.copyTo(pathImg);
			cvtColor(pathImg, pathImg, CV_GRAY2RGB);

			drawer.DrawPath(info, pathImg, fullImg, oldImg, correctionTime, snapshot);
			manager.SaveImage(fullImg, manager.PathDir, manager.Filenames[i]);
		}

		double averageLifeTime = info.SumLifeTime / info.NumberOfDeadPoints;
		printf("Average lifeTime: %f\n", averageLifeTime);
		output << snapshot.Filename << " (avg. lifeTime): " << averageLifeTime << endl;
		manager.SaveImage(outImg, manager.ResultDir, manager.Filenames[i]);
	}

	output.close();
	system("pause");
}