#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265359
/*
SATELLITE ALTITUDE FROM EARTH SURFACE ASSUMED TO BE 500KM THROUGHOUT
*/
int main()
{
	double zenAngDeg[91] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
		59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
		78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90 };

	double zenAng[91];
	
	for (int p = 0; p< 91; p++)
	{
		zenAng[p] = zenAngDeg[p] * (PI / 180);
	}

	double a[91] = {6371000, 6371000, 6371000, 6371000, 6371000, 
					6371000, 6371000, 6371000, 6371000, 6371000, 
					6371000, 6371000, 6371000, 6371000, 6371000, 
					-6371000, -6371000, -6371000, -6371000, -6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	double b[91] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					6371000, 6371000, 6371000, 6371000, 6371000,
					6371000, 6371000, 6371000, 6371000, 6371000,
					6371000, 6371000, 6371000, 6371000, 6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000, 
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	
	double c[91] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					6371000, 6371000, 6371000, 6371000, 6371000,
					6371000, 6371000, 6371000, 6371000, 6371000,
					6371000, 6371000, 6371000, 6371000, 6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000,
					-6371000, -6371000, -6371000, -6371000, -6371000 };

	double x[91], y[91], z[91], satProj_x, satProj_y, satProj_z, distABCsatProj,
			xyzDistanceCheck;

	for (int j = 0; j < 91; j++)
	{
		/* Solve for the length of the side of the triangle connecting satellite,
		satellite projection onto Earth surface, and ground station that lies
		on the Earth surface (Assume Earth curvature negligible.) */
		distABCsatProj = tan(zenAng[j]) * 500000;
		printf("distABCsatProj = %lf\n", distABCsatProj);

		if (b[j] == 0 & c[j] == 0)
		{
			/* Set Z-coordinate of satellite projection onto Earth surface to be zero to
			make computations in only two dimensions and thus simpler */
			satProj_z = 0;

			/* Solve for the two unknowns of the projection X- and Y-coordinates*/
			
			// INSERT IF STATEMENT TO TREAT NEGATIVE CASE?
			satProj_x = ((2 * pow(6371000, 2)) - pow(distABCsatProj, 2)) / (2 * 6371000);
			satProj_y = sqrt(pow(6371000, 2) - pow(satProj_x, 2));

			printf("satProj_x = %lf\n", satProj_x);
			printf("satProj_y = %lf\n", satProj_y);

			/* Compute the coordinates of the satellite by translating the point
			representing the satellite projection away from the Earth by t metres
			along the line connecting the projection to Earth centre */

			// t derived from t<proj_x, proj_y, proj_z> = <x,y,z> and distance requirements
			//t = 6871000 / 6371000;
			if (a[j] < 0)
			{
				x[j] = -1.07848061529 * satProj_x;
			}
			else 
			{
				x[j] = 1.07848061529 * satProj_x;
			}
			y[j] = 1.07848061529 * satProj_y;
			z[j] = 1.07848061529 * satProj_z;

			xyzDistanceCheck = sqrt(pow(x[j], 2) + pow(y[j], 2) + pow(z[j], 2));
		}
		else if (a[j] == 0 & c[j] == 0)
		{
			/* Set Z-coordinate of satellite projection onto Earth surface to be zero to
			   make computations in only two dimensions and thus simpler */
			satProj_z = 0;

			/* Solve for the two unknowns of the projection X- and Y-coordinates*/

			// INSERT IF STATEMENT TO TREAT NEGATIVE CASE?
			satProj_y = ((2 * pow(6371000, 2)) - pow(distABCsatProj, 2)) / (2 * 6371000);
			satProj_x = sqrt(pow(6371000, 2) - pow(satProj_y, 2));

			printf("satProj_x = %lf\n", satProj_x);
			printf("satProj_y = %lf\n", satProj_y);

			/* Compute the coordinates of the satellite by translating the point
			   representing the satellite projection away from the Earth by t metres
			   along the line connecting the projection to Earth centre */
			
			// t derived from t<proj_x, proj_y, proj_z> = <x,y,z> and distance requirements
			//t = 6871000 / 6371000;  
			x[j] = 1.07848061529 * satProj_x;
			
			if (b[j] < 0)
			{
				y[j] = -1.07848061529 * satProj_y;
			}
			else
			{
				y[j] = -1.07848061529 * satProj_y;
			}
			z[j] = 1.07848061529 * satProj_z;

			xyzDistanceCheck = sqrt(pow(x[j], 2) + pow(y[j], 2) + pow(z[j], 2));
		}
		else if (a[j] == 0 & b[j] == 0)
		{
			/* Set Y-coordinate of satellite projection onto Earth surface to be zero to
			make computations in only two dimensions and thus simpler */
			satProj_y = 0;

			/* Solve for the two unknowns of the projection X- and Y-coordinates*/

			// INSERT IF STATEMENT TO TREAT NEGATIVE CASE?
			satProj_z = ((2 * pow(6371000, 2)) - pow(distABCsatProj, 2)) / (2 * 6371000);
			satProj_x = sqrt(pow(6371000, 2) - pow(satProj_z, 2));

			printf("satProj_x = %lf\n", satProj_x);
			printf("satProj_z = %lf\n", satProj_z);

			/* Compute the coordinates of the satellite by translating the point
			representing the satellite projection away from the Earth by t metres
			along the line connecting the projection to Earth centre */

			// t derived from t<proj_x, proj_y, proj_z> = <x,y,z> and distance requirements
			//t = 6871000 / 6371000;
			x[j] = 1.07848061529 * satProj_x;
			y[j] = 1.07848061529 * satProj_y;
			
			if (c[j] < 0)
			{
				z[j] = -1.07848061529 * satProj_z;
			}
			else
			{
				z[j] = 1.07848061529 * satProj_z;
			}

			xyzDistanceCheck = sqrt(pow(x[j],2) + pow(y[j], 2) + pow(z[j],2));

		}
		printf("%d\n%lf\n%lf\n%lf\nDistance check is %lf\n\n\n", j, x[j], y[j], z[j], xyzDistanceCheck);
		//printf("%d\n%lf\n%lf\n%lf\n\n\n", j, x[j], y[j], z[j]);
	}
	for (int k = 0; k < 91; k++)
	{
		printf("%lf\n", x[k]);
	}
	printf("\n\n");

	for (int m = 0; m < 91; m++)
	{
		printf("%lf\n", y[m]);
	}

	printf("\n\n");

	for (int n = 0; n < 91; n++)
	{
		printf("%lf\n", z[n]);
	}
	//printf("%lf\n", tan(11)); //Recall: arg in rad
	getchar();
    return 0;
}

