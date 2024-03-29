﻿#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\refraction.h"

#define EARTHRAD 6371000

/*
------------------------------------------------------
refraction.c

Given a pair of coordinates in a three-dimensional inertial reference frame,
one representing a point in space and the other near the Earth surface,
this program computes the apparent point near the Earth surface observed 
from the spacecraft, accounting for atmospheric refraction only.

ASSUMPTIONS:
1. The Earth is modeled as a sphere.
2. Local terrain is not taken into account.
3. Input coordinates are assumed to be defined with respect to
   an inertial reference frame with origin at the Earth Centre, such as the GCI.
4. Input coordinates are assumed to be in metres.

REFERENCES:
Noerdlinger, P. (1999): Atmospheric refraction effects in Earth remote sensing
Liu, N. (2018)        : Acquisition and Tracking Strategies for Satellite to Ground 
			Optical Communication Systems (Ryerson University MaSc thesis)
------------------------------------------------------
*/

/*
------------------------------------------------------
satProjection

PURPOSE:		Compute the point on the Earth surface representing 
			the projection of a point in space along the line connecting it to Earth centre
INPUT ARGUMENTS:	Coordinates of the point in space representing spacecraft position
OUTPUT ARGUMENTS:	Coordinates of the point on the Earth surface representing the satellite's projection
RETURNED VALUE:		None
FUNCTIONS CALLED:	None
VER./DATE:		1.0 30 Jan 2019
PROGRAMMER:		JFG
NOTES:
------------------------------------------------------
*/

void satProjection(double x, double y, double z, 
		double *satProj_x, double *satProj_y, double *satProj_z)
{
	static double alt_sat, t;

	/* Compute distance from spacecraft to Earth centre/origin */
	alt_sat = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	
	/* Compute the amount by which to scale each spacecraft coordinate to compute its projection onto Earth surface;
	   obtained by solving system of two geometric equations */
	t = EARTHRAD / alt_sat;
	
	/* Scale each spacecraft coordinate */
	*satProj_x = t * x;
	*satProj_y = t * y;
	*satProj_z = t * z;

	return;
}

/*
------------------------------------------------------
zenithAngle.c

PURPOSE:		Compute the zenith angle of the spacecraft and the distance 
			from the projection of the spacecraft onto Earth's surface to the unrefracted ground station
INPUT ARGUMENTS:	Coordinates of the spacecraft and unrefracted ground station
OUTPUT ARGUMENTS:	zenAng, *distGS2satProj
RETURNED VALUE:		zenAng
FUNCTIONS CALLED:	atan, pow, satProjection, sqrt
VER./DATE:		1.0 30 Jan 2019
PROGRAMMER:		JFG
NOTES:
------------------------------------------------------
*/

double zenithAngle(double x, double y, double z, double a, double b, double c, 
				   double *distGS2satProj)
{
	static double alt_sat, height_sat, satProj_x, satProj_y, satProj_z;
	double zenAng;

	satProjection(x, y, z, &satProj_x, &satProj_y, &satProj_z);
	printf("satProj_x in zenithAngle = %.16lf\nsatProj_y in zenithAngle is %.16lf\nsatProj_z in zenithAngle is %.16lf\n",
		satProj_x, satProj_y, satProj_z);

	/* Compute distance between spacecraft projection onto Earth surface and unrefracted ground station */
	*distGS2satProj = sqrt(pow((satProj_x - a), 2) + pow((satProj_y - b), 2) + pow((satProj_z - c), 2));

	/* Compute distance of spacecraft from Earth centre/origin */
	alt_sat = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	
	/* Compute distance of satellite and Earth surface */
	height_sat = alt_sat - EARTHRAD;

	/* Solve the triangle between the spacecraft, its projection onto the Earth surface, 
	   and the unrefracted ground station for the zenith angle of the spacecraft */
	zenAng = atan(*distGS2satProj / height_sat);

	return zenAng;
}

/*
------------------------------------------------------
refractiveIndex.c

PURPOSE:		Compute the refractive index of the atmosphere with altitude
REFERENCES:		(1) Noerdlinger (1999) p. 371
INPUT ARGUMENTS:	Coordinates of the location at which the refractive index is to be computed		
OUTPUT ARGUMENTS:	Refractive index of Earth atmosphere at input location
RETURNED VALUE:		mu (refractive index)
FUNCTIONS CALLED:	None
VER./DATE:		1.0 30 Jan 2019
PROGRAMMER:		JFG
NOTES:			Only the refractive index at the ground station is used in subsequent 
			computations. Therefore, the only model included is for locations in the troposphere.
------------------------------------------------------
*/

double refractiveIndex(double q, double r, double s)
{
	double Molec_mean, g_0, R_gas, alt_tropop, r_Ltrop, T_tropop, T_sealevel, alt_observer,
		tempFac, Gamma, densFac, mu;


	Molec_mean = 28.825;        // Mean tropospheric molecular weight
	g_0 = 9.805;                // Mean sea-level acceleration of gravity, in m/s^2
	R_gas = 8314.3;             // Ideal gas constant, in (J (k mol^-1) K^-1)
	alt_tropop = 10500;         // Approximate altitude of the tropopause, in metres
	r_Ltrop = 0.0065;           // Tropospheric temperature rate with altitude (lapse rate), in K/m
	T_tropop = 68.25;			// Mean temperature at the lowest point of tropopause, in K
	
	
	//T_sealevel = 288.115;		//Noerdlinger value for global mean sealevel temp (K)
	T_sealevel = 273.15;        // Mean 2018 temperature of Toronto is 282.066 (K)

	/* Compute distance of location in atmosphere from Earth centre */
	alt_observer = sqrt(pow(q, 2) + pow(r, 2) + pow(s, 2));

	/* Compute lapse rate of troposphere, i.e. change of temp with distance of satellite from geoid */
	tempFac = 1.0 - r_Ltrop * (alt_observer - EARTHRAD) / T_sealevel;

	Gamma = (Molec_mean * g_0) / (R_gas * r_Ltrop) - 1;

	densFac = pow(tempFac, Gamma);

	mu = 1.0 + (0.0002905 * densFac);

	return mu;
}

/*
------------------------------------------------------
deltaAngle.c

PURPOSE:	    Compute the angular displacement between refracted and unrefracted ground station coordinates	
INPUT ARGUMENTS:    Ground station and spacecraft coordinates (a,b,c) and (x,y,z)
OUTPUT ARGUMENTS:   Angular displacement, dAng
RETURNED VALUE:     dAng
FUNCTIONS CALLED:   asin, zenithAngle
VER./DATE:	    1.0 30 Jan 2019
PROGRAMMER:	    JFG
NOTES:

------------------------------------------------------
*/

double deltaAngle(double x, double y, double z, double a, double b, double c)
{
	double mu_0, xi_ang, dAng, z_prime, zenAng, z_0, distGS2satProj, theta, zed;


	zenAng = zenithAngle(x, y, z, a, b, c, &distGS2satProj);
	printf("zenAng in deltaAngle = %.12lf\n", zenAng);
	printf("distGS2satProj in deltaAngle = %.16lf\n", distGS2satProj);

	theta = distGS2satProj / EARTHRAD;
	printf("theta in deltaAngle = %.16lf\n", theta);

	z_0 = PI - (PI - (zenAng + theta));
	printf("z_0 in deltaAngle = %.16lf\n", z_0);

	zed = asin((sin(z_0) * EARTHRAD) / (EARTHRAD + 15));
	printf("zed = %.16lf\n", zed);

	dAng = z_0 - zed;
	printf("dAng in deltaAngle = %.16lf\n", dAng);

	return dAng;
}

/*
------------------------------------------------------
void main()
PURPOSE:			Compute refracted ground station coordinates
FUNCTIONS CALLED:	deltaAngle, pow, satProjection, sqrt
VER./DATE:			1.0 30 Jan 2019
PROGRAMMER:			JFG
NOTES:				Testing version

------------------------------------------------------
*/


void main(void)
{
	double linearDisplacement, dAng, d[88], e[88], f[88],
		height_sat, projSatGSDiff_x, projSatGSDiff_y, projSatGSDiff_z,
		distProjSat2GS;
	static double alt_sat, mu_0, satProj_x, satProj_y, satProj_z;

	/*
	Define input coordinates (a,b,c) for the unrefracted position of the ground station
	and (x,y,z) for the position of the spacecraft
	*/
	double a[88] = { 6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	double b[88] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	double c[88] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		6371000.0, 6371000.0, 6371000.0, 6371000.0, 6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		-6371000.0, -6371000.0, -6371000.0, -6371000.0, -6371000.0,
		-6371000.0, -6371000.0 };

	double x[88] = { 6871000.000013, 6870993.553012, 6870974.196290, 6870941.882601, 6870896.532929,
		6870838.036079, 6870766.248094, 6870680.991498, 6870582.054363, 6870469.189177,
		6870342.111515, 6870200.498503, 6870043.987043, 6869872.171806, 6869684.602949,
		-6869480.783555, -6869260.166758, -6869022.152525, -6868766.084065, -6868491.243829,
		-6868196.849046, -6867882.046759, -6867545.908301, -6867187.423138, -6866805.492021,
		-6866398.919348, -6865966.404653, -6865506.533102, -6865017.764874, -6864498.423286,
		311250.628638, 323918.191201, 336853.432073, 350073.029579, 363594.754864,
		377437.574291, 391621.762976, 406169.030930, 421102.663498, 436447.678054,
		452230.999223, 468481.655254, 485230.998617, 502512.954403, 520364.300738,
		538824.986122, 557938.489556, 577752.230309, 598318.035558, 619692.675633,
		641938.478586, 665124.038139, 689325.031973, 714625.170951, 741117.304358,
		768904.711852, 798102.619942, 828839.989739, 861261.634214,	895530.737832,
		931831.870399, 970374.611627, 1011397.935241, 1055175.544192, 1102022.405500,
		1152302.809919,	1206440.385636,	1264930.638105,	1328356.786073,	1397409.941697,
		1472915.076927,	1555864.785189,	1647463.673013,	1749187.435399,	1862862.493821,
		1990774.845461,	2135821.027604,	2301720.699561,	2493320.599776,	2717035.354516,
		2981493.407760,	3298483.483822,	3684301.907601,	4161443.039518,	4759550.200962,
		5508812.580476,	6382848.336317,	6820303.968142 };

	double y[88] = { 0.000000, 9412.472374, 18830.668804, 28260.327264, 37707.213631,
		47177.135817, 56675.958123, 66209.615883, 75784.130476, 85405.624802,
		95080.339290, 104814.648539, 114615.078688, 124488.325614, 134441.274074,
		144481.017908, 154614.881441, 164850.442215, 175195.555221, 185658.378791,
		196247.402351, 206971.476238, 217839.843814, 228862.176142, 240048.609505,
		251409.786101, 262956.898266, 274701.736638, 286656.742715, 298835.066335,
		6863946.681491, 6863360.547544, 6862737.847643, 6862076.207252, 6861373.029825,
		6860625.472775, 6859830.420275, 6858984.452416, 6858083.810145, 6857124.355333,
		6856101.525176, 6855010.279997, 6853845.043343, 6852599.633047, 6851267.181675,
		-6849840.044447, -6848309.692329, -6846666.587511, -6844900.037875, -6842998.026300,
		-6840947.009726, -6838731.681684, -6836334.690495, -6833736.303459, -6830914.004828,
		-6827842.012251, -6824490.692221, -6820825.849674, -6816807.859813, -6812390.600793,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000 };

	double z[88] = { 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		6807520.133314, 6802133.056129, 6796154.443269, 6789495.236843, 6782048.924768,
		6773687.270197, 6764254.770193, 6753561.392403, 6741372.949857, 6727398.193583,
		6711271.278702, 6692527.606994, 6670570.024089, 6644620.705203, 6613651.361335,
		-6576279.838545, -6530613.258969, -6474003.616109, -6402655.182571, -6310971.389769,
		-6190423.092136, -6027490.996022, -5799703.479819, -5467452.133217, -4955534.571032,
		-4106534.421306, -2543597.475182, 833123.509645 };

	for (int j = 0; j < 88; j++)
	{
		/* Compute the coordinates of the point on the Earth's surface representing
		the projection of the satellite along the line connecting it to the origin/Earth centre */

		printf("a[%d] = %lf\nb[%d] = %lf\nc[%d] = %lf\nx[%d] = %lf\ny[%d] = %lf\nz[%d] = %lf\n",
			j, a[j], j, b[j], j, c[j], j, x[j], j, y[j], j, z[j]);

		satProjection(x[j], y[j], z[j], &satProj_x, &satProj_y, &satProj_z);

		/* Compute the vector difference of the the point on the Earth's surface representing
		the projection of the satellite along the line connecting it to the origin/Earth centre
		and the vector <a, b, c> corresponding to the point representing the ground station location */

		projSatGSDiff_x = abs(a[j] - satProj_x);
		projSatGSDiff_y = abs(b[j] - satProj_y);
		projSatGSDiff_z = abs(c[j] - satProj_z);

		/* Compute the refracted ground station coordinates by translating the unrefracted ground station coordinates
		along the line through the unrefracted ground station parallel to the above difference vector */

		distProjSat2GS = sqrt(pow((satProj_x - a[j]), 2) + pow((satProj_y - b[j]), 2) + pow((satProj_z - c[j]), 2));
		
		/* Compute the displacement from the unrefracted ground station coordinates to its refracted coordinates
		by calculating the arc length subtended by the angular difference between the points.
		At reasonable zenith angles we can assume this displacement is linear. */

		dAng = deltaAngle(x[j], y[j], z[j], a[j], b[j], c[j]);
		linearDisplacement = 6371000 * dAng;
		printf("linearDisplacement in main for test case %d = %.16lf\n", j, linearDisplacement);

		if (linearDisplacement == 0.0)
		{
			d[j] = a[j];
			e[j] = b[j];
			f[j] = c[j];
		}
		else
		{
			if (satProj_x > a[j])
			{
				d[j] = a[j] + ((linearDisplacement / distProjSat2GS) * projSatGSDiff_x);
			}
			else
			{
				d[j] = a[j] - ((linearDisplacement / distProjSat2GS) * projSatGSDiff_x);
			}
			if (satProj_y > b[j])
			{
				e[j] = b[j] + ((linearDisplacement / distProjSat2GS) * projSatGSDiff_y);
			}
			else
			{
				e[j] = b[j] - ((linearDisplacement / distProjSat2GS) * projSatGSDiff_y);
			}
			if (satProj_z > c[j])
			{
				f[j] = c[j] + ((linearDisplacement / distProjSat2GS) * projSatGSDiff_z);
			}
			else
			{
				f[j] = c[j] - ((linearDisplacement / distProjSat2GS) * projSatGSDiff_z);
			}
		}
		printf("Output coords for test case %d are\n%.16lf\n%.16lf\n%.16lf\n", j, d[j], e[j], f[j]);
		printf("Distance from refracted to unrefracted GS = %.16\lf\n\n\n", sqrt(pow((d[j] - a[j]), 2) + pow((e[j] - b[j]), 2) + pow((f[j] - c[j]), 2)));
	}
	getchar();
	return; 
}
