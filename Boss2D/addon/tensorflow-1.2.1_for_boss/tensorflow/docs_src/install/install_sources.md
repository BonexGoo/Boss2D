# Installing TensorFlow from Sources

This guide explains how to build TensorFlow sources into a TensorFlow
binary and how to install that TensorFlow binary.  Note that we provide
well-tested, pre-built TensorFlow binaries for Linux, Mac, and Windows
systems. In addition, there are pre-built TensorFlow
[docker images](https://hub.docker.com/r/tensorflow/tensorflow/).
So, don't build a TensorFlow binary yourself unless you are very
comfortable building complex packages from source and dealing with
the inevitable aftermath should things not go exactly as documented.

If the last paragraph didn't scare you off, welcome.  This guide explains
how to build TensorFlow on the following operating systems:

*   Ubuntu
*   Mac OS X

We don't officially support building TensorFlow on Windows; however, you may try
to build TensorFlow on Windows if you don't mind using the highly experimental
[Bazel on Windows](https://bazel.build/versions/master/docs/windows.html)
or
[TensorFlow CMake build](https://github.com/tensorflow/tensorflow/tree/r0.12/tensorflow/contrib/cmake).


## Determine which TensorFlow to install

You must choose one of the following types of TensorFlow to build and
install:

* **TensorFlow with CPU support only**. If your system does not have a
  NVIDIA® GPU, build and install this version. Note that this version of
  TensorFlow is typically easier to build and install, so even if you
  have an NVIDIA GPU, we recommend building and installing this version
  first.
* **TensorFlow with GPU support**. TensorFlow programs typically run
  significantly faster on a GPU than on a CPU. Therefore, if your system
  has a NVIDIA GPU and you need to run performance-critical applications,
  you should ultimately build and install this version.
  Beyond the NVIDIA GPU itself, your system must also fulfill the NVIDIA
  software requirements described in one of the following documents:

  * @{$install_linux#NVIDIARequirements$Installing TensorFlow on Ubuntu}
  * @{$install_mac#NVIDIARequirements$Installing TensorFlow on Mac OS}


## Clone the TensorFlow repository

Start the process of building TensorFlow by cloning a TensorFlow
repository.

To clone **the latest** TensorFlow repository, issue the following command:

<pre>$ <b>git clone https://github.com/tensorflow/tensorflow</b> </pre>

The preceding <code>git clone</code> command creates a subdirectory
named `tensorflow`.  After cloning, you may optionally build a
**specific branch** (such as a release branch) by invoking the
following commands:

<pre>
$ <b>cd tensorflow</b>
$ <b>git checkout</b> <i>Branch</i> # where <i>Branch</i> is the desired branch
</pre>

For example, to work with the `r1.0` release instead of the master release,
issue the following command:

<pre>$ <b>git checkout r1.0</b></pre>

Next, you must prepare your environment for
[Linux](#PrepareLinux)
or
[Mac OS](#PrepareMac)


<a name="#PrepareLinux"></a>
## Prepare environment for Linux

Before building TensorFlow on Linux, install the following build
tools on your system:

  * bazel
  * TensorFlow Python dependencies
  * optionally, NVIDIA packages to support TensorFlow for GPU.


### Install Bazel

If bazel is not installed on your system, install it now by following
[these directions](https://bazel.build/versions/master/docs/install.html).


### Install TensorFlow Python dependencies

To install TensorFlow, you must install the following packages:

  * `numpy`, which is a numerical processing package that TensorFlow requires.
  * `dev`, which enables adding extensions to Python.
  * `pip`, which enables you to install and manage certain Python packages.
  * `wheel`, which enables you to manage Python compressed packages in
    the wheel (.whl) format.

To install these packages for Python 2.7, issue the following command:

<pre>
$ <b>sudo apt-get install python-numpy python-dev python-pip python-wheel</b>
</pre>

To install these packages for Python 3.n, issue the following command:

<pre>
$ <b>sudo apt-get install python3-numpy python3-dev python3-pip python3-wheel</b>
</pre>


### Optional: install TensorFlow for GPU prerequisites

If you are building TensorFlow without GPU support, skip this section.

The following NVIDIA <i>hardware</i> must be installed on your system:

  * GPU card with CUDA Compute Capability 3.0 or higher.  See
    [NVIDIA documentation](https://developer.nvidia.com/cuda-gpus)
    for a list of supported GPU cards.

The following NVIDIA <i>software</i> must be installed on your system:

  * NVIDIA's Cuda Toolkit (>= 7.0). We recommend version 8.0.
    For details, see
    [NVIDIA's documentation](http://docs.nvidia.com/cuda/cuda-installation-guide-linux/#axzz4VZnqTJ2A).
    Ensure that you append the relevant Cuda pathnames to the
    `LD_LIBRARY_PATH` environment variable as described in the
    NVIDIA documentation.
  * The NVIDIA drivers associated with NVIDIA's Cuda Toolkit.
  * cuDNN (>= v3). We recommend version 5.1. For details, see
    [NVIDIA's documentation](https://developer.nvidia.com/cudnn),
    particularly the description of appending the appropriate pathname
    to your `LD_LIBRARY_PATH` environment variable.

Finally, you must also install `libcupti-dev` by invoking the following
command:

<pre> $ <b>sudo apt-get install libcupti-dev</b> </pre>


### Next

After preparing the environment, you must now
[configure the installation](#ConfigureInstallation).


<a name="PrepareMac"></a>
## Prepare environment for Mac OS

Before building TensorFlow, you must install the following on your system:

  * bazel
  * TensorFlow Python dependencies.
  * optionally, NVIDIA packages to support TensorFlow for GPU.


### Install bazel

If bazel is not installed on your system, install it now by following
[these directions](https://bazel.build/versions/master/docs/install.html#mac-os-x).


### Install python dependencies

To install TensorFlow, you must install the following packages:

  * six
  * numpy, which is a numerical processing package that TensorFlow requires.
  * wheel, which enables you to manage Python compressed packages
    in the wheel (.whl) format.

You may install the python dependencies using pip. If you don't have pip
on your machine, we recommend using homebrew to install Python and pip as
[documented here](http://docs.python-guide.org/en/latest/starting/install/osx/).
If you follow these instructions, you will not need to disable SIP.

After installing pip, invoke the following commands:

<pre> $ <b>sudo pip install six numpy wheel</b> </pre>



### Optional: install TensorFlow for GPU prerequisites

If you do not have brew installed, install it by following
[these instructions](http://brew.sh/).

After installing brew, install GNU coreutils by issuing the following command:

<pre>$ <b>brew install coreutils</b></pre>

If you want to compile tensorflow and have XCode 7.3 and CUDA 7.5 installed,
note that Xcode 7.3 is not yet compatible with CUDA 7.5.  To remedy this
problem, do either of the following:

  * Upgrade to CUDA 8.0.
  * Download Xcode 7.2 and select it as your default by issuing the following
    command:

    <pre> $ <b>sudo xcode-select -s /Application/Xcode-7.2/Xcode.app</b></pre>

**NOTE:** Your system must fulfill the NVIDIA software requirements described
in one of the following documents:

  * @{$install_linux#NVIDIARequirements$Installing TensorFlow on Linux}
  * @{$install_mac#NVIDIARequirements$Installing TensorFlow on Mac OS}


<a name="ConfigureInstallation"></a>
## Configure the installation

The root of the source tree contains a bash script named
<code>configure</code>. This script asks you to identify the pathname of all
relevant TensorFlow dependencies and specify other build configuration options
such as compiler flags. You must run this script *prior* to
creating the pip package and installing TensorFlow.

If you wish to build TensorFlow with GPU, `configure` will ask
you to specify the version numbers of Cuda and cuDNN. If several
versions of Cuda or cuDNN are installed on your system, explicitly select
the desired version instead of relying on the system default.

Here is an example execution of the `configure` script.  Note that your
own input will likely differ from our sample input:


<pre>
$ <b>cd tensorflow</b>  # cd to the top-level directory created
$ <b>./configure</b>
Please specify the location of python. [Default is /usr/bin/python]: <b>/usr/bin/python2.7</b>
Please specify optimization flags to use during compilation when bazel option "--config=opt" is specified [Default is -march=native]:
Do you wish to use jemalloc as the malloc implementation? [Y/n]
jemalloc enabled
Do you wish to build TensorFlow with Google Cloud Platform support? [y/N]
No Google Cloud Platform support will be enabled for TensorFlow
Do you wish to build TensorFlow with Hadoop File System support? [y/N]
No Hadoop File System support will be enabled for TensorFlow
Do you wish to build TensorFlow with the XLA just-in-time compiler (experimental)? [y/N]
No XLA JIT support will be enabled for TensorFlow
Found possible Python library paths:
  /usr/local/lib/python2.7/dist-packages
  /usr/lib/python2.7/dist-packages
Please input the desired Python library path to use.  Default is [/usr/local/lib/python2.7/dist-packages]
Using python library path: /usr/local/lib/python2.7/dist-packages
Do you wish to build TensorFlow with OpenCL support? [y/N] N
No OpenCL support will be enabled for TensorFlow
Do you wish to build TensorFlow with CUDA support? [y/N] Y
CUDA support will be enabled for TensorFlow
Please specify which gcc should be used by nvcc as the host compiler. [Default is /usr/bin/gcc]:
Please specify the Cuda SDK version you want to use, e.g. 7.0. [Leave empty to use system default]: <b>8.0</b>
Please specify the location where CUDA 8.0 toolkit is installed. Refer to README.md for more details. [Default is /usr/local/cuda]:
Please specify the cuDNN version you want to use. [Leave empty to use system default]: <b>5</b>
Please specify the location where cuDNN 5 library is installed. Refer to README.md for more details. [Default is /usr/local/cuda]:
Please specify a list of comma-separated Cuda compute capabilities you want to build with.
You can find the compute capability of your device at: https://developer.nvidia.com/cuda-gpus.
Please note that each additional compute capability significantly increases your build time and binary size.
[Default is: "3.5,5.2"]: <b>3.0</b>
Setting up Cuda include
Setting up Cuda lib
Setting up Cuda bin
Setting up Cuda nvvm
Setting up CUPTI include
Setting up CUPTI lib64
Configuration finished
</pre>

If you told `configure` to build for GPU support, then `configure`
will create a canonical set of symbolic links to the Cuda libraries
on your system.  Therefore, every time you change the Cuda library paths,
you must rerun the `configure` script before re-invoking
the <code>bazel build</code> command.

Note the following:

  * Although it is possible to build both Cuda and non-Cuda configs
    under the same source tree, we recommend running `bazel clean` when
    switching between these two configurations in the same source tree.
  * If you don't run the `configure` script *before* running the
    `bazel build` command, the `bazel build` command will fail.


## Build the pip package

To build a pip package for TensorFlow with CPU-only support,
invoke the following command:

<pre>
$ <b>bazel build --config=opt //tensorflow/tools/pip_package:build_pip_package</b>
</pre>

To build a pip package for TensorFlow with GPU support,
invoke the following command:

<pre>$ <b>bazel build --config=opt --config=cuda //tensorflow/tools/pip_package:build_pip_package</b> </pre>

**NOTE on gcc 5 or later:** the binary pip packages available on the TensorFlow website are built with gcc 4, which uses the older ABI. To make your build compatible with the older ABI, you need to add `--cxxopt="-D_GLIBCXX_USE_CXX11_ABI=0"` to your `bazel build` command. ABI compatibility allows custom ops built against the TensorFlow pip package to continue to work against your built package.

<b>Tip:</b> By default, building TensorFlow from sources consumes
a lot of RAM.  If RAM is an issue on your system, you may limit RAM usage
by specifying <code>--local_resources 2048,.5,1.0</code> while
invoking `bazel`.

The <code>bazel build</code> command builds a script named
`build_pip_package`.  Running this script as follows will build
a `.whl` file within the `/tmp/tensorflow_pkg` directory:

<pre>
$ <b>bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg</b>
</pre>


## Install the pip package

Invoke `pip install` to install that pip package.
The filename of the `.whl` file depends on your platform.
For example, the following command will install the pip package

for TensorFlow 1.2.1 on Linux:

<pre>
$ <b>sudo pip install /tmp/tensorflow_pkg/tensorflow-1.2.1-py2-none-any.whl</b>
</pre>

## Validate your installation

Validate your TensorFlow installation by doing the following:

Start a terminal.

Change directory (`cd`) to any directory on your system other than the
`tensorflow` subdirectory from which you invoked the `configure` command.

Invoke python:

<pre>$ <b>python</b></pre>

Enter the following short program inside the python interactive shell:

```python
# Python
import tensorflow as tf
hello = tf.constant('Hello, TensorFlow!')
sess = tf.Session()
print(sess.run(hello))
```

If the system outputs the following, then you are ready to begin writing
TensorFlow programs:

<pre>Hello, TensorFlow!</pre>

If you are new to TensorFlow, see @{$get_started/get_started$Getting Started with
TensorFlow}.

If the system outputs an error message instead of a greeting, see [Common
installation problems](#common_installation_problems).

## Common installation problems

The installation problems you encounter typically depend on the
operating system.  See the "Common installation problems" section
of one of the following guides:

  * @{$install_linux#CommonInstallationProblems$Installing TensorFlow on Linux}
  * @{$install_mac#CommonInstallationProblems$Installing TensorFlow on Mac OS}
  * @{$install_windows#CommonInstallationProblems$Installing TensorFlow on Windows}

Beyond the errors documented in those two guides, the following table
notes additional errors specific to building TensorFlow.  Note that we
are relying on Stack Overflow as the repository for build and installation
problems.  If you encounter an error message not listed in the preceding
two guides or in the following table, search for it on Stack Overflow.  If
Stack Overflow doesn't show the error message, ask a new question on
Stack Overflow and specify the `tensorflow` tag.

<table>
<tr> <th>Stack Overflow Link</th> <th>Error Message</th> </tr>

<tr>
  <td><a href="http://stackoverflow.com/q/42013316">42013316</a></td>
  <td><pre>ImportError: libcudart.so.8.0: cannot open shared object file:
  No such file or directory</pre></td>
</tr>

<tr>
  <td><a href="http://stackoverflow.com/q/42013316">42013316</a></td>
  <td><pre>ImportError: libcudnn.5: cannot open shared object file:
  No such file or directory</pre></td>
</tr>

<tr>
  <td><a href="http://stackoverflow.com/q/35953210">35953210</a></td>
  <td>Invoking `python` or `ipython` generates the following error:
  <pre>ImportError: cannot import name pywrap_tensorflow</pre></td>
</tr>
</table>
