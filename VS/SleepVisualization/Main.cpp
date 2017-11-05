 /****************************************************************
	FILE:			Main.cpp
	AUTHOR:			Tommy Deng
					u/sttic

	NOTES:			What was I thinking with this? It's so messy.
					It makes me wonder if I was even thinking at
					all ... ever.
 ****************************************************************/



#include "CImg.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cimg_library;

class SleepData
{
private:
	string date_start, time_start;
	string date_end, time_end;
public:
	SleepData()
	{
		date_start = "YYYY-MM-DD";
		time_start = "HH:MM";

		date_end = "YYYY-MM-DD";
		time_end = "HH:MM";
	}

	SleepData(string date_start, string time_start, string date_end, string time_end)
	{
		this->date_start = date_start;
		this->time_start = time_start;

		this->date_end = date_end;
		this->time_end = time_end;
	}

	string get_date_start() { return date_start; }
	string get_time_start() { return time_start; }
	string get_date_end() { return date_end; }
	string get_time_end() { return time_end; }

	void set_date_start(string date_start) { this->date_start = date_start; }
	void set_time_start(string time_start) { this->time_start = time_start; }
	void set_date_end(string date_end) { this->time_end = time_end; }
	void set_time_end(string time_end) { this->time_end = time_end; }

	// YYYY-MM-DD
	void parse_date()
	{
		date_start.replace(4, 1, "-");
		date_start.replace(7, 1, "-");

		date_end.replace(4, 1, "-");
		date_end.replace(7, 1, "-");
	}

	void parse_date_crossover()
	{
		int hour_start = atoi(time_start.substr(0, 2).c_str());
		int hour_end = atoi(time_end.substr(0, 2).c_str());

		if (
				(time_start.substr(time_start.size() - 1 - 1, 2) == "pm"
					&& time_end.substr(time_end.size() - 1 - 1, 2) == "am")
			||
				(time_start.substr(time_start.size() - 1 - 1, 2) == "pm"
					&& time_end.substr(time_end.size() - 1 - 1, 2) == "pm"
					&& hour_end < hour_start
					&& hour_start != 12)
			||
				(time_start.substr(time_start.size() - 1 - 1, 2) == "pm"
					&& time_end.substr(time_end.size() - 1 - 1, 2) == "pm"
					&& hour_end == 12)
			)
		{
			int year, month, day;
			year = atoi(date_end.substr(0, 4).c_str());
			month = atoi(date_end.substr(5, 2).c_str());
			day = atoi(date_end.substr(8, 2).c_str());

			// correct leap year rules?
			bool leap = (year % 4 == 0 && (year % 400 != 0 || year % 1000 == 0));
			int num_of_days;

			// increment the date_end to the next date
			if (month == 2)
				if (!leap)
					num_of_days = 28;
				else
					num_of_days = 29;
			else if (month == 4 || month == 6 || month == 9 || month == 11)
				num_of_days = 30;
			else
				num_of_days = 31;

			if (day < num_of_days)
				day++;
			else if (month < 12)
			{
				month++;
				day = 1;
			}
			else
			{
				year++;
				month = 1;
				day = 1;
			}

			// adding placeholder 0 if num a single digit number
			stringstream m, d;
			if (month < 10)
				m << "0" << month;
			else
				m << month;

			if (day < 10)
				d << "0" << day;
			else
				d << day;

			stringstream end;
			end << year << "-" << m.str() << "-" << d.str();

			date_end = end.str();
		}
	}

	// TODO: merge parse_time_start() and parse_time_end() into single
	// - the code is virtually the same; very inefficient to keep them seperate

	// 24H instead of 12H
	void parse_time_start()
	{
		string hour_start = time_start.substr(0, time_start.find(":"));
		string min_start = time_start.substr(time_start.find(":") + 1, 2);

		if (time_start.substr(time_start.size() - 1 - 1, 2) == "pm"
			&& time_start.substr(0, 2) != "12")
		{
			int hour = atoi(hour_start.c_str());
			hour += 12;

			stringstream ssHour;
			ssHour << hour;
			hour_start = ssHour.str();
		}

		if (time_start.substr(time_start.size() - 1 - 1, 2) == "am"
			&& time_start.substr(0, 2) == "12")
		{
			hour_start = "00";
		}

		stringstream ss;
		if (hour_start.size() < 2)
			ss << "0";
		ss << hour_start << ":" << min_start;

		time_start = ss.str();
	}

	// 24H instead of 12H
	void parse_time_end()
	{
		string hour_end = time_end.substr(0, time_end.find(":"));
		string min_end = time_end.substr(time_end.find(":") + 1, 2);

		if (time_end.substr(time_end.size() - 1 - 1, 2) == "pm"
			&& time_end.substr(0, 2) != "12")
		{
			int hour = atoi(hour_end.c_str());
			hour += 12;

			stringstream ssHour;
			ssHour << hour;
			hour_end = ssHour.str();
		}

		if (time_end.substr(time_end.size() - 1 - 1, 2) == "am"
			&& time_end.substr(0, 2) == "12")
		{
			hour_end = "00";
		}

		stringstream ss;
		if (hour_end.size() < 2)
			ss << "0" << hour_end << ":" << min_end;
		else
			ss << hour_end << ":" << min_end;

		time_end = ss.str();
	}

	// e.g. 24:30 -> 00:30
	void parse_anomaly()
	{
		if (time_start.substr(0, 2) == "24")
			time_start.replace(0, 2, "00");
		else if (time_end.substr(0, 2) == "24")
			time_end.replace(0, 2, "00");
	}
	// ISO 8601: YYYY-MM-DDTHH:MM
	string toString()
	{
		stringstream ss;
		ss << date_start << "T" << time_start << ", " << date_end << "T" << time_end;

		return ss.str();
	}
};

class FileInfo
{
private:
	vector<string> entry;
	vector<SleepData> data;
public:
	FileInfo() {}

	void loadFile()
	{
		ifstream myFile;
		myFile.open("sleep_tracker.txt");

		if (myFile.fail())
		{
			cerr << "Error opening file, \"sleep_tracker.txt\"." << endl;
			exit(1);
		}

		if (myFile.is_open())
		{
			while (!myFile.eof())
			{
				string input;
				myFile >> input;

				entry.push_back(input);
				//data.push_back(SleepData(input));
			}
			myFile.close();
		}
	}

	void initEntries()
	{
		// loop through vector of entries
		for (unsigned int i = 0; i < entry.size(); i++)
		{
			// loop through 'characters' of each entry
			for (unsigned int j = 0; j < entry[i].size(); j++)
			{
				// find where there's a time_start to time_end (the middle of one, between)
				if (entry[i].substr(j, 1) == "-")
				{
					string date_start = entry[i].substr(0, 10);

					// find beginning of start_time (first "_" left of "-")
					int start_pos;
					unsigned int k = j;
					while (k > 0 && entry[i].substr(k, 1) != "_")
					{
						start_pos = k;
						k--;
					}

					int end_pos;
					k = j;
					while (k < entry[i].size() && entry[i].substr(k, 1) != "_" && entry[i].substr(k, 1) != ";")
					{
						end_pos = k;
						k++;
					}

					string time_start = entry[i].substr(start_pos, j - start_pos);
					string time_end = entry[i].substr(j + 1, end_pos - j);

					// the second date_start (in the date_end parameter) is a default/placeholder value
					data.push_back(SleepData(date_start, time_start, date_start, time_end));
				}
			}
		}
	}

	void parse()
	{
		for (unsigned int i = 0; i < data.size(); i++)
		{
			data[i].parse_date();
			data[i].parse_date_crossover();
			data[i].parse_time_start();
			data[i].parse_time_end();
			data[i].parse_anomaly();
		}
	}

	void initRecord(vector<vector<string>>& record)
	{
		for (unsigned int i = 0; i < data.size(); i++)
		{
			stringstream start, end;
			start << data[i].get_date_start() << "T" << data[i].get_time_start();
			end << data[i].get_date_end() << "T" << data[i].get_time_end();

			vector<string> entry2;
			entry2.push_back(start.str());
			entry2.push_back(end.str());

			record.push_back(entry2);

			// output all entries
			//cout << data[i].toString() << endl;
		}

		// when populating the image, the record vector would go out of bounds (by one)
		vector<string> dodgyFix(2, "END");
		record.push_back(dodgyFix);
	}

	void outputRecord(const vector<vector<string>> record)
	{
		// output record
		for (unsigned int i = 0; i < record.size(); i++)
		{
			for (unsigned int j = 0; j < record[i].size(); j++)
			{
				if (j == 0)
					cout << record[i][j] << ", ";
				else
					cout << record[i][j];
			}
			cout << endl;
		}

		cout << endl;
	}

	void writeRecordFile(const vector<vector<string>> record)
	{
		ofstream myFile;
		myFile.open("data.txt");

		if (myFile.fail())
		{
			cerr << "Error opening file, \"data.txt\"." << endl;
			exit(1);
		}

		if (myFile.is_open())
		{
			// output record
			for (unsigned int i = 0; i < record.size() - 1; i++)	// the -1 to exclude last dummy entry
			{
				for (unsigned int j = 0; j < record[i].size(); j++)
				{
					if (j == 0)
						myFile << record[i][j] << ", ";
					else
						myFile << record[i][j];
				}
				if (i < record.size() - 2) { myFile << endl; }
			}
		}
	
	}

	unsigned int getNumEntries() {	return entry.size(); }
};

class Image
{
private:
	int record_size;
	int line_height;
	int x_record;
	int y_record;

	CImg<unsigned char> img;
public:
	Image() {}

	Image(const vector<vector<string>> record, int line_height, int right_padding)
	{
		record_size = record.size();
		this->line_height = line_height;
		x_record = (24 * 60) + right_padding;					// minutes in a day ( + extra padding for month info)
		y_record = record_size * line_height;

		img = CImg<unsigned char>(x_record, y_record, 1, 3);
		img.fill(128);
	}

	void populate(const vector<vector<string>> record, int numDays)
	{
		cout << "Populating image; crawling ... This may take a few seconds." << endl;

		// TODO: make the colours part of the private group
		// debug/guide colours

		/*const unsigned char black[] = { 0,0,0 };
		const unsigned char red[] = { 255,0,0 };
		const unsigned char green[] = { 0,255,0 };
		const unsigned char blue[] = { 0,0,255 };
		const unsigned char purple[] = { 255,0,255 };

		const unsigned char orange_light[] = { 255,176,59 };
		const unsigned char orange_dark[] = { 173,119,40 };
		const unsigned char orange_light_dark[] = { 242,167,56 };
		const unsigned char orange_dark_dark[] = { 164,113,38 };
		
		const unsigned char blue_light[] = { 0,163,136 };
		const unsigned char blue_dark[] = { 0,111,92 };
		const unsigned char blue_light_dark[] = { 0,155,129 };
		const unsigned char blue_dark_dark[] = { 0,105,87 };*/


		// recolour to final version (keeping the mismatched variable names)
		const unsigned char black[] = { 0,0,0 };
		const unsigned char red[] = { 229,229,229 };				// light grey - even hour
		const unsigned char green[] = { 225,225,225 };				// darker grey - odd hour
		const unsigned char blue[] = { 196,196,196 };				// light grey2 - DEC
		const unsigned char purple[] = { 128,128,128 };				// dark grey - JAN

		const unsigned char orange_light[] = { 229,229,229 };		// grey shine light (even hour)
		const unsigned char orange_dark[] = { 225,225,225 };		// grey shadow light (odd hour)
		const unsigned char orange_light_dark[] = { 220,220,220 };	// grey shine dark (even hour)
		const unsigned char orange_dark_dark[] = { 215,215,215 };	// grey shadow dark (odd hour)

		const unsigned char blue_light[] = { 68,140,193 };			// blue shine light (even hour)
		const unsigned char blue_dark[] = { 58,134,189 };			// blue shadow light (odd hour)
		const unsigned char blue_light_dark[] = { 36,103,162 };		// blue shine dark (even hour)
		const unsigned char blue_dark_dark[] = { 32,95,154 };		// blue shadow dark (odd hour)



		// for the black and white version (heat map)
		//const unsigned char orange_light[] = { 0,0,0 };
		//const unsigned char orange_dark[] = { 0,0,0 };
		//const unsigned char orange_light_dark[] = { 0,0,0 };
		//const unsigned char orange_dark_dark[] = { 0,0,0 };

		//const unsigned char blue_light[] = { 255,255,255 };
		//const unsigned char blue_dark[] = { 255,255,255 };
		//const unsigned char blue_light_dark[] = { 255,255,255 };
		//const unsigned char blue_dark_dark[] = { 255,255,255 };

		// start date-time; [0][0]
		int year, month, day;
		year = atoi(record[0][0].substr(0, 4).c_str());
		month = atoi(record[0][0].substr(5, 2).c_str());
		day = atoi(record[0][0].substr(8, 2).c_str());

		// position in record vector
		int x_vRec = 0;
		int y_vRec = 0;

		// canvas position
		int x_pos = 0;
		int y_pos = 0;

		string time;
		bool awake = true;

		int entry, hour, minute;

		bool fillAlternator = true;
		int buffer_month = month;
		img.draw_rectangle(24 * 60, 0, x_record, y_record, blue);	// preliminary padding fill

		// loop through each day/entry
		for (entry = 0; entry < numDays; entry++)
		{
			for (hour = 0; hour < 24; hour++)
			{
				for (minute = 0; minute < 60; minute++)
				{
					time = intISO_toString(year, month, day, hour, minute);

					// add new month guides to side (64 pixel padding)
					if (buffer_month != month)
					{
						if (fillAlternator)
							img.draw_rectangle(24 * 60, y_pos, x_record, y_record, purple);
						else
							img.draw_rectangle(24 * 60, y_pos, x_record, y_record, blue);
						fillAlternator = !fillAlternator;

						stringstream ss;
						ss << year << "-";
						if (month < 10) ss << "0";
						ss << month;
						img.draw_text(24 * 60 + 10, y_pos + 10, ss.str().c_str(), black);

						buffer_month = month;
					}

					// toggle awake status whenever the time crosses time_start or time_end
					if (time == record[x_vRec][y_vRec])
					{
						awake = !awake;

						if (y_vRec == 0)
						{
							y_vRec++;
						}
						else
						{
							x_vRec++;
							y_vRec = 0;
						}

					}

					if (awake)
					{
						if (hour % 2 == 0)
						{
							img.draw_rectangle(x_pos, y_pos, x_pos, y_pos + line_height - 1, orange_light);
							img.draw_rectangle(x_pos, y_pos + line_height, x_pos, y_pos + line_height, orange_dark);
						}
						else
						{
							img.draw_rectangle(x_pos, y_pos, x_pos, y_pos + line_height - 1, orange_light_dark);
							img.draw_rectangle(x_pos, y_pos + line_height, x_pos, y_pos + line_height, orange_dark_dark);
						}
					}
					else
					{
						if (hour % 2 == 0)
						{
							img.draw_rectangle(x_pos, y_pos, x_pos, y_pos + line_height - 1, blue_light);
							img.draw_rectangle(x_pos, y_pos + line_height, x_pos, y_pos + line_height, blue_dark);
						}
						else
						{
							img.draw_rectangle(x_pos, y_pos, x_pos, y_pos + line_height - 1, blue_light_dark);
							img.draw_rectangle(x_pos, y_pos + line_height, x_pos, y_pos + line_height, blue_dark_dark);
						}

					}

					x_pos++;
				}
			}

			//cout << "R\t" << record[x_vRec][y_vRec] << endl;

			x_pos = 0;
			y_pos += line_height + 1;

			incrDate(year, month, day);
		}

		// add hour key on bottom
		for (int h = 0; h < 24; h++)
		{
			for (int m = 0; m < 60; m++)
			{
				if (h % 2 == 0)
				{
					img.draw_rectangle(x_pos, y_pos, x_pos, y_pos + y_record, red);
				}
				else
				{
					img.draw_rectangle(x_pos, y_pos, x_pos, y_pos + y_record, green);
				}

				x_pos++;
			}
		}

	}

	void incrDate(int& year, int& month, int& day)
	{
		bool leap = (year % 4 == 0 && (year % 400 != 0 || year % 1000 == 0));
		int num_of_days;

		if (month == 2)
			if (!leap)
				num_of_days = 28;
			else
				num_of_days = 29;
		else if (month == 4 || month == 6 || month == 9 || month == 11)
			num_of_days = 30;
		else
			num_of_days = 31;

		if (day < num_of_days)
			day++;
		else if (month < 12)
		{
			month++;
			day = 1;
		}
		else
		{
			year++;
			month = 1;
			day = 1;
		}
	}

	string intISO_toString(int year, int month, int day, int hour, int minute)
	{
		stringstream ss;
		ss << year << "-";

		if (month < 10)
			ss << "0";
		ss << month << "-";

		if (day < 10)
			ss << "0";
		ss << day << "T";

		if (hour < 10)
			ss << "0";
		ss << hour << ":";

		if (minute < 10)
			ss << "0";
		ss << minute;

		return ss.str();
	}

	void save()
	{
		img.normalize(0, 255);
		img.save("file.bmp");
	}

	void display()
	{
		img.display("Sleep Visualization [OC], u/sttic");
	}
	
};

class Analysis
{
private:
	int numDays;

	vector<int> totalSleep;
	vector<string> date;

	vector<vector<vector<vector<int>>>> vWeek;		// week -> day(7) -> hour(24) -> minute(60) : int awake (0 or 1)
													// average the values within a certain period (i.e. all mondays from 09h-10h)
													// result: statistical liklihood of being awake (0.0 = 0%, 1.0 = 100%)
	vector<vector<double>> day_average;

public:
	Analysis() { numDays = 0; }

	Analysis(int numDays) { this->numDays = numDays; }

	void crawl(const vector<vector<string>> record)
	{
		cout << "Analyzing data; crawling ... This may take a few seconds." << endl;
		cout << endl;

		vector<int> vMinute(60, -1);
		vector<vector<int>> vHour(24, vMinute);
		vector<vector<vector<int>>> vDay(7, vHour);

		int numDaysMod = 0;
		if (numDays % 7 != 0) numDaysMod = 1;
		for (int i = 0; i < (numDays / 7) + numDaysMod; i++)
			vWeek.push_back(vDay);

		// start date-time; [0][0]
		int year, month, day;
		year = atoi(record[0][0].substr(0, 4).c_str());
		month = atoi(record[0][0].substr(5, 2).c_str());
		day = atoi(record[0][0].substr(8, 2).c_str());

		// position in record vector
		int x_vRec = 0;
		int y_vRec = 0;

		string time;
		bool awake = true;

		int entry, hour, minute;

		// counters (keep track of statistics)
		int count_week = 0;		// first week of sleep tracking, 2016-12-12
		int count_day = 0;

		int dailySleep = 0;

		// loop through each day/entry
		for (entry = 0; entry < numDays; entry++)
		{
			count_day++;
			for (hour = 0; hour < 24; hour++)
			{
				for (minute = 0; minute < 60; minute++)
				{
					time = intISO_toString(year, month, day, hour, minute);

					// toggle awake status whenever the time crosses time_start or time_end
					if (time == record[x_vRec][y_vRec])
					{
						awake = !awake;

						if (y_vRec == 0)
						{
							y_vRec++;
						}
						else
						{
							x_vRec++;
							y_vRec = 0;
						}

					}

					if (awake)
					{
						vWeek[count_week][count_day - 1][hour][minute] = 1;
					}
					else
					{
						dailySleep++;
						vWeek[count_week][count_day - 1][hour][minute] = 0;
					}
					
				}
			}

			//cout << "R\t" << record[x_vRec][y_vRec] << endl;

			if (count_day == 7)
			{
				count_week++;
				count_day = 0;
			}


			date.push_back(time.substr(0, 10));
			totalSleep.push_back(dailySleep);
			dailySleep = 0;

			incrDate(year, month, day);
		}

	}

	// [2017 - 07 - 12] average sleep per day of week(i.e.average sleep gotten on mondays)
	void countDayOfWeek()
	{
		const int DAYS_PER_WEEK = 7;

		// vector of each day of week containing both the total sleep and total occurances of that day
		// type double is (inefficiently) used to allow for the array to hold the average
		vector<double> data(3, 0);									// total sleep per 'day of week', total 'day of week' occurances, average ([0]/[1])
		vector<vector<double>> day_of_week(DAYS_PER_WEEK, data);	// [7] days of the week (MON, TUE, WED, ...)
																	//	- total sleep for each day of the week (i.e. all mondays)
																	//	- number of days of the week (i.e. number of mondays)

		// fill data vector with base number of 'day of week' occurances
		for (int i = 0; i < DAYS_PER_WEEK; i++)
		{
			day_of_week[i][1] = numDays / DAYS_PER_WEEK;
		}

		// add remainder to total 'day of week' occurances
		int remainder = numDays % DAYS_PER_WEEK;

		for (int i = 0; i < remainder; i++)
		{
			day_of_week[i][1]++;
		}

		ofstream myFile;
		myFile.open("day_of_week.txt");

		// determine total sleep per day of week (from total daily sleep) and average
		for (int i = 0; i < DAYS_PER_WEEK; i++)
		{
			for (unsigned int j = i; j < totalSleep.size(); j += 7)
			{
				day_of_week[i][0] += totalSleep[j];
			}

			// average
			day_of_week[i][2] = day_of_week[i][0] / day_of_week[i][1];

			// output
			cout << i << ", " << day_of_week[i][2];
			cout << endl;

			// write to file
			myFile << i << ", " << day_of_week[i][2];
			if (i != 6) myFile << endl;
		}

		cout << endl;
	}

	void incrDate(int& year, int& month, int& day)
	{
		bool leap = (year % 4 == 0 && (year % 400 != 0 || year % 1000 == 0));
		int num_of_days;

		if (month == 2)
			if (!leap)
				num_of_days = 28;
			else
				num_of_days = 29;
		else if (month == 4 || month == 6 || month == 9 || month == 11)
			num_of_days = 30;
		else
			num_of_days = 31;

		if (day < num_of_days)
			day++;
		else if (month < 12)
		{
			month++;
			day = 1;
		}
		else
		{
			year++;
			month = 1;
			day = 1;
		}
	}

	string intISO_toString(int year, int month, int day, int hour, int minute)
	{
		stringstream ss;
		ss << year << "-";

		if (month < 10)
			ss << "0";
		ss << month << "-";

		if (day < 10)
			ss << "0";
		ss << day << "T";

		if (hour < 10)
			ss << "0";
		ss << hour << ":";

		if (minute < 10)
			ss << "0";
		ss << minute;

		return ss.str();
	}

	void displayTotalSleep()
	{
		for (unsigned int i = 0; i < totalSleep.size(); i++)
			cout << date[i] << ", " << totalSleep[i] << endl;

		cout << endl;
	}

	void writeFileTotalSleep()
	{
		ofstream myFile;
		myFile.open("totalDailySleep.txt");
		if (myFile.is_open())
		{
			for (unsigned int i = 0; i < totalSleep.size(); i++)
			{
				myFile << date[i] << ", " << totalSleep[i];
				if (i < totalSleep.size() - 1) { myFile << endl; }
			}
		}
	}

	vector<vector<vector<vector<int>>>> getWeek()
	{
		return vWeek;
	}

	void heat_map_hour()
	{
		vector<double> count_hour(24, 0);
		vector<vector<double>> count_day(7, count_hour);
		vector<vector<vector<double>>> total_count(2, count_day);

		for (unsigned int week = 0; week < vWeek.size(); week++)
		{
			for (int day = 0; day < 7; day++)
			{
				for (int hour = 0; hour < 24; hour++)
				{
					for (int minute = 0; minute < 60; minute++)
					{
						if (vWeek[week][day][hour][minute] != -1)
						{
							// counts minutes awake (i.e. counts up for mondays at 09:00)
							total_count[0][day][hour] += vWeek[week][day][hour][minute];		
							// counts total minutes passed (on particular [day][time])
							total_count[1][day][hour]++;

						}
					}
				}
			}
		}

		ofstream myFile;
		myFile.open("heat_map.txt");

		vector<double> hour_average(24);
		for (int i = 0; i < 7; i++)
			day_average.push_back(hour_average);

		if (myFile.is_open())
		{
			for (int day = 0; day < 7; day++)
			{
				cout << day << ", ";
				myFile << day << ", ";
				for (int hour = 0; hour < 24; hour++)
				{
					day_average[day][hour] = total_count[0][day][hour] / total_count[1][day][hour];

					cout << day_average[day][hour];	
					myFile << day_average[day][hour];

					if (hour != 23) cout << ", ";
					if (hour != 23) myFile << ", ";
				}
				cout << endl;
				if (day != 6) myFile << endl;
			}
		}

		cout << endl;
	}

	void heat_map_minute()
	{
		vector<double> count_min(60, 0);
		vector<vector<double>> count_hour(24, count_min);
		vector<vector<vector<double>>> count_day(7, count_hour);
		vector<vector<vector<vector<double>>>> total_count(2, count_day);

		// since being awake is stored as a 1, and asleep as 0
		// instead of incrementing a counter whenever it's a 1
		// you can more simply sum all of them (add them all)

		for (unsigned int week = 0; week < vWeek.size(); week++)
		{
			for (int day = 0; day < 7; day++)
			{
				for (int hour = 0; hour < 24; hour++)
				{
					for (int minute = 0; minute < 60; minute++)
					{
						if (vWeek[week][day][hour][minute] != -1)
						{
							total_count[0][day][hour][minute] += vWeek[week][day][hour][minute];
							total_count[1][day][hour][minute]++;

						}
					}
				}
			}
		}

		ofstream myFile;
		myFile.open("heat_map_minute.txt");

		vector<double> min_average(60, 0);
		vector<vector<double>> hour_average(24, min_average);
		vector<vector<vector<double>>> weekday_average(7, hour_average);

		if (myFile.is_open())
		{
			for (int day = 0; day < 7; day++)
			{
				cout << day << ", ";
				myFile << day << ", ";
				for (int hour = 0; hour < 24; hour++)
				{
					for (int minute = 0; minute < 60; minute++)
					{
						weekday_average[day][hour][minute] = total_count[0][day][hour][minute] / total_count[1][day][hour][minute];

						cout << weekday_average[day][hour][minute];
						myFile << weekday_average[day][hour][minute];

						if (minute != 59) cout << ", ";
						if (minute != 59) myFile << ", ";

					}
					if (hour != 23) cout << ", ";
					if (hour != 23) myFile << ", ";
				}
				cout << endl;
				if (day != 6) myFile << endl;
			}
		}

		cout << endl;
	}

	void outputVWeek()
	{
		int i = 0;

		for (unsigned int week = 0; week < vWeek.size(); week++)
		{
			for (int day = 0; day < 7; day++)
			{
				for (int hour = 0; hour < 24; hour++)
				{
					for (int minute = 0; minute < 60; minute++)
					{
						stringstream ss;

						ss << "#";
						if (i < 10) ss << "00000";
						else if (i < 100) ss << "0000";
						else if (i < 1000) ss << "000";
						else if (i < 10000) ss << "00";
						else if (i < 100000) ss << "0";
						ss << i << "\t";

						ss << "W";
						if (week < 10) ss << "0";
						ss << week;

						ss << "D";
						if (day < 10) ss << "0";
						ss << day;

						ss << "h";
						if (hour < 10) ss << "0";
						ss << hour;

						ss << "m";
						if (minute < 10) ss << "0";
						ss << minute;

						ss << "\t";
						ss << vWeek[week][day][hour][minute];

						cout << ss.str() << endl;

						i++;
					}
				}
			}
		}
	}

	void writeFileVWeek()
	{
		ofstream myFile;
		myFile.open("week.txt");
		if (myFile.is_open())
		{
			unsigned int i = 0;

			for (unsigned int week = 0; week < vWeek.size(); week++)
			{
				for (int day = 0; day < 7; day++)
				{
					for (int hour = 0; hour < 24; hour++)
					{
						for (int minute = 0; minute < 60; minute++)
						{
							myFile << week << "\t"
								<< day << "\t"
								<< hour << "\t"
								<< minute << "\t"
								<< vWeek[week][day][hour][minute];
							if (i < (vWeek.size() * 7 * 24 * 60) - 1) myFile << endl;
							i++;
						}
					}
				}
			}
		}
	}
};

int main() {
	
	vector<vector<string>> record;
	FileInfo data;

	data.loadFile();
	data.initEntries();
	data.parse();
	data.initRecord(record);
		data.outputRecord(record);
		data.writeRecordFile(record);

	int numDays, line_height;
	numDays = data.getNumEntries();
	line_height = 7;

	Analysis analysis = Analysis(numDays);
	analysis.crawl(record);
		analysis.displayTotalSleep();

	analysis.heat_map_hour();
	analysis.heat_map_minute();

				//analysis.outputVWeek();
				analysis.writeFileVWeek();		// not useful
		analysis.writeFileTotalSleep();

	analysis.countDayOfWeek();
	
	Image img = Image(record, line_height - 1, 60);
	img.populate(record, numDays);
		img.save();
	img.display();

	
}