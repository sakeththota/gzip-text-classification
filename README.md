# Gzip Text Classification

The full blog post for this project can be found [here](https://skth.vercel.app/blog/gzip-knn)

## Results
Training on the AG News dataset I was able to achieve about 85% accuracy after testing 1/3
of the test samples. The original paper acheived a 92% accuracy.

Due to cpu limitations, I was not able to test on the whole set. In the future I would like to try
C++'s thread library as much of the computation time here can be parallelized.

## References
- [Tsoding Implementation](https://github.com/tsoding/data-mining-in-c)
- [Tsoding Video](https://www.youtube.com/watch?v=kH-hqG34ylA)
- [Original Paper](https://arxiv.org/abs/2212.09410)
- [rapidcsv](https://github.com/d99kris/rapidcsv)
- [zlib](https://zlib.net)
- [k-NN](https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm)
- [k-NN example img](https://www.analyticsvidhya.com/blog/2018/03/introduction-k-neighbours-algorithm-clustering/)
