# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
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
"""Tests for Momentum."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.compiler.tests.xla_test import XLATestCase
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import resource_variable_ops
from tensorflow.python.ops import variables
from tensorflow.python.platform import test
from tensorflow.python.training import momentum as momentum_lib


class MomentumOptimizerTest(XLATestCase):

  def _update_nesterov_momentum_numpy(self, var, accum, g, lr, momentum):
    var += accum * lr * momentum
    accum = accum * momentum + g
    var -= lr * accum
    var -= accum * lr * momentum
    return var, accum

  def testBasic(self):
    for dtype in self.float_types:
      with self.test_session(), self.test_scope():
        var0 = resource_variable_ops.ResourceVariable([1.0, 2.0], dtype=dtype)
        var1 = resource_variable_ops.ResourceVariable([3.0, 4.0], dtype=dtype)
        grads0 = constant_op.constant([0.1, 0.1], dtype=dtype)
        grads1 = constant_op.constant([0.01, 0.01], dtype=dtype)
        mom_opt = momentum_lib.MomentumOptimizer(
            learning_rate=2.0, momentum=0.9)
        mom_update = mom_opt.apply_gradients(
            zip([grads0, grads1], [var0, var1]))
        variables.global_variables_initializer().run()
        # Check we have slots
        self.assertEqual(["momentum"], mom_opt.get_slot_names())
        slot0 = mom_opt.get_slot(var0, "momentum")
        self.assertEquals(slot0.get_shape(), var0.get_shape())
        self.assertFalse(slot0 in variables.trainable_variables())
        slot1 = mom_opt.get_slot(var1, "momentum")
        self.assertEquals(slot1.get_shape(), var1.get_shape())
        self.assertFalse(slot1 in variables.trainable_variables())

        # Fetch params to validate initial values
        self.assertAllClose([1.0, 2.0], var0.eval())
        self.assertAllClose([3.0, 4.0], var1.eval())
        # Step 1: the momentum accumulators where 0. So we should see a normal
        # update: v -= grad * learning_rate
        mom_update.run()
        # Check that the momentum accumulators have been updated.
        self.assertAllCloseAccordingToType(np.array([0.1, 0.1]), slot0.eval())
        self.assertAllCloseAccordingToType(np.array([0.01, 0.01]), slot1.eval())
        # Check that the parameters have been updated.
        self.assertAllCloseAccordingToType(
            np.array([1.0 - (0.1 * 2.0), 2.0 - (0.1 * 2.0)]), var0.eval())
        self.assertAllCloseAccordingToType(
            np.array([3.0 - (0.01 * 2.0), 4.0 - (0.01 * 2.0)]), var1.eval())
        # Step 2: the momentum accumulators contain the previous update.
        mom_update.run()
        # Check that the momentum accumulators have been updated.
        self.assertAllCloseAccordingToType(
            np.array([(0.9 * 0.1 + 0.1), (0.9 * 0.1 + 0.1)]), slot0.eval())
        self.assertAllCloseAccordingToType(
            np.array([(0.9 * 0.01 + 0.01), (0.9 * 0.01 + 0.01)]), slot1.eval())
        # Check that the parameters have been updated.
        self.assertAllCloseAccordingToType(
            np.array([
                1.0 - (0.1 * 2.0) - ((0.9 * 0.1 + 0.1) * 2.0),
                2.0 - (0.1 * 2.0) - ((0.9 * 0.1 + 0.1) * 2.0)
            ]), var0.eval())
        self.assertAllCloseAccordingToType(
            np.array([
                2.98 - ((0.9 * 0.01 + 0.01) * 2.0), 3.98 - (
                    (0.9 * 0.01 + 0.01) * 2.0)
            ]), var1.eval())

  def testNesterovMomentum(self):
    for dtype in self.float_types:
      with self.test_session(), self.test_scope():
        var0 = resource_variable_ops.ResourceVariable([1.0, 2.0], dtype=dtype)
        var1 = resource_variable_ops.ResourceVariable([3.0, 4.0], dtype=dtype)
        var0_np = np.array([1.0, 2.0], dtype=dtype)
        var1_np = np.array([3.0, 4.0], dtype=dtype)
        accum0_np = np.array([0.0, 0.0], dtype=dtype)
        accum1_np = np.array([0.0, 0.0], dtype=dtype)
        cost = 5 * var0 * var0 + 3 * var1
        global_step = resource_variable_ops.ResourceVariable(
            array_ops.zeros([], dtypes.int32), name="global_step")
        mom_op = momentum_lib.MomentumOptimizer(
            learning_rate=2.0, momentum=0.9, use_nesterov=True)
        opt_op = mom_op.minimize(cost, global_step, [var0, var1])
        variables.global_variables_initializer().run()
        for _ in range(1, 5):
          opt_op.run()
          var0_np, accum0_np = self._update_nesterov_momentum_numpy(
              var0_np, accum0_np, var0_np * 10, 2.0, 0.9)
          var1_np, accum1_np = self._update_nesterov_momentum_numpy(var1_np,
                                                                    accum1_np,
                                                                    3, 2.0, 0.9)
          self.assertAllClose(var0_np, var0.eval())
          self.assertAllClose(var1_np, var1.eval())

  def testTensorLearningRateAndMomentum(self):
    for dtype in self.float_types:
      with self.test_session(), self.test_scope():
        var0 = resource_variable_ops.ResourceVariable([1.0, 2.0], dtype=dtype)
        var1 = resource_variable_ops.ResourceVariable([3.0, 4.0], dtype=dtype)
        grads0 = constant_op.constant([0.1, 0.1], dtype=dtype)
        grads1 = constant_op.constant([0.01, 0.01], dtype=dtype)
        mom_opt = momentum_lib.MomentumOptimizer(
            learning_rate=constant_op.constant(2.0),
            momentum=constant_op.constant(0.9))
        mom_update = mom_opt.apply_gradients(
            zip([grads0, grads1], [var0, var1]))
        variables.global_variables_initializer().run()
        # Check we have slots
        self.assertEqual(["momentum"], mom_opt.get_slot_names())
        slot0 = mom_opt.get_slot(var0, "momentum")
        self.assertEquals(slot0.get_shape(), var0.get_shape())
        self.assertFalse(slot0 in variables.trainable_variables())
        slot1 = mom_opt.get_slot(var1, "momentum")
        self.assertEquals(slot1.get_shape(), var1.get_shape())
        self.assertFalse(slot1 in variables.trainable_variables())

        # Fetch params to validate initial values
        self.assertAllClose([1.0, 2.0], var0.eval())
        self.assertAllClose([3.0, 4.0], var1.eval())
        # Step 1: the momentum accumulators where 0. So we should see a normal
        # update: v -= grad * learning_rate
        mom_update.run()
        # Check that the momentum accumulators have been updated.
        self.assertAllCloseAccordingToType(np.array([0.1, 0.1]), slot0.eval())
        self.assertAllCloseAccordingToType(np.array([0.01, 0.01]), slot1.eval())
        # Check that the parameters have been updated.
        self.assertAllCloseAccordingToType(
            np.array([1.0 - (0.1 * 2.0), 2.0 - (0.1 * 2.0)]), var0.eval())
        self.assertAllCloseAccordingToType(
            np.array([3.0 - (0.01 * 2.0), 4.0 - (0.01 * 2.0)]), var1.eval())
        # Step 2: the momentum accumulators contain the previous update.
        mom_update.run()
        # Check that the momentum accumulators have been updated.
        self.assertAllCloseAccordingToType(
            np.array([(0.9 * 0.1 + 0.1), (0.9 * 0.1 + 0.1)]), slot0.eval())
        self.assertAllCloseAccordingToType(
            np.array([(0.9 * 0.01 + 0.01), (0.9 * 0.01 + 0.01)]), slot1.eval())
        # Check that the parameters have been updated.
        self.assertAllCloseAccordingToType(
            np.array([
                1.0 - (0.1 * 2.0) - ((0.9 * 0.1 + 0.1) * 2.0),
                2.0 - (0.1 * 2.0) - ((0.9 * 0.1 + 0.1) * 2.0)
            ]), var0.eval())
        self.assertAllCloseAccordingToType(
            np.array([
                2.98 - ((0.9 * 0.01 + 0.01) * 2.0), 3.98 - (
                    (0.9 * 0.01 + 0.01) * 2.0)
            ]), var1.eval())


if __name__ == "__main__":
  test.main()
