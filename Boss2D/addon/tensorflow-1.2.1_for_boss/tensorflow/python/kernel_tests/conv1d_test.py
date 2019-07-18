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
"""Tests for convolution related functionality in tensorflow.ops.nn."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import nn_ops
from tensorflow.python.platform import test


class Conv1DTest(test.TestCase):

  def testBasic(self):
    """Test that argument passing to conv2d is handled properly."""

    x = constant_op.constant([1, 2, 3, 4], dtype=dtypes.float32)
    x = array_ops.expand_dims(x, 0)  # Add batch dimension
    x = array_ops.expand_dims(x, 2)  # And depth dimension
    filters = constant_op.constant([2, 1], dtype=dtypes.float32)
    filters = array_ops.expand_dims(filters, 1)  # in_channels
    filters = array_ops.expand_dims(filters, 2)  # out_channels
    # Filters is 2x1x1
    for stride in [1, 2]:
      with self.test_session():
        c = nn_ops.conv1d(x, filters, stride, padding="VALID")
        reduced = array_ops.squeeze(c)
        output = reduced.eval()
        if stride == 1:
          self.assertEqual(len(output), 3)
          self.assertAllClose(output,
                              [2 * 1 + 1 * 2, 2 * 2 + 1 * 3, 2 * 3 + 1 * 4])
        else:
          self.assertEqual(len(output), 2)
          self.assertAllClose(output, [2 * 1 + 1 * 2, 2 * 3 + 1 * 4])


if __name__ == "__main__":
  test.main()
