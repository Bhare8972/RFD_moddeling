/*
 * $Id: const_mksa.hpp 224 2012-08-07 19:21:54Z jdl3 $
 * Copyright (C) 2012 John D Lamb
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CCGSL_CONST_MKSA_HPP
#define CCGSL_CONST_MKSA_HPP

#include<gsl/gsl_const_mksa.h>

namespace gsl {
  /**
   * Various constants. This namespace is fairly pointless since all the
   * constants are probably more sensibly use through the GSL constants, but it is here
   * for completeness.
   */
  namespace const_mksa {
    /**
     * @return Speed of light in a vacuum
     */
    inline double const speed_of_light(){ return GSL_CONST_MKSA_SPEED_OF_LIGHT; }
    /**
     * @return The gravitational constant \f$G\f$
     */
    inline double const gravitational_constant(){ return GSL_CONST_MKSA_GRAVITATIONAL_CONSTANT; }
    /**
     * @return Plank's constant \f$h\f$
     */
    inline double const Planks_constant_h(){ return GSL_CONST_MKSA_PLANCKS_CONSTANT_H; }
    /**
     * @return Plank's constant \f$\hbar\f$
     */
    inline double const Planks_constant_hbar(){ return GSL_CONST_MKSA_PLANCKS_CONSTANT_HBAR; }
    /**
     * @return One astronomical unit (earth--sun distance)
     */
    inline double const astronomical_unit(){ return GSL_CONST_MKSA_ASTRONOMICAL_UNIT; }
    /**
     * @return Distance of one light year
     */
    inline double const light_year(){ return GSL_CONST_MKSA_LIGHT_YEAR; }
    /**
     * @return Distance of one parsec
     */
    inline double const parsec(){ return GSL_CONST_MKSA_PARSEC; }
    /**
     * @return The standard gravitational acceleration on Earth
     */
    inline double const grav_accel(){ return GSL_CONST_MKSA_GRAV_ACCEL; }
    /**
     * @return The energy of one electron volt \f$eV\f$
     */
    inline double const electron_volt(){ return GSL_CONST_MKSA_ELECTRON_VOLT; }
    /**
     * @return The mass of an electron
     */
    inline double const mass_electron(){ return GSL_CONST_MKSA_MASS_ELECTRON; }
    /**
     * @return The mass of an muon
     */
    inline double const mass_muon(){ return GSL_CONST_MKSA_MASS_MUON; }
    /**
     * @return The mass of an proton
     */
    inline double const mass_proton(){ return GSL_CONST_MKSA_MASS_PROTON; }
    /**
     * @return The mass of an neutron
     */
    inline double const mass_neutron(){ return GSL_CONST_MKSA_MASS_NEUTRON; }
    /**
     * @return The Rydberg constant in energy units
     */
    inline double const Rydberg(){ return GSL_CONST_MKSA_RYDBERG; }
    /**
     * @return The Boltzman constant \f$k\f$
     */
    inline double const Boltzmann(){ return GSL_CONST_MKSA_BOLTZMANN; }
    /**
     * @return The molar gas constant \f$R_0\f$
     */
    inline double const molar_gas(){ return GSL_CONST_MKSA_MOLAR_GAS; }
    /**
     * @return The molar standard gas volume \f$V_0\f$
     */
    inline double const standard_gas_volume(){ return GSL_CONST_MKSA_STANDARD_GAS_VOLUME; }
    /**
     * @return The number of seconds in one minute
     */
    inline double const minute(){ return GSL_CONST_MKSA_MINUTE; }
    /**
     * @return The number of seconds in one hour
     */
    inline double const hour(){ return GSL_CONST_MKSA_HOUR; }
    /**
     * @return The number of seconds in one day
     */
    inline double const day(){ return GSL_CONST_MKSA_DAY; }
    /**
     * @return The number of seconds in one week
     */
    inline double const week(){ return GSL_CONST_MKSA_WEEK; }
    /**
     * @return The length of one inch
     */
    inline double const inch(){ return GSL_CONST_MKSA_INCH; }
    /**
     * @return The length of one foot
     */
    inline double const foot(){ return GSL_CONST_MKSA_FOOT; }
    /**
     * @return The length of one yard
     */
    inline double const yard(){ return GSL_CONST_MKSA_YARD; }
    /**
     * @return The length of one mile
     */
    inline double const mile(){ return GSL_CONST_MKSA_MILE; }
    /**
     * @return The length of one nautical mile
     */
    inline double const nautical_mile(){ return GSL_CONST_MKSA_NAUTICAL_MILE; }
    /**
     * @return The length of one fathom
     */
    inline double const fathom(){ return GSL_CONST_MKSA_FATHOM; }
    /**
     * @return The length of one mil
     */
    inline double const mil(){ return GSL_CONST_MKSA_MIL; }
    /**
     * @return The length of one point
     */
    inline double const point(){ return GSL_CONST_MKSA_POINT; }
    /**
     * @return The length of one \f$\mbox{\TeX}\f$ point
     */
    inline double const TeXpoint(){ return GSL_CONST_MKSA_TEXPOINT; }
    /**
     * @return The length of one micron
     */
    inline double const micron(){ return GSL_CONST_MKSA_MICRON; }
    /**
     * @return The length of one angstrom
     */
    inline double const angstrom(){ return GSL_CONST_MKSA_ANGSTROM; }
    /**
     * @return The area of one hectare
     */
    inline double const hectare(){ return GSL_CONST_MKSA_HECTARE; }
    /**
     * @return The area of one acre
     */
    inline double const acre(){ return GSL_CONST_MKSA_ACRE; }
    /**
     * @return The area of one barn
     */
    inline double const barn(){ return GSL_CONST_MKSA_BARN; }
    /**
     * @return The volume of one litre/liter
     */
    inline double const liter(){ return GSL_CONST_MKSA_LITER; }
    /**
     * @return The volume of one litre
     */
    inline double const gallon(){ return GSL_CONST_MKSA_US_GALLON; }
    /**
     * @return The volume of one gallon
     */
    inline double const quart(){ return GSL_CONST_MKSA_QUART; }
    /**
     * @return The volume of one pint
     */
    inline double const pint(){ return GSL_CONST_MKSA_PINT; }
    /**
     * @return The volume of one cup
     */
    inline double const cup(){ return GSL_CONST_MKSA_CUP; }
    /**
     * @return The volume of one fluid ounce
     */
    inline double const fluid_ounce(){ return GSL_CONST_MKSA_FLUID_OUNCE; }
    /**
     * @return The volume of one tablespoon
     */
    inline double const tablespoon(){ return GSL_CONST_MKSA_TABLESPOON; }
    /**
     * @return The volume of one teaspoon
     */
    inline double const teaspoon(){ return GSL_CONST_MKSA_TEASPOON; }
    /**
     * @return The volume of one Canadian gallon
     */
    inline double const Canadian_gallon(){ return GSL_CONST_MKSA_CANADIAN_GALLON; }
    /**
     * @return The volume of one UK gallon
     */
    inline double const UK_gallon(){ return GSL_CONST_MKSA_UK_GALLON; }
    /**
     * @return The speed of one mile per hour
     */
    inline double const miles_per_hour(){ return GSL_CONST_MKSA_MILES_PER_HOUR; }
    /**
     * @return The speed of one kilometer per hour
     */
    inline double const kilometers_per_hour(){ return GSL_CONST_MKSA_KILOMETERS_PER_HOUR; }
    /**
     * @return The speed of one knot
     */
    inline double const knot(){ return GSL_CONST_MKSA_KNOT; }
    /**
     * @return The mass of one pound
     */
    inline double const pound_mass(){ return GSL_CONST_MKSA_POUND_MASS; }
    /**
     * @return The mass of one ounce
     */
    inline double const ounce_mass(){ return GSL_CONST_MKSA_OUNCE_MASS; }
    /**
     * @return The mass of one ton
     */
    inline double const ton(){ return GSL_CONST_MKSA_TON; }
    /**
     * @return The mass of one tonne (metric)
     */
    inline double const metric_tonne(){ return GSL_CONST_MKSA_METRIC_TON; }
    /**
     * @return The mass of one UK ton
     */
    inline double const UK_ton(){ return GSL_CONST_MKSA_UK_TON; }
    /**
     * @return The mass of one troy ounce
     */
    inline double const troy_ounce(){ return GSL_CONST_MKSA_TROY_OUNCE; }
    /**
     * @return The mass of one carat
     */
    inline double const carat(){ return GSL_CONST_MKSA_CARAT; }
    /**
     * @return The unified atomic mass amu
     */
    inline double const unified_atomic_mass(){ return GSL_CONST_MKSA_UNIFIED_ATOMIC_MASS; }
    /**
     * @return The force of one gram weight
     */
    inline double const gram_force(){ return GSL_CONST_MKSA_GRAM_FORCE; }
    /**
     * @return The force of one pound weight
     */
    inline double const pound_force(){ return GSL_CONST_MKSA_POUND_FORCE; }
    /**
     * @return The force of one kilopound weight
     */
    inline double const kilopound_force(){ return GSL_CONST_MKSA_KILOPOUND_FORCE; }
    /**
     * @return The force of one poundal
     */
    inline double const poundal(){ return GSL_CONST_MKSA_POUNDAL; }
    /**
     * @return The energy of one calorie
     */
    inline double const calorie(){ return GSL_CONST_MKSA_CALORIE; }
    /**
     * @return The energy of one British thermal unit
     */
    inline double const BTU(){ return GSL_CONST_MKSA_BTU; }
    /**
     * @return The energy of one therm
     */
    inline double const therm(){ return GSL_CONST_MKSA_THERM; }
    /**
     * @return The power of one horsepower
     */
    inline double const horsepower(){ return GSL_CONST_MKSA_HORSEPOWER; }
    /**
     * @return The pressure of one bar
     */
    inline double const bar(){ return GSL_CONST_MKSA_BAR; }
    /**
     * @return The pressure of one standard atmosphere
     */
    inline double const std_atmosphere(){ return GSL_CONST_MKSA_STD_ATMOSPHERE; }
    /**
     * @return The pressure of one torr
     */
    inline double const torr(){ return GSL_CONST_MKSA_TORR; }
    /**
     * @return The pressure of one meter of mercury
     */
    inline double const meter_of_mercury(){ return GSL_CONST_MKSA_METER_OF_MERCURY; }
    /**
     * @return The pressure of one inch of mercury
     */
    inline double const inch_of_mercury(){ return GSL_CONST_MKSA_INCH_OF_MERCURY; }
    /**
     * @return The pressure of one inch of water
     */
    inline double const inch_of_water(){ return GSL_CONST_MKSA_INCH_OF_WATER; }
    /**
     * @return The pressure of one pound per square inch
     */
    inline double const PSI(){ return GSL_CONST_MKSA_PSI; }
    /**
     * @return The dynamic viscosity of one poise
     */
    inline double const poise(){ return GSL_CONST_MKSA_POISE; }
    /**
     * @return The kinematic viscosity of one stokes
     */
    inline double const stokes(){ return GSL_CONST_MKSA_STOKES; }
    /**
     * @return The luminance of one stilb
     */
    inline double const stilb(){ return GSL_CONST_MKSA_STILB; }
    /**
     * @return The luminous flux of one lumen
     */
    inline double const lumen(){ return GSL_CONST_MKSA_LUMEN; }
    /**
     * @return The illuminance of one lux
     */
    inline double const lux(){ return GSL_CONST_MKSA_LUX; }
    /**
     * @return The lluminance of one phot
     */
    inline double const phot(){ return GSL_CONST_MKSA_PHOT; }
    /**
     * @return The luminance of one footcandle
     */
    inline double const footcandle(){ return GSL_CONST_MKSA_FOOTCANDLE; }
    /**
     * @return The luminance of one lambert
     */
    inline double const lambert(){ return GSL_CONST_MKSA_LAMBERT; }
    /**
     * @return The luminance of one footlambert
     */
    inline double const footlambert(){ return GSL_CONST_MKSA_FOOTLAMBERT; }
    /**
     * @return The activity of one curie
     */
    inline double const curie(){ return GSL_CONST_MKSA_CURIE; }
    /**
     * @return The exposure of one roentgen
     */
    inline double const roentgen(){ return GSL_CONST_MKSA_ROENTGEN; }
    /**
     * @return The absorbed dose of one rad
     */
    inline double const rad(){ return GSL_CONST_MKSA_RAD; }
    /**
     * @return The mass of the Sun
     */
    inline double const solar_mass(){ return GSL_CONST_MKSA_SOLAR_MASS; }
    /**
     * @return The Bohr radius \f$a_0\f$
     */
    inline double const Bohr_radius(){ return GSL_CONST_MKSA_BOHR_RADIUS; }
    /**
     * @return The force of one Newton
     */
    inline double const Newton(){ return GSL_CONST_MKSA_NEWTON; }
    /**
     * @return The force of one Dyne
     */
    inline double const Dyne(){ return GSL_CONST_MKSA_DYNE; }
    /**
     * @return The energy of one Joule
     */
    inline double const Joule(){ return GSL_CONST_MKSA_JOULE; }
    /**
     * @return The energy of one erg
     */
    inline double const erg(){ return GSL_CONST_MKSA_ERG; }
    /**
     * @return The Stefan--Boltzman radiation constant \f$\sigma\f$
     */
    inline double const Stefan_Boltzmann_constant(){ return GSL_CONST_MKSA_STEFAN_BOLTZMANN_CONSTANT; }
    /**
     * @return The Thomson cross section \f$\sigma_{\mbox{T}}\f$
     */
    inline double const Thomson_cross_section(){ return GSL_CONST_MKSA_THOMSON_CROSS_SECTION; }
    /**
     * @return The Bohr magneton \f$\mu_{\mbox{B}}\f$
     */
    inline double const Bohr_magneton(){ return GSL_CONST_MKSA_BOHR_MAGNETON; }
    /**
     * @return The nuclear magneton \f$\mu_{\mbox{N}}\f$
     */
    inline double const nuclear_magneton(){ return GSL_CONST_MKSA_NUCLEAR_MAGNETON; }
    /**
     * @return The absolute value of the magnetic moment of the electron \f$\mu_{\mbox{e}}\f$
     */
    inline double const electon_magnetic_moment(){ return GSL_CONST_MKSA_ELECTRON_MAGNETIC_MOMENT; }
    /**
     * @return The absolute value of the magnetic moment of the proton \f$\mu_{\mbox{p}}\f$
     */
    inline double const proton_magnetic_moment(){ return GSL_CONST_MKSA_PROTON_MAGNETIC_MOMENT; }
    /**
     * @return The molar charge of one Faraday
     */
    inline double const Faraday(){ return GSL_CONST_MKSA_FARADAY; }
    /**
     * @return The charge on an electron
     */
    inline double const electon_charge(){ return GSL_CONST_MKSA_ELECTRON_CHARGE; }
    /**
     * @return The permitivity of free space
     */
    inline double const vacuum_permitivity(){ return GSL_CONST_MKSA_VACUUM_PERMITTIVITY; }
    /**
     * @return The permeability of free space
     */
    inline double const vacuum_permeability(){ return GSL_CONST_MKSA_VACUUM_PERMEABILITY; }
    /**
     * @return The electrical dipole moment of one Debye
     */
    inline double const Debye(){ return GSL_CONST_MKSA_DEBYE; }
    /**
     * @return the magnetic field of one Gauss
     */
    inline double const Gauss(){ return GSL_CONST_MKSA_GAUSS; }
  }
}
#endif