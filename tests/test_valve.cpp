/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.2
 Module:       test_valve.cpp
 Description:  Tests EPANET toolkit api functions
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 07/28/2022
 ******************************************************************************
*/

/*
   Tests PCV valve with position curve
*/

#include <boost/test/unit_test.hpp>

#include "test_toolkit.hpp"

BOOST_AUTO_TEST_SUITE (test_valve)

BOOST_FIXTURE_TEST_CASE(test_PCV_valve, FixtureOpenClose)

{
    int npts = 5;
    double x[] = { 0.0, 0.25, 0.5, 0.75, 1. };
    double y[] = {0.0, 0.089, 0.184, 0.406, 1.0};
    double v;
    int linkIndex, curveIndex;

    // Make steady state run
    error = EN_settimeparam(ph, EN_DURATION, 0);
    BOOST_REQUIRE(error == 0);

    // Convert pipe 22 to a PCV
    error = EN_getlinkindex(ph, (char*)"22", &linkIndex);
    BOOST_REQUIRE(error == 0);
    error = EN_setlinktype(ph, &linkIndex, EN_PCV, EN_UNCONDITIONAL);
    BOOST_REQUIRE(error == 0);
    error = EN_setlinkvalue(ph, linkIndex, EN_DIAMETER, 12);
    BOOST_REQUIRE(error == 0);
    error = EN_setlinkvalue(ph, linkIndex, EN_MINORLOSS, 0.19);

    // Create the PCV's position-loss curve
    error = EN_addcurve(ph, (char*)"ValveCurve");
    BOOST_REQUIRE(error == 0);
    error = EN_getcurveindex(ph, (char*)"ValveCurve", &curveIndex);
    BOOST_REQUIRE(error == 0);
    error = EN_setcurve(ph, curveIndex, x, y, npts);
    BOOST_REQUIRE(error == 0);

    // Assign curve & initial setting to PCV
    error = EN_setlinkvalue(ph, linkIndex, EN_PCV_CURVE, curveIndex);
    BOOST_REQUIRE(error == 0);
    error = EN_setlinkvalue(ph, linkIndex, EN_INITSETTING, 0.35);
    BOOST_REQUIRE(error == 0);

    // Solve for hydraulics
    error = EN_solveH(ph);
    BOOST_REQUIRE(error == 0);

    // The PCV interpolated relative flow coeff. at 0.35 open is 0.127.
    // This translates to a minor loss coeff. of 0.19 / 0.127^2 = 11.78.
    // If the PCV were replaced with a TCV at that setting the resulting
    // head loss would be 0.0255 ft which should equal the PCV result. 
    error = EN_getlinkvalue(ph, linkIndex, EN_HEADLOSS, &v);
    BOOST_REQUIRE(error == 0);
    BOOST_REQUIRE(abs(v - 0.0255) < 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
