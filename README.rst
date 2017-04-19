php2json
========

.. image:: https://travis-ci.org/mbachry/php2json.svg?branch=master
    :alt: Build status
    :target: https://travis-ci.org/mbachry/php2json

A small library that aims to provide a fast way of decoding PHP
serialization format. Use it if you need to migrate large amount of
PHP data and performance is essential.

The library provides only one function::

    >>> import php2json
    >>> php2json.php2json(b's:3:"foo";')
    b'"foo"'

It's goal is to convert PHP serializer string into JSON string as fast
as possible. The assumption is that JSON decoders (such as standard
json module or `ujson`_) are much better optimized than
`phpserialize`_.

.. _ujson: https://pypi.python.org/pypi/ujson
.. _phpserialize: https://pypi.python.org/pypi/phpserialize/

Here's a simple benchmark where I pit phpserialize against php2json
combined with ujson and json::

    phpserialize: 6.60s
    php2json+ujson: 0.15s
    php2json+json: 0.19s
    speedup (ujson): 4337%
    speedup (json): 3485%

Installation
------------

Install with::

    pip install php2json

Tests
-----

Run test with::

    pip install tox
    make clean
    tox .

Benchmark
---------

Run benchmark with::

    pip install -r requirements-test.txt
    make bench

Limitations
-----------

Few limitations apply:

* only deserialization is possible

* PHP objects are not supported

* behaviour is undefined in presence of unicode strings

* input data must adhere to JSON standards, most notably: array keys
  must be strings or values easily convertable to strings (such as
  integers)
