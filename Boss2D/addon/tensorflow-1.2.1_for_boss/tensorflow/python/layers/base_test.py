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
"""Tests for tf.layers.base."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import copy

from tensorflow.python.framework import ops
from tensorflow.python.layers import base as base_layers
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import init_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.ops import random_ops
from tensorflow.python.ops import variable_scope
from tensorflow.python.platform import test


class BaseLayerTest(test.TestCase):

  def testLayerProperties(self):
    layer = base_layers.Layer(name='my_layer')
    self.assertListEqual(layer.variables, [])
    self.assertListEqual(layer.trainable_variables, [])
    self.assertListEqual(layer.non_trainable_variables, [])
    self.assertListEqual(layer.updates, [])
    self.assertListEqual(layer.losses, [])
    self.assertEqual(layer.built, False)
    layer = base_layers.Layer(name='my_layer', trainable=False)
    self.assertEqual(layer.trainable, False)

  def testAddWeight(self):
    with self.test_session():
      layer = base_layers.Layer(name='my_layer')

      # Test basic variable creation.
      variable = layer.add_variable(
          'my_var', [2, 2], initializer=init_ops.zeros_initializer())
      self.assertEqual(variable.name, 'my_layer/my_var:0')
      self.assertListEqual(layer.variables, [variable])
      self.assertListEqual(layer.trainable_variables, [variable])
      self.assertListEqual(layer.non_trainable_variables, [])
      self.assertListEqual(
          layer.variables,
          ops.get_collection(ops.GraphKeys.TRAINABLE_VARIABLES))

      # Test non-trainable variable creation.
      # layer.add_variable should work even outside `build` and `call`.
      variable_2 = layer.add_variable(
          'non_trainable_var', [2, 2],
          initializer=init_ops.zeros_initializer(),
          trainable=False)
      self.assertListEqual(layer.variables, [variable, variable_2])
      self.assertListEqual(layer.trainable_variables, [variable])
      self.assertListEqual(layer.non_trainable_variables, [variable_2])
      self.assertEqual(
          len(ops.get_collection(ops.GraphKeys.TRAINABLE_VARIABLES)), 1)

      # Test with regularizer.
      regularizer = lambda x: math_ops.reduce_sum(x) * 1e-3
      variable = layer.add_variable(
          'reg_var', [2, 2],
          initializer=init_ops.zeros_initializer(),
          regularizer=regularizer)
      self.assertEqual(len(layer.losses), 1)

  def testGetVariable(self):
    with self.test_session():

      class MyLayer(base_layers.Layer):

        def build(self, input_shape):
          self.my_var = self.add_variable(
              'my_var', [2, 2], initializer=init_ops.zeros_initializer())

        def call(self, inputs):
          return inputs * 2

      layer = MyLayer(name='my_layer')
      inputs = random_ops.random_uniform((5,), seed=1)
      layer.apply(inputs)
      layer.apply(inputs)
      self.assertListEqual([v.name for v in layer.variables],
                           ['my_layer/my_var:0'])

      # Creating a layer with no scope leads to lazy construction of
      # the scope at apply() time.  It uses scope "<current scope>/base_name"
      lazy_layer = MyLayer(_reuse=True)
      with variable_scope.variable_scope('new_scope'):
        # This should attempt to reuse 'my_var' in 'new_scope'
        with self.assertRaisesRegexp(
            ValueError, r'new_scope/my_layer/my_var does not exist'):
          lazy_layer.apply(inputs)
        with variable_scope.variable_scope('my_layer'):
          variable_scope.get_variable('my_var', [2, 2])

        # Smoke test: it runs.
        lazy_layer.apply(inputs)
        # The variables were created outside of the Layer, and
        # reuse=True, so the Layer does not own them and they are not
        # stored in its collection.
        self.assertListEqual(lazy_layer.variables, [])
        self.assertEqual(lazy_layer._scope.name, 'new_scope/my_layer')

      # Creating a layer with no scope leads to lazy construction of
      # the scope at apply() time.  If 'scope' argument is passed to
      # apply(), it uses that scope when accessing variables.
      lazy_layer = MyLayer(_reuse=True)
      with variable_scope.variable_scope('new_scope') as new_scope:
        # This should attempt to reuse 'my_var' in 'new_scope'
        with self.assertRaisesRegexp(
            ValueError, r'new_scope/my_var does not exist'):
          lazy_layer.apply(inputs, scope=new_scope)
        variable_scope.get_variable('my_var', [2, 2])

        # Smoke test: it runs.
        lazy_layer.apply(inputs, scope=new_scope)
        # The variables were created outside of the Layer, and
        # reuse=True, so the Layer does not own them and they are not
        # stored in its collection.
        self.assertListEqual(lazy_layer.variables, [])
        self.assertEqual(lazy_layer._scope.name, 'new_scope')

      with ops.Graph().as_default():
        inputs_ng = random_ops.random_uniform((5,), seed=1)
        with self.assertRaisesRegexp(ValueError,
                                     r'graph are not the same'):
          layer.apply(inputs_ng)

  def testCall(self):

    class MyLayer(base_layers.Layer):

      def call(self, inputs):
        return math_ops.square(inputs)

    layer = MyLayer(name='my_layer')
    inputs = random_ops.random_uniform((5,), seed=1)
    outputs = layer.apply(inputs)
    self.assertEqual(layer.built, True)
    self.assertEqual(outputs.op.name, 'my_layer/Square')

  def testFirstCallCanCreateVariablesButSecondCanNotWhenBuildEmpty(self):

    class MyLayer(base_layers.Layer):

      def build(self, _):
        # Do not mark the layer as built.
        pass

      def call(self, inputs):
        self.my_var = self.add_variable('my_var', [2, 2])
        if self.built:
          # Skip creating on the first call; try to create after it's
          # built.  This is expected to fail.
          self.add_variable('this_will_break_on_second_call', [2, 2])
        return inputs + math_ops.square(self.my_var)

    layer = MyLayer(name='my_layer')
    inputs = random_ops.random_uniform((2,), seed=1)
    outputs = layer.apply(inputs)
    self.assertEqual(layer.built, True)
    self.assertEqual(outputs.op.name, 'my_layer/add')
    self.assertListEqual(
        [v.name for v in layer.variables], ['my_layer/my_var:0'])
    with self.assertRaisesRegexp(ValueError,
                                 'my_layer/this_will_break_on_second_call'):
      layer.apply(inputs)
    # The list of variables hasn't changed.
    self.assertListEqual(
        [v.name for v in layer.variables], ['my_layer/my_var:0'])

  def testDeepCopy(self):

    class MyLayer(base_layers.Layer):

      def call(self, inputs):
        return math_ops.square(inputs)

    layer = MyLayer(name='my_layer')
    inputs = random_ops.random_uniform((5,), seed=1)
    outputs = layer.apply(inputs)
    self.assertEqual(layer.built, True)
    self.assertEqual(outputs.op.name, 'my_layer/Square')

    layer_copy = copy.deepcopy(layer)
    self.assertEqual(layer_copy.name, layer.name)
    self.assertEqual(layer_copy._scope.name, layer._scope.name)
    self.assertEqual(layer_copy._graph, layer._graph)

  def testScopeNaming(self):

    class PrivateLayer(base_layers.Layer):

      def call(self, inputs):
        return None

    inputs = random_ops.random_uniform((5,))
    default_layer = PrivateLayer()
    _ = default_layer.apply(inputs)
    self.assertEqual(default_layer._scope.name, 'private_layer')
    default_layer1 = PrivateLayer()
    default_layer1.apply(inputs)
    self.assertEqual(default_layer1._scope.name, 'private_layer_1')
    my_layer = PrivateLayer(name='my_layer')
    my_layer.apply(inputs)
    self.assertEqual(my_layer._scope.name, 'my_layer')
    my_layer1 = PrivateLayer(name='my_layer')
    my_layer1.apply(inputs)
    self.assertEqual(my_layer1._scope.name, 'my_layer_1')
    my_layer2 = PrivateLayer(name='my_layer')
    my_layer2.apply(inputs)
    self.assertEqual(my_layer2._scope.name, 'my_layer_2')
    # Name scope shouldn't affect names.
    with ops.name_scope('some_name_scope'):
      default_layer2 = PrivateLayer()
      default_layer2.apply(inputs)
      self.assertEqual(default_layer2._scope.name, 'private_layer_2')
      my_layer3 = PrivateLayer(name='my_layer')
      my_layer3.apply(inputs)
      self.assertEqual(my_layer3._scope.name, 'my_layer_3')
      other_layer = PrivateLayer(name='other_layer')
      other_layer.apply(inputs)
      self.assertEqual(other_layer._scope.name, 'other_layer')
    # Variable scope gets added to scope names.
    with variable_scope.variable_scope('var_scope'):
      default_layer_scoped = PrivateLayer()
      default_layer_scoped.apply(inputs)
      self.assertEqual(default_layer_scoped._scope.name,
                       'var_scope/private_layer')
      my_layer_scoped = PrivateLayer(name='my_layer')
      my_layer_scoped.apply(inputs)
      self.assertEqual(my_layer_scoped._scope.name, 'var_scope/my_layer')
      my_layer_scoped1 = PrivateLayer(name='my_layer')
      my_layer_scoped1.apply(inputs)
      self.assertEqual(my_layer_scoped1._scope.name, 'var_scope/my_layer_1')

  def testInputSpecNdimCheck(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = base_layers.InputSpec(ndim=2)

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()
    with self.assertRaisesRegexp(ValueError,
                                 r'requires a defined rank'):
      layer.apply(array_ops.placeholder('int32'))

    with self.assertRaisesRegexp(ValueError,
                                 r'expected ndim=2'):
      layer.apply(array_ops.placeholder('int32', shape=(None,)))

    # Works
    layer.apply(array_ops.placeholder('int32', shape=(None, None)))

  def testInputSpecMinNdimCheck(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = base_layers.InputSpec(min_ndim=2)

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()
    with self.assertRaisesRegexp(ValueError,
                                 r'requires a defined rank'):
      layer.apply(array_ops.placeholder('int32'))

    with self.assertRaisesRegexp(ValueError,
                                 r'expected min_ndim=2'):
      layer.apply(array_ops.placeholder('int32', shape=(None,)))

    # Works
    layer.apply(array_ops.placeholder('int32', shape=(None, None)))
    layer.apply(array_ops.placeholder('int32', shape=(None, None, None)))

  def testInputSpecMaxNdimCheck(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = base_layers.InputSpec(max_ndim=2)

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()
    with self.assertRaisesRegexp(ValueError,
                                 r'requires a defined rank'):
      layer.apply(array_ops.placeholder('int32'))

    with self.assertRaisesRegexp(ValueError,
                                 r'expected max_ndim=2'):
      layer.apply(array_ops.placeholder('int32', shape=(None, None, None)))

    # Works
    layer.apply(array_ops.placeholder('int32', shape=(None, None)))
    layer.apply(array_ops.placeholder('int32', shape=(None,)))

  def testInputSpecDtypeCheck(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = base_layers.InputSpec(dtype='float32')

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()
    with self.assertRaisesRegexp(ValueError,
                                 r'expected dtype=float32'):
      layer.apply(array_ops.placeholder('int32'))

    # Works
    layer.apply(array_ops.placeholder('float32', shape=(None, None)))

  def testInputSpecAxesCheck(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = base_layers.InputSpec(axes={-1: 2})

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()
    with self.assertRaisesRegexp(ValueError,
                                 r'expected axis'):
      layer.apply(array_ops.placeholder('int32', shape=(None, 3)))

    # Works
    layer.apply(array_ops.placeholder('int32', shape=(None, None, 2)))
    layer.apply(array_ops.placeholder('int32', shape=(None, 2)))

  def testInputSpecShapeCheck(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = base_layers.InputSpec(shape=(None, 3))

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()
    with self.assertRaisesRegexp(ValueError,
                                 r'expected shape'):
      layer.apply(array_ops.placeholder('int32', shape=(None, 2)))

    # Works
    layer.apply(array_ops.placeholder('int32', shape=(None, 3)))
    layer.apply(array_ops.placeholder('int32', shape=(2, 3)))

  def testNoInputSpec(self):

    class CustomerLayer(base_layers.Layer):

      def __init__(self):
        super(CustomerLayer, self).__init__()
        self.input_spec = None

      def call(self, inputs):
        return inputs

    layer = CustomerLayer()

    # Works
    layer.apply(array_ops.placeholder('int32'))
    layer.apply(array_ops.placeholder('int32', shape=(2, 3)))


if __name__ == '__main__':
  test.main()
