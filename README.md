Utilities for fst weighting
===========================

This is a set of tools for weighting finite-state transducers using data.

Building
--------

If you have `g++-4.7` installed, simply

    $ make
    
should be sufficient. In any case, you need a c++ 11 capable compiler. E.g. if you're using clang++, just change the line

    CXX=g++-4.7
into

    CXX=clang++

in the `Makefile` and then run

    $ make

`weight_fst`
------------

The utility `weight_fst` reads `word_form<TAB>analysis` string pairs from `STDIN`. It traverses the argument transducer using each of the string pairs and if the string pair is recognized, it adds +1 weight to each transition on the corresponding path and +1 weight to the final state corresponding to the end of the string pair.

Usage of `weight_fst`:

`USAGE: cat string_pairs | ./weight_fst input_fst_file output_fst_file [PLAIN | PERCEPTRON | AVG_PERCEPTRON]`

In case the argument transducer is non-deterministic, `weight_fst` will weight the *first path that it finds* corresponding the a string pair. 

`normalize_weights`
-------------------

The utility `normalize_weights` performs a state-wise normalization of weights in a transducer. It will first perform Laplace smoothing which adds a fixed amount of weight (+1) to all transitions in the transducer and all finite states.
It then normalizes each state `s` by dividing each transition weight and the final weight in state s with the combined transition and final weight of the state.

Finally, all weights are converted into tropical weights using the transformation x -> -log(x).

Example
-------

This is an example where a small analyzer is transformed into a weighted transducer and converted into optimized lookup format.

    $ hfst-fst2txt dog.hfst 
    0	1	d	d	0.000000
    1	2	o	o	0.000000
    2	3	g	g	0.000000
    3	4	@0@	+N	0.000000
    3	4	@0@	+V	0.000000
    4	0.000000
   
    $ cat dog_pairs 
    dog	dog+N
    dog	dog+N
    dog	dog+V

    $ cat dog_pairs | ./weight_fst dog.hfst dog.weighted.hfst PLAIN
    Reading input fst from dog.hfst.
    Reading string pairs from STDIN.
    Writing weighted fst to dog.weighted.hfst

    $ ./normalize_weights dog.weighted.hfst dog.weighted.hfst.norm
    $ hfst-fst2txt dog.weighted.hfst.norm 
    0	1	d	d	-0.000000
    1	2	o	o	-0.000000
    2	3	g	g	-0.000000
    3	4	@0@	+N	0.510826
    3	4	@0@	+V	0.916291
    4	-0.000000

    $ hfst-fst2fst -w dog.weighted.hfst.norm -o dog.distribution.hfst
