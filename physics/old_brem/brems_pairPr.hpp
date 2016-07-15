// Doubly differential cross section for
// Bremsstrahlung and pair production
// -------------------------------------
// (C) Christoph Koehn, Ute Ebert
// Centrum Wiskunde en Informatica
// P.O. Box 94079, NL-1090 GB, Amsterdam

#include <math.h>
// Define functions for doubly differential
// cross section for Bremstrahlung and
// pair production, also to be set in the
// beginning of the code


double brems(double E_kin,double omega,double Theta_i);	// Doubly differential cross section for Bremsstrahlung
                                                        // which depends on the kinetic energy of the incident electron,
                                                        // frequency of the emitted photon and angle between both
                                                        // (in radian)

double pair_pr(double E_kin,double omega,double Theta_p);	// Doubly differential cross section for pair production
                                                                // which depends on the kinetic energy of the generated positron,
                                                                // frequency of the incident photon and angle between both
                                                                // (in radian)

// -----------------------------------------------------------------------------------------------------------------



// functions for calculating doubly differential
// cross sections for Bremsstrahlung and pair
// production

// Bremsstrahlung
double brems(double E_kin,double omega,double Theta_i)
{
  // define constants
  int i;

  //define constants
  double hbar=1.054571726e-34;				// Planck's constant
  double SpeedOfLight=299792458.0;			// Speed of light in vacuum
  double SpeedOfLight2=299792458.0*299792458.0;		// Speed of light in vacuum, squared
  double electronMass=9.10938291e-31;			// Mass of an electron
  double electronCharge=1.602176565e-19;		// Charge of an electron (without sign)
  double rest_energy=electronMass*SpeedOfLight2;        // Rest energy of the electron
  double alpha_fine=7.2973525698e-3;	                // finestructure constant
  double PI=3.14159265358979;				// PI

  // define physical quantities: energies and momenta
  double E_i=E_kin+rest_energy;								// total energy of incident electron
  double E_f=E_i-hbar*omega;								// total energy of outgoing electron
  double p_i=sqrt(E_i*E_i*1/SpeedOfLight2-electronMass*electronMass*SpeedOfLight2);	// momentum of incident electron
  double p_f=sqrt(E_f*E_f*1/SpeedOfLight2-electronMass*electronMass*SpeedOfLight2);  	// momentum of outgoing electron

  // define help functions
  int Z=7;										// atomic number, can be changed arbitrarily
  double A=Z*Z*pow(alpha_fine,3)/(4*PI*PI)*p_f/p_i*hbar*hbar/omega;		// overall prefactor, defined in (11)
  double B=Z*Z*pow(alpha_fine,3)/(2*PI)*p_f/p_i*hbar*hbar/omega;		// overall prefactor for Theta_i=0
  // delta functions defined in (40) and (41)
  double tri_1=-p_i*p_i-p_f*p_f-hbar*hbar*1/SpeedOfLight2*omega*omega+2*hbar*1/SpeedOfLight*omega*p_i*cos(Theta_i);
  double tri_2=-2*hbar*1/SpeedOfLight*omega*p_f+2*p_i*p_f*cos(Theta_i);

  double I[8]={0};									// parts of the cross section
  double cross=0;									// total result of cross section


  // all terms which are needed for the cross section
  if (Theta_i!=0)									// calculate cross section for all angles, except for 0 degree
  {
    I[0]=2*PI*A/sqrt(tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))*log((tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i))-(tri_1+tri_2)*sqrt(tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))+tri_1*tri_2)/(-tri_2*tri_2-(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i))-(tri_1-tri_2)*sqrt(tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))+tri_1*tri_2))*(1+SpeedOfLight*tri_2/(p_f*(E_i-p_i*SpeedOfLight*cos(Theta_i)))-(p_i*SpeedOfLight*sin(Theta_i))*(p_i*SpeedOfLight*sin(Theta_i))/((E_i-SpeedOfLight*p_i*cos(Theta_i))*(E_i-SpeedOfLight*p_i*cos(Theta_i)))-2*hbar*hbar*omega*omega*p_f*tri_2/(SpeedOfLight*(E_i-SpeedOfLight*p_i*cos(Theta_i))*(tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))));
    I[1]=-2*PI*A*SpeedOfLight/(p_f*(E_i-SpeedOfLight*p_i*cos(Theta_i)))*log((E_f+SpeedOfLight*p_f)/(E_f-SpeedOfLight*p_f));
    I[2]=2*PI*A/sqrt((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i)))*log(((E_f+p_f*SpeedOfLight)*((2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i))*(E_f-p_f*SpeedOfLight)+(tri_1+tri_2)*((tri_2*E_f+tri_1*p_f*SpeedOfLight)-sqrt((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i))))))/((E_f-p_f*SpeedOfLight)*((2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i))*(-E_f-p_f*SpeedOfLight)+(tri_1-tri_2)*((tri_2*E_f+tri_1*p_f*SpeedOfLight)-sqrt((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i)))))))*(-((tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))*(pow(E_f,3)+E_f*p_f*p_f*SpeedOfLight2)+p_f*SpeedOfLight*(2*(tri_1*tri_1-(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))*E_f*p_f*SpeedOfLight+tri_1*tri_2*(3*E_f*E_f+p_f*p_f*SpeedOfLight2)))/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i)))-SpeedOfLight*(tri_2*E_f+tri_1*p_f*SpeedOfLight)/(p_f*(E_i-SpeedOfLight*p_i*cos(Theta_i))) -(4*E_i*E_i*p_f*p_f*(2*(tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)-(2*electronMass*SpeedOfLight2*p_i*p_f*sin(Theta_i))*(2*electronMass*SpeedOfLight2*p_i*p_f*sin(Theta_i)))*(tri_1*E_f+tri_2*p_f*SpeedOfLight))/(((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i)))*((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i)))) +(8*(p_i*p_f*rest_energy*sin(Theta_i))*(p_i*p_f*rest_energy*sin(Theta_i))*(E_i*E_i+E_f*E_f)-2*(hbar*omega*p_i*sin(Theta_i))*(hbar*omega*p_i*sin(Theta_i))*p_f*SpeedOfLight*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+2*hbar*hbar*omega*omega*p_f*electronMass*electronMass*pow(SpeedOfLight,3)*(tri_2*E_f+tri_1*p_f*SpeedOfLight))/((E_i-SpeedOfLight*p_i*cos(Theta_i))*((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i)))));
    I[3]=-4*PI*A*p_f*SpeedOfLight*(tri_2*E_f+tri_1*p_f*SpeedOfLight)/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+4*(rest_energy*p_i*p_f*sin(Theta_i))*(rest_energy*p_i*p_f*sin(Theta_i)))-16*PI*E_i*E_i*p_f*p_f*A*(tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)/(((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+4*(rest_energy*p_i*p_f*sin(Theta_i))*(rest_energy*p_i*p_f*sin(Theta_i)))*((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+4*(rest_energy*p_i*p_f*sin(Theta_i))*(rest_energy*p_i*p_f*sin(Theta_i))));
    I[4]=4*PI*A/((-tri_2*tri_2+tri_1*tri_1-(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))*((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i))))*(hbar*hbar*omega*omega*p_f*p_f/(E_i-SpeedOfLight*p_i*cos(Theta_i))*(E_f*(2*tri_2*tri_2*(tri_2*tri_2-tri_1*tri_1)+8*(p_i*p_f*sin(Theta_i))*(p_i*p_f*sin(Theta_i))*(tri_2*tri_2+tri_1*tri_1))+p_f*SpeedOfLight*(2*tri_1*tri_2*(tri_2*tri_2-tri_1*tri_1)+tri_1*tri_2*(4*p_i*p_f*sin(Theta_i))*(4*p_i*p_f*sin(Theta_i))))/(tri_2*tri_2+(2*p_i*p_f*sin(Theta_i))*(2*p_i*p_f*sin(Theta_i)))+2*(hbar*omega*p_i*sin(Theta_i))*(hbar*omega*p_i*sin(Theta_i))*(2*tri_1*tri_2*p_f*SpeedOfLight+2*tri_2*tri_2*E_f+8*(p_i*p_f*sin(Theta_i))*(p_i*p_f*sin(Theta_i))*E_f)/(E_i-p_i*SpeedOfLight*cos(Theta_i))+2*E_i*E_i*p_f*p_f*(2*(tri_2*tri_2-tri_1*tri_1)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+8*(p_i*p_f*sin(Theta_i))*(p_i*p_f*sin(Theta_i))*((tri_1*tri_1+tri_2*tri_2)*(E_f*E_f+p_f*p_f*SpeedOfLight2)+4*tri_1*tri_2*E_f*p_f*SpeedOfLight))/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)+(2*rest_energy*p_i*p_f*sin(Theta_i))*(2*rest_energy*p_i*p_f*sin(Theta_i))) +8*(p_i*p_f*sin(Theta_i))*(p_i*p_f*sin(Theta_i))*(E_i*E_i+E_f*E_f)*(tri_2*p_f*SpeedOfLight+tri_1*E_f)/(E_i-SpeedOfLight*p_i*cos(Theta_i)));
    I[5]=16*PI*E_f*E_f*p_i*p_i*sin(Theta_i)*sin(Theta_i)*A/((E_i-SpeedOfLight*p_i*cos(Theta_i))*(E_i-SpeedOfLight*p_i*cos(Theta_i))*(-tri_2*tri_2+tri_1*tri_1-4*p_i*p_i*p_f*p_f*sin(Theta_i)*sin(Theta_i)));
  }
  else											// calculate cross section for 0 degree
  {
    I[0]=-2*B*p_f*SpeedOfLight/(tri_2*E_f+tri_1*p_f*SpeedOfLight);
    I[1]=B*p_f*p_f*SpeedOfLight2*(-tri_1*tri_1+tri_2*tri_2)/(tri_2*(tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight))*log((tri_1+tri_2)/(tri_1-tri_2));
    I[2]=B*(2*tri_1*E_f*p_f*SpeedOfLight+tri_2*(E_f*E_f+p_f*p_f*SpeedOfLight2))/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight))*log((E_f+p_f*SpeedOfLight)/(E_f-p_f*SpeedOfLight));
    I[3]=-B*(4*E_i*p_f)*(4*E_i*p_f)/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight));
    I[4]=-8*B*E_i*E_i*p_f*p_f*(tri_1*E_f+tri_2*p_f*SpeedOfLight)/(pow(tri_2*E_f+tri_1*p_f*SpeedOfLight,3))*log(((tri_1-tri_2)*(E_f-p_f*SpeedOfLight))/((tri_1+tri_2)*(E_f+p_f*SpeedOfLight)));
    I[5]=-B*4*hbar*hbar*p_f*p_f*omega*omega/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(E_i-p_i*SpeedOfLight)*tri_2);
    I[6]=2*B*hbar*hbar*p_f*p_f*omega*omega*(2*tri_1*tri_2*E_f+p_f*SpeedOfLight*(tri_1*tri_1+tri_2*tri_2))/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)*(E_i-p_i*SpeedOfLight)*tri_2*tri_2)*log((tri_1+tri_2)/(tri_1-tri_2));
    I[7]=2*B*hbar*hbar*omega*omega*p_f*(E_f*E_f-p_f*p_f*SpeedOfLight2)/((tri_2*E_f+tri_1*p_f*SpeedOfLight)*(tri_2*E_f+tri_1*p_f*SpeedOfLight)*(E_i-SpeedOfLight*p_i)*SpeedOfLight)*log((E_f-p_f*SpeedOfLight)/(E_f+p_f*SpeedOfLight));
  }

  // sum up all terms to get value for cross section
  for (i=0;i<=7;i++)
  {
    cross+=I[i];
  }


  return cross;										// give back result of cross section calculation
}


// pair production
double pair_pr(double E_kin,double omega,double Theta_p)
{
  int i;

  //define constants
  double hbar=1.054571726e-34;				// Planck's constant
  double SpeedOfLight=299792458.0;			// Speed of light in vacuum
  double SpeedOfLight2=299792458.0*299792458.0;		// Speed of light in vacuum, squared
  double electronMass=9.10938291e-31;			// Mass of an electron
  double electronCharge=1.602176565e-19;		// Charge of an electron (without sign)
  double rest_energy=electronMass*SpeedOfLight2;        // Rest energy of the electron
  double alpha_fine=7.2973525698e-3;	                // finestructure constant
  double PI=3.14159265358979;				// PI

  // define physical quantities: energies and momenta
  double E_p=E_kin+rest_energy;								// total energy of created positron
  double E_m=(hbar*omega-rest_energy)-E_kin;						// total energy of created electron
  double p_p=sqrt(E_p*E_p*1/SpeedOfLight2-electronMass*electronMass*SpeedOfLight2);	// momentum of created positron
  double p_m=sqrt(E_m*E_m*1/SpeedOfLight2-electronMass*electronMass*SpeedOfLight2);  	// momentum of created electron

  // define help functions
  int Z=7;										// atomic number, can be changed arbitrarily
  double A=Z*Z*pow(alpha_fine,3)*SpeedOfLight2*p_p*p_m/(4*PI*PI*hbar*pow(omega,3));	// overall prefactor, defined in (96)
  double B=Z*Z*pow(alpha_fine,3)*SpeedOfLight2*p_p*p_m/(2*PI*hbar*pow(omega,3));		// overall prefactor for Theta_p=0 and Theta_p=180 degree
  // delta functions defined in (40) and (41)
  double tri_1=-p_p*p_p-p_m*p_m-hbar*hbar*1/SpeedOfLight2*omega*omega+2*hbar*1/SpeedOfLight*omega*p_p*cos(Theta_p);
  double tri_2=2*hbar*1/SpeedOfLight*omega*p_m-2*p_p*p_m*cos(Theta_p);

  double I[8]={0};									// parts of the cross section
  double cross=0;									// total result of cross section

  // all terms which are needed for the cross section
  if (Theta_p!=0 && Theta_p!=PI)							// calculate cross section for all angles, except for 0 and 180 degree
  {
    I[0]=2*PI*A/sqrt(tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))*log((tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p))-(tri_1+tri_2)*sqrt(tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))+tri_1*tri_2)/(-tri_2*tri_2-(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p))-(tri_1-tri_2)*sqrt(tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))+tri_1*tri_2))*(-1 -SpeedOfLight*tri_2/(p_m*(E_p-p_p*SpeedOfLight*cos(Theta_p)))+(p_p*SpeedOfLight*sin(Theta_p))*(p_p*SpeedOfLight*sin(Theta_p))/((E_p-SpeedOfLight*p_p*cos(Theta_p))*(E_p-SpeedOfLight*p_p*cos(Theta_p)))-2*hbar*hbar*omega*omega*p_m*tri_2/(SpeedOfLight*(E_p-SpeedOfLight*p_p*cos(Theta_p))*(tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))));
    I[1]=2*PI*A*SpeedOfLight/(p_m*(E_p-SpeedOfLight*p_p*cos(Theta_p)))*log((E_m+SpeedOfLight*p_m)/(E_m-SpeedOfLight*p_m));
    I[2]=2*PI*A/sqrt((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))*log(((E_m+p_m*SpeedOfLight)*((2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p))*(E_m-p_m*SpeedOfLight)+(tri_1+tri_2)*((tri_2*E_m+tri_1*p_m*SpeedOfLight)-sqrt((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p))))))/((E_m-p_m*SpeedOfLight)*((2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p))*(-E_m-p_m*SpeedOfLight)+(tri_1-tri_2)*((tri_2*E_m+tri_1*p_m*SpeedOfLight)-std::sqrt((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))))))*(((tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))*(pow(E_m,3)+E_m*p_m*p_m*SpeedOfLight2)+p_m*SpeedOfLight*(2*(tri_1*tri_1-(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))*E_m*p_m*SpeedOfLight+tri_1*tri_2*(3*E_m*E_m+p_m*p_m*SpeedOfLight2)))/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))+SpeedOfLight*(tri_2*E_m+tri_1*p_m*SpeedOfLight)/(p_m*(E_p-SpeedOfLight*p_p*cos(Theta_p)))+(4*E_p*E_p*p_m*p_m*(2*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)-(2*electronMass*SpeedOfLight2*p_p*p_m*sin(Theta_p))*(2*electronMass*SpeedOfLight2*p_p*p_m*sin(Theta_p)))*(tri_1*E_m+tri_2*p_m*SpeedOfLight))/(((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))*((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p))))+(-8*(p_p*p_m*rest_energy*sin(Theta_p))*(p_p*p_m*rest_energy*sin(Theta_p))*(E_p*E_p+E_m*E_m)-2*(hbar*omega*p_p*sin(Theta_p))*(hbar*omega*p_p*sin(Theta_p))*p_m*SpeedOfLight*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+2*hbar*hbar*omega*omega*p_m*electronMass*electronMass*pow(SpeedOfLight,3)*(tri_2*E_m+tri_1*p_m*SpeedOfLight))/((E_p-SpeedOfLight*p_p*cos(Theta_p))*((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))));
    I[3]=4*PI*A*p_m*SpeedOfLight*(tri_2*E_m+tri_1*p_m*SpeedOfLight)/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+4*(rest_energy*p_p*p_m*sin(Theta_p))*(rest_energy*p_p*p_m*sin(Theta_p)))+16*PI*E_p*E_p*p_m*p_m*A*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)/(((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))*((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p))));
    I[4]=4*PI*A/((-tri_2*tri_2+tri_1*tri_1-(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))*((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p))))*((hbar*omega*p_m)*(hbar*omega*p_m)/(E_p-SpeedOfLight*p_p*cos(Theta_p))*(E_m*(2*tri_2*tri_2*(tri_2*tri_2-tri_1*tri_1)+8*(p_p*p_m*sin(Theta_p))*(p_p*p_m*sin(Theta_p))*(tri_2*tri_2+tri_1*tri_1))+p_m*SpeedOfLight*(2*tri_1*tri_2*(tri_2*tri_2-tri_1*tri_1)+tri_1*tri_2*(4*p_p*p_m*sin(Theta_p))*(4*p_p*p_m*sin(Theta_p))))/(tri_2*tri_2+(2*p_p*p_m*sin(Theta_p))*(2*p_p*p_m*sin(Theta_p)))+2*(hbar*omega*p_p*sin(Theta_p))*(hbar*omega*p_p*sin(Theta_p))*(2*tri_1*tri_2*p_m*SpeedOfLight+2*tri_2*tri_2*E_m+8*(p_p*p_m*sin(Theta_p))*(p_p*p_m*sin(Theta_p))*E_m)/(E_p-p_p*SpeedOfLight*cos(Theta_p))-2*E_p*E_p*p_m*p_m*(2*(tri_2*tri_2-tri_1*tri_1)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+8*(p_p*p_m*sin(Theta_p))*(p_p*p_m*sin(Theta_p))*((tri_1*tri_1+tri_2*tri_2)*(E_m*E_m+p_m*p_m*SpeedOfLight2)+4*tri_1*tri_2*E_m*p_m*SpeedOfLight))/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)+(2*rest_energy*p_p*p_m*sin(Theta_p))*(2*rest_energy*p_p*p_m*sin(Theta_p)))-8*(p_p*p_m*sin(Theta_p))*(p_p*p_m*sin(Theta_p))*(E_p*E_p+E_m*E_m)*(tri_2*p_m*SpeedOfLight+tri_1*E_m)/(E_p-SpeedOfLight*p_p*cos(Theta_p)));
    I[5]=-16*PI*E_m*E_m*p_p*p_p*sin(Theta_p)*sin(Theta_p)*A/((E_p-SpeedOfLight*p_p*cos(Theta_p))*(E_p-SpeedOfLight*p_p*cos(Theta_p))*(-tri_2*tri_2+tri_1*tri_1-4*p_p*p_p*p_m*p_m*sin(Theta_p)*sin(Theta_p)));
  }
  else
  {
    if (Theta_p!=PI)									// calculate cross section for 0 degree
    {
      I[0]=2*B*p_m*SpeedOfLight/(tri_2*E_m+tri_1*p_m*SpeedOfLight);
      I[1]=-B*p_m*SpeedOfLight2*(-tri_1*tri_1+tri_2*tri_2)/(tri_2*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight))*log((tri_1+tri_2)/(tri_1-tri_2));
      I[2]=-B*(2*tri_1*E_m*p_m*SpeedOfLight+tri_2*(E_m*E_m+p_m*p_m*SpeedOfLight2))/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight))*log((E_m+p_m*SpeedOfLight)/(E_m-p_m*SpeedOfLight));
      I[3]=B*(4*E_p*p_m)*(4*E_p*p_m)/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight));
      I[4]=8*B*E_p*E_p*p_m*p_m*(tri_1*E_m+tri_2*p_m*SpeedOfLight)/(pow(tri_2*E_m+tri_1*p_m*SpeedOfLight,3))*log(((tri_1-tri_2)*(E_m-p_m*SpeedOfLight))/((tri_1+tri_2)*(E_m+p_m*SpeedOfLight)));
      I[5]=-B*4*hbar*hbar*p_m*p_m*omega*omega/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(E_p-p_p*SpeedOfLight)*tri_2);
      I[6]=2*B*hbar*hbar*p_m*p_m*omega*omega*(2*tri_1*tri_2*E_m+p_m*SpeedOfLight*(tri_1*tri_1+tri_2*tri_2))/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(E_p-p_p*SpeedOfLight)*tri_2*tri_2)*log((tri_1+tri_2)/(tri_1-tri_2));
      I[7]=2*B*hbar*hbar*omega*omega*p_m*(E_m*E_m-p_m*p_m*SpeedOfLight2)/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(E_p-SpeedOfLight*p_p)*SpeedOfLight)*log((E_m-p_m*SpeedOfLight)/(E_m+p_m*SpeedOfLight));
    }
    else										// calculate cross section for 180 degree
    {
      I[0]=2*B*p_m*SpeedOfLight/(tri_2*E_m+tri_1*p_m*SpeedOfLight);
      I[1]=-B*p_m*p_m*SpeedOfLight2*(-tri_1*tri_1+tri_2*tri_2)/(tri_2*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight))*log((tri_1+tri_2)/(tri_1-tri_2));
      I[2]=-B*(2*tri_1*E_m*p_m*SpeedOfLight+tri_2*(E_m*E_m+p_m*p_m*SpeedOfLight2))/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight))*log((E_m+p_m*SpeedOfLight)/(E_m-p_m*SpeedOfLight));
      I[3]=B*(4*E_p*p_m)*(4*E_p*p_m)/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight));
      I[4]=8*B*E_p*p_m*E_p*p_m*(tri_1*E_m+tri_2*p_m*SpeedOfLight)/(std::pow(tri_2*E_m+tri_1*p_m*SpeedOfLight,3))*log(((tri_1-tri_2)*(E_m-p_m*SpeedOfLight))/((tri_1+tri_2)*(E_m+p_m*SpeedOfLight)));
      I[5]=-B*4*hbar*hbar*p_m*p_m*omega*omega/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(E_p+p_p*SpeedOfLight)*tri_2);
      I[6]=2*B*hbar*hbar*p_m*p_m*omega*omega*(2*tri_1*tri_2*E_m+p_m*SpeedOfLight*(tri_1*tri_1+tri_2*tri_2))/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(E_p+p_p*SpeedOfLight)*tri_2*tri_2)*log((tri_1+tri_2)/(tri_1-tri_2));
      I[7]=2*B*hbar*hbar*omega*omega*p_m*(E_m*E_m-p_m*p_m*SpeedOfLight2)/((tri_2*E_m+tri_1*p_m*SpeedOfLight)*(tri_2*E_m+tri_1*p_m*SpeedOfLight)*(E_p+SpeedOfLight*p_p)*SpeedOfLight)*log((E_m-p_m*SpeedOfLight)/(E_m+p_m*SpeedOfLight));
    }
  }


  // sum up all terms to get value for cross section
  for (i=0;i<=7;i++)
  {
    cross+=I[i];
  }

  return cross;										// give back result of cross section calculation
}
