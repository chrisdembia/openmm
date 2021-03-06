/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2010-2014 Stanford University and the Authors.      *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "openmm/internal/AssertionUtilities.h"
#include "openmm/CustomTorsionForce.h"
#include "openmm/serialization/XmlSerializer.h"
#include <iostream>
#include <sstream>

using namespace OpenMM;
using namespace std;

void testSerialization() {
    // Create a Force.

    CustomTorsionForce force("5*sin(x)^2+y*z");
    force.setForceGroup(3);
    force.addGlobalParameter("x", 1.3);
    force.addGlobalParameter("y", 2.221);
    force.addPerTorsionParameter("z");
    vector<double> params(1);
    params[0] = 1.0;
    force.addTorsion(1, 2, 3, 4, params);
    params[0] = -3.3;
    force.addTorsion(4, 0, 1, 5, params);
    params[0] = 2.1;
    force.addTorsion(3, 7, 6, 8, params);

    // Serialize and then deserialize it.

    stringstream buffer;
    XmlSerializer::serialize<CustomTorsionForce>(&force, "Force", buffer);
    CustomTorsionForce* copy = XmlSerializer::deserialize<CustomTorsionForce>(buffer);

    // Compare the two forces to see if they are identical.

    CustomTorsionForce& force2 = *copy;
    ASSERT_EQUAL(force.getForceGroup(), force2.getForceGroup());
    ASSERT_EQUAL(force.getEnergyFunction(), force2.getEnergyFunction());
    ASSERT_EQUAL(force.getNumPerTorsionParameters(), force2.getNumPerTorsionParameters());
    for (int i = 0; i < force.getNumPerTorsionParameters(); i++)
        ASSERT_EQUAL(force.getPerTorsionParameterName(i), force2.getPerTorsionParameterName(i));
    ASSERT_EQUAL(force.getNumGlobalParameters(), force2.getNumGlobalParameters());
    for (int i = 0; i < force.getNumGlobalParameters(); i++) {
        ASSERT_EQUAL(force.getGlobalParameterName(i), force2.getGlobalParameterName(i));
        ASSERT_EQUAL(force.getGlobalParameterDefaultValue(i), force2.getGlobalParameterDefaultValue(i));
    }
    ASSERT_EQUAL(force.getNumTorsions(), force2.getNumTorsions());
    for (int i = 0; i < force.getNumTorsions(); i++) {
        int a1, a2, b1, b2, c1, c2, d1, d2;
        vector<double> params1, params2;
        force.getTorsionParameters(i, a1, b1, c1, d1, params1);
        force2.getTorsionParameters(i, a2, b2, c2, d2, params2);
        ASSERT_EQUAL(a1, a2);
        ASSERT_EQUAL(b2, b2);
        ASSERT_EQUAL(c2, c2);
        ASSERT_EQUAL(d2, d2);
        ASSERT_EQUAL(params1.size(), params2.size());
        for (int j = 0; j < params1.size(); j++)
            ASSERT_EQUAL(params1[j], params2[j]);
    }
}

int main() {
    try {
        testSerialization();
    }
    catch(const exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }
    cout << "Done" << endl;
    return 0;
}
