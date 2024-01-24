# Gzip and k-NN for Text Classification

A proof of concept implementation for using Gzip and k-NN to perform parameter-free text classification.

### References

Inspired by:

- https://github.com/tsoding/data-mining-in-c
- https://www.youtube.com/watch?v=kH-hqG34ylA

Original Paper: https://arxiv.org/abs/2212.09410

CSV Parsing Library: https://github.com/d99kris/rapidcsv
Gzip Compression Library: https://zlib.net

### Motivation

DNNs are used often for text classification for their high levels of accuracy, but they're computationally intensive due to large amounts of labeled data and parameters.

### Proposed Solution

Using a combination of a simple compressor like gzip and a k-nearest neighbor classifier, it is possible to achieve similar or even better results without any training, pretraining, or fine tuning. This light weight and universal solution also covers cases where there may be limited labeled data for DNNs to perform well.

### Gzip and Lossless Compressors

Used primarily in the past for plagiarism detection, he reason compressor-based distance metrics work is because are good at capturing regularity, and objects from the same category share more regularity than those that aren't.

Given the following texts:

$x_1 = \text{Japan’s Seiko Epson Corp. said Wednesday it has developed a 12-gram flying microrobot, the world’s lightest.}$
$x_2 = \text{The latest tiny flying robot that could help in search and rescue or surveillance has been unveiled in Japan.}$
$x_3 = \text{Michael Phelps won the gold medal in the 400 individual medley and set a world record in a time of 4 minutes 8.26 seconds}$

If we take $C(•)$ to represent compressed lengths:
$$C(x_1x_2) - C(x_1) < C(x_1x_3) - C(x_1)$$

Since $x_1$ and $x_2$ share more regularity than $x_1$ and $x_3$, more bytes are saved encoding $x_2$ if we know $x_1$ than they would be encoding $x_3$. This is referred to as Prediction by Partial Matching (PPM) which estimates cross entropy between the probability distribution built on class c and the document d.

### Approach

We begin by computing the Normalized Compression Distance (NCD) between each test sample and each training sample.

$$NCD(x,y) = \frac{ C(xy) - min(C(x), C(y)) }{ max(C(x), C(y)) }$$

With the distance matrix NCD provides, we can then use k-nearest neighbors algorithm (k-NN) to classify the test sample.
