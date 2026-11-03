# Copyright (C) 2026 Open Source Robotics Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import copy
import math
import unittest

from gz.math import AdditivelySeparableScalarField3d
from gz.math import AdditivelySeparableScalarField3f
from gz.math import Intervald, PiecewiseScalarField3d, PiecewiseScalarField3f
from gz.math import Polynomial3d, Polynomial3f, Region3d
from gz.math import Vector3d, Vector3f, Vector4d, Vector4f


class TestPiecewiseScalarField3(unittest.TestCase):

    def setUp(self):
        quadratic = Polynomial3d(Vector4d(0., 1., 0., 0.))
        zero = Polynomial3d(Vector4d.ZERO)
        left = AdditivelySeparableScalarField3d(
            1., quadratic, zero, zero)
        right = AdditivelySeparableScalarField3d(
            -1., quadratic, zero, zero)
        fixed = Intervald.closed(0., 0.)
        self.field = PiecewiseScalarField3d([
            (Region3d(Intervald.closed(-2., -1.), fixed, fixed),
             left),
            (Region3d(Intervald.closed(1., 2.), fixed, fixed),
             right),
        ])

    def test_evaluate(self):
        self.assertEqual(self.field.evaluate(Vector3d(-1., 0., 0.)), 1.)
        self.assertEqual(self.field(Vector3d(1., 0., 0.)), -1.)
        self.assertTrue(math.isnan(self.field(Vector3d.ZERO)))

        one = Polynomial3f(Vector4f(0., 0., 0., 1.))
        constant = AdditivelySeparableScalarField3f(
            2. / 3., one, one, one)
        field = PiecewiseScalarField3f.throughout(constant)
        self.assertEqual(field(Vector3f.ONE), 2.)

    def test_minimum(self):
        self.assertEqual(self.field.minimum(), -1.)
        value, point = self.field.minimum(Vector3d.NAN)
        self.assertEqual(value, -1.)
        self.assertEqual(point, Vector3d.UNIT_X)

        undefined = PiecewiseScalarField3d()
        self.assertTrue(math.isnan(undefined.minimum()))
        value, point = undefined.minimum(Vector3d.ZERO)
        self.assertTrue(math.isnan(value))
        self.assertTrue(math.isnan(point.x()))
        self.assertTrue(math.isnan(point.y()))
        self.assertTrue(math.isnan(point.z()))

    def test_copy_and_string(self):
        expected = str(self.field)
        self.assertIn("if (x, y, z) in", expected)
        self.assertEqual(repr(self.field), expected)
        self.assertEqual(str(copy.copy(self.field)), expected)
        self.assertEqual(str(copy.deepcopy(self.field)), expected)
        self.assertEqual(str(PiecewiseScalarField3d()), "undefined")


if __name__ == '__main__':
    unittest.main()
