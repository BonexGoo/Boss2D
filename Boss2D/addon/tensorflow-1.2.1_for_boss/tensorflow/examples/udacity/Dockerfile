FROM gcr.io/tensorflow/tensorflow:latest
MAINTAINER Vincent Vanhoucke <vanhoucke@google.com>

# Pillow needs libjpeg by default as of 3.0.
RUN apt-get update && apt-get install -y --no-install-recommends \
        libjpeg8-dev \
        && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN pip install scikit-learn pyreadline Pillow
RUN rm -rf /notebooks/*
ADD *.ipynb /notebooks/
WORKDIR /notebooks
CMD ["/run_jupyter.sh"]
