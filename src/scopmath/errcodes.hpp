/** @file errcodes.hpp
 *  @copyright (c) 1984-90 Duke University
 */
#pragma once
namespace neuron::scopmath {
inline constexpr auto ROUNDOFF = 1.e-20;
inline constexpr auto ZERO = 1.e-8;
inline constexpr auto STEP = 1.e-6;
inline constexpr auto CONVERGE = 1.e-6;
inline constexpr auto MAXCHANGE = 0.05;
inline constexpr auto INITSIMPLEX = 0.25;
inline constexpr auto MAXITERS = 50;
inline constexpr auto MAXSMPLXITERS = 100;
inline constexpr auto MAXSTEPS = 20;
inline constexpr auto MAXHALVE = 15;
inline constexpr auto MAXORDER = 6;
inline constexpr auto MAXTERMS = 3;
inline constexpr auto MAXFAIL = 10;
inline constexpr auto MAX_JAC_ITERS = 20;
inline constexpr auto MAX_GOOD_ORDER = 2;
inline constexpr auto MAX_GOOD_STEPS = 3;
inline constexpr auto SUCCESS = 0;
inline constexpr auto EXCEED_ITERS = 1;
inline constexpr auto SINGULAR = 2;
inline constexpr auto PRECISION = 3;
inline constexpr auto CORR_FAIL = 4;
inline constexpr auto INCONSISTENT = 5;
inline constexpr auto BAD_START = 6;
inline constexpr auto NODATA = 7;
inline constexpr auto NO_SOLN = 8;
inline constexpr auto LOWMEM = 9;
inline constexpr auto DIVCHECK = 10;
inline constexpr auto NOFORCE = 11;
inline constexpr auto DIVERGED = 12;
inline constexpr auto NEG_ARG = 13;
inline constexpr auto RANGE = 14;
}  // namespace neuron::scopmath
