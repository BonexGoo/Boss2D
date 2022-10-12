#!/bin/bash

set -xe

arch=$1

source $(dirname "$0")/tc-tests-utils.sh

mkdir -p ${TASKCLUSTER_ARTIFACTS} || true

cp ${DS_ROOT_TASK}/DeepSpeech/ds/tensorflow/bazel*.log ${TASKCLUSTER_ARTIFACTS}/

case $arch in
"--x86_64")
  release_folder="Release-iphonesimulator"
  artifact_name="deepspeech_ios.framework.x86_64.tar.xz"
  ;;
"--arm64")
  release_folder="Release-iphoneos"
  artifact_name="deepspeech_ios.framework.arm64.tar.xz"
;;
esac

${TAR} -cf - \
       -C ${DS_ROOT_TASK}/DeepSpeech/ds/native_client/swift/DerivedData/Build/Products/${release_folder}/ deepspeech_ios.framework \
       | ${XZ} > "${TASKCLUSTER_ARTIFACTS}/${artifact_name}"