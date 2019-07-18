# Copyright 2016 The TensorFlow Authors. All Rights Reserved.
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
"""Identity bijector."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.framework import constant_op
from tensorflow.python.ops.distributions import bijector


__all__ = [
    "Identity",
]


class Identity(bijector.Bijector):
  """Compute Y = g(X) = X.

    Example Use:

    ```python
    # Create the Y=g(X)=X transform which is intended for Tensors with 1 batch
    # ndim and 1 event ndim (i.e., vector of vectors).
    identity = Identity(event_ndims=1)
    x = [[1., 2],
         [3, 4]]
    x == identity.forward(x) == identity.inverse(x)
    ```

  """

  def __init__(self, validate_args=False, event_ndims=0, name="identity"):
    super(Identity, self).__init__(
        is_constant_jacobian=True,
        event_ndims=event_ndims,
        validate_args=validate_args,
        name=name)

  def _forward(self, x):
    return x

  def _inverse(self, y):
    return y

  def _inverse_log_det_jacobian(self, y):
    return constant_op.constant(0., dtype=y.dtype)

  def _forward_log_det_jacobian(self, x):
    return constant_op.constant(0., dtype=x.dtype)
