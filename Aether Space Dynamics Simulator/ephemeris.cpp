#include "stdafx.h"
#include "ephemeris.h"

Ephemeris_Class::Ephemeris_Class()
{
}

Ephemeris_Class::Ephemeris_Class(const Ephemeris_Class& other)
{
}


Ephemeris_Class::~Ephemeris_Class()
{
}

void Ephemeris_Class::Initialize()
{
	std::string months_name[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	date_ = int(time(NULL) / 86400.0);
	calculations_epoch = 2440587.5 + date_;
	calculations_epoch_b = 0.0;
	calculations_epoch_c = calculations_epoch - 2456971.5;

	JdToYmd(long(calculations_epoch + 0.5), &year, &month, &day);
	if (day<10) date_eph = std::to_string(year) + "-" + months_name[month - 1] + "-" + "0" + std::to_string(day);
	else date_eph = std::to_string(year) + "-" + months_name[month - 1] + "-" + std::to_string(day);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//								Converts a Julian date into integers of Gregorian years, months and days											//
void Ephemeris_Class::JdToYmd(const long lJD, int *piYear, int *piMonth, int *piDay)
{
#ifndef JULDATE_USE_ALTERNATE_METHOD

	long a, b, c, d, e, z, alpha;

	z = lJD;
	if (z < 2299161L)
		a = z;
	else
	{
		alpha = (long)((z - 1867216.25) / 36524.25);
		a = z + 1 + alpha - alpha / 4;
	}
	b = a + 1524;
	c = (long)((b - 122.1) / 365.25);
	d = (long)(365.25 * c);
	e = (long)((b - d) / 30.6001);
	*piDay = (int)b - d - (long)(30.6001 * e);
	*piMonth = (int)(e < 13.5) ? e - 1 : e - 13;
	*piYear = (int)(*piMonth > 2.5) ? (c - 4716) : c - 4715;
	if (*piYear <= 0)
		*piYear -= 1;

#else

	long t1, t2, yr, mo;

	t1 = lJD + 68569L;
	t2 = 4L * t1 / 146097L;
	t1 = t1 - (146097L * t2 + 3L) / 4L;
	yr = 4000L * (t1 + 1L) / 1461001L;
	t1 = t1 - 1461L * yr / 4L + 31L;
	mo = 80L * t1 / 2447L;
	*piDay = (int)(t1 - 2447L * mo / 80L);
	t1 = mo / 11L;
	*piMonth = (int)(mo + 2L - 12L * t1);
	*piYear = (int)(100L * (t2 - 49L) + yr + t1);

	// Correct for BC years
	if (*piYear <= 0)
		*piYear -= 1;

#endif

	return;
}
//																																					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool Ephemeris_Class::UpdateEphemeris(std::string ephemeris_filename, double &eccentricity_, double &inclination_, double &LAN_,
	double &AP_, double &TA_, double &SMA_, bool &ephemeris_ok_)
{
	double phi_d = 3.1415926535897932384626433832795;
	std::string eccentricity_eph = "";
	std::string inclination_eph = "";
	std::string LAN_eph = "";
	std::string AP_eph = "";
	std::string TA_eph = "";
	std::string SMA_eph = "";

	bool keep_checking = true;
	bool analize_data = false;
	bool data_found = false;
	char input;
	std::ifstream ephemeris_file;
	ephemeris_file.open(ephemeris_filename);

	// If it could not open the file then exit.
	if (ephemeris_file.fail()) return false;

	if (ephemeris_file.is_open())
	{
		while (!ephemeris_file.eof() && keep_checking)
		{
			ephemeris_file.get(input);

			if (input == date_eph.at(0))
			{
				ephemeris_file.get(input);

				if (input == date_eph.at(1))
				{
					ephemeris_file.get(input);

					if (input == date_eph.at(2))
					{
						ephemeris_file.get(input);

						if (input == date_eph.at(3))
						{
							ephemeris_file.get(input);

							if (input == date_eph.at(4))
							{
								ephemeris_file.get(input);

								if (input == date_eph.at(5))
								{
									ephemeris_file.get(input);

									if (input == date_eph.at(6))
									{
										ephemeris_file.get(input);

										if (input == date_eph.at(7))
										{
											ephemeris_file.get(input);

											if (input == date_eph.at(8))
											{
												ephemeris_file.get(input);

												if (input == date_eph.at(9))
												{
													ephemeris_file.get(input);

													if (input == date_eph.at(10))
													{

														analize_data = true;
														while (analize_data)
														{
															ephemeris_file.get(input);
															if (input == 'E')
															{
																ephemeris_file.get(input);
																if (input == 'C')
																{
																	ephemeris_file.get(input);
																	if (input == '=')
																	{
																		ephemeris_file.get(input);
																		for (int n = 0; n < 22; n++)
																		{
																			ephemeris_file.get(input);
																			eccentricity_eph += input;
																		}
																	}
																}
															}


															if (input == 'I')
															{
																ephemeris_file.get(input);
																if (input == 'N')
																{
																	ephemeris_file.get(input);
																	if (input == '=')
																	{
																		ephemeris_file.get(input);
																		for (int n = 0; n < 22; n++)
																		{
																			ephemeris_file.get(input);
																			inclination_eph += input;
																		}
																	}
																}
															}


															if (input == 'O')
															{
																ephemeris_file.get(input);
																if (input == 'M')
																{
																	ephemeris_file.get(input);
																	if (input == '=')
																	{
																		ephemeris_file.get(input);
																		for (int n = 0; n < 22; n++)
																		{
																			ephemeris_file.get(input);
																			LAN_eph += input;
																		}
																	}
																}
															}

															if (input == 'W')
															{
																ephemeris_file.get(input);
																if (input == ' ')
																{
																	ephemeris_file.get(input);
																	if (input == '=')
																	{
																		ephemeris_file.get(input);
																		for (int n = 0; n < 22; n++)
																		{
																			ephemeris_file.get(input);
																			AP_eph += input;
																		}
																	}
																}
															}


															if (input == 'T')
															{
																ephemeris_file.get(input);
																if (input == 'A')
																{
																	ephemeris_file.get(input);
																	if (input == '=')
																	{
																		ephemeris_file.get(input);
																		for (int n = 0; n < 22; n++)
																		{
																			ephemeris_file.get(input);
																			TA_eph += input;
																		}
																	}
																}
															}

															if (input == 'A')
															{
																ephemeris_file.get(input);
																if (input == ' ')
																{
																	ephemeris_file.get(input);
																	if (input == '=')
																	{
																		ephemeris_file.get(input);
																		for (int n = 0; n < 22; n++)
																		{
																			ephemeris_file.get(input);
																			SMA_eph += input;
																		}
																		analize_data = false;
																		keep_checking = false;
																		data_found = true;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}


		if (data_found)
		{
			ephemeris_ok_ = true;
			eccentricity_ = stod(eccentricity_eph);
			inclination_ = stod(inclination_eph)* (phi_d / 180.0);
			LAN_ = stod(LAN_eph)* (phi_d / 180.0);
			AP_ = stod(AP_eph)* (phi_d / 180.0);
			TA_ = stod(TA_eph)* (phi_d / 180.0);
			SMA_ = stod(SMA_eph)* 149597870700.0;
		}

		ephemeris_file.close();
	}

	return true;
}