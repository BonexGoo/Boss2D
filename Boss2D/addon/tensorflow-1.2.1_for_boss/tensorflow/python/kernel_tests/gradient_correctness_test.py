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
"""Tests for tensorflow.ops.argmax_op."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.ops import gradients_impl
from tensorflow.python.ops import math_ops
from tensorflow.python.platform import test


class GradientCorrectnessTest(test.TestCase):

  def testMultipleOutputChainedGradients(self):
    with self.test_session() as sess:
      x = constant_op.constant(1.0, dtype=dtypes.float32)
      yexp = math_ops.exp(x)
      yexplog = math_ops.log(yexp)
      grads = gradients_impl.gradients([yexp, yexplog], [x])
      grad_vals = sess.run(grads)
      exp1_plus_one = (1.0 + np.exp(1.0)).astype(np.float32)
      # [dexp(x)/dx + d(log(exp(x)))/dx] @ x=1 == exp(1) + 1
      self.assertAllClose(grad_vals[0], exp1_plus_one)


if __name__ == '__main__':
  test.main()
