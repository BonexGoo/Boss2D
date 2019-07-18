#!/usr/bin/env python

'''
The sample demonstrates how to train Random Trees classifier
(or Boosting classifier, or MLP, or Knearest, or Support Vector Machines) using the provided dataset.

We use the sample database letter-recognition.data
from UCI Repository, here is the link:

Newman, D.J. & Hettich, S. & Blake, C.L. & Merz, C.J. (1998).
UCI Repository of machine learning databases
[http://www.ics.uci.edu/~mlearn/MLRepository.html].
Irvine, CA: University of California, Department of Information and Computer Science.

The dataset consists of 20000 feature vectors along with the
responses - capital latin letters A..Z.
The first 10000 samples are used for training
and the remaining 10000 - to test the classifier.
======================================================
USAGE:
  letter_recog.py [--model <model>]
                  [--data <data fn>]
                  [--load <model fn>] [--save <model fn>]

  Models: RTrees, KNearest, Boost, SVM, MLP
'''

# Python 2/3 compatibility
from __future__ import print_function

import numpy as np
import cv2

def load_base(fn):
    a = np.loadtxt(fn, np.float32, delimiter=',', converters={ 0 : lambda ch : ord(ch)-ord('A') })
    samples, responses = a[:,1:], a[:,0]
    return samples, responses

class LetterStatModel(object):
    class_n = 26
    train_ratio = 0.5

    def load(self, fn):
        self.model.load(fn)
    def save(self, fn):
        self.model.save(fn)

    def unroll_samples(self, samples):
        sample_n, var_n = samples.shape
        new_samples = np.zeros((sample_n * self.class_n, var_n+1), np.float32)
        new_samples[:,:-1] = np.repeat(samples, self.class_n, axis=0)
        new_samples[:,-1] = np.tile(np.arange(self.class_n), sample_n)
        return new_samples

    def unroll_responses(self, responses):
        sample_n = len(responses)
        new_responses = np.zeros(sample_n*self.class_n, np.int32)
        resp_idx = np.int32( responses + np.arange(sample_n)*self.class_n )
        new_responses[resp_idx] = 1
        return new_responses

class RTrees(LetterStatModel):
    def __init__(self):
        self.model = cv2.ml.RTrees_create()

    def train(self, samples, responses):
        sample_n, var_n = samples.shape
        var_types = np.array([cv2.ml.VAR_NUMERICAL] * var_n + [cv2.ml.VAR_CATEGORICAL], np.uint8)
        #CvRTParams(10,10,0,false,15,0,true,4,100,0.01f,CV_TERMCRIT_ITER));
        params = dict(max_depth=10 )
        self.model.train(samples, cv2.ml.ROW_SAMPLE, responses, varType = var_types, params = params)

    def predict(self, samples):
        return [self.model.predict(s) for s in samples]


class KNearest(LetterStatModel):
    def __init__(self):
        self.model = cv2.ml.KNearest_create()

    def train(self, samples, responses):
        self.model.train(samples, responses)

    def predict(self, samples):
        retval, results, neigh_resp, dists = self.model.find_nearest(samples, k = 10)
        return results.ravel()


class Boost(LetterStatModel):
    def __init__(self):
        self.model = cv2.ml.Boost_create()

    def train(self, samples, responses):
        sample_n, var_n = samples.shape
        new_samples = self.unroll_samples(samples)
        new_responses = self.unroll_responses(responses)
        var_types = np.array([cv2.ml.VAR_NUMERICAL] * var_n + [cv2.ml.VAR_CATEGORICAL, cv2.ml.VAR_CATEGORICAL], np.uint8)
        #CvBoostParams(CvBoost::REAL, 100, 0.95, 5, false, 0 )
        params = dict(max_depth=5) #, use_surrogates=False)
        self.model.train(new_samples, cv2.ml.ROW_SAMPLE, new_responses, varType = var_types, params=params)

    def predict(self, samples):
        new_samples = self.unroll_samples(samples)
        pred = np.array( [self.model.predict(s, returnSum = True) for s in new_samples] )
        pred = pred.reshape(-1, self.class_n).argmax(1)
        return pred


class SVM(LetterStatModel):
    def __init__(self):
        self.model = cv2.ml.SVM_create()

    def train(self, samples, responses):
        params = dict( kernel_type = cv2.ml.SVM_LINEAR,
                       svm_type = cv2.ml.SVM_C_SVC,
                       C = 1 )
        self.model.train(samples, responses, params = params)

    def predict(self, samples):
        return self.model.predict_all(samples).ravel()


class MLP(LetterStatModel):
    def __init__(self):
        self.model = cv2.ml.ANN_MLP_create()

    def train(self, samples, responses):
        sample_n, var_n = samples.shape
        new_responses = self.unroll_responses(responses).reshape(-1, self.class_n)

        layer_sizes = np.int32([var_n, 100, 100, self.class_n])
        self.model.create(layer_sizes)

        # CvANN_MLP_TrainParams::BACKPROP,0.001
        params = dict( term_crit = (cv2.TERM_CRITERIA_COUNT, 300, 0.01),
                       train_method = cv2.ml.ANN_MLP_TRAIN_PARAMS_BACKPROP,
                       bp_dw_scale = 0.001,
                       bp_moment_scale = 0.0 )
        self.model.train(samples, np.float32(new_responses), None, params = params)

    def predict(self, samples):
        ret, resp = self.model.predict(samples)
        return resp.argmax(-1)


if __name__ == '__main__':
    import getopt
    import sys

    print(__doc__)

    models = [RTrees, KNearest, Boost, SVM, MLP] # NBayes
    models = dict( [(cls.__name__.lower(), cls) for cls in models] )


    args, dummy = getopt.getopt(sys.argv[1:], '', ['model=', 'data=', 'load=', 'save='])
    args = dict(args)
    args.setdefault('--model', 'rtrees')
    args.setdefault('--data', '../data/letter-recognition.data')

    print('loading data %s ...' % args['--data'])
    samples, responses = load_base(args['--data'])
    Model = models[args['--model']]
    model = Model()

    train_n = int(len(samples)*model.train_ratio)
    if '--load' in args:
        fn = args['--load']
        print('loading model from %s ...' % fn)
        model.load(fn)
    else:
        print('training %s ...' % Model.__name__)
        model.train(samples[:train_n], responses[:train_n])

    print('testing...')
    train_rate = np.mean(model.predict(samples[:train_n]) == responses[:train_n])
    test_rate  = np.mean(model.predict(samples[train_n:]) == responses[train_n:])

    print('train rate: %f  test rate: %f' % (train_rate*100, test_rate*100))

    if '--save' in args:
        fn = args['--save']
        print('saving model to %s ...' % fn)
        model.save(fn)
    cv2.destroyAllWindows()
