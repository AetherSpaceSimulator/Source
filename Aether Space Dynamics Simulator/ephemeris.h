#ifndef _EPHEMERIS_H_
#define _EPHEMERIS_H_

#include "stdafx.h"

class Ephemeris_Class
{
public:
	Ephemeris_Class();
	Ephemeris_Class(const Ephemeris_Class&);
	~Ephemeris_Class();

	void Initialize();
	void JdToYmd(const long lJD, int *piYear, int *piMonth, int *piDay);
	bool UpdateEphemeris(std::string ephemeris_filename, double &eccentricity_, double &inclination_, double &LAN_,
		double &AP_, double &TA_, double &SMA_, bool &ephemeris_ok_);

	double get_calculations_epoch_c(){ return calculations_epoch_c; };

private:
	int year;
	int month;
	int day;
	std::string date_eph;
	double calculations_epoch;
	double calculations_epoch_b;
	double calculations_epoch_c;
	double date_;
};

#endif