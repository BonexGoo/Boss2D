# Copyright 2015 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Tests for tensorflow.ops.numerics."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import control_flow_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.ops import numerics
from tensorflow.python.platform import test


class VerifyTensorAllFiniteTest(test.TestCase):

  def testVerifyTensorAllFiniteSucceeds(self):
    x_shape = [5, 4]
    x = np.random.random_sample(x_shape).astype(np.float32)
    with self.test_session(use_gpu=True):
      t = constant_op.constant(x, shape=x_shape, dtype=dtypes.float32)
      t_verified = numerics.verify_tensor_all_finite(t,
                                                     "Input is not a number.")
      self.assertAllClose(x, t_verified.eval())

  def testVerifyTensorAllFiniteFails(self):
    x_shape = [5, 4]
    x = np.random.random_sample(x_shape).astype(np.float32)
    my_msg = "Input is not a number."

    # Test NaN.
    x[0] = np.nan
    with self.test_session(use_gpu=True):
      with self.assertRaisesOpError(my_msg):
        t = constant_op.constant(x, shape=x_shape, dtype=dtypes.float32)
        t_verified = numerics.verify_tensor_all_finite(t, my_msg)
        t_verified.eval()

    # Test Inf.
    x[0] = np.inf
    with self.test_session(use_gpu=True):
      with self.assertRaisesOpError(my_msg):
        t = constant_op.constant(x, shape=x_shape, dtype=dtypes.float32)
        t_verified = numerics.verify_tensor_all_finite(t, my_msg)
        t_verified.eval()


class NumericsTest(test.TestCase):

  def testInf(self):
    with self.test_session(graph=ops.Graph()):
      t1 = constant_op.constant(1.0)
      t2 = constant_op.constant(0.0)
      a = math_ops.div(t1, t2)
      check = numerics.add_check_numerics_ops()
      a = control_flow_ops.with_dependencies([check], a)
      with self.assertRaisesOpError("Inf"):
        a.eval()

  def testNaN(self):
    with self.test_session(graph=ops.Graph()):
      t1 = constant_op.constant(0.0)
      t2 = constant_op.constant(0.0)
      a = math_ops.div(t1, t2)
      check = numerics.add_check_numerics_ops()
      a = control_flow_ops.with_dependencies([check], a)
      with self.assertRaisesOpError("NaN"):
        a.eval()

  def testBoth(self):
    with self.test_session(graph=ops.Graph()):
      t1 = constant_op.constant([1.0, 0.0])
      t2 = constant_op.constant([0.0, 0.0])
      a = math_ops.div(t1, t2)
      check = numerics.add_check_numerics_ops()
      a = control_flow_ops.with_dependencies([check], a)
      with self.assertRaisesOpError("Inf and NaN"):
        a.eval()

  def testPassThrough(self):
    with self.test_session(graph=ops.Graph()):
      t1 = constant_op.constant([1.0, 2.0, 3.0, 4.0, 5.0, 6.0], shape=[2, 3])
      checked = array_ops.check_numerics(t1, message="pass through test")
      value = checked.eval()
      self.assertAllEqual(np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]), value)
      self.assertEqual([2, 3], checked.get_shape())


if __name__ == "__main__":
  test.main()
