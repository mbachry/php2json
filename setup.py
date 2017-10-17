from setuptools import setup, Extension


with open('README.rst') as readme_file:
    readme = readme_file.read()


setup(
    name='php2json',
    version='0.2',
    description="Convert PHP serializer strings to JSON",
    long_description=readme,
    author="Marcin Bachry",
    author_email='hegel666@gmail.com',
    url='https://github.com/mbachry/php2json',
    # setup_requires=['cython'],
    ext_modules=[
        Extension('php2json',
                  sources=['php2json.pyx', 'lib/php2json.c'],
                  extra_compile_args=['-std=gnu99'])
    ],
    license="MIT",
    zip_safe=False,
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        "Programming Language :: Python :: 2",
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
    ]
)
