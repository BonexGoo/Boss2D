# Copyright 2016 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BAvSIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Tests for summary ops."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import six

from tensorflow.core.framework import summary_pb2
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor_util
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import summary_ops
from tensorflow.python.platform import test


class SummaryOpsTest(test.TestCase):

  def _SummarySingleValue(self, s):
    summ = summary_pb2.Summary()
    summ.ParseFromString(s)
    self.assertEqual(len(summ.value), 1)
    return summ.value[0]

  def _AssertNumpyEq(self, actual, expected):
    self.assertTrue(np.array_equal(actual, expected))

  def testNodeNames(self):
    with self.test_session() as sess:
      c = constant_op.constant(1)
      s1 = summary_ops.tensor_summary("s1", c)
      with ops.name_scope("foo"):
        s2 = summary_ops.tensor_summary("s2", c)
        with ops.name_scope("zod"):
          s3 = summary_ops.tensor_summary("s3", c)
          s4 = summary_ops.tensor_summary("TensorSummary", c)
      summ1, summ2, summ3, summ4 = sess.run([s1, s2, s3, s4])

    v1 = self._SummarySingleValue(summ1)
    self.assertEqual(v1.node_name, "s1")

    v2 = self._SummarySingleValue(summ2)
    self.assertEqual(v2.node_name, "foo/s2")

    v3 = self._SummarySingleValue(summ3)
    self.assertEqual(v3.node_name, "foo/zod/s3")

    v4 = self._SummarySingleValue(summ4)
    self.assertEqual(v4.node_name, "foo/zod/TensorSummary")

  def testScalarSummary(self):
    with self.test_session() as sess:
      const = constant_op.constant(10.0)
      summ = summary_ops.tensor_summary("foo", const)
      result = sess.run(summ)

    value = self._SummarySingleValue(result)
    n = tensor_util.MakeNdarray(value.tensor)
    self._AssertNumpyEq(n, 10)

  def testStringSummary(self):
    s = six.b("foobar")
    with self.test_session() as sess:
      const = constant_op.constant(s)
      summ = summary_ops.tensor_summary("foo", const)
      result = sess.run(summ)

    value = self._SummarySingleValue(result)
    n = tensor_util.MakeNdarray(value.tensor)
    self._AssertNumpyEq(n, s)

  def testManyScalarSummary(self):
    with self.test_session() as sess:
      const = array_ops.ones([5, 5, 5])
      summ = summary_ops.tensor_summary("foo", const)
      result = sess.run(summ)
    value = self._SummarySingleValue(result)
    n = tensor_util.MakeNdarray(value.tensor)
    self._AssertNumpyEq(n, np.ones([5, 5, 5]))

  def testManyStringSummary(self):
    strings = [[six.b("foo bar"), six.b("baz")], [six.b("zoink"), six.b("zod")]]
    with self.test_session() as sess:
      const = constant_op.constant(strings)
      summ = summary_ops.tensor_summary("foo", const)
      result = sess.run(summ)
    value = self._SummarySingleValue(result)
    n = tensor_util.MakeNdarray(value.tensor)
    self._AssertNumpyEq(n, strings)

  def testManyBools(self):
    bools = [True, True, True, False, False, False]
    with self.test_session() as sess:
      const = constant_op.constant(bools)
      summ = summary_ops.tensor_summary("foo", const)
      result = sess.run(summ)

    value = self._SummarySingleValue(result)
    n = tensor_util.MakeNdarray(value.tensor)
    self._AssertNumpyEq(n, bools)


if __name__ == "__main__":
  test.main()
