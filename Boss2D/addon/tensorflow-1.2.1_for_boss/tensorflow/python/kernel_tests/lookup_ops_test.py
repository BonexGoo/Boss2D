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
"""Tests for lookup ops."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import numpy as np

from tensorflow.python.client import session
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import errors_impl
from tensorflow.python.framework import ops
from tensorflow.python.framework import sparse_tensor
from tensorflow.python.framework import test_util
from tensorflow.python.ops import lookup_ops
from tensorflow.python.platform import test
from tensorflow.python.training import server_lib


class HashTableOpTest(test.TestCase):

  def testHashTable(self):
    with self.test_session():
      default_val = -1
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      self.assertAllEqual(3, table.size().eval())

      input_string = constant_op.constant(["brain", "salad", "tank"])
      output = table.lookup(input_string)
      self.assertAllEqual([3], output.get_shape())

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testHashTableFindHighRank(self):
    with self.test_session():
      default_val = -1
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      self.assertAllEqual(3, table.size().eval())

      input_string = constant_op.constant(
          [["brain", "salad"], ["tank", "tarkus"]])
      output = table.lookup(input_string)

      result = output.eval()
      self.assertAllEqual([[0, 1], [-1, -1]], result)

  def testHashTableInitWithPythonArrays(self):
    with self.test_session():
      default_val = -1
      keys = ["brain", "salad", "surgery"]
      values = [0, 1, 2]
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(
              keys, values, value_dtype=dtypes.int64), default_val)
      table.init.run()

      self.assertAllEqual(3, table.size().eval())

      input_string = constant_op.constant(["brain", "salad", "tank"])
      output = table.lookup(input_string)

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testHashTableInitWithNumPyArrays(self):
    with self.test_session():
      default_val = -1
      keys = np.array(["brain", "salad", "surgery"], dtype=np.str)
      values = np.array([0, 1, 2], dtype=np.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      self.assertAllEqual(3, table.size().eval())

      input_string = constant_op.constant(["brain", "salad", "tank"])
      output = table.lookup(input_string)

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testMultipleHashTables(self):
    with self.test_session() as sess:
      default_val = -1
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)

      table1 = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table2 = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table3 = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)

      lookup_ops.tables_initializer().run()
      self.assertAllEqual(3, table1.size().eval())
      self.assertAllEqual(3, table2.size().eval())
      self.assertAllEqual(3, table3.size().eval())

      input_string = constant_op.constant(["brain", "salad", "tank"])
      output1 = table1.lookup(input_string)
      output2 = table2.lookup(input_string)
      output3 = table3.lookup(input_string)

      out1, out2, out3 = sess.run([output1, output2, output3])
      self.assertAllEqual([0, 1, -1], out1)
      self.assertAllEqual([0, 1, -1], out2)
      self.assertAllEqual([0, 1, -1], out3)

  def testHashTableWithTensorDefault(self):
    with self.test_session():
      default_val = constant_op.constant(-1, dtypes.int64)
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      input_string = constant_op.constant(["brain", "salad", "tank"])
      output = table.lookup(input_string)

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testHashTableWithSparseTensorInput(self):
    with self.test_session() as sess:
      default_val = constant_op.constant(-1, dtypes.int64)
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      sp_indices = [[0, 0], [0, 1], [1, 0]]
      sp_shape = [2, 2]
      input_tensor = sparse_tensor.SparseTensor(
          constant_op.constant(sp_indices, dtypes.int64),
          constant_op.constant(["brain", "salad", "tank"]),
          constant_op.constant(sp_shape, dtypes.int64))
      output = table.lookup(input_tensor)

      out_indices, out_values, out_shape = sess.run(output)

      self.assertAllEqual([0, 1, -1], out_values)
      self.assertAllEqual(sp_indices, out_indices)
      self.assertAllEqual(sp_shape, out_shape)

  def testSignatureMismatch(self):
    with self.test_session():
      default_val = -1
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      input_string = constant_op.constant([1, 2, 3], dtypes.int64)
      with self.assertRaises(TypeError):
        table.lookup(input_string)

      with self.assertRaises(TypeError):
        lookup_ops.HashTable(
            lookup_ops.KeyValueTensorInitializer(keys, values), "UNK")

  def testDTypes(self):
    with self.test_session():
      default_val = -1
      with self.assertRaises(TypeError):
        lookup_ops.HashTable(
            lookup_ops.KeyValueTensorInitializer(["a"], [1], [dtypes.string],
                                                 dtypes.int64), default_val)

  def testNotInitialized(self):
    with self.test_session():
      default_val = -1
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(
              ["a"], [1], value_dtype=dtypes.int64), default_val)

      input_string = constant_op.constant(["brain", "salad", "surgery"])
      output = table.lookup(input_string)

      with self.assertRaisesOpError("Table not initialized"):
        output.eval()

  def testInitializeTwice(self):
    with self.test_session():
      default_val = -1
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2], dtypes.int64)
      table = lookup_ops.HashTable(
          lookup_ops.KeyValueTensorInitializer(keys, values), default_val)
      table.init.run()

      with self.assertRaisesOpError("Table already initialized"):
        table.init.run()

  def testInitializationWithInvalidDimensions(self):
    with self.test_session():
      default_val = -1
      keys = constant_op.constant(["brain", "salad", "surgery"])
      values = constant_op.constant([0, 1, 2, 3, 4], dtypes.int64)

      with self.assertRaises(ValueError):
        lookup_ops.HashTable(
            lookup_ops.KeyValueTensorInitializer(keys, values), default_val)

  def testMultipleSessions(self):
    # Start a server
    server = server_lib.Server(
        {
            "local0": ["localhost:0"]
        }, protocol="grpc", start=True)
    # Create two sessions sharing the same state
    session1 = session.Session(server.target)
    session2 = session.Session(server.target)

    default_val = -1
    keys = constant_op.constant(["brain", "salad", "surgery"])
    values = constant_op.constant([0, 1, 2], dtypes.int64)
    table = lookup_ops.HashTable(
        lookup_ops.KeyValueTensorInitializer(keys, values),
        default_val,
        name="t1")

    # Init the table in the first session.
    with session1:
      table.init.run()
      self.assertAllEqual(3, table.size().eval())

    # Init the table in the second session and verify that we do not get a
    # "Table already initialized" error.
    with session2:
      table.init.run()
      self.assertAllEqual(3, table.size().eval())


class IndexTableFromFile(test.TestCase):

  def _createVocabFile(self, basename, values=("brain", "salad", "surgery")):
    vocabulary_file = os.path.join(self.get_temp_dir(), basename)
    with open(vocabulary_file, "w") as f:
      f.write("\n".join(values) + "\n")
    return vocabulary_file

  def test_string_index_table_from_file(self):
    vocabulary_file = self._createVocabFile("f2i_vocab1.txt")
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file, num_oov_buckets=1)
      ids = table.lookup(constant_op.constant(["salad", "surgery", "tarkus"]))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, 3), ids.eval())

  def test_int32_index_table_from_file(self):
    vocabulary_file = self._createVocabFile(
        "f2i_vocab2.txt", values=("42", "1", "-1000"))
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file,
          num_oov_buckets=1,
          key_dtype=dtypes.int32)
      ids = table.lookup(
          constant_op.constant((1, -1000, 11), dtype=dtypes.int32))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, 3), ids.eval())

  def test_int64_index_table_from_file(self):
    vocabulary_file = self._createVocabFile(
        "f2i_vocab3.txt", values=("42", "1", "-1000"))
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file,
          num_oov_buckets=1,
          key_dtype=dtypes.int64)
      ids = table.lookup(
          constant_op.constant((1, -1000, 11), dtype=dtypes.int64))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, 3), ids.eval())

  def test_index_table_from_file_with_default_value(self):
    default_value = -42
    vocabulary_file = self._createVocabFile("f2i_vocab4.txt")
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file, default_value=default_value)
      ids = table.lookup(constant_op.constant(["salad", "surgery", "tarkus"]))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, default_value), ids.eval())

  def test_index_table_from_file_with_oov_buckets(self):
    vocabulary_file = self._createVocabFile("f2i_vocab5.txt")
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file, num_oov_buckets=1000)
      ids = table.lookup(
          constant_op.constant(["salad", "surgery", "tarkus", "toccata"]))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual(
          (
              1,  # From vocabulary file.
              2,  # From vocabulary file.
              867,  # 3 + fingerprint("tarkus") mod 300.
              860),  # 3 + fingerprint("toccata") mod 300.
          ids.eval())

  def test_index_table_from_file_with_only_oov_buckets(self):
    self.assertRaises(
        ValueError, lookup_ops.index_table_from_file, vocabulary_file=None)

  def test_index_table_from_file_with_vocab_size_too_small(self):
    vocabulary_file = self._createVocabFile("f2i_vocab6.txt")
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file, vocab_size=2)
      ids = table.lookup(constant_op.constant(["salad", "surgery", "tarkus"]))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, -1, -1), ids.eval())
      self.assertEqual(2, table.size().eval())

  def test_index_table_from_file_with_vocab_size_too_large(self):
    vocabulary_file = self._createVocabFile("f2i_vocab7.txt")
    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file, vocab_size=4)
      self.assertRaisesRegexp(errors_impl.InvalidArgumentError,
                              "Invalid vocab_size", table.init.run)

  def test_index_table_from_file_with_vocab_size(self):
    vocabulary_file = self._createVocabFile("f2i_vocab8.txt")

    self.assertRaises(
        ValueError,
        lookup_ops.index_table_from_file,
        vocabulary_file=vocabulary_file,
        vocab_size=0)

    with self.test_session():
      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file, vocab_size=3)
      ids = table.lookup(constant_op.constant(["salad", "surgery", "tarkus"]))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, -1), ids.eval())
      self.assertEqual(3, table.size().eval())

  def test_index_table_from_file_with_invalid_hashers(self):
    vocabulary_file = self._createVocabFile("invalid_hasher.txt")
    with self.test_session():
      with self.assertRaises(TypeError):
        lookup_ops.index_table_from_file(
            vocabulary_file=vocabulary_file,
            vocab_size=3,
            num_oov_buckets=1,
            hasher_spec=1)

      table = lookup_ops.index_table_from_file(
          vocabulary_file=vocabulary_file,
          vocab_size=3,
          num_oov_buckets=1,
          hasher_spec=lookup_ops.HasherSpec("my-awesome-hash", None))

      self.assertRaises(ValueError, table.lookup,
                        constant_op.constant(["salad", "surgery", "tarkus"]))


class KeyValueTensorInitializerTest(test.TestCase):

  def test_string(self):
    with ops.Graph().as_default(), self.test_session():
      init = lookup_ops.KeyValueTensorInitializer(
          ("brain", "salad", "surgery"), (0, 1, 2), dtypes.string, dtypes.int64)
      table = lookup_ops.HashTable(init, default_value=-1)
      table.init.run()

  def test_int64(self):
    with ops.Graph().as_default(), self.test_session():
      init = lookup_ops.KeyValueTensorInitializer((42, 1, -1000), (0, 1, 2),
                                                  dtypes.int64, dtypes.int64)
      table = lookup_ops.HashTable(init, default_value=-1)
      table.init.run()

  def test_int32(self):
    with ops.Graph().as_default(), self.test_session():
      init = lookup_ops.KeyValueTensorInitializer((42, 1, -1000), (0, 1, 2),
                                                  dtypes.int32, dtypes.int64)
      table = lookup_ops.HashTable(init, default_value=-1)
      with self.assertRaisesRegexp(
          errors_impl.OpError, "No OpKernel was registered"):
        table.init.run()


class IndexTableFromTensor(test.TestCase):

  def test_index_table_from_tensor_with_tensor_init(self):
    with self.test_session():
      table = lookup_ops.index_table_from_tensor(
          vocabulary_list=("brain", "salad", "surgery"), num_oov_buckets=1)
      ids = table.lookup(constant_op.constant(("salad", "surgery", "tarkus")))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, 3), ids.eval())

  def test_int32_index_table_from_tensor_with_tensor_init(self):
    with self.test_session():
      table = lookup_ops.index_table_from_tensor(
          vocabulary_list=(42, 1, -1000), num_oov_buckets=1, dtype=dtypes.int32)
      ids = table.lookup(
          constant_op.constant((1, -1000, 11), dtype=dtypes.int32))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, 3), ids.eval())

  def test_int64_index_table_from_tensor_with_tensor_init(self):
    with self.test_session():
      table = lookup_ops.index_table_from_tensor(
          vocabulary_list=(42, 1, -1000), num_oov_buckets=1, dtype=dtypes.int64)
      ids = table.lookup(
          constant_op.constant((1, -1000, 11), dtype=dtypes.int64))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, 3), ids.eval())

  def test_index_table_from_tensor_with_default_value(self):
    default_value = -42
    with self.test_session():
      table = lookup_ops.index_table_from_tensor(
          vocabulary_list=["brain", "salad", "surgery"],
          default_value=default_value)
      ids = table.lookup(constant_op.constant(["salad", "surgery", "tarkus"]))

      self.assertRaises(errors_impl.OpError, ids.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((1, 2, default_value), ids.eval())

  def test_index_table_from_tensor_missing_vocabulary_list(self):
    with self.test_session():
      with self.assertRaisesRegexp(ValueError,
                                   "vocabulary_list must be specified"):
        lookup_ops.index_table_from_tensor(
            vocabulary_list=None, num_oov_buckets=1)

  def test_index_table_from_tensor_empty_vocabulary_list(self):
    with self.test_session():
      table = lookup_ops.index_table_from_tensor(
          vocabulary_list=np.array([], dtype=np.str_), num_oov_buckets=1)
      ids = table.lookup(constant_op.constant(["salad", "surgery", "brain"]))
      self.assertRaises(errors_impl.OpError, ids.eval)
      with self.assertRaisesRegexp(
          errors_impl.OpError, "keys and values cannot be empty"):
        lookup_ops.tables_initializer().run()

  def test_index_table_from_tensor_with_invalid_hashers(self):
    with self.test_session():
      with self.assertRaises(TypeError):
        lookup_ops.index_table_from_tensor(
            vocabulary_list=["brain", "salad", "surgery"],
            num_oov_buckets=1,
            hasher_spec=1)

      table = lookup_ops.index_table_from_tensor(
          vocabulary_list=["brain", "salad", "surgery"],
          num_oov_buckets=1,
          hasher_spec=lookup_ops.HasherSpec("my-awesome-hash", None))

      self.assertRaises(ValueError, table.lookup,
                        constant_op.constant(["salad", "surgery", "tarkus"]))


class IndexToStringTableFromFileTest(test.TestCase):

  def _createVocabFile(self, basename):
    vocabulary_file = os.path.join(self.get_temp_dir(), basename)
    with open(vocabulary_file, "w") as f:
      f.write("\n".join(["brain", "salad", "surgery"]) + "\n")
    return vocabulary_file

  def test_index_to_string_table(self):
    vocabulary_file = self._createVocabFile("i2f_vocab1.txt")
    with self.test_session():
      table = lookup_ops.index_to_string_table_from_file(
          vocabulary_file=vocabulary_file)
      features = table.lookup(constant_op.constant([0, 1, 2, 3], dtypes.int64))
      self.assertRaises(errors_impl.OpError, features.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((b"brain", b"salad", b"surgery", b"UNK"),
                          features.eval())

  def test_index_to_string_table_with_default_value(self):
    default_value = b"NONE"
    vocabulary_file = self._createVocabFile("f2i_vocab2.txt")
    with self.test_session():
      table = lookup_ops.index_to_string_table_from_file(
          vocabulary_file=vocabulary_file, default_value=default_value)
      features = table.lookup(constant_op.constant([1, 2, 4], dtypes.int64))
      self.assertRaises(errors_impl.OpError, features.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((b"salad", b"surgery", default_value),
                          features.eval())

  def test_index_to_string_table_with_vocab_size_too_small(self):
    default_value = b"NONE"
    vocabulary_file = self._createVocabFile("f2i_vocab2.txt")
    with self.test_session():
      table = lookup_ops.index_to_string_table_from_file(
          vocabulary_file=vocabulary_file,
          vocab_size=2,
          default_value=default_value)
      features = table.lookup(constant_op.constant([1, 2, 4], dtypes.int64))
      self.assertRaises(errors_impl.OpError, features.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((b"salad", default_value, default_value),
                          features.eval())

  def test_index_to_string_table_with_vocab_size_too_large(self):
    vocabulary_file = self._createVocabFile("f2i_vocab6.txt")
    with self.test_session():
      table = lookup_ops.index_to_string_table_from_file(
          vocabulary_file=vocabulary_file, vocab_size=4)
      features = table.lookup(constant_op.constant([1, 2, 4], dtypes.int64))

      self.assertRaises(errors_impl.OpError, features.eval)
      init = lookup_ops.tables_initializer()
      self.assertRaisesRegexp(errors_impl.InvalidArgumentError,
                              "Invalid vocab_size", init.run)

  def test_index_to_string_table_with_vocab_size(self):
    vocabulary_file = self._createVocabFile("f2i_vocab7.txt")
    with self.test_session():
      table = lookup_ops.index_to_string_table_from_file(
          vocabulary_file=vocabulary_file, vocab_size=3)
      features = table.lookup(constant_op.constant([1, 2, 4], dtypes.int64))

      self.assertRaises(errors_impl.OpError, features.eval)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((b"salad", b"surgery", b"UNK"), features.eval())


class IndexToStringTableFromTensorTest(test.TestCase):

  def test_index_to_string_table_from_tensor(self):
    with self.test_session():
      vocabulary_list = constant_op.constant(["brain", "salad", "surgery"])
      table = lookup_ops.index_to_string_table_from_tensor(
          vocabulary_list=vocabulary_list)

      indices = constant_op.constant([0, 1, 2, 3], dtypes.int64)
      features = table.lookup(indices)
      self.assertRaises(errors_impl.OpError, features.eval)
      lookup_ops.tables_initializer().run()

      self.assertAllEqual((b"brain", b"salad", b"surgery", b"UNK"),
                          features.eval())

  def test_duplicate_entries(self):
    with self.test_session():
      vocabulary_list = constant_op.constant(["hello", "hello"])
      table = lookup_ops.index_to_string_table_from_tensor(
          vocabulary_list=vocabulary_list)
      indices = constant_op.constant([0, 1, 4], dtypes.int64)
      features = table.lookup(indices)
      lookup_ops.tables_initializer().run()
      self.assertAllEqual((b"hello", b"hello", b"UNK"), features.eval())

  def test_index_to_string_with_default_value(self):
    default_value = b"NONE"
    with self.test_session():
      vocabulary_list = constant_op.constant(["brain", "salad", "surgery"])
      table = lookup_ops.index_to_string_table_from_tensor(
          vocabulary_list=vocabulary_list, default_value=default_value)
      indices = constant_op.constant([1, 2, 4], dtypes.int64)
      features = table.lookup(indices)
      self.assertRaises(errors_impl.OpError, features.eval)

      lookup_ops.tables_initializer().run()
      self.assertAllEqual((b"salad", b"surgery", default_value),
                          features.eval())


class InitializeTableFromFileOpTest(test.TestCase):

  def _createVocabFile(self, basename, values=("brain", "salad", "surgery")):
    vocabulary_file = os.path.join(self.get_temp_dir(), basename)
    with open(vocabulary_file, "w") as f:
      f.write("\n".join(values) + "\n")
    return vocabulary_file

  def testInitializeStringTable(self):
    vocabulary_file = self._createVocabFile("one_column_1.txt")

    with self.test_session():
      default_value = -1
      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(
              vocabulary_file, dtypes.string,
              lookup_ops.TextFileIndex.WHOLE_LINE, dtypes.int64,
              lookup_ops.TextFileIndex.LINE_NUMBER), default_value)
      table.init.run()

      output = table.lookup(constant_op.constant(["brain", "salad", "tank"]))

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testInitializeInt64Table(self):
    vocabulary_file = self._createVocabFile(
        "one_column_int64.txt", values=("42", "1", "-1000"))

    with self.test_session():
      default_value = -1
      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(
              vocabulary_file, dtypes.int64,
              lookup_ops.TextFileIndex.WHOLE_LINE, dtypes.int64,
              lookup_ops.TextFileIndex.LINE_NUMBER), default_value)
      table.init.run()

      output = table.lookup(
          constant_op.constant((42, 1, 11), dtype=dtypes.int64))

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testInitializeIndexTable(self):
    vocabulary_file = self._createVocabFile("one_column_2.txt")

    with self.test_session():
      default_value = "UNK"
      key_index = lookup_ops.TextFileIndex.LINE_NUMBER
      value_index = lookup_ops.TextFileIndex.WHOLE_LINE
      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.int64,
                                         key_index, dtypes.string, value_index),
          default_value)
      table.init.run()

      input_values = constant_op.constant([0, 1, 2, 3], dtypes.int64)
      output = table.lookup(input_values)

      result = output.eval()
      self.assertAllEqual([b"brain", b"salad", b"surgery", b"UNK"], result)

  def testMultiColumn(self):
    vocabulary_file = os.path.join(self.get_temp_dir(), "three_columns.txt")
    with open(vocabulary_file, "w") as f:
      f.write("\n".join(["0\tbrain\t1", "1\tsalad\t5", "2\tsurgery\t6"]) + "\n")

    with self.test_session():
      default_value = -1
      key_index = 1
      value_index = 2

      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.string,
                                         key_index, dtypes.int64, value_index),
          default_value)
      table.init.run()

      input_string = constant_op.constant(["brain", "salad", "surgery"])
      output = table.lookup(input_string)

      result = output.eval()
      self.assertAllEqual([1, 5, 6], result)

  def testInvalidDataTypeInMultiColumn(self):
    vocabulary_file = os.path.join(self.get_temp_dir(), "three_columns.txt")
    with open(vocabulary_file, "w") as f:
      f.write("\n".join(["0\tbrain\t1", "1\tsalad\t5", "2\tsurgery\t6"]) + "\n")

    with self.test_session():
      default_value = -1
      key_index = 2
      value_index = 1
      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.string,
                                         key_index, dtypes.int64, value_index),
          default_value)
      with self.assertRaisesOpError("is not a valid"):
        table.init.run()

  def testInvalidDataType(self):
    vocabulary_file = self._createVocabFile("one_column_3.txt")

    with self.test_session():
      default_value = "UNK"
      key_index = lookup_ops.TextFileIndex.WHOLE_LINE
      value_index = lookup_ops.TextFileIndex.LINE_NUMBER

      with self.assertRaises(ValueError):
        lookup_ops.HashTable(
            lookup_ops.TextFileInitializer(vocabulary_file, dtypes.int64,
                                           key_index, dtypes.string,
                                           value_index), default_value)

  def testInvalidIndex(self):
    vocabulary_file = self._createVocabFile("one_column_4.txt")
    with self.test_session():
      default_value = -1
      key_index = 1  # second column of the line
      value_index = lookup_ops.TextFileIndex.LINE_NUMBER
      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.string,
                                         key_index, dtypes.int64, value_index),
          default_value)

      with self.assertRaisesOpError("Invalid number of columns"):
        table.init.run()

  def testInitializeSameTableWithMultipleNodes(self):
    vocabulary_file = self._createVocabFile("one_column_5.txt")

    with self.test_session() as sess:
      shared_name = "shared-one-columm"
      default_value = -1
      table1 = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.string,
                                         lookup_ops.TextFileIndex.WHOLE_LINE,
                                         dtypes.int64,
                                         lookup_ops.TextFileIndex.LINE_NUMBER),
          default_value,
          shared_name=shared_name)
      table2 = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.string,
                                         lookup_ops.TextFileIndex.WHOLE_LINE,
                                         dtypes.int64,
                                         lookup_ops.TextFileIndex.LINE_NUMBER),
          default_value,
          shared_name=shared_name)
      table3 = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(vocabulary_file, dtypes.string,
                                         lookup_ops.TextFileIndex.WHOLE_LINE,
                                         dtypes.int64,
                                         lookup_ops.TextFileIndex.LINE_NUMBER),
          default_value,
          shared_name=shared_name)

      lookup_ops.tables_initializer().run()

      input_string = constant_op.constant(["brain", "salad", "tank"])

      output1 = table1.lookup(input_string)
      output2 = table2.lookup(input_string)
      output3 = table3.lookup(input_string)

      out1, out2, out3 = sess.run([output1, output2, output3])
      self.assertAllEqual([0, 1, -1], out1)
      self.assertAllEqual([0, 1, -1], out2)
      self.assertAllEqual([0, 1, -1], out3)

  def testInitializeTableWithNoFilename(self):
    with self.test_session():
      default_value = -1
      with self.assertRaises(ValueError):
        lookup_ops.HashTable(
            lookup_ops.TextFileInitializer(
                "", dtypes.string, lookup_ops.TextFileIndex.WHOLE_LINE,
                dtypes.int64, lookup_ops.TextFileIndex.LINE_NUMBER),
            default_value)

  def testInitializeWithVocabSize(self):
    with self.test_session():
      default_value = -1
      vocab_size = 3
      vocabulary_file1 = self._createVocabFile("one_column6.txt")
      table1 = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(
              vocabulary_file1,
              dtypes.string,
              lookup_ops.TextFileIndex.WHOLE_LINE,
              dtypes.int64,
              lookup_ops.TextFileIndex.LINE_NUMBER,
              vocab_size=vocab_size), default_value)

      # Initialize from file.
      table1.init.run()
      self.assertEquals(vocab_size, table1.size().eval())

      vocabulary_file2 = self._createVocabFile("one_column7.txt")
      vocab_size = 5
      table2 = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(
              vocabulary_file2,
              dtypes.string,
              lookup_ops.TextFileIndex.WHOLE_LINE,
              dtypes.int64,
              lookup_ops.TextFileIndex.LINE_NUMBER,
              vocab_size=vocab_size), default_value)
      with self.assertRaisesOpError("Invalid vocab_size"):
        table2.init.run()

      vocab_size = 1
      vocabulary_file3 = self._createVocabFile("one_column3.txt")
      table3 = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(
              vocabulary_file3,
              dtypes.string,
              lookup_ops.TextFileIndex.WHOLE_LINE,
              dtypes.int64,
              lookup_ops.TextFileIndex.LINE_NUMBER,
              vocab_size=vocab_size), default_value)

      # Smaller vocab size reads only vocab_size records.
      table3.init.run()
      self.assertEquals(vocab_size, table3.size().eval())

  def testFeedVocabularyName(self):
    vocabulary_file = self._createVocabFile("feed_vocabulary.txt")

    with self.test_session():
      default_value = -1
      table = lookup_ops.HashTable(
          lookup_ops.TextFileInitializer(
              "old_file.txt", dtypes.string,
              lookup_ops.TextFileIndex.WHOLE_LINE, dtypes.int64,
              lookup_ops.TextFileIndex.LINE_NUMBER), default_value)

      # Initialize with non existing file (old_file.txt) should fail.
      # TODO(yleon): Update message, which might change per FileSystem.
      with self.assertRaisesOpError("old_file.txt"):
        table.init.run()

      # Initialize the model feeding the vocabulary file.
      filenames = ops.get_collection(ops.GraphKeys.ASSET_FILEPATHS)
      table.init.run(feed_dict={filenames[0]: vocabulary_file})

      input_string = constant_op.constant(["brain", "salad", "tank"])
      output = table.lookup(input_string)

      result = output.eval()
      self.assertAllEqual([0, 1, -1], result)

  def testInvalidFilenames(self):
    vocabulary_file = self._createVocabFile("filename_shape.txt")

    with self.test_session():
      default_value = -1

      # Invalid data type
      other_type = constant_op.constant(1)
      with self.assertRaises(ValueError):
        lookup_ops.HashTable(
            lookup_ops.TextFileInitializer(
                other_type, dtypes.string, lookup_ops.TextFileIndex.WHOLE_LINE,
                dtypes.int64, lookup_ops.TextFileIndex.LINE_NUMBER),
            default_value)

      # Non-scalar filename
      filenames = constant_op.constant([vocabulary_file, vocabulary_file])
      with self.assertRaises(ValueError):
        lookup_ops.HashTable(
            lookup_ops.TextFileInitializer(
                filenames, dtypes.string, lookup_ops.TextFileIndex.WHOLE_LINE,
                dtypes.int64, lookup_ops.TextFileIndex.LINE_NUMBER),
            default_value)

  def testIdToStringTable(self):
    vocab_file = self._createVocabFile("feat_to_id_1.txt")
    with self.test_session():
      default_value = "UNK"
      vocab_size = 3
      table = lookup_ops.HashTable(
          lookup_ops.TextFileStringTableInitializer(
              vocab_file, vocab_size=vocab_size), default_value)

      table.init.run()

      input_values = constant_op.constant([0, 1, 2, 3], dtypes.int64)

      out = table.lookup(input_values)
      self.assertAllEqual([b"brain", b"salad", b"surgery", b"UNK"], out.eval())
      self.assertEquals(vocab_size, table.size().eval())

  def testStringToIdTable(self):
    vocab_file = self._createVocabFile("feat_to_id_2.txt")
    with self.test_session():
      default_value = -1
      vocab_size = 3
      table = lookup_ops.HashTable(
          lookup_ops.TextFileIdTableInitializer(
              vocab_file, vocab_size=vocab_size), default_value)
      table.init.run()

      input_string = constant_op.constant(["brain", "salad", "surgery", "UNK"])

      out = table.lookup(input_string)
      self.assertAllEqual([0, 1, 2, -1], out.eval())
      self.assertEquals(vocab_size, table.size().eval())

  def testInt64ToIdTable(self):
    vocab_file = self._createVocabFile(
        "feat_to_id_3.txt", values=("42", "1", "-1000"))
    with self.test_session():
      default_value = -1
      vocab_size = 3
      table = lookup_ops.HashTable(
          lookup_ops.TextFileIdTableInitializer(
              vocab_file, vocab_size=vocab_size, key_dtype=dtypes.int64),
          default_value)
      table.init.run()

      out = table.lookup(
          constant_op.constant((42, 1, -1000, 11), dtype=dtypes.int64))
      self.assertAllEqual((0, 1, 2, -1), out.eval())
      self.assertEquals(vocab_size, table.size().eval())


class IdTableWithHashBucketsTest(test.TestCase):

  def _createVocabFile(self, basename, values=("brain", "salad", "surgery")):
    vocabulary_file = os.path.join(self.get_temp_dir(), basename)
    with open(vocabulary_file, "w") as f:
      f.write("\n".join(values) + "\n")
    return vocabulary_file

  def testStringIdTableWithHashBuckets(self):
    vocab_file = self._createVocabFile("feat_to_id_1.txt")
    with self.test_session():
      default_value = -1
      vocab_size = 3
      oov_buckets = 1
      table = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size), default_value),
          oov_buckets)

      table.init.run()

      input_string = constant_op.constant(["brain", "salad", "surgery", "UNK"])

      out = table.lookup(input_string)
      self.assertAllEqual([0, 1, 2, 3], out.eval())
      self.assertEquals(vocab_size + oov_buckets, table.size().eval())

  def testInt32IdTableWithHashBuckets(self):
    vocab_file = self._createVocabFile("feat_to_id_2.txt", ("42", "1", "-1000"))
    with self.test_session():
      default_value = -1
      vocab_size = 3
      oov_buckets = 1
      table = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size, key_dtype=dtypes.int64),
              default_value),
          oov_buckets,
          key_dtype=dtypes.int32)

      table.init.run()

      values = constant_op.constant((42, 1, -1000, 11), dtype=dtypes.int32)

      out = table.lookup(values)
      self.assertAllEqual([0, 1, 2, 3], out.eval())
      self.assertEquals(vocab_size + oov_buckets, table.size().eval())

  def testInt64IdTableWithHashBuckets(self):
    vocab_file = self._createVocabFile("feat_to_id_3.txt", ("42", "1", "-1000"))
    with self.test_session():
      default_value = -1
      vocab_size = 3
      oov_buckets = 1
      table = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size, key_dtype=dtypes.int64),
              default_value), oov_buckets)

      table.init.run()

      values = constant_op.constant((42, 1, -1000, 11), dtype=dtypes.int64)

      out = table.lookup(values)
      self.assertAllEqual([0, 1, 2, 3], out.eval())
      self.assertEquals(vocab_size + oov_buckets, table.size().eval())

  def testStringIdTableWithOnlyHashBucket(self):
    with self.test_session():
      oov_buckets = 5

      # Set a table that only uses hash buckets, for each input value returns
      # an id calculated by fingerprint("input") mod oov_buckets.
      table = lookup_ops.IdTableWithHashBuckets(None, oov_buckets)
      table.init.run()

      values = constant_op.constant(("brain", "salad", "surgery"))

      out = table.lookup(values)
      self.assertAllEqual(
          [
              3,  # fingerprint("brain") mod 5.
              1,  # fingerprint("salad") mod 5.
              4  # fingerprint("surgery") mod 5
          ],
          out.eval())
      self.assertEquals(oov_buckets, table.size().eval())

  def testInt32IdTableWithOnlyHashBucket(self):
    with self.test_session():
      oov_buckets = 5

      # Set a table that only uses hash buckets, for each input value returns
      # an id calculated by fingerprint("input") mod oov_buckets.
      table = lookup_ops.IdTableWithHashBuckets(
          None, oov_buckets, key_dtype=dtypes.int32)
      table.init.run()

      input_string = constant_op.constant([42, 1, -1000], dtype=dtypes.int32)

      out = table.lookup(input_string)
      self.assertAllEqual(
          [
              1,  # fingerprint("42") mod 5.
              4,  # fingerprint("1") mod 5.
              2  # fingerprint("-1000") mod 5
          ],
          out.eval())
      self.assertEquals(oov_buckets, table.size().eval())

  def testFloat64IdTableWithOnlyHashBucket(self):
    with self.test_session():
      with self.assertRaisesRegexp(TypeError, "Invalid key_dtype"):
        lookup_ops.IdTableWithHashBuckets(
            None, num_oov_buckets=5, key_dtype=dtypes.float64)

  def testBoolIdTableWithOnlyHashBucket(self):
    with self.test_session():
      with self.assertRaisesRegexp(TypeError, "Invalid key_dtype"):
        lookup_ops.IdTableWithHashBuckets(
            None, num_oov_buckets=5, key_dtype=dtypes.bool)

  def testIdTableWithHashBucketsWithMultipleInitializers(self):
    vocab_file = self._createVocabFile("feat_to_id_4.txt")
    with self.test_session() as sess:
      default_value = -1
      vocab_size = 3
      oov_buckets = 3

      vocab_table = lookup_ops.HashTable(
          lookup_ops.TextFileIdTableInitializer(
              vocab_file, vocab_size=vocab_size), default_value)
      table1 = lookup_ops.IdTableWithHashBuckets(
          vocab_table,
          oov_buckets,
          hasher_spec=lookup_ops.FastHashSpec,
          name="table1")

      table2 = lookup_ops.IdTableWithHashBuckets(
          vocab_table,
          oov_buckets,
          hasher_spec=lookup_ops.StrongHashSpec((1, 2)),
          name="table2")

      lookup_ops.tables_initializer().run()

      input_string = constant_op.constant(
          ["fruit", "brain", "salad", "surgery", "UNK"])

      out1 = table1.lookup(input_string)
      out2 = table2.lookup(input_string)

      out1, out2 = sess.run([out1, out2])
      self.assertAllEqual([5, 0, 1, 2, 5], out1)
      self.assertAllEqual([5, 0, 1, 2, 3], out2)
      self.assertEquals(vocab_size + oov_buckets, table1.size().eval())
      self.assertEquals(vocab_size + oov_buckets, table2.size().eval())
      test_util.assert_ops_in_graph({
          "table1_Lookup/hash_bucket": "StringToHashBucketFast",
          "table2_Lookup/hash_bucket": "StringToHashBucketStrong",
      }, sess.graph)

  def testIdTableWithHashBucketsInitializationAcrossSessions(self):
    vocab_file = self._createVocabFile("feat_to_id_5.txt")
    shared_name = "across-sessions"
    with self.test_session():
      default_value = -1
      vocab_size = 3
      oov_buckets = 1
      table1 = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size),
              default_value,
              shared_name=shared_name), oov_buckets)

      table1.init.run()

      input_string_1 = constant_op.constant(
          ["brain", "salad", "surgery", "UNK"])

      out1 = table1.lookup(input_string_1)

      self.assertAllEqual([0, 1, 2, 3], out1.eval())
      self.assertEquals(vocab_size + oov_buckets, table1.size().eval())

    with self.test_session():
      default_value = -1
      vocab_size = 3
      oov_buckets = 1

      # Underlying lookup table already initialized in previous session.
      # No need to call table2.init.run()
      table2 = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size),
              default_value,
              shared_name=shared_name), oov_buckets)

      input_string_2 = constant_op.constant(["fruit", "salad", "UNK"])

      out2 = table2.lookup(input_string_2)

      self.assertAllEqual([3, 1, 3], out2.eval())
      self.assertEquals(vocab_size + oov_buckets, table2.size().eval())

  def testIdTableWithHashBucketsWithMultipleInitializersDifferentDefault(self):
    vocab_file = self._createVocabFile("feat_to_id_6.txt")
    with self.test_session() as sess:
      default_value1 = -1
      vocab_size = 3
      oov_buckets = 0
      table1 = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size), default_value1),
          oov_buckets)

      default_value2 = -2
      table2 = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(
                  vocab_file, vocab_size=vocab_size), default_value2),
          oov_buckets)

      lookup_ops.tables_initializer().run()

      input_string_1 = constant_op.constant(
          ["brain", "salad", "surgery", "UNK"])
      input_string_2 = constant_op.constant(["fruit", "salad", "UNK"])

      out1 = table1.lookup(input_string_1)
      out2 = table2.lookup(input_string_2)

      out1, out2 = sess.run([out1, out2])
      self.assertAllEqual([0, 1, 2, -1], out1)
      self.assertAllEqual([-2, 1, -2], out2)
      self.assertEquals(vocab_size + oov_buckets, table1.size().eval())
      self.assertEquals(vocab_size + oov_buckets, table2.size().eval())

  def testSparseTensor(self):
    vocab_file = self._createVocabFile("feat_to_id_7.txt")
    input_indices = [[0, 0], [0, 1], [2, 0], [2, 2], [3, 0]]
    input_shape = [4, 4]
    with self.test_session() as sess:
      sp_features = sparse_tensor.SparseTensor(
          constant_op.constant(input_indices, dtypes.int64),
          constant_op.constant(["brain", "salad", "brain", "surgery", "tarkus"],
                               dtypes.string),
          constant_op.constant(input_shape, dtypes.int64))

      table = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.TextFileIdTableInitializer(vocab_file, vocab_size=3),
              -1), 1)
      table.init.run()

      sp_ids = table.lookup(sp_features)

      self.assertAllEqual([5], sp_ids.values._shape_as_list())

      sp_ids_ind, sp_ids_val, sp_ids_shape = sess.run(
          [sp_ids.indices, sp_ids.values, sp_ids.dense_shape])

      self.assertAllEqual(input_indices, sp_ids_ind)
      self.assertAllEqual([0, 1, 0, 2, 3], sp_ids_val)
      self.assertAllEqual(input_shape, sp_ids_shape)

  def testInt32SparseTensor(self):
    input_indices = [[0, 0], [0, 1], [2, 0], [2, 2], [3, 0]]
    input_shape = [4, 4]
    with self.test_session() as sess:
      sp_features = sparse_tensor.SparseTensor(
          constant_op.constant(input_indices, dtypes.int64),
          constant_op.constant([42, 1, 42, -1000, 11], dtypes.int32),
          constant_op.constant(input_shape, dtypes.int64))

      table = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.KeyValueTensorInitializer(
                  (42, 1, -1000), (0, 1, 2), dtypes.int64, dtypes.int64), -1),
          1,
          key_dtype=dtypes.int32)
      table.init.run()

      sp_ids = table.lookup(sp_features)

      self.assertAllEqual([5], sp_ids.values._shape_as_list())

      sp_ids_ind, sp_ids_val, sp_ids_shape = sess.run(
          [sp_ids.indices, sp_ids.values, sp_ids.dense_shape])

      self.assertAllEqual(input_indices, sp_ids_ind)
      self.assertAllEqual([0, 1, 0, 2, 3], sp_ids_val)
      self.assertAllEqual(input_shape, sp_ids_shape)

  def testInt64SparseTensor(self):
    input_indices = [[0, 0], [0, 1], [2, 0], [2, 2], [3, 0]]
    input_shape = [4, 4]
    with self.test_session() as sess:
      sp_features = sparse_tensor.SparseTensor(
          constant_op.constant(input_indices, dtypes.int64),
          constant_op.constant([42, 1, 42, -1000, 11], dtypes.int64),
          constant_op.constant(input_shape, dtypes.int64))

      table = lookup_ops.IdTableWithHashBuckets(
          lookup_ops.HashTable(
              lookup_ops.KeyValueTensorInitializer(
                  (42, 1, -1000), (0, 1, 2), dtypes.int64, dtypes.int64), -1),
          1,
          key_dtype=dtypes.int64)
      table.init.run()

      sp_ids = table.lookup(sp_features)

      self.assertAllEqual([5], sp_ids.values._shape_as_list())

      sp_ids_ind, sp_ids_val, sp_ids_shape = sess.run(
          [sp_ids.indices, sp_ids.values, sp_ids.dense_shape])

      self.assertAllEqual(input_indices, sp_ids_ind)
      self.assertAllEqual([0, 1, 0, 2, 3], sp_ids_val)
      self.assertAllEqual(input_shape, sp_ids_shape)

  def testIdTableWithHashBucketsWithInvalidHashers(self):
    vocab_file = self._createVocabFile("feat_to_id_4.txt")
    with self.test_session():
      default_value = -1
      vocab_size = 3
      oov_buckets = 1
      lookup_table = lookup_ops.HashTable(
          lookup_ops.TextFileIdTableInitializer(
              vocab_file, vocab_size=vocab_size), default_value)

      with self.assertRaises(TypeError):
        lookup_ops.IdTableWithHashBuckets(
            lookup_table, oov_buckets, hasher_spec=1)

      table = lookup_ops.IdTableWithHashBuckets(
          lookup_table,
          oov_buckets,
          hasher_spec=lookup_ops.HasherSpec("my-awesome-hash", None))

      input_string = constant_op.constant(["brain", "salad", "surgery", "UNK"])

      with self.assertRaises(ValueError):
        table.lookup(input_string)

      with self.assertRaises(ValueError):
        table = lookup_ops.IdTableWithHashBuckets(
            lookup_table,
            oov_buckets,
            hasher_spec=lookup_ops.StrongHashSpec([]))

      with self.assertRaises(ValueError):
        table = lookup_ops.IdTableWithHashBuckets(
            lookup_table,
            oov_buckets,
            hasher_spec=lookup_ops.StrongHashSpec([1, 2, 3]))

      with self.assertRaises(TypeError):
        table = lookup_ops.IdTableWithHashBuckets(
            lookup_table,
            oov_buckets,
            hasher_spec=lookup_ops.StrongHashSpec([None, 2]))


if __name__ == "__main__":
  test.main()
